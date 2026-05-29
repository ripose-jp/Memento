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

#include <QObject>

#include <memory>

#include <QRectF>

#ifdef MEMENTO_SYSTEM_QCORO
#include <QCoroQmlTask>
#include <QCoroTask>
#else
#include <qcoro/qml/qcoroqmltask.h>
#include <qcoro/qcorotask.h>
#endif // MEMENTO_SYSTEM_QCORO

class MpvController;
class Settings;

#ifdef MEMENTO_OCR_SUPPORT
class OcrModel;
#endif // MEMENTO_OCR_SUPPORT

/**
 * @brief QML interface for OCRing a selected player region.
 */
class OcrController : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Create an OcrController.
     *
     * @param settings The application settings object.
     * @param parent The parent object.
     */
    explicit OcrController(Settings *settings, QObject *parent = nullptr);
    virtual ~OcrController();

    /**
     * @brief OCR a selected rectangle in player coordinates.
     *
     * @param controller The controller for the player to capture.
     * @param selection The selected region in player coordinates.
     * @param playerWidth The width of the player item.
     * @param playerHeight The height of the player item.
     * @return A map with "text" on success or "error" on failure.
     */
    Q_INVOKABLE QCoro::QmlTask readRegion(
        MpvController *controller,
        const QRectF &selection,
        double playerWidth,
        double playerHeight);

private:
    /**
     * @brief Async implementation for readRegion.
     *
     * @param controller The controller for the player to capture.
     * @param selection The selected region in player coordinates.
     * @param playerWidth The width of the player item.
     * @param playerHeight The height of the player item.
     * @return An awaitable task with map with "text" on success or "error" on
     * failure.
     */
    QCoro::Task<QVariantMap> readRegionAsync(
        MpvController *controller,
        QRectF selection,
        double playerWidth,
        double playerHeight);

#ifdef MEMENTO_OCR_SUPPORT
    /**
     * @brief Get the loaded OCR model, recreating it if settings changed.
     */
    OcrModel *model();
#endif // MEMENTO_OCR_SUPPORT

    /* Application settings */
    Settings *m_settings{nullptr};

#ifdef MEMENTO_OCR_SUPPORT
    /* Lazily loaded OCR model */
    std::unique_ptr<OcrModel> m_model;

    /* Name of the manga_ocr HF model to use */
    QString m_modelName;

    /* true to use the GPU when doing OCR, false otherwise */
    bool m_useGpu{false};
#endif // MEMENTO_OCR_SUPPORT
};
