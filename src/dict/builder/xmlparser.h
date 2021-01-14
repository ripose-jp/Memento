/*
jmdict, a frontend to the JMdict file. http://mandrill.fuxx0r.net/jmdict.php
Copyright (C) 2004 Florian Bluemel (florian.bluemel@uni-dortmund.de)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <expat.h>
#include <istream>
#include <string>
#include <map>

namespace xml {

class Tag {
public:
    Tag(const std::string& name, const char** attrs) : m_name(name) {
        while (*attrs) {
            m_attributes[*attrs] = *(attrs + 1);
            attrs += 2;
        }
    }

    const std::string& name() const {
        return m_name;
    }

    std::string attribute(const std::string& name) const {
        std::map<std::string, std::string>::const_iterator val = m_attributes.find(name);
        if (val == m_attributes.end())
            return "";
        return val->second;
    }

    const std::string& text() const {
        return m_text;
    }

    void append(const std::string& t) {
        m_text += t;
    }

    void append(const char* t, int len) {
        m_text.append(t, len);
    }

private:
    std::string m_name;
    std::string m_text;
    std::map<std::string, std::string> m_attributes;
};

template<class Stack>
class Parser {
public:
    Parser(Stack& stack) : m_stack(stack) {
        m_parser = XML_ParserCreate(0);
        XML_SetUserData(m_parser, this);
        XML_SetElementHandler(m_parser, &Parser::start, &Parser::end);
        XML_SetCharacterDataHandler(m_parser, Parser::chardata);
    }

    ~Parser() {
        XML_ParserFree(m_parser);
    }

    void parse(std::istream& in) {
        const size_t BLOCK_SIZE = 1 << 15;
        while (in) {
            char* buffer = static_cast<char*>(XML_GetBuffer(m_parser, BLOCK_SIZE));
            in.read(buffer, BLOCK_SIZE);
            XML_ParseBuffer(m_parser, static_cast<int>(in.gcount()), in.eof());
        }
    }

private:
    static void start(void* data, const char* e, const char** a) {
        Parser& parser = *static_cast<Parser*>(data);
        parser.m_stack.push(Tag(e, a));
    }

    static void chardata(void* data, const XML_Char* text, int len) {
        Parser& parser = *static_cast<Parser*>(data);
        parser.m_stack.top().append(text, len);
    }
        
    static void end(void* data, const char*) {
        Parser& parser = *static_cast<Parser*>(data);
        parser.m_stack.pop();
    }
        
    XML_Parser m_parser;
    Stack& m_stack;
};

} // namespace xml
