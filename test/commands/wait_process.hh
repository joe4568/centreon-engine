/*
** Copyright 2011-2012 Merethis
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

#ifndef TEST_COMMANDS_WAIT_PROCESS_HH
#  define TEST_COMMANDS_WAIT_PROCESS_HH

#  include <cassert>
#  include <cstdlib>
#  include "com/centreon/concurrency/condvar.hh"
#  include "com/centreon/concurrency/locker.hh"
#  include "com/centreon/concurrency/mutex.hh"
#  include "com/centreon/engine/commands/command.hh"
#  include "com/centreon/engine/commands/command_listener.hh"
#  include "com/centreon/engine/namespace.hh"

CCE_BEGIN()

namespace         commands {
  /**
   *  @class wait_process
   *  @brief Wait the response of the asynchrone command.
   */
  class           wait_process : public command_listener {
  public:
    /**
     *  Constructor.
     *
     *  @param[in] cmd Process.
     */
                  wait_process(command* cmd) : _cmd(cmd) {
      _cmd->set_listener(this);
    }

    /**
     *  Destructor.
     */
                  ~wait_process() throw () {}

    /**
     *  Get command result.
     *
     *  @return Command execution result.
     */
    result const& get_result() const throw () {
      return (_res);
    }

    /**
     *  Wait for process to terminate.
     */
    void          wait() const throw () {
      concurrency::locker lock(&_mtx);
      while (!_res.command_id)
        _condvar.wait(&_mtx);
      return ;
    }

  private:
    /**
     *  Copy constructor.
     *
     *  @param[in] right Object to copy.
     */
                  wait_process(wait_process const& right)
      : command_listener(right) {
      _internal_copy(right);
    }

    /**
     *  Assignment operator.
     *
     *  @param[in] right Object to copy.
     *
     *  @return This object.
     */
    wait_process& operator=(wait_process const& right) {
      _internal_copy(right);
      return (*this);
    }

    /**
     *  Copy internal data members.
     *
     *  @param[in] right Object to copy.
     */
    void          _internal_copy(wait_process const& right) {
      (void)right;
      assert(!"cannot copy class waiting for process termination");
      abort();
      return ;
    }

    /**
     *  Called when process has finished.
     *
     *  @param[in] res Result.
     */
    void          finished(result const& res) throw () {
      concurrency::locker lock(&_mtx);
      _res = res;
      _cmd->set_listener(NULL);
      _condvar.wake_all();
      return ;
    }

    command*      _cmd;
    mutable com::centreon::concurrency::condvar
                  _condvar;
    mutable com::centreon::concurrency::mutex
                  _mtx;
    result        _res;
  };
}

CCE_END()

#endif // !TEST_COMMANDS_WAIT_PROCESS_HH

