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

#include "anki/ankiclient.h"

#include <QClipboard>
#include <QDebug>
#include <QGuiApplication>
#include <QNetworkReply>
#include <QtConcurrent>

#ifdef MEMENTO_SYSTEM_QCORO
#include <QCoroFuture>
#include <QCoroNetworkReply>
#else
#include <qcoro/core/qcorofuture.h>
#include <qcoro/network/qcoronetworkreply.h>
#endif // MEMENTO_SYSTEM_QCORO

#include "anki/ankiconnect.h"
#include "state/context.h"

/* The minimum AnkiConnect version required */
static constexpr int MIN_ANKICONNECT_VERSION = 6;

namespace Key
{

static constexpr const char *SUCCESS = "success";
static constexpr const char *ERROR = "error";
static constexpr const char *RESULT = "result";
static constexpr const char *DECKS = "decks";
static constexpr const char *MODELS = "models";
static constexpr const char *FIELDS = "fields";
static constexpr const char *EXPRESSION = "expression";
static constexpr const char *READING = "reading";
static constexpr const char *CHARACTER = "character";
static constexpr const char *ID = "id";
static constexpr const char *IDS = "ids";

} // namespace Key

namespace Error
{

static constexpr const char *NO_PROFILE_SET = "No Anki profile set";

} // namespace Error

/* Begin Constructor/Destructors */

AnkiClient::AnkiClient(Context *context, QObject *parent) :
    QObject(parent),
    m_context(context),
    m_manager(this),
    m_subtitleFilterRegex(m_context->settings()->searchRemoveRegex())
{
    constexpr int TIMEOUT = 5000;
    m_manager.setTransferTimeout(TIMEOUT);

    connect(
        m_context->settings(), &Settings::searchRemoveRegexChanged,
        this, &AnkiClient::updateSubtitleFilterRegex
    );
}

/* End Constructor/Destructors */
/* Begin Getters */

const AnkiProfile *AnkiClient::profile() const noexcept
{
    return m_context->ankiConfig()->profile();
}

/* End Getters */
/* Begin Slots */

void AnkiClient::updateSubtitleFilterRegex(const QString &filter)
{
    m_subtitleFilterRegex = QRegularExpression(filter);
}

/* End Slots */
/* Begin Commands */

QCoro::QmlTask AnkiClient::testConnection()
{
    return testConnectionAsync();
}

QCoro::Task<QVariantMap> AnkiClient::testConnectionAsync()
{
    QVariantMap result;

    if (profile() == nullptr)
    {
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr(Error::NO_PROFILE_SET);
        co_return result;
    }

    std::unique_ptr<QNetworkReply> reply =
        co_await makeRequest(profile(), AnkiConnect::Action::VERSION);

    QString error;
    QJsonObject replyObj = processReply(*reply, error);
    if (replyObj.isEmpty())
    {
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = std::move(error);
        co_return result;
    }

    if (!replyObj[AnkiConnect::Req::RESULT].isDouble())
    {
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr("AnkiConnect result is not a number");
        co_return result;
    }

    if (replyObj[AnkiConnect::Req::RESULT].toInt() < MIN_ANKICONNECT_VERSION)
    {
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr("AnkiConnect version %1 < %2").arg(
            QString::number(replyObj[AnkiConnect::Req::RESULT].toInt()),
            QString::number(MIN_ANKICONNECT_VERSION)
        );
        co_return result;
    }

    result[Key::SUCCESS] = true;
    co_return result;
}

QCoro::QmlTask AnkiClient::getDeckNames()
{
    return getDeckNamesAsync();
}

QCoro::Task<QVariantMap> AnkiClient::getDeckNamesAsync()
{
    if (profile() == nullptr)
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr(Error::NO_PROFILE_SET);
        co_return result;
    }

    QVariantMap result =
        co_await requestStringList(profile(), AnkiConnect::Action::DECK_NAMES);
    if (!result[Key::SUCCESS].toBool())
    {
        co_return result;
    }

    QVariant decks = result.take(Key::RESULT);
    result[Key::DECKS] = std::move(decks);
    co_return result;
}

QCoro::QmlTask AnkiClient::getModelNames()
{
    return getModelNamesAsync();
}

QCoro::Task<QVariantMap> AnkiClient::getModelNamesAsync()
{
    if (profile() == nullptr)
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr(Error::NO_PROFILE_SET);
        co_return result;
    }

    QVariantMap result =
        co_await requestStringList(profile(), AnkiConnect::Action::MODEL_NAMES);
    if (!result[Key::SUCCESS].toBool())
    {
        co_return result;
    }

    QVariant models = result.take(Key::RESULT);
    result[Key::MODELS] = std::move(models);
    co_return result;
}

QCoro::QmlTask AnkiClient::getFieldNames(const QString &model)
{
    return getFieldNamesAsync(model);
}

QCoro::Task<QVariantMap> AnkiClient::getFieldNamesAsync(const QString &model)
{
    if (profile() == nullptr)
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr(Error::NO_PROFILE_SET);
        co_return result;
    }

    QJsonObject params;
    params[AnkiConnect::Param::MODEL_NAME] = model;
    QVariantMap result = co_await requestStringList(
        profile(), AnkiConnect::Action::FIELD_NAMES, std::move(params)
    );
    if (!result[Key::SUCCESS].toBool())
    {
        co_return result;
    }

    QVariant fields = result.take(Key::RESULT);
    result[Key::FIELDS] = std::move(fields);
    co_return result;
}

QCoro::QmlTask AnkiClient::noteAddable(const Term *term)
{
    return noteAddableAsync(term);
}

QCoro::Task<QVariantMap> AnkiClient::noteAddableAsync(const Term *term)
{
    if (profile() == nullptr)
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr(Error::NO_PROFILE_SET);
        co_return result;
    }

    std::unique_ptr<AnkiProfile> profileCopy{profile()->clone(this)};
    std::unique_ptr<Term> termCopy{term->clone(this)};
    populate(termCopy.get());
    QJsonObject params = co_await QtConcurrent::run(
        [this, &profile = *profileCopy, &term = *termCopy] () -> QJsonObject
        {
            QJsonArray notes;

            term.setReadingAsExpression(false);
            Anki::Note::Context ctx = Anki::Note::build(
                *m_context, profile, term, false
            );
            notes.append(std::move(ctx.ankiObject));

            if (!term.reading().isEmpty())
            {
                term.setReadingAsExpression(true);
                ctx = Anki::Note::build(*m_context, profile, term, false);
                notes.append(std::move(ctx.ankiObject));
            }

            QJsonObject params;
            params[AnkiConnect::Note::CAN_ADD_NOTES_PARAM] = std::move(notes);
            return params;
        }
    );

    QVariantMap canAddResult = co_await requestBoolList(
        profileCopy.get(), AnkiConnect::Action::CAN_ADD_NOTES, std::move(params)
    );
    if (!canAddResult[Key::SUCCESS].toBool())
    {
        co_return canAddResult;
    }

    constexpr qsizetype EXPRESSION_INDEX = 0;
    constexpr qsizetype READING_INDEX = 1;
    QVariantList canAddList = canAddResult[Key::RESULT].toList();

    QVariantMap result;
    result[Key::SUCCESS] = true;
    result[Key::EXPRESSION] = EXPRESSION_INDEX < canAddList.size() &&
        canAddList[EXPRESSION_INDEX].toBool();
    result[Key::READING] = READING_INDEX < canAddList.size() &&
        canAddList[READING_INDEX].toBool();
    co_return result;
}

QCoro::QmlTask AnkiClient::noteAddable(const Kanji *kanji)
{
    return noteAddableAsync(kanji);
}

QCoro::Task<QVariantMap> AnkiClient::noteAddableAsync(const Kanji *kanji)
{
    if (profile() == nullptr)
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr(Error::NO_PROFILE_SET);
        co_return result;
    }

    std::unique_ptr<AnkiProfile> profileCopy{profile()->clone(this)};
    std::unique_ptr<Kanji> kanjiCopy{kanji->clone(this)};
    populate(kanjiCopy.get());
    QJsonObject params = co_await QtConcurrent::run(
        [this, &profile = *profileCopy, &kanji = *kanjiCopy] () -> QJsonObject
        {
            QJsonArray notes;

            Anki::Note::Context ctx =
                Anki::Note::build(*m_context, profile, kanji, false);
            notes.append(std::move(ctx.ankiObject));

            QJsonObject params;
            params[AnkiConnect::Note::CAN_ADD_NOTES_PARAM] = notes;
            return params;
        }
    );

    QVariantMap canAddResult = co_await requestBoolList(
        profileCopy.get(), AnkiConnect::Action::CAN_ADD_NOTES, std::move(params)
    );
    if (!canAddResult[Key::SUCCESS].toBool())
    {
        co_return canAddResult;
    }

    constexpr qsizetype CHARACTER_INDEX = 0;
    QVariantList canAddList = canAddResult[Key::RESULT].toList();

    QVariantMap result;
    result[Key::SUCCESS] = true;
    result[Key::CHARACTER] = CHARACTER_INDEX < canAddList.size() &&
        canAddList[CHARACTER_INDEX].toBool();
    co_return result;
}

QCoro::QmlTask AnkiClient::addNote(const Term *term)
{
    return addNoteAsync(term);
}

QCoro::Task<QVariantMap> AnkiClient::addNoteAsync(const Term *term)
{
    if (profile() == nullptr)
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr(Error::NO_PROFILE_SET);
        co_return result;
    }

    std::unique_ptr<AnkiProfile> profileCopy{profile()->clone(this)};
    std::unique_ptr<Term> termCopy{term->clone(this)};
    populate(termCopy.get());
    Anki::Note::Context ctx = co_await QtConcurrent::run(
        [this, &profile = *profileCopy, &term = *termCopy]
        () -> Anki::Note::Context
        {
            return Anki::Note::build(*m_context, profile, term, true);
        }
    );
    co_return co_await addNoteHelper(profileCopy.get(), std::move(ctx));
}

QCoro::QmlTask AnkiClient::addNote(const Kanji *kanji)
{
    return addNoteAsync(kanji);
}

QCoro::Task<QVariantMap> AnkiClient::addNoteAsync(const Kanji *kanji)
{
    if (profile() == nullptr)
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr(Error::NO_PROFILE_SET);
        co_return result;
    }

    std::unique_ptr<AnkiProfile> profileCopy{profile()->clone(this)};
    std::unique_ptr<Kanji> kanjiCopy{kanji->clone(this)};
    populate(kanjiCopy.get());
    Anki::Note::Context ctx = co_await QtConcurrent::run(
        [this, &profile = *profileCopy, &kanji = *kanjiCopy]
        () -> Anki::Note::Context
        {
            return Anki::Note::build(*m_context, profile, kanji, true);
        }
    );
    co_return co_await addNoteHelper(profileCopy.get(), std::move(ctx));
}

QCoro::Task<QVariantMap> AnkiClient::addNoteHelper(
    const AnkiProfile *profile, Anki::Note::Context &&ctx)
{
    if (!ctx.fileMap.isEmpty())
    {
        QVariantMap addMediaResult =
            co_await addMedia(profile, std::move(ctx.fileMap));
        if (!addMediaResult[Key::SUCCESS].toBool())
        {
            co_return addMediaResult;
        }
    }

    QJsonObject params;
    params[AnkiConnect::Param::ADD_NOTE] = std::move(ctx.ankiObject);
    QVariantMap addNoteResult = co_await requestInt(
        profile, AnkiConnect::Action::ADD_NOTE, std::move(params)
    );
    if (!addNoteResult[Key::SUCCESS].toBool())
    {
        co_return addNoteResult;
    }

    QVariantMap result;
    result[Key::SUCCESS] = true;
    result[Key::ID] = addNoteResult[Key::RESULT].toInt();
    co_return result;
}

QCoro::Task<QVariantMap> AnkiClient::addMedia(
    const AnkiProfile *profile, QList<GlossaryBuilder::FileInfo> fileMap)
{
    QJsonArray actions;
    for (const GlossaryBuilder::FileInfo &info : fileMap)
    {
        QJsonObject command;
        command[AnkiConnect::Req::ACTION] =
            AnkiConnect::Action::STORE_MEDIA_FILE;
        QJsonObject fileParams;
        fileParams[AnkiConnect::Note::DATA] = FileUtils::toBase64(info.path);
        fileParams[AnkiConnect::Note::FILENAME] = info.name;
        command[AnkiConnect::Req::PARAMS] = fileParams;

        actions.append(std::move(command));
    }

    QJsonObject params;
    params[AnkiConnect::Param::ACTIONS] = std::move(actions);
    std::unique_ptr<QNetworkReply> reply = co_await makeRequest(
        profile, AnkiConnect::Action::MULTI, std::move(params)
    );

    QString error;
    QJsonObject multiResult = processReply(*reply, error);
    if (!error.isEmpty())
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = std::move(error);
        co_return result;
    }
    else if (multiResult.isEmpty())
    {
        QVariantMap result;
        result[Key::SUCCESS] = true;
        result[Key::RESULT] = QStringList{};
        co_return result;
    }
    else if (!multiResult[AnkiConnect::Req::RESULT].isArray())
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr("Result is not an array");
        co_return result;
    }

    QStringList filenames;
    QJsonArray actionResults = multiResult[AnkiConnect::Req::RESULT].toArray();
    for (QJsonValueRef actionResult : actionResults)
    {
        if (!actionResult.isString())
        {
            QVariantMap result;
            result[Key::SUCCESS] = false;
            result[Key::ERROR] = tr("Result is not a string");
            co_return result;
        }
        filenames.emplaceBack(actionResult.toString());
    }

    QVariantMap result;
    result[Key::SUCCESS] = true;
    result[Key::RESULT] = std::move(filenames);
    co_return result;
}

QCoro::QmlTask AnkiClient::openBrowse(const QString &deck, const QString &query)
{
    return openBrowseAsync(deck, query);
}

QCoro::Task<QVariantMap> AnkiClient::openBrowseAsync(
    const QString &deck, const QString &query)
{
    if (profile() == nullptr)
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr(Error::NO_PROFILE_SET);
        co_return result;
    }

    QJsonObject params;
    QString queryStr;
    switch (profile()->duplicatePolicy())
    {
        case Anki::DuplicatePolicyDifferentDeck:
        case Anki::DuplicatePolicySameDeck:
            queryStr += "\"deck:" + deck + "\" ";
            [[fallthrough]];

        case Anki::DuplicatePolicyNone:
        default:
            queryStr += '(';
            queryStr += query;
            queryStr += ')';
            break;
    }
    params[AnkiConnect::Param::QUERY] = std::move(queryStr);
    std::unique_ptr<QNetworkReply> reply = co_await makeRequest(
        profile(), AnkiConnect::Action::GUI_BROWSE, params
    );

    QString error;
    QJsonObject replyObj = processReply(*reply, error);
    if (!error.isEmpty())
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = std::move(error);
        co_return result;
    }
    else if (replyObj.isEmpty())
    {
        QVariantMap result;
        result[Key::SUCCESS] = true;
        result[Key::IDS] = QVariantList{};
        co_return result;
    }
    else if (!replyObj[AnkiConnect::Req::RESULT].isArray())
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr("Result is not an array");
        co_return result;
    }

    QVariantList ids;
    QJsonArray resultArray = replyObj[AnkiConnect::Req::RESULT].toArray();
    for (QJsonValueRef addable : resultArray)
    {
        if (!addable.isDouble())
        {
            QVariantMap result;
            result[Key::SUCCESS] = false;
            result[Key::ERROR] = tr("Response was not an array of bool");
            co_return result;
        }
        ids.emplaceBack(addable.toInt());
    }

    QVariantMap result;
    result[Key::SUCCESS] = true;
    result[Key::RESULT] = std::move(ids);
    co_return result;
}

QCoro::QmlTask AnkiClient::openDuplicates(const Term *term)
{
    return openDuplicatesAsync(term);
}

QCoro::Task<QVariantMap> AnkiClient::openDuplicatesAsync(const Term *term)
{
    if (profile() == nullptr)
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr(Error::NO_PROFILE_SET);
        co_return result;
    }

    std::unique_ptr<AnkiProfile> profileCopy{profile()->clone(this)};
    std::unique_ptr<Term> termCopy{term->clone(this)};

    QVariantMap fieldNamesResult =
        co_await getFieldNamesAsync(profileCopy->termModel());
    if (!fieldNamesResult[Key::SUCCESS].toBool())
    {
        co_return fieldNamesResult;
    }

    QStringList fieldNames = fieldNamesResult[Key::FIELDS].toStringList();
    if (fieldNames.isEmpty())
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr("The current model has no fields");
        co_return result;
    }
    QString fieldKey = fieldNames.front();

    termCopy->setReadingAsExpression(false);
    Anki::Note::Context ctx = co_await QtConcurrent::run(
        [this, &profile = *profileCopy, &term = *termCopy]
        () -> Anki::Note::Context
        {
            return Anki::Note::build(*m_context, profile, term, false);
        }
    );
    QString fieldValue =
        ctx.ankiObject[AnkiConnect::Note::FIELDS][fieldKey].toString();

    QString query = QString("%1:\"%2\"")
        .arg(fieldKey)
        .arg(fieldValue.replace('\\', "\\\\").replace('"', "\\\""));

    if (!termCopy->reading().isEmpty())
    {
        termCopy->setReadingAsExpression(true);
        ctx = co_await QtConcurrent::run(
            [this, &profile = *profileCopy, &term = *termCopy]
            () -> Anki::Note::Context
            {
                return Anki::Note::build(*m_context, profile, term, false);
            }
        );
        QString fieldValue =
            ctx.ankiObject[AnkiConnect::Note::FIELDS][fieldKey].toString();

        query += QString(" OR %1:\"%2\"")
            .arg(fieldKey)
            .arg(fieldValue.replace('\\', "\\\\").replace('"', "\\\""));
    }

    co_return co_await openBrowseAsync(profileCopy->termDeck(), query);
}

QCoro::QmlTask AnkiClient::openDuplicates(const Kanji *kanji)
{
    return openDuplicatesAsync(kanji);
}

QCoro::Task<QVariantMap> AnkiClient::openDuplicatesAsync(const Kanji *kanji)
{
    if (profile() == nullptr)
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr(Error::NO_PROFILE_SET);
        co_return result;
    }

    std::unique_ptr<AnkiProfile> profileCopy{profile()->clone(this)};
    std::unique_ptr<Kanji> kanjiCopy{kanji->clone(this)};

    QVariantMap fieldNamesResult =
        co_await getFieldNamesAsync(profileCopy->termModel());
    if (!fieldNamesResult[Key::SUCCESS].toBool())
    {
        co_return fieldNamesResult;
    }

    QStringList fieldNames = fieldNamesResult[Key::FIELDS].toStringList();
    if (fieldNames.isEmpty())
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr("The current model has no fields");
        co_return result;
    }
    QString fieldKey = fieldNames.front();

    Anki::Note::Context ctx = co_await QtConcurrent::run(
        [this, &profile = *profileCopy, &kanji = *kanjiCopy]
        () -> Anki::Note::Context
        {
            return Anki::Note::build(*m_context, profile, kanji, false);
        }
    );
    QString fieldValue =
        ctx.ankiObject[AnkiConnect::Note::FIELDS][fieldKey].toString();

    QString query = QString("%1:\"%2\"")
        .arg(fieldKey)
        .arg(fieldValue.replace('\\', "\\\\").replace('"', "\\\""));

    co_return co_await openBrowseAsync(profileCopy->kanjiDeck(), query);
}

/* End Commands */
/* Begin Network Helpers */

QCoro::Task<QVariantMap> AnkiClient::requestStringList(
    const AnkiProfile *profile,
    const QString &action,
    const QJsonObject &params)
{
    std::unique_ptr<QNetworkReply> reply =
        co_await makeRequest(profile, action, params);

    QString error;
    QJsonObject replyObj = processReply(*reply, error);
    if (!error.isEmpty())
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = std::move(error);
        co_return result;
    }
    else if (replyObj.isEmpty())
    {
        QVariantMap result;
        result[Key::SUCCESS] = true;
        result[Key::RESULT] = QStringList{};
        co_return result;
    }
    else if (!replyObj[AnkiConnect::Req::RESULT].isArray())
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr("Result is not an array");
        co_return result;
    }

    QStringList values;
    QJsonArray resultArray = replyObj[AnkiConnect::Req::RESULT].toArray();
    for (const QJsonValueRef string : resultArray)
    {
        if (!string.isString())
        {
            QVariantMap result;
            result[Key::SUCCESS] = false;
            result[Key::ERROR] = tr("Response was not an array of strings");
        }
        values.emplaceBack(string.toString());
    }

    QVariantMap result;
    result[Key::SUCCESS] = true;
    result[Key::RESULT] = std::move(values);
    co_return result;
}

QCoro::Task<QVariantMap> AnkiClient::requestBoolList(
    const AnkiProfile *profile,
    const QString &action,
    const QJsonObject &params)
{
    std::unique_ptr<QNetworkReply> reply =
        co_await makeRequest(profile, action, params);

    QString error;
    QJsonObject replyObj = processReply(*reply, error);
    if (!error.isEmpty())
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = std::move(error);
        co_return result;
    }
    else if (replyObj.isEmpty())
    {
        QVariantMap result;
        result[Key::SUCCESS] = true;
        result[Key::RESULT] = QVariantList{};
        co_return result;
    }
    else if (!replyObj[AnkiConnect::Req::RESULT].isArray())
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr("Result is not an array");
        co_return result;
    }

    QVariantList values;
    QJsonArray resultArray = replyObj[AnkiConnect::Req::RESULT].toArray();
    for (QJsonValueRef boolean : resultArray)
    {
        if (!boolean.isBool())
        {
            QVariantMap result;
            result[Key::SUCCESS] = false;
            result[Key::ERROR] = tr("Response was not an array of bool");
            co_return result;
        }
        values.emplaceBack(boolean.toBool());
    }

    QVariantMap result;
    result[Key::SUCCESS] = true;
    result[Key::RESULT] = std::move(values);
    co_return result;
}

QCoro::Task<QVariantMap> AnkiClient::requestInt(
    const AnkiProfile *profile,
    const QString &action,
    const QJsonObject &params)
{
    std::unique_ptr<QNetworkReply> reply =
        co_await makeRequest(profile, action, params);

    QString error;
    QJsonObject replyObj = processReply(*reply, error);
    if (!error.isEmpty())
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = std::move(error);
        co_return result;
    }
    else if (replyObj.isEmpty())
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr("AnkiConnect result was an empty value");
        co_return result;
    }

    if (!replyObj[AnkiConnect::Req::RESULT].isDouble())
    {
        QVariantMap result;
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr("AnkiConnect result is not a double");
        co_return result;
    }

    QVariantMap result;
    result[Key::SUCCESS] = true;
    result[Key::RESULT] = replyObj[AnkiConnect::Req::RESULT].toInt();
    co_return result;
}

QCoro::Task<std::unique_ptr<QNetworkReply>> AnkiClient::makeRequest(
    const AnkiProfile *profile,
    const QString &action,
    const QJsonObject &params)
{
    QNetworkRequest request;
    request.setUrl(QUrl(
        QString("http://%1:%2")
            .arg(profile->hostname())
            .arg(profile->port())
    ));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject jsonMsg;
    jsonMsg[AnkiConnect::Req::ACTION] = action;
    jsonMsg[AnkiConnect::Reply::VERSION] = MIN_ANKICONNECT_VERSION;
    if (!params.isEmpty())
    {
        jsonMsg[AnkiConnect::Req::PARAMS] = params;
    }
    QJsonDocument jsonDoc(jsonMsg);

    co_return std::unique_ptr<QNetworkReply>{
        co_await m_manager.post(request, jsonDoc.toJson())
    };
}

QJsonObject AnkiClient::processReply(QNetworkReply &reply, QString &error)
{
    switch (reply.error())
    {
        case QNetworkReply::NoError:
        {
            QJsonDocument replyDoc = QJsonDocument::fromJson(reply.readAll());
            if (replyDoc.isNull())
            {
                error = tr("Reply was not JSON");
                qDebug() << reply.readAll();
                return {};
            }
            else if (!replyDoc.isObject())
            {
                error = tr("Reply was not an object");
                return {};
            }

            QJsonObject replyObj = replyDoc.object();
            if (replyObj.length() != 2)
            {
                error = tr("Anki response has unexpected number of fields");
            }
            else if (!replyObj.contains(AnkiConnect::Reply::ERROR))
            {
                error = tr("Anki response is missing error field");
            }
            else if (!replyObj.contains(AnkiConnect::Req::RESULT))
            {
                error = tr("Anki response is missing result field");
            }
            else if (!replyObj[AnkiConnect::Reply::ERROR].isNull())
            {
                error = replyObj[AnkiConnect::Reply::ERROR].toString();
            }
            else
            {
                return replyObj;
            }
            break;
        }

        default:
            error = reply.errorString();
            break;
    }

    return {};
}

/* End Network Helpers */
/* Begin Expression Helpers */

void AnkiClient::populate(Expression *expression) const
{
    MpvState *state = m_context->player()->state();
    expression->setTitle(state->title());
    QString subtitleText = state->subtitle()->text();
    expression->setSubtitle(subtitleText.remove(m_subtitleFilterRegex));
    expression->setStartTime(
        state->subtitle()->startTime() + state->subtitle()->delay()
    );
    expression->setEndTime(
        state->subtitle()->endTime() + state->subtitle()->delay()
    );
    expression->setSubtitle2(state->secondarySubtitle()->text());

    /* Set {context} fields */
    const SubtitleListModel *primaryList =
        m_context->subtitleLists()->primary();
    if (primaryList != nullptr)
    {
        const std::vector<SubtitleEntry> &primaryListItems =
            primaryList->items();
        QModelIndexList primarySelection =
            primaryList->selectionModel()->selectedIndexes();
        std::ranges::sort(
            primarySelection,
            [] (const QModelIndex &lhs, const QModelIndex &rhs) -> bool
            {
                return lhs.row() < rhs.row();
            }
        );
        QStringList contextList;
        double contextEndTime = 0;
        for (const QModelIndex &index : primarySelection)
        {
            QString text = primaryListItems[index.row()].text;
            text.remove(m_subtitleFilterRegex);
            if (text.isEmpty())
            {
                continue;
            }
            contextList.emplaceBack(std::move(text));
            contextEndTime = std::max(
                contextEndTime, primaryListItems[index.row()].end
            );
        }
        if (!contextList.isEmpty())
        {
            expression->setContextStartTime(
                primaryListItems[primarySelection.front().row()].start +
                    state->subtitle()->delay()
            );
            expression->setContextEndTime(
                contextEndTime + state->subtitle()->delay()
            );
            expression->setContext(contextList.join('\n'));
        }
    }

    /* Set {context-2} */
    const SubtitleListModel *secondaryList =
        m_context->subtitleLists()->secondary();
    if (secondaryList != nullptr)
    {
        const std::vector<SubtitleEntry> &secondaryListItems =
            secondaryList->items();
        QModelIndexList secondarySelection =
            secondaryList->selectionModel()->selectedIndexes();
        std::ranges::sort(
            secondarySelection,
            [] (const QModelIndex &lhs, const QModelIndex &rhs) -> bool
            {
                return lhs.row() < rhs.row();
            }
        );
        QStringList context2List;
        for (const QModelIndex &index : secondarySelection)
        {
            QString text = secondaryListItems[index.row()].text;
            if (text.isEmpty())
            {
                continue;
            }
            context2List.emplaceBack(std::move(text));
        }
        if (!context2List.isEmpty())
        {
            expression->setContext2(context2List.join('\n'));
        }
    }

    expression->setClipboard(QGuiApplication::clipboard()->text());
}

/* End Expression Helpers */
