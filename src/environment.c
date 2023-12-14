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

EnvironmentResult environment_get(Environment *environment, Token name)
{
    EnvironmentEntry *entry = shgetp(environment->values, name.lexeme);
    if (entry != NULL)
    {
        return (EnvironmentResult){.value = entry->value};
    }

    return (EnvironmentResult){
        .token = name,
        .err_msg = sdscatfmt(sdsempty(), "Undefined variable '%s'.", name.lexeme),
    };
}

EnvironmentResult environment_assign(Environment *environment, Token name, Object *value)
{
    EnvironmentEntry *entry = shgetp(environment->values, name.lexeme);
    if (entry != NULL)
    {
        entry->value = value;
        return (EnvironmentResult){.value = value};
    }

    return (EnvironmentResult){
        .token = name,
        .err_msg = sdscatfmt(sdsempty(), "Undefined variable '%s'.", name.lexeme),
    };
}
