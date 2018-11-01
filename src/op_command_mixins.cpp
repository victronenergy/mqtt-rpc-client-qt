#include "op_command_mixins.h"

bool NormalExitStatusMixin::is_succesful() {
    return last_response["exitstatus"] == EXIT_STATUS_NORMAL_EXIT;
};
