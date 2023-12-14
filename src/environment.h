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

typedef struct EnvironmentResult
{
    Token token;
    sds err_msg;
    Object *value;
} EnvironmentResult;

Environment new_environment(void);
void environment_define(Environment *environment, sds name, Object *value);
EnvironmentResult environment_get(Environment *environment, Token name);
EnvironmentResult environment_assign(Environment *environment, Token name, Object *value);
