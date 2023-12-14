#pragma once

#include "object.h"
#include "token.h"

#include <sds.h>

typedef struct EnvironmentEntry
{
    sds key;
    Object *value;
} EnvironmentEntry;

typedef struct Environment
{
    EnvironmentEntry *values;
} Environment;

typedef struct GetResult
{
    Token token;
    sds err_msg;
    Object *value;
} GetResult;

Environment new_environment(void);
void environment_define(Environment *environment, sds name, Object *value);
GetResult environment_get(Environment *environment, Token name);
