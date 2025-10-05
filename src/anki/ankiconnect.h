////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Ripose
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

namespace AnkiConnect
{
    namespace Req
    {
        constexpr const char *ACTION = "action";
        constexpr const char *RESULT = "result";
        constexpr const char *PARAMS = "params";
    }

    namespace Reply
    {
        constexpr const char *ERROR = "error";
        constexpr const char *VERSION = "version";
    }

    namespace Action
    {
        constexpr const char *DECK_NAMES = "deckNames";
        constexpr const char *MODEL_NAMES = "modelNames";
        constexpr const char *FIELD_NAMES = "modelFieldNames";
        constexpr const char *VERSION = "version";
        constexpr const char *CAN_ADD_NOTES = "canAddNotes";
        constexpr const char *ADD_NOTE = "addNote";
        constexpr const char *GUI_BROWSE = "guiBrowse";
        constexpr const char *STORE_MEDIA_FILE = "storeMediaFile";
        constexpr const char *MULTI = "multi";
    }

    namespace Param
    {
        constexpr const char *MODEL_NAME = "modelName";
        constexpr const char *QUERY = "query";
        constexpr const char *ADD_NOTE = "note";
        constexpr const char *ACTIONS = "actions";
    }

    namespace Note
    {
        constexpr const char *CAN_ADD_NOTES_PARAM = "notes";

        constexpr const char *DECK = "deckName";
        constexpr const char *MODEL = "modelName";
        constexpr const char *FIELDS = "fields";
        constexpr const char *OPTIONS = "options";
        constexpr const char *TAGS = "tags";
        constexpr const char *AUDIO = "audio";
        constexpr const char *VIDEO = "video";
        constexpr const char *PICTURE = "picture";
        constexpr const char *DATA = "data";
        constexpr const char *URL = "url";
        constexpr const char *PATH = "path";
        constexpr const char *FILENAME = "filename";
        constexpr const char *SKIPHASH = "skipHash";

        namespace Option
        {

            constexpr const char *ALLOW_DUP = "allowDuplicate";
            constexpr const char *SCOPE = "duplicateScope";
            constexpr const char *SCOPE_CHECK_DECK = "deck";

        }
    }
}
