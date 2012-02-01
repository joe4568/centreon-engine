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

#include <QCoreApplication>
#include <QDebug>
#include <exception>
#include "error.hh"
#include "test/unittest.hh"
#include "commands/set.hh"
#include "commands/raw.hh"

using namespace com::centreon::engine;
using namespace com::centreon::engine::commands;

/**
 *  Check if the command exist in the set command.
 *
 *  @param[in] name The command name.
 *
 *  @return True if the command exist, false otherwise.
 */
static bool command_exit(QString const& name) {
  try {
    set::instance().get_command(name);
  }
  catch (...) {
    return (false);
  }
  return (true);
}

/**
 *  Check if the set command works.
 */
int main_test() {
  // get instance.
  set& cmd_set = set::instance();

  // add commands.
  raw raw1("raw1", "raw1 argv1 argv2");
  cmd_set.add_command(raw1);

  QSharedPointer<commands::command> pcmd2(raw1.clone());
  cmd_set.add_command(pcmd2);

  QSharedPointer<commands::command> pcmd3(new raw("pcmd3", "pcmd3 argv1 argv2"));
  cmd_set.add_command(pcmd3);

  // get commands.
  if (command_exit("raw1") == false)
    throw (engine_error() << "error: get_command failed, 'raw1' not found.");

  if (command_exit("pcmd3") == false)
    throw (engine_error() << "error: get_command failed, 'pcmd3' not found.");

  if (command_exit("undef") == true)
    throw (engine_error() << "error: get_command failed, 'undef' found.");

  // remove commands.
  cmd_set.remove_command("pcmd3");
  if (command_exit("pcmd3") == true)
    throw (engine_error() << "error: remove_command failed, 'pcmd3' found.");

  cmd_set.remove_command("raw1");
  if (command_exit("raw1") == true)
    throw (engine_error() << "error: remove_command failed, 'raw1' found.");

  cmd_set.remove_command("undef");

  return (0);
}

/**
 *  Init unit test.
 */
int main(int argc, char** argv) {
  QCoreApplication app(argc, argv);
  unittest utest(&main_test);
  QObject::connect(&utest, SIGNAL(finished()), &app, SLOT(quit()));
  utest.start();
  app.exec();
  return (utest.ret());
}