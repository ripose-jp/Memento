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

#include "quick/keytracker.h"

#include <QGuiApplication>
#include <QKeyEvent>
#include <QKeySequence>

KeyTracker::KeyTracker(QObject *parent) : QObject(parent)
{

}

KeyTracker::~KeyTracker()
{

}

Qt::KeyboardModifiers KeyTracker::modifiers() const
{
    return m_modifiers;
}

void KeyTracker::setModifiers(Qt::KeyboardModifiers value)
{
    if (m_modifiers == value)
    {
        return;
    }
    m_modifiers = value;
    emit modifiersChanged(m_modifiers);
}

bool KeyTracker::modifierHeld(Setting::Modifier key) const
{
    switch (key)
    {
        case Setting::Modifier::ModifierAlt:
            return modifiers() & Qt::KeyboardModifier::AltModifier;
        case Setting::Modifier::ModifierControl:
            return modifiers() & Qt::KeyboardModifier::ControlModifier;
        case Setting::Modifier::ModifierShift:
            return modifiers() & Qt::KeyboardModifier::ShiftModifier;
        case Setting::Modifier::ModifierSuper:
            return modifiers() & Qt::KeyboardModifier::MetaModifier;
    }
    return false;
}

QString KeyTracker::keyComboToString(int key, int modifiers)
{
    return QKeySequence(key | modifiers).toString();
}

bool KeyTracker::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type())
    {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            setModifiers(keyEvent->modifiers());
            break;
        }

        case QEvent::ApplicationDeactivate:
        case QEvent::WindowDeactivate:
        case QEvent::FocusOut:
            setModifiers(Qt::NoModifier);
            break;

        case QEvent::ApplicationActivate:
        case QEvent::WindowActivate:
        case QEvent::FocusIn:
            setModifiers(QGuiApplication::queryKeyboardModifiers());
            break;

        default:
            break;
    }

    return QObject::eventFilter(obj, event);
}
