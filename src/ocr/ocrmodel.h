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

#pragma once

#ifdef MEMENTO_OCR_SUPPORT

#include <QFuture>
#include <QReadWriteLock>
#include <QtGlobal>

namespace mocr
{
    class model;
}

/**
 * @brief OcrModel provides an interface for converting images into text.
 */
class OcrModel
{
public:
    /**
     * @brief Initializes an OcrModel object.
     *
     * @param model  The model to load. This can be a local file, HuggingFace
     * repo, or URL.
     * @param useGpu True to use the GPU is available, false otherwise.
     */
    OcrModel(const QString &model, bool useGpu);
    virtual ~OcrModel();

    /**
     * @brief Get text from an image.
     *
     * @param image The image to get text from.
     * @return A future containing the text of the image.
     */
    QFuture<QString> getText(const QImage &image);

private:
    /**
     * Get the the model object.
     *
     * @return A pointer to the model object.
     */
    inline mocr::model *getModel() const;

    /* A future containing the model. This is necessary because loading the
     * model takes a long time, so multithreading its construction prevents
     * the UI from being blocked. */
    QFuture<mocr::model *> m_model;

    /* A lock to prevent the model from being deleted while it is in use. */
    QReadWriteLock m_modelLock;
};

#endif // MEMENTO_OCR_SUPPORT
