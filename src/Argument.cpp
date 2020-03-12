#include "Argument.h"

#include "Call.h"
#include "Function.h"

#include <tastr/ast/ast.hpp>

Typecheck typecheck_inner(SEXP value, Argument* argument) {
    Typecheck result;

    Function* function = argument->get_call()->get_function();

    if (function->has_invalid_type_declaration()) {
        result = Typecheck::NotAvailable;
        return result;
    }

    const tastr::ast::TypeNode& type(function->get_parameter_type(
        argument->get_formal_parameter_position()));

    if (argument->is_dot_dot_dot() && type.is_vararg_type_node()) {
        result = Typecheck::Match;
    } else if (type_of_sexp(value) == MISSINGSXP) {
        result = Typecheck::Undefined;
    } else if (type_of_sexp(value) != PROMSXP) {
        result = satisfies(value, type);
    }

    return result;
}

void Argument::typecheck(SEXP value) {
    outer_type_ = type_of_sexp(value);

    SEXP inner = value;
    while (type_of_sexp(inner) == PROMSXP) {
        SEXP expr = dyntrace_get_promise_expression(value);
        SEXP val = dyntrace_get_promise_value(value);
        if (val != R_UnboundValue) {
            inner = val;
        } else if (type_of_sexp(expr) != LANGSXP) {
            inner = expr;
        } else {
            inner_type_ = type_of_sexp(expr);
            typecheck_result_ = Typecheck::Undefined;
            return;
        }
    }

    inner_type_ = type_of_sexp(inner);
    typecheck_result_ = typecheck_inner(inner, this);
}
