#include "Argument.h"

#include "Call.h"
#include "Function.h"

#include <tastr/ast/ast.hpp>

void Argument::typecheck(SEXP value) {
    Function* function = get_call()->get_function();

    if (function->has_invalid_type_declaration()) {
        typecheck_result_ = Typecheck::NotAvailable;
        return;
    }

    const tastr::ast::TypeNode& type(
        function->get_parameter_type(get_formal_parameter_position()));

    if (is_dot_dot_dot() && type.is_vararg_type_node()) {
        typecheck_result_ = Typecheck::Match;
    } else if (type_of_sexp(value) == MISSINGSXP) {
        typecheck_result_ = Typecheck::Undefined;
    } else if (type_of_sexp(value) != PROMSXP) {
        typecheck_result_ = satisfies(value, type);
    } else {
        SEXP expr = dyntrace_get_promise_expression(value);
        SEXP val = dyntrace_get_promise_value(value);
        if (val != R_UnboundValue) {
            /* we call it recursively because val could be a promise */
            typecheck(val);
        } else if (type_of_sexp(expr) != LANGSXP) {
            typecheck_result_ = satisfies(expr, type);
        } else {
            typecheck_result_ = Typecheck::Undefined;
        }
    }
}
