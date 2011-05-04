/*
** Copyright 2011      Merethis
**
** This file is part of Centreon Engine.
**
** Centreon Engine is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Engine is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Engine. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCE_ERROR_HH
# define CCE_ERROR_HH

# include <QString>
# include <exception>
# include <string>

namespace            com {
  namespace          centreon {
    namespace        engine {

      /**
       *  @class error error.hh
       *  @brief Base exception class.
       *
       *  Simple exception class containing an error message and a flag to
       *  determine if the error that generated the exception was either fatal
       *  or not.
       */
      class          error : public std::exception {
      private:
	mutable char _buffer[4096];
	unsigned int _current;
	bool         _fatal;
	template     <typename T>
	void         _insert_with_snprintf(T t, char const* format);

      public:
# ifdef NDEBUG
	             error() throw();
# else
	             error(char const* file, char const* function, int line) throw();
# endif // !NDEBUG
	             error(error const& e) throw ();
	             ~error() throw ();
	error&       operator=(error const& e) throw ();
	error&       operator<<(char c) throw ();
	error&       operator<<(char const* str) throw ();
	error&       operator<<(int i) throw ();
	error&       operator<<(unsigned int u) throw ();
	error&       operator<<(long l) throw ();
	error&       operator<<(long long ll) throw ();
	error&       operator<<(unsigned long long ull) throw ();
	error&       operator<<(std::string const& str) throw ();
	error&       operator<<(QString const& str) throw ();
	bool         is_fatal() const throw ();
	void         set_fatal(bool fatal) throw ();
	char const*  what() const throw ();
      };
    }
  }
}

# ifdef NDEBUG
#  define engine_error() com::centron::engine::error()
# else
#  define engine_error() com::centreon::engine::error(__FILE__, __FUNCTION__, __LINE__)
# endif // !NDEBUG

#endif // !CCE_ERROR_HH