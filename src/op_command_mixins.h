#ifndef OP_COMMAND_MIXINS_H
#define OP_COMMAND_MIXINS_H

#include "op_command.h"

class NormalExitStatusMixin : virtual public OpCommand {
    bool is_succesful();
};

#endif // OP_COMMAND_MIXINS
