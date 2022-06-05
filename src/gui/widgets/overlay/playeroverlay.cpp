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

#include "playercontrols.h"
#include "playermenu.h"
#include "subtitlewidget.h"

#include "gui/playeradapter.h"
#include "gui/widgets/definition/definitionwidget.h"
#include "util/constants.h"
#include "util/globalmediator.h"

/* Begin Constructor/Destructor */

PlayerOverlay::PlayerOverlay(QWidget *parent)
    : QVBoxLayout(parent),
      m_player(parent),
      m_definition(new DefinitionWidget(false, m_player))
{
    /* Hide the definition widget */
    m_definition->hide();

    /* Fix the margins */
    setSpacing(0);
    setContentsMargins(QMargins(0, 0, 0, 0));
    setMargin(0);

    /* Add the menubar */
    m_menu = new PlayerMenu;
    m_menu->hideMenu();
    QGraphicsOpacityEffect *menuEffect = new QGraphicsOpacityEffect;
    menuEffect->setOpacity(0);
    m_menu->setGraphicsEffect(menuEffect);
    addWidget(m_menu);

    /* Add space between the menu and subtitle */
    addStretch();

    /* Add the subtitle */
    m_subtitle = new SubtitleWidget;
    addWidget(m_subtitle, 0, Qt::AlignmentFlag::AlignCenter);

    /* Add the generic widget spacer */
    m_spacer = new QWidget;
    m_spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_spacer->setFixedWidth(0);
    m_spacer->setMouseTracking(true);
    addWidget(m_spacer);

    /* Add the player controls */
    m_controls = new PlayerControls;
    m_controls->hide();
    m_controls->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_controls->setVolumeLimit(
        GlobalMediator::getGlobalMediator()->getPlayerAdapter()->getMaxVolume()
    );
    m_player->setMinimumWidth(m_controls->minimumWidth());
    QGraphicsOpacityEffect *controlsEffect = new QGraphicsOpacityEffect;
    controlsEffect->setOpacity(0);
    m_controls->setGraphicsEffect(controlsEffect);
    addWidget(m_controls);

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();
    connect(
        mediator, &GlobalMediator::interfaceSettingsChanged,
        this, &PlayerOverlay::initSettings,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::behaviorSettingsChanged,
        this, &PlayerOverlay::initSettings,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::windowOSCStateCycled,
        this, &PlayerOverlay::cycleOSCVisibility,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerResized,
        this, &PlayerOverlay::repositionSubtitles,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerMouseMoved,
        this, &PlayerOverlay::handleMouseMovement,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerFullscreenChanged,
        this, &PlayerOverlay::hideOverlay,
        Qt::QueuedConnection
    );
    connect(
        m_menu, &PlayerMenu::aboutToHide,
        &m_hideTimer, qOverload<>(&QTimer::start),
        Qt::QueuedConnection
    );

    connect(
        mediator, &GlobalMediator::menuSubtitleSizeIncrease,
        this, &PlayerOverlay::increaseSubScale,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::menuSubtitleSizeDecrease,
        this, &PlayerOverlay::decreaseSubScale,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::menuSubtitlesMoveUp,
        this, &PlayerOverlay::moveSubsUp,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::menuSubtitlesMoveDown,
        this, &PlayerOverlay::moveSubsDown,
        Qt::QueuedConnection
    );

    connect(
        mediator, &GlobalMediator::termsChanged,
        this, &PlayerOverlay::setTerms
    );
    connect(
        mediator, &GlobalMediator::requestDefinitionDelete,
        m_definition, &DefinitionWidget::hide,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerSubtitleChanged,
        m_definition, &DefinitionWidget::hide,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerResized,
        m_definition, &DefinitionWidget::hide,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::subtitleExpired,
        m_definition, &DefinitionWidget::hide,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::subtitleHidden,
        m_definition, &DefinitionWidget::hide,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::controlsHidden,
        m_definition, &DefinitionWidget::hide,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerPauseStateChanged, this,
        [=] (const bool paused) {
            if (!paused)
            {
                m_definition->hide();
            }
        },
        Qt::QueuedConnection
    );

    connect(
        m_definition, &DefinitionWidget::widgetHidden,
        mediator, &GlobalMediator::definitionsHidden,
        Qt::QueuedConnection
    );
    connect(
        m_definition, &DefinitionWidget::widgetShown,
        mediator, &GlobalMediator::definitionsShown,
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

    settings.beginGroup(SETTINGS_INTERFACE);
    m_settings.subOffset = settings.value(
        SETTINGS_INTERFACE_SUB_OFFSET,
        SETTINGS_INTERFACE_SUB_OFFSET_DEFAULT
    ).toDouble();
#if defined(Q_OS_WIN)
    m_settings.showMenuBar = settings.value(
        SETTINGS_INTERFACE_MENUBAR_FULLSCREEN,
        SETTINGS_INTERFACE_MENUBAR_FULLSCREEN_DEFAULT
    ).toBool();
    if (m_settings.visibility == OSCVisibility::Visible)
    {
        showOverlay();
    }
#endif
    settings.endGroup();

    settings.beginGroup(SETTINGS_BEHAVIOR);
    m_hideTimer.setSingleShot(true);
    m_hideTimer.setInterval(
        settings.value(
            SETTINGS_BEHAVIOR_OSC_DURATION,
            SETTINGS_BEHAVIOR_OSC_DURATION_DEFAULT
        ).toInt()
    );
    m_settings.fadeDuration = settings.value(
        SETTINGS_BEHAVIOR_OSC_FADE,
        SETTINGS_BEHAVIOR_OSC_FADE_DEFAULT
    ).toInt();
    m_settings.cursorMinMove = settings.value(
        SETTINGS_BEHAVIOR_OSC_MIN_MOVE,
        SETTINGS_BEHAVIOR_OSC_MIN_MOVE_DEFAULT
    ).toInt();
    settings.endGroup();

    repositionSubtitles();
}

/* End Initializers */
/* Begin Handlers */

void PlayerOverlay::cycleOSCVisibility()
{
    m_definition->hide();

    QString msg = "OSC Visibility: ";
    switch (m_settings.visibility)
    {
    case OSCVisibility::Auto:
        m_settings.visibility = OSCVisibility::Visible;
        msg += "visible";
        showOverlay();
        break;
    case OSCVisibility::Visible:
        m_settings.visibility = OSCVisibility::Hidden;
        msg += "hidden";
        hideOverlay();
        break;
    case OSCVisibility::Hidden:
        m_settings.visibility = OSCVisibility::Auto;
        msg += "auto";
        hideOverlay();
        break;
    }
    GlobalMediator::getGlobalMediator()->getPlayerAdapter()->showText(msg);
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
    const QPoint mousePos = m_player->mapFromGlobal(QCursor::pos());

    int x = mousePos.x() - (m_definition->width() / 2);
    if (x < 0)
    {
        x = 0;
    }
    else if (x > m_player->width() - m_definition->width())
    {
        x = m_player->width() - m_definition->width();
    }

    int y = m_subtitle->pos().y() - m_definition->height();
    if (y < 0)
    {
        y = m_subtitle->pos().y() + m_subtitle->height();
    }

    if (y + m_definition->height() > m_player->height())
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
    int height = m_player->height() * m_settings.subOffset;
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
    settings.beginGroup(SETTINGS_INTERFACE);
    double scale = settings.value(
        SETTINGS_INTERFACE_SUB_SCALE,
        SETTINGS_INTERFACE_SUB_SCALE_DEFAULT
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
    settings.setValue(SETTINGS_INTERFACE_SUB_SCALE, scale);
    Q_EMIT GlobalMediator::getGlobalMediator()->interfaceSettingsChanged();
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
    settings.beginGroup(SETTINGS_INTERFACE);
    double offset = settings.value(
        SETTINGS_INTERFACE_SUB_OFFSET,
        SETTINGS_INTERFACE_SUB_OFFSET_DEFAULT
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
    settings.setValue(SETTINGS_INTERFACE_SUB_OFFSET, offset);
    Q_EMIT GlobalMediator::getGlobalMediator()->interfaceSettingsChanged();
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
    return m_player->childrenRegion().contains(
        m_player->mapFromGlobal(QCursor::pos())
    );
}

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

    if (underMouse() || m_menu->menuOpen() || m_definition->isVisible())
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
