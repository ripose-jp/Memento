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

#include <QKeySequence>
#include <QSet>

/**
 * @brief Object holding information about a set of keybinds.
 */
class KeybindProfile : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        QString name
        READ name
        WRITE setName
        NOTIFY nameChanged
    )

    Q_PROPERTY(
        QString cardAdd
        READ cardAdd
        WRITE setCardAdd
        NOTIFY cardAddChanged
    )

    Q_PROPERTY(
        QString cardNext
        READ cardNext
        WRITE setCardNext
        NOTIFY cardNextChanged
    )

    Q_PROPERTY(
        QString cardPrevious
        READ cardPrevious
        WRITE setCardPrevious
        NOTIFY cardPreviousChanged
    )

    Q_PROPERTY(
        QString openFile
        READ openFile
        WRITE setOpenFile
        NOTIFY openFileChanged
    )

    Q_PROPERTY(
        QString openUrl
        READ openUrl
        WRITE setOpenUrl
        NOTIFY openUrlChanged
    )

    Q_PROPERTY(
        QString oscVisibility
        READ oscVisibility
        WRITE setOscVisibility
        NOTIFY oscVisibilityChanged
    )

    Q_PROPERTY(
        QString showSearch
        READ showSearch
        WRITE setShowSearch
        NOTIFY showSearchChanged
    )

    Q_PROPERTY(
        QString showSubtitleList
        READ showSubtitleList
        WRITE setShowSubtitleList
        NOTIFY showSubtitleListChanged
    )

    Q_PROPERTY(
        QString startOcr
        READ startOcr
        WRITE setStartOcr
        NOTIFY startOcrChanged
    )

    Q_PROPERTY(
        QString subtitleAutoPause
        READ subtitleAutoPause
        WRITE setSubtitleAutoPause
        NOTIFY subtitleAutoPauseChanged
    )

    Q_PROPERTY(
        QString subtitleDecreaseSize
        READ subtitleDecreaseSize
        WRITE setSubtitleDecreaseSize
        NOTIFY subtitleDecreaseSizeChanged
    )

    Q_PROPERTY(
        QString subtitleIncreaseSize
        READ subtitleIncreaseSize
        WRITE setSubtitleIncreaseSize
        NOTIFY subtitleIncreaseSizeChanged
    )

    Q_PROPERTY(
        QString subtitleMoveDown
        READ subtitleMoveDown
        WRITE setSubtitleMoveDown
        NOTIFY subtitleMoveDownChanged
    )

    Q_PROPERTY(
        QString subtitleMoveUp
        READ subtitleMoveUp
        WRITE setSubtitleMoveUp
        NOTIFY subtitleMoveUpChanged
    )

    Q_PROPERTY(
        QString subtitleShow
        READ subtitleShow
        WRITE setSubtitleShow
        NOTIFY subtitleShowChanged
    )

    Q_PROPERTY(
        QString subtitleListCopySelected
        READ subtitleListCopySelected
        WRITE setSubtitleListCopySelected
        NOTIFY subtitleListCopySelectedChanged
    )

    Q_PROPERTY(
        QString subtitleListFind
        READ subtitleListFind
        WRITE setSubtitleListFind
        NOTIFY subtitleListFindChanged
    )

    Q_PROPERTY(
        QString subtitleListFindNext
        READ subtitleListFindNext
        WRITE setSubtitleListFindNext
        NOTIFY subtitleListFindNextChanged
    )

    Q_PROPERTY(
        QString subtitleListFindPrevious
        READ subtitleListFindPrevious
        WRITE setSubtitleListFindPrevious
        NOTIFY subtitleListFindPreviousChanged
    )

public:
    KeybindProfile(QObject *parent = nullptr);

    /**
     * @brief Get a deep copy of all properties.
     *
     * @param parent The new parent of the copy.
     * @return A deep copy of this profile with the parent set to parent.
     */
    [[nodiscard]]
    KeybindProfile *clone(QObject *parent = nullptr) const;

    /**
     * @brief Get the name of this clone.
     *
     * @return The name of the profile.
     */
    [[nodiscard]]
    const QString &name() const noexcept;

    /**
     * @brief Set the name of the profile.
     *
     * @param value The new name set to set.
     */
    void setName(const QString &value);

    /**
     * @brief Get the card add key sequence.
     *
     * @return The card add key sequence.
     */
    [[nodiscard]]
    QString cardAdd() const;

    /**
     * @brief Set the card add key sequence.
     *
     * @param value The card add key sequence.
     */
    void setCardAdd(const QString &value);

    /**
     * @brief Get the card next key sequence.
     *
     * @return The card next key sequence.
     */
    [[nodiscard]]
    QString cardNext() const;

    /**
     * @brief Set the card next key sequence.
     *
     * @param value The card next key sequence.
     */
    void setCardNext(const QString &value);

    /**
     * @brief Get the card previous key sequence.
     *
     * @return The card previous key sequence.
     */
    [[nodiscard]]
    QString cardPrevious() const;

    /**
     * @brief Set the card previous key sequence.
     *
     * @param value The card previous key sequence.
     */
    void setCardPrevious(const QString &value);

    /**
     * @brief Get the open file key sequence.
     *
     * @return The open file key sequence.
     */
    [[nodiscard]]
    QString openFile() const;

    /**
     * @brief Set the open file key sequence.
     *
     * @param value The open file key sequence.
     */
    void setOpenFile(const QString &value);

    /**
     * @brief Get the open URL key sequence.
     *
     * @return The open URL key sequence.
     */
    [[nodiscard]]
    QString openUrl() const;

    /**
     * @brief Set the open URL key sequence.
     *
     * @param value The open URL key sequence.
     */
    void setOpenUrl(const QString &value);

    /**
     * @brief Get the OSC visibility key sequence.
     *
     * @return The OSC visibility key sequence.
     */
    [[nodiscard]]
    QString oscVisibility() const;

    /**
     * @brief Set the OSC visibility key sequence.
     *
     * @param value The OSC visibility key sequence.
     */
    void setOscVisibility(const QString &value);

    /**
     * @brief Get the show search key sequence.
     *
     * @return The show search key sequence.
     */
    [[nodiscard]]
    QString showSearch() const;

    /**
     * @brief Set the show search key sequence.
     *
     * @param value The show search key sequence.
     */
    void setShowSearch(const QString &value);

    /**
     * @brief Get the show subtitle list key sequence.
     *
     * @return The show subtitle list key sequence.
     */
    [[nodiscard]]
    QString showSubtitleList() const;

    /**
     * @brief Set the show subtitle list key sequence.
     *
     * @param value The show subtitle list key sequence.
     */
    void setShowSubtitleList(const QString &value);

    /**
     * @brief Get the start OCR key sequence.
     *
     * @return The start OCR key sequence.
     */
    [[nodiscard]]
    QString startOcr() const;

    /**
     * @brief Set the start OCR key sequence.
     *
     * @param value The start OCR key sequence.
     */
    void setStartOcr(const QString &value);

    /**
     * @brief Get the subtile auto pause key sequence.
     *
     * @return The subtitle auto pause key sequence.
     */
    [[nodiscard]]
    QString subtitleAutoPause() const;

    /**
     * @brief Set the subtitle auto pause key sequence.
     *
     * @param value The subtitle auto pause key sequence.
     */
    void setSubtitleAutoPause(const QString &value);

    /**
     * @brief Get the subtile decrease size key sequence.
     *
     * @return The subtitle decrease size key sequence.
     */
    [[nodiscard]]
    QString subtitleDecreaseSize() const;

    /**
     * @brief Set the subtitle auto pause key sequence.
     *
     * @param value The subtitle auto pause key sequence.
     */
    void setSubtitleDecreaseSize(const QString &value);

    /**
     * @brief Get the subtile increase size key sequence.
     *
     * @return The subtitle increase size key sequence.
     */
    [[nodiscard]]
    QString subtitleIncreaseSize() const;

    /**
     * @brief Set the subtitle auto pause key sequence.
     *
     * @param value The subtitle auto pause key sequence.
     */
    void setSubtitleIncreaseSize(const QString &value);

    /**
     * @brief Get the subtile move down key sequence.
     *
     * @return The subtitle move down key sequence.
     */
    [[nodiscard]]
    QString subtitleMoveDown() const;

    /**
     * @brief Set the subtitle move down key sequence.
     *
     * @param value The subtitle move down key sequence.
     */
    void setSubtitleMoveDown(const QString &value);

    /**
     * @brief Get the subtile move up key sequence.
     *
     * @return The subtitle move up key sequence.
     */
    [[nodiscard]]
    QString subtitleMoveUp() const;

    /**
     * @brief Set the subtitle move up key sequence.
     *
     * @param value The subtitle move up key sequence.
     */
    void setSubtitleMoveUp(const QString &value);

    /**
     * @brief Get the subtile show key sequence.
     *
     * @return The subtitle show key sequence.
     */
    [[nodiscard]]
    QString subtitleShow() const;

    /**
     * @brief Set the subtitle show key sequence.
     *
     * @param value The subtitle show key sequence.
     */
    void setSubtitleShow(const QString &value);

    /**
     * @brief Get the subtile list copy selected key sequence.
     *
     * @return The subtile list copy selected key sequence.
     */
    [[nodiscard]]
    QString subtitleListCopySelected() const;

    /**
     * @brief Set the subtile list copy selected key sequence.
     *
     * @param value The subtile list copy selected key sequence.
     */
    void setSubtitleListCopySelected(const QString &value);

    /**
     * @brief Get the subtile list find key sequence.
     *
     * @return The subtile list find key sequence.
     */
    [[nodiscard]]
    QString subtitleListFind() const;

    /**
     * @brief Set the subtile list find key sequence.
     *
     * @param value The subtile list find key sequence.
     */
    void setSubtitleListFind(const QString &value);

    /**
     * @brief Get the subtile list find next key sequence.
     *
     * @return The subtile list find next key sequence.
     */
    [[nodiscard]]
    QString subtitleListFindNext() const;

    /**
     * @brief Set the subtile list find next key sequence.
     *
     * @param value The subtile list find next key sequence.
     */
    void setSubtitleListFindNext(const QString &value);

    /**
     * @brief Get the subtile list find previous key sequence.
     *
     * @return The subtile list find previous key sequence.
     */
    [[nodiscard]]
    QString subtitleListFindPrevious() const;

    /**
     * @brief Set the subtile list find previous key sequence.
     *
     * @param value The subtile list find previous key sequence.
     */
    void setSubtitleListFindPrevious(const QString &value);

signals:
    /**
     * @brief Emitted when the name of the profile is changed.
     *
     * @param value The new name.
     */
    void nameChanged(const QString &value);

    /**
     * @brief Emitted when the card add key sequence is changed.
     *
     * @param value The new card add key sequence.
     */
    void cardAddChanged(const QString &value);

    /**
     * @brief Emitted when the card next key sequence is changed.
     *
     * @param value The new card next key sequence.
     */
    void cardNextChanged(const QString &value);

    /**
     * @brief Emitted when the card previous key sequence is changed.
     *
     * @param value The new card previous key sequence.
     */
    void cardPreviousChanged(const QString &value);

    /**
     * @brief Emitted when the open file key sequence is changed.
     *
     * @param value The new open file key sequence.
     */
    void openFileChanged(const QString &value);

    /**
     * @brief Emitted when the open URL key sequence is changed.
     *
     * @param value The new open URL key sequence.
     */
    void openUrlChanged(const QString &value);

    /**
     * @brief Emitted when the OSC visibility key sequence is changed.
     *
     * @param value The new OSC visibility key sequence.
     */
    void oscVisibilityChanged(const QString &value);

    /**
     * @brief Emitted when the show search key sequence is changed.
     *
     * @param value The new show search key sequence.
     */
    void showSearchChanged(const QString &value);

    /**
     * @brief Emitted when the show subtitle list key sequence is changed.
     *
     * @param value The new show subtitle list key sequence.
     */
    void showSubtitleListChanged(const QString &value);

    /**
     * @brief Emitted when the start OCR key sequence is changed.
     *
     * @param value The new start OCR key sequence.
     */
    void startOcrChanged(const QString &value);

    /**
     * @brief Emitted when the subtitle auto pause key sequence is changed.
     *
     * @param value The new subtitle auto pause key sequence.
     */
    void subtitleAutoPauseChanged(const QString &value);

    /**
     * @brief Emitted when the subtitle decrease size key sequence is changed.
     *
     * @param value The new subtile decrease size key sequence.
     */
    void subtitleDecreaseSizeChanged(const QString &value);

    /**
     * @brief Emitted when the subtitle increase size key sequence is changed.
     *
     * @param value The new subtile increase size key sequence.
     */
    void subtitleIncreaseSizeChanged(const QString &value);

    /**
     * @brief Emitted when the subtitle move down key sequence is changed.
     *
     * @param value The new subtile move down key sequence.
     */
    void subtitleMoveDownChanged(const QString &value);

    /**
     * @brief Emitted when the subtitle move up key sequence is changed.
     *
     * @param value The new subtile move up key sequence.
     */
    void subtitleMoveUpChanged(const QString &value);

    /**
     * @brief Emitted when the subtitle show key sequence is changed.
     *
     * @param value The new subtile show key sequence.
     */
    void subtitleShowChanged(const QString &value);

    /**
     * @brief Emitted when the subtitle list copy selected key sequence is
     * changed.
     *
     * @param value The new subtile list copy selected key sequence.
     */
    void subtitleListCopySelectedChanged(const QString &value);

    /**
     * @brief Emitted when the subtitle list find key sequence is changed.
     *
     * @param value The new subtile list find key sequence.
     */
    void subtitleListFindChanged(const QString &value);

    /**
     * @brief Emitted when the subtitle list find next key sequence is changed.
     *
     * @param value The new subtile list find next key sequence.
     */
    void subtitleListFindNextChanged(const QString &value);

    /**
     * @brief Emitted when the subtitle list find previous key sequence is
     * changed.
     *
     * @param value The new subtile list find previous key sequence.
     */
    void subtitleListFindPreviousChanged(const QString &value);

private:
    /**
     * @brief Set the sequence to the given value if valid.
     *
     * @param[out] seq the sequence to update.
     * @param value The key sequence to set.
     * @return true to emit a changed signal,
     * @return false otherwise.
     */
    bool setSequence(QKeySequence &seq, const QString &value);

    /* Name of the profile */
    QString m_name{tr("Default")};

    /* Add the current card */
    QKeySequence m_cardAdd{Qt::KeyboardModifier::ControlModifier | Qt::Key_D};

    /* Skip to the next card */
    QKeySequence m_cardNext{
        Qt::KeyboardModifier::ControlModifier |
#if defined(Q_OS_MACOS)
        Qt::KeyboardModifier::KeypadModifier |
#endif // defined(Q_OS_MACOS)
        Qt::Key_Down
    };

    /* Skip to the previous card */
    QKeySequence m_cardPrevious{
        Qt::KeyboardModifier::ControlModifier |
#if defined(Q_OS_MACOS)
        Qt::KeyboardModifier::KeypadModifier |
#endif // defined(Q_OS_MACOS)
        Qt::Key_Up
    };

    /* Show the open file menu */
    QKeySequence m_openFile{QKeySequence::Open};

    /* Show the open URL menu */
    QKeySequence m_openUrl{Qt::KeyboardModifier::ControlModifier | Qt::Key_U};

    /* Cycles OSC visibility */
    QKeySequence m_oscVisibility{Qt::Key_Delete};

    /* Show auxilary search */
    QKeySequence m_showSearch{
        Qt::KeyboardModifier::ControlModifier | Qt::Key_R
    };

    /* Show subtitle list */
    QKeySequence m_showSubtitleList{
        Qt::KeyboardModifier::ControlModifier | Qt::Key_E
    };

    /* Start OCR */
    QKeySequence m_startOcr{
#ifdef MEMENTO_OCR_SUPPORT
        Qt::KeyboardModifier::ControlModifier | Qt::Key_L
#endif // MEMENTO_OCR_SUPPORT
    };

    /* Toggle subtitle auto pause */
    QKeySequence m_subtitleAutoPause{
        Qt::KeyboardModifier::ControlModifier | Qt::Key_P
    };

    /* Decrease subtitle size */
    QKeySequence m_subtitleDecreaseSize{
        Qt::KeyboardModifier::ControlModifier | Qt::Key_1
    };

    /* Increase subtitle size */
    QKeySequence m_subtitleIncreaseSize{
        Qt::KeyboardModifier::ControlModifier | Qt::Key_2
    };

    /* Decrease subtitle size */
    QKeySequence m_subtitleMoveDown{
        Qt::KeyboardModifier::AltModifier | Qt::Key_2
    };

    /* Move subtitles up */
    QKeySequence m_subtitleMoveUp{
        Qt::KeyboardModifier::AltModifier  | Qt::Key_1
    };

    /* Move subtitles down */
    QKeySequence m_subtitleShow{
        Qt::KeyboardModifier::ControlModifier | Qt::Key_0
    };

    /* Copy the selected subtitle list content */
    QKeySequence m_subtitleListCopySelected{
        Qt::KeyboardModifier::ControlModifier |
        Qt::KeyboardModifier::ShiftModifier |
        Qt::Key_C
    };

    /* Open the subtitle list search  */
    QKeySequence m_subtitleListFind{QKeySequence::Find};

    /* Find the next instance in the subtitle list search */
    QKeySequence m_subtitleListFindNext{QKeySequence::FindNext};

    /* Find the previous instance in the subtitle list search */
    QKeySequence m_subtitleListFindPrevious{QKeySequence::FindPrevious};

    /* The set of existing key binds */
    QSet<QString> m_existingBinds = {
        "Ctrl+,",
        m_cardAdd.toString(),
        m_cardNext.toString(),
        m_cardPrevious.toString(),
        m_openFile.toString(),
        m_openUrl.toString(),
        m_oscVisibility.toString(),
        m_showSearch.toString(),
        m_showSubtitleList.toString(),
#ifdef MEMENTO_OCR_SUPPORT
        m_startOcr.toString(),
#endif // MEMENTO_OCR_SUPPORT
        m_subtitleAutoPause.toString(),
        m_subtitleDecreaseSize.toString(),
        m_subtitleIncreaseSize.toString(),
        m_subtitleMoveDown.toString(),
        m_subtitleMoveUp.toString(),
        m_subtitleShow.toString(),
        m_subtitleListCopySelected.toString(),
        m_subtitleListFind.toString(),
        m_subtitleListFindNext.toString(),
        m_subtitleListFindPrevious.toString(),
    };
};
