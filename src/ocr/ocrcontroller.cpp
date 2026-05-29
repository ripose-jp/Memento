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

#include "ocr/ocrcontroller.h"

#include <QImage>
#include <QRect>
#include <QSizeF>
#include <QVariantMap>

#ifdef MEMENTO_SYSTEM_QCORO
#include <QCoroFuture>
#else
#include <qcoro/core/qcorofuture.h>
#endif // MEMENTO_SYSTEM_QCORO

#include "player/mpvcontroller.h"
#include "player/mpvplayer.h"
#include "player/mpvstate.h"
#include "setting/settings.h"

#ifdef MEMENTO_OCR_SUPPORT
#include "ocr/ocrmodel.h"
#endif // MEMENTO_OCR_SUPPORT

static constexpr const char *KEY_ERROR = "error";
#ifdef MEMENTO_OCR_SUPPORT
static constexpr const char *KEY_TEXT = "text";
#endif // MEMENTO_OCR_SUPPORT

OcrController::OcrController(Settings *settings, QObject *parent) :
    QObject(parent),
    m_settings(settings)
{

}

OcrController::~OcrController()
{

}

QCoro::QmlTask OcrController::readRegion(
    MpvController *controller,
    const QRectF &selection,
    double playerWidth,
    double playerHeight)
{
    return readRegionAsync(controller, selection, playerWidth, playerHeight);
}

QCoro::Task<QVariantMap> OcrController::readRegionAsync(
    MpvController *controller,
    QRectF selection,
    double playerWidth,
    double playerHeight)
{
    QVariantMap result;

#ifndef MEMENTO_OCR_SUPPORT
    Q_UNUSED(controller)
    Q_UNUSED(selection)
    Q_UNUSED(playerWidth)
    Q_UNUSED(playerHeight)
    result[KEY_ERROR] = tr("OCR support is not available in this build.");
    co_return result;
#else
    if (m_settings == nullptr || !m_settings->ocrEnabled())
    {
        result[KEY_ERROR] = tr("OCR is disabled.");
        co_return result;
    }

    if (controller == nullptr ||
        controller->player() == nullptr ||
        controller->player()->state() == nullptr)
    {
        result[KEY_ERROR] = tr("No player is available.");
        co_return result;
    }

    if (selection.width() <= 0 ||
        selection.height() <= 0 ||
        playerWidth <= 0 ||
        playerHeight <= 0)
    {
        result[KEY_ERROR] = tr("No region was selected.");
        co_return result;
    }

    MpvState *state = controller->player()->state();
    if (state->videoWidth() <= 0 || state->videoHeight() <= 0)
    {
        result[KEY_ERROR] = tr("No video is loaded.");
        co_return result;
    }

    QImage screenshot = controller->screenshotRaw(true);
    if (screenshot.isNull())
    {
        result[KEY_ERROR] = tr("Could not capture a video frame.");
        co_return result;
    }

    const QSizeF playerSize(playerWidth, playerHeight);
    const QSizeF imageSize(screenshot.size());
    const QSizeF displayedSize = imageSize.scaled(
        playerSize,
        Qt::KeepAspectRatio
    );
    const QRectF displayedVideo(
        (playerWidth - displayedSize.width()) / 2.0,
        (playerHeight - displayedSize.height()) / 2.0,
        displayedSize.width(),
        displayedSize.height()
    );
    const QRectF selectedVideo =
        selection.normalized().intersected(displayedVideo);
    if (selectedVideo.width() <= 0 || selectedVideo.height() <= 0)
    {
        result[KEY_ERROR] = tr("The selected region does not include video.");
        co_return result;
    }

    const double xScale = imageSize.width() / displayedVideo.width();
    const double yScale = imageSize.height() / displayedVideo.height();
    QRect imageRect(
        qRound((selectedVideo.x() - displayedVideo.x()) * xScale),
        qRound((selectedVideo.y() - displayedVideo.y()) * yScale),
        qRound(selectedVideo.width() * xScale),
        qRound(selectedVideo.height() * yScale)
    );
    imageRect = imageRect.intersected(QRect(QPoint(0, 0), screenshot.size()));
    if (imageRect.width() <= 0 || imageRect.height() <= 0)
    {
        result[KEY_ERROR] = tr("The selected region is empty.");
        co_return result;
    }

    const QImage crop = screenshot.copy(imageRect);
    QString text = co_await qCoro(model()->getText(crop)).takeResult();
    text = text.trimmed();
    if (text.isEmpty())
    {
        result[KEY_ERROR] = tr("No text was detected.");
        co_return result;
    }

    result[KEY_TEXT] = text;
    co_return result;
#endif // MEMENTO_OCR_SUPPORT
}

#ifdef MEMENTO_OCR_SUPPORT
OcrModel *OcrController::model()
{
    const QString modelName = m_settings->ocrModel();
    const bool useGpu = m_settings->ocrUseGpu();
    if (!m_model ||
        m_modelName != modelName ||
        m_useGpu != useGpu)
    {
        m_model = std::make_unique<OcrModel>(modelName, useGpu);
        m_modelName = modelName;
        m_useGpu = useGpu;
    }
    return m_model.get();
}
#endif // MEMENTO_OCR_SUPPORT
