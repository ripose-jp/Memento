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

#ifndef OCROVERLAY_H
#define OCROVERLAY_H

#include <QWidget>

#include <memory>

#include <QFutureWatcher>
#include <QRubberBand>

#include "ocr/ocrmodel.h"
#include "state/context.h"

/**
 * Widget for selecting an area of its parent and running OCR on it.
 */
class OCROverlay : public QWidget
{
    Q_OBJECT

public:
    /**
     * Construct a new OCROverlay widget.
     * @param context The application context.
     * @param parent  The parent that will have OCR run it.
     */
    OCROverlay(Context *context, QWidget *parent = nullptr);
    virtual ~OCROverlay() = default;

private Q_SLOTS:
    /**
     * Initializes settings with respect to settings.
     */
    void initOCRSettings();

Q_SIGNALS:
    /**
     * Emitted when an OCR search completes.
     * @param text The returned text.
     */
    void finished(const QString &text) const;

    /**
     * Emitted when this widget is hidden.
     */
    void widgetHidden() const;

protected:
    /**
     * Called when the mouse is pressed. Shows the OCR area.
     * @param event The mouse press event.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * Called when the mouse is moved. Resizes the OCR area.
     * @param event The mouse move event.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * Called when the mouse is released. Executes an OCR search.
     * @param event The mouse release event.
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

    /**
     * Called when this widget is hidden.
     * @param event The hide event.
     */
    void hideEvent(QHideEvent *event) override;

private Q_SLOTS:
    /**
     * Handles a keypress event. This is used to cancel the overlay on escape.
     * @param event The keypress event.
     */
    void handleKeyPress(QKeyEvent *event);

private:
    /**
     * Uses OCR to get text from a given area of its parent.
     * @param rect The area of sample from the parent widget.
     */
    void getText(QRect rect);

    /* The application context */
    Context *m_context = nullptr;

    /* The OCR model */
    std::unique_ptr<OCRModel> m_model = nullptr;

    /* Watches the last QFuture returned from OCRModel */
    QFutureWatcher<QString> m_resultWatcher;

    /* The QRubberBand widget that represents the selected area */
    QRubberBand *m_rubberBand = nullptr;

    /* The point where the mouse was first pressed in the last search */
    QPoint m_startPoint;
};

#endif // OCROVERLAY_H
