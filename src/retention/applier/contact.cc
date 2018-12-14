/*
** Copyright 2011-2013,2016-2018 Centreon
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

#include "com/centreon/engine/broker.hh"
#include "com/centreon/engine/configuration/applier/state.hh"
#include "com/centreon/engine/globals.hh"
#include "com/centreon/engine/not_found.hh"
#include "com/centreon/engine/retention/applier/contact.hh"
#include "com/centreon/engine/statusdata.hh"
#include "com/centreon/engine/string.hh"

using namespace com::centreon::engine;
using namespace com::centreon::engine::configuration::applier;
using namespace com::centreon::engine::retention;

/**
 *  Update contact list.
 *
 *  @param[in] config The global configuration.
 *  @param[in] lst    The contact list to update.
 */
void applier::contact::apply(
       configuration::state const& config,
       list_contact const& lst) {
  for (list_contact::const_iterator it(lst.begin()), end(lst.end());
       it != end;
       ++it) {
    try {
      engine::contact& cntct(
        *configuration::applier::state::instance().contacts_find(
           (*it)->contact_name()));
      _update(config, **it, cntct);
    }
    catch (...) {
      // Ignore exception for the retention.
    }
  }
}

/**
 *  Update internal contact base on contact retention.
 *
 *  @param[in]      config The global configuration.
 *  @param[in]      state The contact retention state.
 *  @param[in, out] obj   The contact to update.
 */
void applier::contact::_update(
       configuration::state const& config,
       retention::contact const& state,
       engine::contact& obj) {
  if (state.modified_attributes().is_set()) {
    obj.set_modified_attributes(*state.modified_attributes() & ~0L);
    // mask out attributes we don't want to retain.
  }
  if (state.modified_host_attributes().is_set()) {
    obj.set_modified_host_attributes(*state.modified_host_attributes()
                                     & ~config.retained_contact_host_attribute_mask());
    // mask out attributes we don't want to retain.
  }
  if (state.modified_service_attributes().is_set()) {
    obj.set_modified_service_attributes(*state.modified_service_attributes()
                                        & ~config.retained_contact_service_attribute_mask());
    // mask out attributes we don't want to retain.
  }

  if (obj.get_retain_status_information()) {
    if (state.last_host_notification().is_set())
      obj.set_last_host_notification(*state.last_host_notification());
    if (state.last_service_notification().is_set())
      obj.set_last_service_notification(*state.last_service_notification());
  }


  if (obj.get_retain_nonstatus_information()) {
    if (state.host_notification_period().is_set()
        && (obj.get_modified_host_attributes()
            & MODATTR_NOTIFICATION_TIMEPERIOD)) {
      try {
        obj.set_host_notification_period(
              &find_timeperiod(state.host_notification_period()));
      }
      catch (not_found const& e) {
        // Target does not exist, remove modified attribute flag.
        (void)e;
        obj.set_modified_host_attributes(
              obj.get_modified_host_attributes()
              - MODATTR_NOTIFICATION_TIMEPERIOD);
      }
    }
    if (state.service_notification_period().is_set()
        && (obj.get_modified_service_attributes()
            & MODATTR_NOTIFICATION_TIMEPERIOD)) {
      try {
        obj.set_service_notification_period(
              &find_timeperiod(state.service_notification_period()));
      }
      catch (not_found const& e) {
        // Target does not exist, remove modified attribute flag.
        (void)e;
        obj.set_modified_service_attributes(
              obj.get_modified_service_attributes()
              - MODATTR_NOTIFICATION_TIMEPERIOD);
      }
    }
    if (state.host_notifications_enabled().is_set()) {
      if (obj.get_modified_host_attributes() & MODATTR_NOTIFICATIONS_ENABLED)
        obj.set_host_notifications_enabled(*state.host_notifications_enabled());
    }
    if (state.service_notifications_enabled().is_set()) {
      if (obj.get_modified_service_attributes() & MODATTR_NOTIFICATIONS_ENABLED)
        obj.set_service_notifications_enabled(
          *state.service_notifications_enabled());
    }

    if (!state.customvariables().empty()
        && (obj.get_modified_attributes() & MODATTR_CUSTOM_VARIABLE)) {
      for (map_customvar::const_iterator
             it(state.customvariables().begin()),
             end(state.customvariables().end());
           it != end;
           ++it)
        obj.set_customvar(customvar(it->first, it->second));
    }
  }
  // Adjust modified attributes if necessary.
  else
    obj.set_modified_attributes(MODATTR_NONE);

  // Adjust modified attributes if no custom variable has been changed.
  if (obj.get_modified_attributes() & MODATTR_CUSTOM_VARIABLE) {
    bool at_least_one_modified(false);
    for (customvar_set::const_iterator
           it(obj.get_customvars().begin()),
           end(obj.get_customvars().end());
         it != end;
         ++it) {
      if (it->second.get_modified()) {
        at_least_one_modified = true;
        break;
      }
    }
    if (!at_least_one_modified)
      obj.set_modified_attributes(
        obj.get_modified_attributes()
        - MODATTR_CUSTOM_VARIABLE);
  }

  // update contact status.
  broker_contact_status(&obj);
}
