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
#include <QKeySequence>

KeyTracker::KeyTracker(QObject *parent) : QObject(parent)
{

}

KeyTracker::~KeyTracker()
{

}

Qt::KeyboardModifiers KeyTracker::modifiers()
{
    return QGuiApplication::keyboardModifiers();
}

bool KeyTracker::modifierHeld(Setting::Modifier key)
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
