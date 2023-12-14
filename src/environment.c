#include "environment.h"

#include <stb_ds.h>

Environment new_environment(void)
{
    Environment environment = {NULL};
    sh_new_arena(environment.values);
    return environment;
}

void environment_define(Environment *environment, sds name, Object *value)
{
    shput(environment->values, name, value);
}

GetResult environment_get(Environment *environment, Token name)
{
    EnvironmentEntry *entry = shgetp(environment->values, name.lexeme);
    if (entry != NULL)
    {
        return (GetResult){.value = entry->value};
    }

    return (GetResult){
        .token = name,
        .err_msg = sdscatfmt(sdsempty(), "Undefined variable '%s'.", name.lexeme),
    };
}
