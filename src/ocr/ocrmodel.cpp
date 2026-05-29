////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2026 Ripose
//
// This file is part of Memento.
//
// Memento is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License.
//
// Memento is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Memento.  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#ifdef MEMENTO_OCR_SUPPORT

#include "ocr/ocrmodel.h"

#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QtConcurrent>

#include <mocr++.h>

static constexpr const char *PYTHON_EXECUTABLE = "PYTHONEXECUTABLE";
static constexpr const char *VIRTUAL_ENV = "VIRTUAL_ENV";

#ifdef Q_OS_WIN
static constexpr const char *VENV_PYTHON = "Scripts/python.exe";
#else
static constexpr const char *VENV_PYTHON = "bin/python";
#endif // Q_OS_WIN

#ifdef Q_OS_MACOS
#include <pthread.h>

static constexpr size_t APPLE_STACK_SIZE = 8388608;
#endif // Q_OS_MACOS

/**
 * @brief Sets the executable embedded Python reports through sys.executable.
 *
 * Some OCR dependencies spawn Python worker processes. Without this, embedded
 * Python can report Memento itself as sys.executable, causing workers to launch
 * another application instance instead of the venv Python interpreter.
 */
static void set_python_executable()
{
    if (qEnvironmentVariableIsSet(PYTHON_EXECUTABLE))
    {
        return;
    }

    const QString venv = qEnvironmentVariable(VIRTUAL_ENV);
    if (venv.isEmpty())
    {
        return;
    }

    const QString python = QDir(venv).filePath(VENV_PYTHON);
    if (QFileInfo::exists(python))
    {
        qputenv(PYTHON_EXECUTABLE, python.toUtf8());
    }
}

#ifdef Q_OS_MACOS
/**
 * @brief Struct passed to init_mocr_model_worker to be used as constructor args
 * for creating an mocr model.
 */
struct init_model_args
{
    /* The model to use */
    QString model;

    /* True to use the GPU, false otherwise */
    bool useGpu;
};

/**
 * @brief Initializes an libmocr model. This is a pthread method.
 * This is necessary because the default thread size on macOS is too small to
 * initialize libmocr. There is also no way to change the stack size of
 * std::thread or QtConcurrent::run().
 *
 * @return A pointer to the initialized model.
 */
static void *init_mocr_model_worker(void *void_args)
{
    set_python_executable();

    init_model_args *args = reinterpret_cast<init_model_args *>(void_args);
    return new mocr::model(args->model.toStdString(), !args->useGpu);
}
#endif // Q_OS_MACOS

OcrModel::OcrModel(const QString &model, bool useGpu)
{
    QWriteLocker locker(&m_modelLock);
#ifdef Q_OS_MACOS
    m_model = QtConcurrent::run(
        [model, useGpu] () -> mocr::model *
        {
            pthread_attr_t attrs;
            pthread_attr_init(&attrs);
            pthread_attr_setstacksize(&attrs, APPLE_STACK_SIZE);

            pthread_t worker;
            init_model_args args{
                .model = model,
                .useGpu = useGpu
            };
            pthread_create(&worker, &attrs, init_mocr_model_worker, &args);
            pthread_attr_destroy(&attrs);

            mocr::model *model = nullptr;
            pthread_join(worker, reinterpret_cast<void **>(&model));
            return model;
        }
    );
#else
    m_model = QtConcurrent::run(
        [model, useGpu] () -> mocr::model *
        {
            set_python_executable();
            return new mocr::model(model.toStdString(), !useGpu);
        }
    );
#endif
}

OcrModel::~OcrModel()
{
    QWriteLocker locker(&m_modelLock);
    delete getModel();
}

QFuture<QString> OcrModel::getText(const QImage &image)
{
    return QtConcurrent::run(
        [this, image] () -> QString
        {
            QReadLocker locker(&m_modelLock);

            mocr::model *model = getModel();
            if (!model->valid())
            {
                qWarning("OCR model is invalid.");
                return "";
            }

            QImage rgbaImage = image.convertToFormat(
                QImage::Format::Format_RGBA8888
            );
            return QString::fromStdString(
                model->read(
                    rgbaImage.bits(),
                    rgbaImage.width(), rgbaImage.height(),
                    mocr::mode::RGBA
                )
            );
        }
    );
}

inline mocr::model *OcrModel::getModel() const
{
    return m_model.result();
}

#endif // MEMENTO_OCR_SUPPORT
