#include "interpreter.h"

#include "expr.h"
#include "main.h"
#include "object.h"
#include "println.h"
#include "stmt.h"
#include "token_type.h"

static bool is_truthy(Object *obj)
{
    if (obj == NULL)
    {
        return false;
    }
    if (obj->type == OBJECT_BOOLEAN)
    {
        return ((BooleanObject *)obj)->value;
    }
    return true;
}

static bool is_equal(Object *a, Object *b)
{
    if (a == NULL && b == NULL)
    {
        return true;
    }
    if (a == NULL || b == NULL)
    {
        return false;
    }
    if (a->type != b->type)
    {
        return false;
    }
    switch (a->type)
    {
    case OBJECT_NUMBER:
        return ((NumberObject *)a)->value == ((NumberObject *)b)->value;
    case OBJECT_STRING:
        return sdscmp(((StringObject *)a)->value, ((StringObject *)b)->value) == 0;
    case OBJECT_BOOLEAN:
        return ((BooleanObject *)a)->value == ((BooleanObject *)b)->value;
    default:
        __builtin_unreachable();
    }
}

typedef struct RuntimeResult
{
    Object *value;
    bool was_error;
    Token token;
    sds message;
} RuntimeResult;

#define ok(call)                                                                                                       \
    ({                                                                                                                 \
        RuntimeResult *result = malloc(sizeof(RuntimeResult));                                                         \
        result->value = call;                                                                                          \
        result->was_error = false;                                                                                     \
        result;                                                                                                        \
    })

static RuntimeResult *check_number_operand(Token op, Object *operand)
{
    if (operand->type == OBJECT_NUMBER)
    {
        return NULL;
    }

    RuntimeResult *result = malloc(sizeof(RuntimeResult));
    result->was_error = true;
    result->token = op;
    result->message = sdscat(sdsempty(), "Operand must be a number.");
    return result;
}

#define check_number_operand(op, operand)                                                                              \
    ({                                                                                                                 \
        RuntimeResult *result = check_number_operand(op, operand);                                                     \
        if (result != NULL)                                                                                            \
        {                                                                                                              \
            return result;                                                                                             \
        }                                                                                                              \
    })

static RuntimeResult *check_number_operands(Token op, Object *left, Object *right)
{
    if (left->type == OBJECT_NUMBER && right->type == OBJECT_NUMBER)
    {
        return NULL;
    }

    RuntimeResult *result = malloc(sizeof(RuntimeResult));
    result->was_error = true;
    result->token = op;
    result->message = sdscat(sdsempty(), "Operands must be numbers.");
    return result;
}

#define check_number_operands(op, left, right)                                                                         \
    ({                                                                                                                 \
        RuntimeResult *result = check_number_operands(op, left, right);                                                \
        if (result != NULL)                                                                                            \
        {                                                                                                              \
            return result;                                                                                             \
        }                                                                                                              \
    })

static RuntimeResult *evaluate(Interpreter *interpreter, Expr *expr)
{
    return expr_accept(expr, &interpreter->expr, interpreter);
}

static RuntimeResult *execute(Interpreter *interpreter, Stmt *stmt)
{
    return stmt_accept(stmt, &interpreter->stmt, interpreter);
}

#define evaluate(interpreter, expr)                                                                                    \
    ({                                                                                                                 \
        RuntimeResult *result = evaluate(interpreter, expr);                                                           \
        if (result->was_error)                                                                                         \
        {                                                                                                              \
            return result;                                                                                             \
        }                                                                                                              \
        result->value;                                                                                                 \
    })

static void *visit_binary_expr(ExprVisitor *visitor, void *data, BinaryExpr *expr)
{
    Interpreter *interpreter = data;
    Object *left = evaluate(interpreter, expr->left);
    Object *right = evaluate(interpreter, expr->right);

    switch (expr->operator.type)
    {
    case TOKEN_BANG_EQUAL:
        return ok(new_boolean_object(!is_equal(left, right)));
    case TOKEN_EQUAL_EQUAL:
        return ok(new_boolean_object(is_equal(left, right)));
    case TOKEN_GREATER:
        check_number_operands(expr->operator, left, right);
        return ok(new_boolean_object(((NumberObject *)left)->value > ((NumberObject *)right)->value));
    case TOKEN_GREATER_EQUAL:
        check_number_operands(expr->operator, left, right);
        return ok(new_boolean_object(((NumberObject *)left)->value >= ((NumberObject *)right)->value));
    case TOKEN_LESS:
        check_number_operands(expr->operator, left, right);
        return ok(new_boolean_object(((NumberObject *)left)->value < ((NumberObject *)right)->value));
    case TOKEN_LESS_EQUAL:
        check_number_operands(expr->operator, left, right);
        return ok(new_boolean_object(((NumberObject *)left)->value <= ((NumberObject *)right)->value));
    case TOKEN_MINUS:
        check_number_operands(expr->operator, left, right);
        return ok(new_number_object(((NumberObject *)left)->value - ((NumberObject *)right)->value));
    case TOKEN_SLASH:
        check_number_operands(expr->operator, left, right);
        return ok(new_number_object(((NumberObject *)left)->value / ((NumberObject *)right)->value));
    case TOKEN_STAR:
        check_number_operands(expr->operator, left, right);
        return ok(new_number_object(((NumberObject *)left)->value * ((NumberObject *)right)->value));
    case TOKEN_PLUS:
        if (left->type == OBJECT_NUMBER && right->type == OBJECT_NUMBER)
        {
            return ok(new_number_object(((NumberObject *)left)->value + ((NumberObject *)right)->value));
        }
        if (left->type == OBJECT_STRING && right->type == OBJECT_STRING)
        {
            return ok(new_string_object(sdscat(((StringObject *)left)->value, ((StringObject *)right)->value)));
        }

        {
            RuntimeResult *result = malloc(sizeof(RuntimeResult));
            result->was_error = true;
            result->token = expr->operator;
            result->message = sdscat(sdsempty(), "Operands must be two numbers or two strings.");
            return result;
        }
    default:
        __builtin_unreachable();
    }
}

static void *visit_grouping_expr(ExprVisitor *visitor, void *data, GroupingExpr *expr)
{
    Interpreter *interpreter = data;
    return evaluate(interpreter, expr->expression);
}

static void *visit_literal_expr(ExprVisitor *visitor, void *data, LiteralExpr *expr)
{
    return ok(expr->value);
}

static void *visit_unary_expr(ExprVisitor *visitor, void *data, UnaryExpr *expr)
{
    Interpreter *interpreter = data;
    Object *right = evaluate(interpreter, expr->right);

    switch (expr->operator.type)
    {
    case TOKEN_BANG:
        return new_boolean_object(!is_truthy(right));
    case TOKEN_MINUS:
        check_number_operand(expr->operator, right);
        return new_number_object(-((NumberObject *)right)->value);
    default:
        __builtin_unreachable();
    }
}

static void *visit_variable_expr(ExprVisitor *visitor, void *data, VariableExpr *expr)
{
    Interpreter *interpreter = data;
    GetResult gotten = environment_get(&interpreter->environment, expr->name);
    if (gotten.err_msg != NULL)
    {
        RuntimeResult *result = malloc(sizeof(RuntimeResult));
        result->was_error = true;
        result->token = gotten.token;
        result->message = gotten.err_msg;
        return result;
    }
    return ok(gotten.value);
}

static void *visit_print_stmt(StmtVisitor *visitor, void *data, PrintStmt *stmt)
{
    Interpreter *interpreter = data;
    Object *value = evaluate(interpreter, stmt->expression);
    print_object(value, stdout);
    println();
    return ok(NULL);
}

static void *visit_expression_stmt(StmtVisitor *visitor, void *data, ExpressionStmt *stmt)
{
    Interpreter *interpreter = data;
    evaluate(interpreter, stmt->expression);
    return ok(NULL);
}

static void *visit_var_stmt(StmtVisitor *visitor, void *data, VarStmt *stmt)
{
    Interpreter *interpreter = data;
    Object *value = NULL;
    if (stmt->initializer != NULL)
    {
        value = evaluate(interpreter, stmt->initializer);
    }
    environment_define(&interpreter->environment, stmt->name.lexeme, value);
    return ok(NULL);
}

Interpreter new_interpreter(void)
{
    Interpreter interpreter = {
        .expr =
            {
                .visit_binary_expr = visit_binary_expr,
                .visit_grouping_expr = visit_grouping_expr,
                .visit_literal_expr = visit_literal_expr,
                .visit_unary_expr = visit_unary_expr,
                .visit_variable_expr = visit_variable_expr,
            },
        .stmt =
            {
                .visit_print_stmt = visit_print_stmt,
                .visit_expression_stmt = visit_expression_stmt,
                .visit_var_stmt = visit_var_stmt,
            },
        .init = true,
        .environment = new_environment(),
    };
    return interpreter;
}

void interpret(Interpreter *interpreter, Stmt **statements)
{
    for (size_t i = 0; i < arrlen(statements); ++i)
    {
        RuntimeResult *result = execute(interpreter, statements[i]);
        if (result->was_error)
        {
            runtime_error(result->token, result->message);
            break;
        }
    }
}
