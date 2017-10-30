/*
** Copyright 2011-2013,2017 Centreon
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

#ifndef CCE_OBJECTS_HOSTSMEMBER_HH
#  define CCE_OBJECTS_HOSTSMEMBER_HH

#  include "com/centreon/engine/host.hh"

/* Forward declaration. */
struct hostgroup_struct;

typedef struct               hostsmember_struct {
  char*                      host_name;
  host*                      host_ptr;
  struct hostsmember_struct* next;
}                            hostsmember;

#  ifdef __cplusplus
extern "C" {
#  endif /* C++ */

hostsmember* add_child_link_to_host(
               host* hst,
               host* child_ptr);
hostsmember* add_host_to_hostgroup(
               hostgroup_struct* temp_hostgroup,
               char const* host_name);
hostsmember* add_parent_host_to_host(
               host* hst,
               char const* host_name);

#  ifdef __cplusplus
}

#    include <ostream>

bool          operator==(
                hostsmember const& obj1,
                hostsmember const& obj2) throw ();
bool          operator!=(
                hostsmember const& obj1,
                hostsmember const& obj2) throw ();
std::ostream& operator<<(std::ostream& os, hostsmember const& obj);

#  endif /* C++ */

#endif // !CCE_OBJECTS_HOSTSMEMBER_HH
