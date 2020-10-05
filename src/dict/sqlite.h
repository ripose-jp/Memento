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
#include <cstring>
#include <string>
#include <cctype>
#include <stdexcept>
#include <sqlite3.h>

namespace sql {
	template<typename T> struct query_traits;
	template<> struct query_traits<const char* const> { static std::string chars() { return "qQs";  } };
	template<> struct query_traits<char* const>       { static std::string chars() { return "qQs";  } };
	template<> struct query_traits<const int>         { static std::string chars() { return "cdi";  } };
	template<> struct query_traits<const unsigned>    { static std::string chars() { return "ouxX"; } };
	typedef int (*callback)(void*, int, char**, char**);

	class query {
		class mem_guard {
		public:
			mem_guard(char* p) : p(p) {}
			~mem_guard() { sqlite3_free(p); }
			operator const char*() const { return p; }
		private:
			char* p;
		};

	public:
		explicit query(const std::string& q = "") : q(q), pos(0) {
			ff();
		}

	query& operator=(const std::string& s) {
            q = s;
            pos = 0;
            ff();
            return *this;
        }

	class wrong_format : public std::logic_error {
	public:
		wrong_format(char type, const std::string& format)
			: std::logic_error(std::string("wrong format string in sql query, expected one of the following: ") 
		                          + format + ", got " + type + ".") {}
	};

	template<typename T> query& operator%(const T& t)
	{
		if(type >= q.size() || query_traits<const T>::chars().find(q[type]) == std::string::npos)
			throw wrong_format(q[type], query_traits<const T>::chars());
		mem_guard repl(sqlite3_mprintf(q.substr(pos, type - pos + 1).c_str(), t));
		put(repl);
		return *this;
	}

        query& operator%(const std::string& s) {
            return *this % s.c_str();
        }

        const std::string& str() const {
            return q;
        }

    private:
        void put(const char* repl) {
            q.replace(pos, type - pos + 1, repl);
            pos += strlen(repl);
            ff();
        }

        void ff() {
            static const std::string CONV("diouxXeEfFgGaAcsCSPnqQ");
            pos = q.find('%', pos);
            while (pos < q.size() - 1 && q[pos + 1] == '%')
                pos = q.find('%', pos + 2);
            type = pos;
            while (type < q.size() && CONV.find(q[type]) == std::string::npos)
                ++type;
        }
        
        std::string q;
        std::string::size_type pos;
        std::string::size_type type;
    };

    struct db {
        explicit db(const std::string& name) {
            if (sqlite3_open(name.c_str(), &raw) != SQLITE_OK)
                throw std::runtime_error("Could not connect to sqlite database '" + name + "'.");
        }
        ~db() {
            sqlite3_close(raw);
        }

        void exec(const std::string& query, callback cb = 0, void* arg = 0) {
            sqlite3_exec(raw, query.c_str(), cb, arg, 0);
        }
        
        void exec(const query& query, callback cb = 0, void* arg = 0) {
            exec(query.str(), cb, arg);
        }
        
    private:
        sqlite3* raw;
    };
} // namespace sql
