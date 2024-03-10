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

#include "ocroverlay.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QRubberBand>
#include <QScreen>
#include <QSettings>
#include <QtConcurrent>

#include "util/constants.h"
#include "util/globalmediator.h"

/* Begin Constructor/Destructor */

OCROverlay::OCROverlay(QWidget *parent)
    : QWidget(parent),
      m_rubberBand(new QRubberBand(QRubberBand::Rectangle, this))
{
    setCursor(Qt::CrossCursor);
    setAutoFillBackground(false);

    m_rubberBand->hide();

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();

    connect(
        &m_resultWatcher, &QFutureWatcher<QString>::finished,
        this, [this] () { Q_EMIT finished(m_resultWatcher.result()); },
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::ocrSettingsChanged,
        this, &OCROverlay::initOCRSettings,
        Qt::DirectConnection
    );
    connect(
        mediator, &GlobalMediator::keyPressed,
        this, &OCROverlay::handleKeyPress,
        Qt::DirectConnection
    );

    initOCRSettings();
}

OCROverlay::~OCROverlay()
{
    delete m_model;
    m_model = nullptr;
}
/* End Constructor/Destructor */
/* Begin Initializers */

void OCROverlay::initOCRSettings()
{
    if (m_model)
    {
        OCRModel *model = m_model;
        m_model = nullptr;
        QFuture<void> deleteFuture =
            QtConcurrent::run([model] { delete model; });
    }

    QSettings settings;
    settings.beginGroup(Constants::Settings::OCR::GROUP);

    bool enabled = settings.value(
            Constants::Settings::OCR::ENABLED,
            Constants::Settings::OCR::ENABLED_DEFAULT
        ).toBool();
    if (!enabled)
    {
        return;
    }

    QString model = settings.value(
            Constants::Settings::OCR::MODEL,
            Constants::Settings::OCR::MODEL_DEFAULT
        ).toString();
    bool useGPU = settings.value(
            Constants::Settings::OCR::ENABLE_GPU,
            Constants::Settings::OCR::ENABLE_GPU_DEFAULT
        ).toBool();
    m_model = new OCRModel(model, useGPU);

    settings.endGroup();
}

/* End Initializers */
/* Begin Event Handlers */

void OCROverlay::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    m_startPoint = event->pos();
    m_rubberBand->setGeometry(
        event->position().x(), event->position().y(), 0, 0
    );
    m_rubberBand->show();
}

void OCROverlay::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    m_rubberBand->setGeometry(QRect::span(m_startPoint, event->pos()));
}

void OCROverlay::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    if (m_rubberBand->isVisible())
    {
        getText(QRect::span(m_startPoint, event->pos()));
    }
    m_rubberBand->hide();
    hide();
}

void OCROverlay::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);

    Q_EMIT widgetHidden();
}

void OCROverlay::handleKeyPress(QKeyEvent *event)
{
    if (isHidden() || event->key() != Qt::Key::Key_Escape)
    {
        return;
    }

    m_rubberBand->hide();
    hide();
    event->accept();
}

/* Hide Event Handlers */
/* Begin Helpers */

void OCROverlay::getText(QRect rect)
{
    if (m_model == nullptr)
    {
        return;
    }

    QOpenGLWidget *glWidget = dynamic_cast<QOpenGLWidget *>(parentWidget());
    if (glWidget == nullptr)
    {
        return;
    }
    qreal ratio = screen() ? screen()->devicePixelRatio() : 1.0;
    QImage image{
        glWidget->grabFramebuffer().copy(QRect(
            static_cast<int>(rect.x() * ratio),
            static_cast<int>(rect.y() * ratio),
            static_cast<int>(rect.width() * ratio),
            static_cast<int>(rect.height() * ratio)
        ))
    };
    m_resultWatcher.setFuture(m_model->getText(image));
}

/* End Helpers */
