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

class DictionarySearch;

/**
 * @brief Describes dictionary metadata.
 */
class DictionaryInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        int64_t id
        READ id
        WRITE setId
        NOTIFY idChanged
    )

    Q_PROPERTY(
        QString name
        READ name
        WRITE setName
        NOTIFY nameChanged
    )

    Q_PROPERTY(
        bool enabled
        READ enabled
        WRITE setEnabled
        NOTIFY enabledChanged
    )

    friend class DictionarySearch;

public:
    DictionaryInfo(QObject *parent = nullptr);
    virtual ~DictionaryInfo();

    /**
     * @brief Creates a deep copy of this object.
     *
     * @param parent The parent of the new object.
     * @return The cloned DictionaryInfo.
     */
    DictionaryInfo *clone(QObject *parent = nullptr) const;

    /**
     * @brief Gets the ID of the dictionary.
     *
     * @return The dictionary id.
     */
    [[nodiscard]]
    int64_t id() const noexcept;

    /**
     * @brief Sets the ID of the dictionary.
     *
     * @param value The new ID of the dictionary.
     */
    void setId(int64_t value);

    /**
     * @brief Gets the name of the dictionary.
     *
     * @return The name of the dictionary.
     */
    [[nodiscard]]
    const QString &name() const noexcept;

    /**
     * @brief Sets the name of the dictionary.
     *
     * @param value The new name of the dictionary.
     */
    void setName(const QString &value);

    /**
     * @brief Gets if the dictionary is enabled.
     *
     * @return true if the dictionary is enabled,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool enabled() const noexcept;

    /**
     * @brief Sets if the dictionary is enabled.
     *
     * @param value true if the dictionary should be enabled, false otherwise.
     */
    void setEnabled(bool value);

signals:
    /**
     * @brief Emitted when the ID of the dictionary changes.
     *
     * @param value The new ID of the dictionary.
     */
    void idChanged(int64_t value);

    /**
     * @brief Emitted when the name of the dictionary changes.
     *
     * @param value The new name of the dictionary.
     */
    void nameChanged(const QString &value);

    /**
     * @brief Emitted when enabled is changed.
     *
     * @param value The new enabled value.
     */
    void enabledChanged(bool value);

protected:
    /* ID of the dictionary */
    int64_t m_id{0};

    /* Name of the dictionary */
    QString m_name;

    /* true if this dictionary is enabled, false otherwise */
    bool m_enabled{false};
};
