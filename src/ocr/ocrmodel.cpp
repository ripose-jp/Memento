////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2022 Ripose
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

#include "ocrmodel.h"

#include <QImage>
#include <QtConcurrent>

#include <mocr++.h>

#ifdef Q_OS_DARWIN
#include <pthread.h>

#define APPLE_STACK_SIZE ((size_t)8388608)
#endif // Q_OS_DARWIN

#ifdef Q_OS_DARWIN
/**
 * Struct passed to init_mocr_model_worker to be used as constructor args for
 * creating an mocr model.
 */
struct init_model_args
{
    /* The model to use */
    QString model;

    /* True to use the GPU, false otherwise */
    bool useGPU;
};

/**
 * Initializes an libmocr model. This is a pthread method.
 * This is necessary because the default thread size on macOS is too small to
 * initialize libmocr. There is also no way to change the stack size of
 * std::thread or QtConcurrent::run().
 * @return A pointer to the initialized model.
 */
static void *init_mocr_model_worker(void *void_args)
{
    init_model_args *args = reinterpret_cast<init_model_args *>(void_args);
    return new mocr::model(args->model.toStdString(), !args->useGPU);
}
#endif // Q_OS_DARWIN

OCRModel::OCRModel(const QString &model, bool useGPU)
{
    QWriteLocker locker(&m_modelLock);
#ifdef Q_OS_DARWIN
    m_model = QtConcurrent::run(
        [model, useGPU] () -> mocr::model *
        {
            pthread_attr_t attrs;
            pthread_attr_init(&attrs);
            pthread_attr_setstacksize(&attrs, APPLE_STACK_SIZE);

            pthread_t worker;
            init_model_args args{
                .model = model,
                .useGPU = useGPU
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
        [model, useGPU] () -> mocr::model *
        {
            return new mocr::model(model.toStdString(), !useGPU);
        }
    );
#endif
}

OCRModel::~OCRModel()
{
    QWriteLocker locker(&m_modelLock);
    delete getModel();
}

QFuture<QString> OCRModel::getText(const QImage &image)
{
    return QtConcurrent::run(
        [this, image] () -> QString
        {
            QReadLocker locker(&m_modelLock);

            mocr::model *model = getModel();
            if (!model->valid())
            {
                qDebug() << "OCR model is invalid.";
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

inline mocr::model *OCRModel::getModel() const
{
    return m_model.result();
}
