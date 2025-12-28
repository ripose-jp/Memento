////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
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

#include "playeroverlay.h"

#include <QGraphicsOpacityEffect>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QSettings>

#include "gui/widgets/common/hittestwidget.h"
#include "gui/widgets/definition/definitionwidget.h"
#include "gui/widgets/overlay/playercontrols.h"
#include "gui/widgets/overlay/playermenu.h"
#include "gui/widgets/overlay/subtitlewidget.h"
#include "player/playeradapter.h"
#include "util/constants.h"

#ifdef OCR_SUPPORT
#include "gui/widgets/overlay/ocroverlay.h"
#endif // OCR_SUPPORT

/* Begin Constructor/Destructor */

PlayerOverlay::PlayerOverlay(Context *context, QWidget *parent) :
    QStackedLayout(parent),
    m_context(std::move(context))
{
    /* Fix the margins */
    setSpacing(0);
    setContentsMargins(0, 0, 0, 0);

    /* Create the container */
    QVBoxLayout *layoutContainer = new QVBoxLayout;
    layoutContainer->setContentsMargins(0, 0, 0, 0);
    layoutContainer->setSpacing(0);
    m_widgetOSC = new HitTestWidget;
    m_widgetOSC->setMouseTracking(true);
    m_widgetOSC->setLayout(layoutContainer);
    addWidget(m_widgetOSC);

    /* Set up the definition widget */
    m_definition = new DefinitionWidget(m_context, false, m_widgetOSC);
    m_definition->hide();

    /* Add the menubar */
    m_menu = new PlayerMenu(m_context);
    m_menu->hideMenu();
    QGraphicsOpacityEffect *menuEffect = new QGraphicsOpacityEffect;
    menuEffect->setOpacity(0);
    m_menu->setGraphicsEffect(menuEffect);
    layoutContainer->addWidget(m_menu);

    /* Add space between the menu and subtitle */
    layoutContainer->addStretch();

    /* Add the subtitle */
    m_subtitle = new SubtitleWidget(m_context);
    layoutContainer->addWidget(m_subtitle, 0, Qt::AlignmentFlag::AlignCenter);

    /* Add the generic widget spacer */
    m_spacer = new QWidget;
    m_spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_spacer->setFixedWidth(0);
    m_spacer->setMouseTracking(true);
    layoutContainer->addWidget(m_spacer);

    /* Add the player controls */
    m_controls = new PlayerControls(m_context);
    m_controls->hide();
    m_controls->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    parentWidget()->setMinimumWidth(m_controls->minimumWidth());
    QGraphicsOpacityEffect *controlsEffect = new QGraphicsOpacityEffect;
    controlsEffect->setOpacity(0);
    m_controls->setGraphicsEffect(controlsEffect);
    layoutContainer->addWidget(m_controls);

#ifdef OCR_SUPPORT
    /* Add the OCR Overlay */
    m_ocrOverlay = new OCROverlay(m_context);
    m_ocrOverlay->hide();
    addWidget(m_ocrOverlay);
#endif // OCR_SUPPORT

    connect(
        m_context, &Context::interfaceSettingsChanged,
        this, &PlayerOverlay::initSettings,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::behaviorSettingsChanged,
        this, &PlayerOverlay::initSettings,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::windowOSCStateCycled,
        this, &PlayerOverlay::cycleOSCVisibility,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::playerResized,
        this, &PlayerOverlay::repositionSubtitles,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::playerMouseMoved,
        this, &PlayerOverlay::handleMouseMovement,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::playerFullscreenChanged,
        this, &PlayerOverlay::hideOverlay,
        Qt::QueuedConnection
    );
#if defined(Q_OS_WIN)
    connect(
        m_context, &Context::playerFullscreenChanged,
        this, &PlayerOverlay::menuBarHandleStateChange,
        Qt::QueuedConnection
    );
#endif
    connect(
        m_menu, &PlayerMenu::aboutToHide,
        &m_hideTimer, qOverload<>(&QTimer::start),
        Qt::QueuedConnection
    );

    connect(
        m_context, &Context::menuSubtitleSizeIncrease,
        this, &PlayerOverlay::increaseSubScale,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::menuSubtitleSizeDecrease,
        this, &PlayerOverlay::decreaseSubScale,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::menuSubtitlesMoveUp,
        this, &PlayerOverlay::moveSubsUp,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::menuSubtitlesMoveDown,
        this, &PlayerOverlay::moveSubsDown,
        Qt::QueuedConnection
    );

    connect(
        m_context, &Context::termsChanged,
        this, &PlayerOverlay::setTerms
    );
    connect(
        m_context, &Context::requestDefinitionDelete,
        m_definition, &DefinitionWidget::hide,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::playerSubtitleChanged,
        m_definition, &DefinitionWidget::hide,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::playerResized,
        m_definition, &DefinitionWidget::hide,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::subtitleExpired,
        m_definition, &DefinitionWidget::hide,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::subtitleHidden,
        m_definition, &DefinitionWidget::hide,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::controlsHidden,
        m_definition, &DefinitionWidget::hide,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::playerPauseStateChanged, this,
        [this] (const bool paused)
        {
            if (!paused)
            {
                m_definition->hide();
            }
        },
        Qt::QueuedConnection
    );

    connect(
        m_definition, &DefinitionWidget::widgetHidden,
        m_context, &Context::definitionsHidden,
        Qt::QueuedConnection
    );
    connect(
        m_definition, &DefinitionWidget::widgetShown,
        m_context, &Context::definitionsShown,
        Qt::QueuedConnection
    );
    connect(
        m_definition,
        &DefinitionWidget::widgetShown,
        m_definition,
        [this] () {
            m_definition->setFocus(Qt::FocusReason::PopupFocusReason);
        },
        Qt::QueuedConnection
    );
    connect(
        m_definition, &DefinitionWidget::widgetHidden,
        &m_hideTimer, qOverload<>(&QTimer::start),
        Qt::QueuedConnection
    );

    connect(
        &m_hideTimer, &QTimer::timeout, this, &PlayerOverlay::hideOverlay,
        Qt::DirectConnection
    );
    connect(
        &m_mouseMovement.tickTimer, &QTimer::timeout,
        this, &PlayerOverlay::handleTickTimeout,
        Qt::DirectConnection
    );

#ifdef OCR_SUPPORT
    connect(
        m_context, &Context::controlsOCRToggled,
        this, &PlayerOverlay::startOCR,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::menuEnterOCRMode,
        this, &PlayerOverlay::startOCR,
        Qt::QueuedConnection
    );
    connect(
        m_ocrOverlay, &OCROverlay::widgetHidden, this, &PlayerOverlay::stopOCR,
        Qt::QueuedConnection
    );
    connect(
        m_ocrOverlay, &OCROverlay::finished,
        m_context, &Context::searchWidgetRequest,
        Qt::QueuedConnection
    );
    connect(
        m_ocrOverlay, &OCROverlay::finished,
        this,
        [this] { emit m_context->requestSearchVisibility(true); },
        Qt::QueuedConnection
    );
#endif // OCR_SUPPORT

    /* Initialize the tick timer */
    m_mouseMovement.lastPoint = QCursor::pos();
    m_mouseMovement.lastTickPoint = m_mouseMovement.lastPoint;
    m_mouseMovement.tickTimer.setSingleShot(false);
    m_mouseMovement.tickTimer.setInterval(100);
    m_mouseMovement.tickTimer.start();

    initSettings();
}

PlayerOverlay::~PlayerOverlay()
{
    m_definition->deleteLater();
}

/* End Constructor/Destructor */
/* Begin Initializers */

void PlayerOverlay::initSettings()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Interface::GROUP);

    m_settings.subOffset = settings.value(
        Constants::Settings::Interface::Subtitle::OFFSET,
        Constants::Settings::Interface::Subtitle::OFFSET_DEFAULT
    ).toDouble();
#if defined(Q_OS_WIN)
    m_settings.showMenuBar = settings.value(
        Constants::Settings::Interface::Subtitle::MENUBAR_FULLSCREEN,
        Constants::Settings::Interface::Subtitle::MENUBAR_FULLSCREEN_DEFAULT
    ).toBool();
    menuBarHandleStateChange(m_menu->window()->isFullScreen());
#endif

    m_definition->setGeometry(
        0,
        0,
        settings.value(
            Constants::Settings::Interface::Subtitle::POPUP_WIDTH,
            Constants::Settings::Interface::Subtitle::POPUP_WIDTH_DEFAULT
        ).toInt(),
        settings.value(
            Constants::Settings::Interface::Subtitle::POPUP_HEIGHT,
            Constants::Settings::Interface::Subtitle::POPUP_HEIGHT_DEFAULT
        ).toInt()
    );

    settings.endGroup();

    settings.beginGroup(Constants::Settings::Behavior::GROUP);

    m_hideTimer.setSingleShot(true);
    m_hideTimer.setInterval(
        settings.value(
            Constants::Settings::Behavior::OSC_DURATION,
            Constants::Settings::Behavior::OSC_DURATION_DEFAULT
        ).toInt()
    );
    m_settings.fadeDuration = settings.value(
        Constants::Settings::Behavior::OSC_FADE,
        Constants::Settings::Behavior::OSC_FADE_DEFAULT
    ).toInt();
    m_settings.cursorMinMove = settings.value(
        Constants::Settings::Behavior::OSC_MIN_MOVE,
        Constants::Settings::Behavior::OSC_MIN_MOVE_DEFAULT
    ).toInt();

    m_widgetOSC->clearRegions();

    bool showPrimarySubtitles
    {
        settings.value(
            Constants::Settings::Behavior::SUBTITLE_CURSOR_SHOW,
            Constants::Settings::Behavior::SUBTITLE_CURSOR_SHOW_DEFAULT
        ).toBool()
    };
    if (showPrimarySubtitles)
    {
        constexpr qreal SHOW_AREA{0.2};

        const QRectF area{0.0, 1.0 - SHOW_AREA, 1.0, SHOW_AREA};

        m_widgetOSC->addRegion(
            area,
            [this] ()
            {
                emit m_context->requestSubtitleWidgetVisibility(true);
            },
            HitTestWidget::MouseEventFlag::Enter
        );
        m_widgetOSC->addRegion(
            area,
            [this] ()
            {
                emit m_context->requestSubtitleWidgetVisibility(false);
            },
            HitTestWidget::MouseEventFlag::Exit
        );
    }
    emit m_context->requestSubtitleWidgetVisibility(!showPrimarySubtitles);

    bool showSecondarySubtitles
    {
        settings.value(
            Constants::Settings::Behavior::SECONDARY_SUBTITLE_CURSOR_SHOW,
            Constants::Settings::Behavior::SECONDARY_SUBTITLE_CURSOR_SHOW_DEFAULT
        ).toBool()
    };
    if (showSecondarySubtitles)
    {
        constexpr qreal SHOW_AREA{0.2};

        const QRectF area{0.0, 0.0, 1.0, SHOW_AREA};

        m_widgetOSC->addRegion(
            area,
            [this] ()
            {
                m_context->getPlayerAdapter()->setSecondarySubVisiblity(true);
            },
            HitTestWidget::MouseEventFlag::Enter
        );
        m_widgetOSC->addRegion(
            area,
            [this] ()
            {
                m_context->getPlayerAdapter()->setSecondarySubVisiblity(false);
            },
            HitTestWidget::MouseEventFlag::Exit
        );
    }

    settings.endGroup();

    repositionSubtitles();
}

/* End Initializers */
/* Begin Handlers */

void PlayerOverlay::cycleOSCVisibility()
{
    m_definition->hide();

    QString msg = tr("OSC Visibility: ");
    switch (m_settings.visibility)
    {
    case OSCVisibility::Auto:
        m_settings.visibility = OSCVisibility::Visible;
        msg += tr("visible");
        showOverlay();
        break;
    case OSCVisibility::Visible:
        m_settings.visibility = OSCVisibility::Hidden;
        msg += tr("hidden");
        hideOverlay();
        break;
    case OSCVisibility::Hidden:
        m_settings.visibility = OSCVisibility::Auto;
        msg += tr("auto");
        hideOverlay();
        break;
    }
    m_context->getPlayerAdapter()->showText(msg);
}

void PlayerOverlay::handleMouseMovement()
{
    m_mouseMovement.lastPoint = QCursor::pos();
}

void PlayerOverlay::handleTickTimeout()
{
    QPoint diff = m_mouseMovement.lastPoint - m_mouseMovement.lastTickPoint;
    if (diff.manhattanLength() > m_settings.cursorMinMove)
    {
        showOverlay();
    }
    m_mouseMovement.lastTickPoint = m_mouseMovement.lastPoint;
}

/* End Handlers */
/* Begin Definition Widget Helpers */

void PlayerOverlay::setTerms(SharedTermList terms, SharedKanji kanji)
{
    m_definition->setTerms(terms, kanji);
    setDefinitionWidgetLocation();
    m_definition->setVisible(terms || kanji);
}

void PlayerOverlay::setDefinitionWidgetLocation()
{
    const QPoint mousePos = parentWidget()->mapFromGlobal(QCursor::pos());

    int x = mousePos.x() - (m_definition->width() / 2);
    if (x < 0)
    {
        x = 0;
    }
    else if (x > parentWidget()->width() - m_definition->width())
    {
        x = parentWidget()->width() - m_definition->width();
    }

    int y = m_subtitle->pos().y() - m_definition->height();
    if (y < 0)
    {
        y = m_subtitle->pos().y() + m_subtitle->height();
    }

    if (y + m_definition->height() > parentWidget()->height())
    {
        m_definition->hide();
    }
    else
    {
        m_definition->move(x, y);
    }
}

/* End Definition Widget Helpers */
/* Begin Subtitle Widget Helpers */

void PlayerOverlay::repositionSubtitles()
{
    int height = parentWidget()->height() * m_settings.subOffset;
    if (m_controls->isVisible())
    {
        height -= m_controls->height();
        if (height < 0)
        {
            height = 0;
        }
    }
    m_spacer->setFixedHeight(height);
}

void PlayerOverlay::updateSubScale(const double inc)
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Interface::GROUP);
    double scale = settings.value(
        Constants::Settings::Interface::Subtitle::SCALE,
        Constants::Settings::Interface::Subtitle::SCALE_DEFAULT
    ).toDouble();
    scale += inc;
    if (scale <= 0.0)
    {
        scale = 0.0;
    }
    else if (scale >= 1.0)
    {
        scale = 1.0;
    }
    settings.setValue(Constants::Settings::Interface::Subtitle::SCALE, scale);
    emit m_context->interfaceSettingsChanged();
}

void PlayerOverlay::increaseSubScale()
{
    updateSubScale(0.005);
}

void PlayerOverlay::decreaseSubScale()
{
    updateSubScale(-0.005);
}

void PlayerOverlay::moveSubtitles(const double inc)
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Interface::GROUP);
    double offset = settings.value(
        Constants::Settings::Interface::Subtitle::OFFSET,
        Constants::Settings::Interface::Subtitle::OFFSET_DEFAULT
    ).toDouble();
    offset += inc;
    if (offset <= 0.0)
    {
        offset = 0.0;
    }
    else if (offset >= 1.0)
    {
        offset = 1.0;
    }
    settings.setValue(Constants::Settings::Interface::Subtitle::OFFSET, offset);
    emit m_context->interfaceSettingsChanged();
}

void PlayerOverlay::moveSubsUp()
{
    moveSubtitles(0.005);
}

void PlayerOverlay::moveSubsDown()
{
    moveSubtitles(-0.005);
}

/* End Subtitle Widget Helpers */
/* Begin General Widget Properties */

bool PlayerOverlay::underMouse() const
{
    return m_widgetOSC->childrenRegion().contains(
        parentWidget()->mapFromGlobal(QCursor::pos())
    );
}

#if defined(Q_OS_WIN)
void PlayerOverlay::menuBarHandleStateChange(bool fullscreen)
{
    switch (m_settings.visibility)
    {
    case OSCVisibility::Visible:
        if (m_settings.showMenuBar)
        {
            showOverlay();
        }
        else if (fullscreen)
        {
            m_menu->hideMenu();
        }
        else
        {
            m_menu->showMenu();
        }
        break;
    case OSCVisibility::Hidden:
    case OSCVisibility::Auto:
        return;
    }
}
#endif

void PlayerOverlay::hideOverlay()
{
    /* Determine how this method should work by checking OSC visibility */
    switch (m_settings.visibility)
    {
    case OSCVisibility::Hidden:
        /* See showOverlay() */
        if (m_menuFade)
        {
            m_menu->showMenu();
        }
        if (m_controlFade)
        {
            m_controls->show();
        }
    case OSCVisibility::Auto:
        break;
    case OSCVisibility::Visible:
        return;
    }

    if (m_settings.visibility != OSCVisibility::Hidden &&
        (underMouse() || m_menu->menuOpen() || m_definition->isVisible()))
    {
        return;
    }

    if (m_menu->menuVisible())
    {
        QPropertyAnimation *menuFade =
            new QPropertyAnimation(m_menu->graphicsEffect(), "opacity");
        menuFade->setDuration(m_settings.fadeDuration);
        menuFade->setStartValue(1);
        menuFade->setEndValue(0);
        menuFade->setEasingCurve(QEasingCurve::InBack);
        connect(
            menuFade, &QPropertyAnimation::finished,
            m_menu, &PlayerMenu::hideMenu
        );
        menuFade->start(QPropertyAnimation::DeleteWhenStopped);
        m_menuFade = menuFade;
    }

    if (m_controls->isVisible())
    {
        QPropertyAnimation *controlFade =
            new QPropertyAnimation(m_controls->graphicsEffect(), "opacity");
        controlFade->setDuration(m_settings.fadeDuration);
        controlFade->setStartValue(1);
        controlFade->setEndValue(0);
        controlFade->setEasingCurve(QEasingCurve::InBack);
        connect(
            controlFade, &QPropertyAnimation::finished,
            m_controls, &QWidget::hide
        );
        connect(
            controlFade, &QPropertyAnimation::finished,
            this, &PlayerOverlay::repositionSubtitles
        );
        controlFade->start(QPropertyAnimation::DeleteWhenStopped);
        m_controlFade = controlFade;
    }
}

void PlayerOverlay::showOverlay()
{
    /* Determine how this method should work by checking OSC visibility */
    switch (m_settings.visibility)
    {
    case OSCVisibility::Visible:
        /* This is a little hack to prevent a race condition where hideOverlay
         * was called, but widgets have yet to be hidden because they are
         * currently fading out.
         */
        if (m_menuFade)
        {
            m_menu->hideMenu();
        }
        if (m_controlFade)
        {
            m_controls->hide();
        }
    case OSCVisibility::Auto:
        break;
    case OSCVisibility::Hidden:
        return;
    }

#ifdef OCR_SUPPORT
    if (m_ocrOverlay->isVisible())
    {
        return;
    }
#endif // OCR_SUPPORT

    /* Restart the timer */
    m_hideTimer.start();

#if defined(Q_OS_WIN)
    if (!m_menu->menuVisible() &&
        (m_settings.showMenuBar || !m_menu->window()->isFullScreen()))
#else
    if (!m_menu->menuVisible())
#endif
    {
        m_menu->showMenu();
        QPropertyAnimation *menuFade =
            new QPropertyAnimation(m_menu->graphicsEffect(), "opacity");
        menuFade->setDuration(m_settings.fadeDuration);
        menuFade->setStartValue(0);
        menuFade->setEndValue(1);
        menuFade->setEasingCurve(QEasingCurve::InBack);
        menuFade->start(QPropertyAnimation::DeleteWhenStopped);
    }

    if (m_controls->isHidden())
    {
        m_controls->show();
        QPropertyAnimation *controlFade =
            new QPropertyAnimation(m_controls->graphicsEffect(), "opacity");
        controlFade->setDuration(m_settings.fadeDuration);
        controlFade->setStartValue(0);
        controlFade->setEndValue(1);
        controlFade->setEasingCurve(QEasingCurve::InBack);
        controlFade->start(QPropertyAnimation::DeleteWhenStopped);

        repositionSubtitles();
    }
}

/* End General Widget Properties */
#ifdef OCR_SUPPORT
/* Begin OCR Handlers */

void PlayerOverlay::startOCR()
{
    m_definition->hide();
    m_menu->hideMenu();
    m_controls->hide();
    setCurrentIndex(1);
}

void PlayerOverlay::stopOCR()
{
    setCurrentIndex(0);
    if (m_settings.visibility == OSCVisibility::Visible)
    {
        showOverlay();
    }
}

/* End OCR Handlers */
#endif // OCR_SUPPORT
