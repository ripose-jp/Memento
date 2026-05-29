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

#include "setting/keybindprofile.h"

KeybindProfile::KeybindProfile(QObject *parent) : QObject(parent)
{

}

KeybindProfile *KeybindProfile::clone(QObject *parent) const
{
    KeybindProfile *copy = new KeybindProfile(parent);
    copy->setName(name());
    copy->setCardAdd(cardAdd());
    copy->setCardNext(cardNext());
    copy->setCardPrevious(cardPrevious());
    copy->setOpenFile(openFile());
    copy->setOpenUrl(openUrl());
    copy->setOscVisibility(oscVisibility());
    copy->setShowSearch(showSearch());
    copy->setShowSubtitleList(showSubtitleList());
    copy->setStartOcr(startOcr());
    copy->setSubtitleAutoPause(subtitleAutoPause());
    copy->setSubtitleDecreaseSize(subtitleDecreaseSize());
    copy->setSubtitleIncreaseSize(subtitleIncreaseSize());
    copy->setSubtitleMoveDown(subtitleMoveDown());
    copy->setSubtitleMoveUp(subtitleMoveUp());
    copy->setSubtitleShow(subtitleShow());
    copy->setSubtitleListCopySelected(subtitleListCopySelected());
    copy->setSubtitleListFind(subtitleListFind());
    copy->setSubtitleListFindNext(subtitleListFindNext());
    copy->setSubtitleListFindPrevious(subtitleListFindPrevious());
    return copy;
}

const QString &KeybindProfile::name() const noexcept
{
    return m_name;
}

void KeybindProfile::setName(const QString &value)
{
    if (m_name == value)
    {
        return;
    }
    m_name = value;
    emit nameChanged(m_name);
}

QString KeybindProfile::cardAdd() const
{
    return m_cardAdd.toString();
}

void KeybindProfile::setCardAdd(const QString &value)
{
    if (setSequence(m_cardAdd, value))
    {
        emit cardAddChanged(m_cardAdd.toString());
    }
}

QString KeybindProfile::cardNext() const
{
    return m_cardNext.toString();
}

void KeybindProfile::setCardNext(const QString &value)
{
    if (setSequence(m_cardNext, value))
    {
        emit cardNextChanged(m_cardNext.toString());
    }
}

QString KeybindProfile::cardPrevious() const
{
    return m_cardPrevious.toString();
}

void KeybindProfile::setCardPrevious(const QString &value)
{
    if (setSequence(m_cardPrevious, value))
    {
        emit cardPreviousChanged(m_cardPrevious.toString());
    }
}

QString KeybindProfile::openFile() const
{
    return m_openFile.toString();
}

void KeybindProfile::setOpenFile(const QString &value)
{
    if (setSequence(m_openFile, value))
    {
        emit openFileChanged(m_openFile.toString());
    }
}

QString KeybindProfile::openUrl() const
{
    return m_openUrl.toString();
}

void KeybindProfile::setOpenUrl(const QString &value)
{
    if (setSequence(m_openUrl, value))
    {
        emit openUrlChanged(m_openUrl.toString());
    }
}

QString KeybindProfile::oscVisibility() const
{
    return m_oscVisibility.toString();
}

void KeybindProfile::setOscVisibility(const QString &value)
{
    if (setSequence(m_oscVisibility, value))
    {
        emit oscVisibilityChanged(m_oscVisibility.toString());
    }
}

QString KeybindProfile::showSearch() const
{
    return m_showSearch.toString();
}

void KeybindProfile::setShowSearch(const QString &value)
{
    if (setSequence(m_showSearch, value))
    {
        emit showSearchChanged(m_showSearch.toString());
    }
}

QString KeybindProfile::showSubtitleList() const
{
    return m_showSubtitleList.toString();
}

void KeybindProfile::setShowSubtitleList(const QString &value)
{
    if (setSequence(m_showSubtitleList, value))
    {
        emit showSubtitleListChanged(m_showSubtitleList.toString());
    }
}

QString KeybindProfile::startOcr() const
{
    return m_startOcr.toString();
}

void KeybindProfile::setStartOcr(const QString &value)
{
    if (setSequence(m_startOcr, value))
    {
        emit startOcrChanged(m_startOcr.toString());
    }
}

QString KeybindProfile::subtitleAutoPause() const
{
    return m_subtitleAutoPause.toString();
}

void KeybindProfile::setSubtitleAutoPause(const QString &value)
{
    if (setSequence(m_subtitleAutoPause, value))
    {
        emit subtitleAutoPauseChanged(m_subtitleAutoPause.toString());
    }
}

QString KeybindProfile::subtitleDecreaseSize() const
{
    return m_subtitleDecreaseSize.toString();
}

void KeybindProfile::setSubtitleDecreaseSize(const QString &value)
{
    if (setSequence(m_subtitleDecreaseSize, value))
    {
        emit subtitleDecreaseSizeChanged(m_subtitleDecreaseSize.toString());
    }
}

QString KeybindProfile::subtitleIncreaseSize() const
{
    return m_subtitleIncreaseSize.toString();
}

void KeybindProfile::setSubtitleIncreaseSize(const QString &value)
{
    if (setSequence(m_subtitleIncreaseSize, value))
    {
        emit subtitleIncreaseSizeChanged(m_subtitleIncreaseSize.toString());
    }
}

QString KeybindProfile::subtitleMoveDown() const
{
    return m_subtitleMoveDown.toString();
}

void KeybindProfile::setSubtitleMoveDown(const QString &value)
{
    if (setSequence(m_subtitleMoveDown, value))
    {
        emit subtitleMoveDownChanged(m_subtitleMoveDown.toString());
    }
}

QString KeybindProfile::subtitleMoveUp() const
{
    return m_subtitleMoveUp.toString();
}

void KeybindProfile::setSubtitleMoveUp(const QString &value)
{
    if (setSequence(m_subtitleMoveUp, value))
    {
        emit subtitleMoveUpChanged(m_subtitleMoveUp.toString());
    }
}

QString KeybindProfile::subtitleShow() const
{
    return m_subtitleShow.toString();
}

void KeybindProfile::setSubtitleShow(const QString &value)
{
    if (setSequence(m_subtitleShow, value))
    {
        emit subtitleShowChanged(m_subtitleShow.toString());
    }
}

QString KeybindProfile::subtitleListCopySelected() const
{
    return m_subtitleListCopySelected.toString();
}

void KeybindProfile::setSubtitleListCopySelected(const QString &value)
{
    if (setSequence(m_subtitleListCopySelected, value))
    {
        emit subtitleListCopySelectedChanged(
            m_subtitleListCopySelected.toString()
        );
    }
}

QString KeybindProfile::subtitleListFind() const
{
    return m_subtitleListFind.toString();
}

void KeybindProfile::setSubtitleListFind(const QString &value)
{
    if (setSequence(m_subtitleListFind, value))
    {
        emit subtitleListFindChanged(m_subtitleListFind.toString());
    }
}

QString KeybindProfile::subtitleListFindNext() const
{
    return m_subtitleListFindNext.toString();
}

void KeybindProfile::setSubtitleListFindNext(const QString &value)
{
    if (setSequence(m_subtitleListFindNext, value))
    {
        emit subtitleListFindNextChanged(m_subtitleListFindNext.toString());
    }
}

QString KeybindProfile::subtitleListFindPrevious() const
{
    return m_subtitleListFindPrevious.toString();
}

void KeybindProfile::setSubtitleListFindPrevious(const QString &value)
{
    if (setSequence(m_subtitleListFindPrevious, value))
    {
        emit subtitleListFindPreviousChanged(
            m_subtitleListFindPrevious.toString()
        );
    }
}

bool KeybindProfile::setSequence(QKeySequence &seq, const QString &value)
{
    QKeySequence newSeq{value};
    if (seq == newSeq)
    {
        return false;
    }
    else if (m_existingBinds.contains(newSeq.toString()))
    {
        return true;
    }
    m_existingBinds.remove(seq.toString());
    seq = std::move(newSeq);
    if (!seq.isEmpty())
    {
        m_existingBinds.insert(seq.toString());
    }
    return true;
}
