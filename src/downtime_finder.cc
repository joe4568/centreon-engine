/*
** Copyright 2016 Centreon
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

#include <cstdlib>
#include "com/centreon/engine/downtime_finder.hh"
#include "com/centreon/engine/objects/downtime.hh"

using namespace com::centreon::engine;

// Helper macro.
#define ARE_STRINGS_MATCHING(stdstring, cstring) \
  ((cstring && (cstring == stdstring)) || (!cstring && stdstring.empty()))

/**
 *  Constructor.
 *
 *  @param[in] list  Active downtime list. The search will be performed
 *                   on this list.
 */
downtime_finder::downtime_finder(scheduled_downtime_struct const* list)
  : _list(list) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
downtime_finder::downtime_finder(downtime_finder const& other)
  : _list(other._list) {}

/**
 *  Destructor.
 */
downtime_finder::~downtime_finder() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 */
downtime_finder& downtime_finder::operator=(
                                    downtime_finder const& other) {
  if (this != &other) {
    _list = other._list;
  }
  return (*this);
}

/**
 *  Find downtimes that match all the criterias.
 *
 *  @param[in] criterias  Search criterias.
 */
downtime_finder::result_set downtime_finder::find_matching_all(
  downtime_finder::criteria_set const& criterias) {
  result_set result;
  // Process all downtimes.
  for (scheduled_downtime const* dt(_list); dt; dt = dt->next) {
    // Process all criterias.
    bool matched_all(true);
    for (criteria_set::const_iterator
           it(criterias.begin()), end(criterias.end());
         it != end;
         ++it) {
      if (!this->_match_criteria(dt, *it))
        matched_all = false;
    }

    // If downtime matched all criterias, add it to the result set.
    if (matched_all)
      result.push_back(dt->downtime_id);
  }
  return (result);
}

/**
 *  Check that a downtime match a specific criteria.
 *
 *  @param[in] dt    Downtime.
 *  @param[in] crit  Search criteria.
 *
 *  @return True if downtime matches the criteria.
 */
bool downtime_finder::_match_criteria(
                        scheduled_downtime_struct const* dt,
                        downtime_finder::criteria const& crit) {
  bool retval(false);
  if (crit.first == "host") {
    retval = ARE_STRINGS_MATCHING(crit.second, dt->host_name);
  }
  else if (crit.first == "service") {
    retval = ARE_STRINGS_MATCHING(crit.second, dt->service_description);
  }
  else if (crit.first == "start") {
    time_t expected(strtoll(crit.second.c_str(), NULL, 0));
    retval = (expected == dt->start_time);
  }
  else if (crit.first == "end") {
    time_t expected(strtoll(crit.second.c_str(), NULL, 0));
    retval = (expected == dt->end_time);
  }
  else if (crit.first == "fixed") {
    bool expected(strtol(crit.second.c_str(), NULL, 0));
    retval = (expected == static_cast<bool>(dt->fixed));
  }
  else if (crit.first == "triggered_by") {
    unsigned long expected(strtoul(crit.second.c_str(), NULL, 0));
    retval = (expected == dt->triggered_by);
  }
  else if (crit.first == "duration") {
    unsigned long expected(strtoul(crit.second.c_str(), NULL, 0));
    retval = (expected == dt->duration);
  }
  else if (crit.first == "author") {
    retval = ARE_STRINGS_MATCHING(crit.second, dt->author);
  }
  else if (crit.first == "comment") {
    retval = ARE_STRINGS_MATCHING(crit.second, dt->comment);
  }
  else {
    retval = false;
  }
  return (retval);
}
