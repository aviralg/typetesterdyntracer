#ifndef TYPETESTERDYNTRACER_TYPECHECKER_H
#define TYPETESTERDYNTRACER_TYPECHECKER_H

#include "stdlibs.h"

#include <tastr/ast/ast.hpp>

enum class Typecheck { Undefined, Mismatch, Match, NotAvailable };

std::ostream& operator<<(std::ostream& os, const Typecheck& typecheck);

std::string to_string(const Typecheck& typecheck);

Typecheck satisfies(SEXP value, const tastr::ast::TypeNode& type);

#endif /* TYPETESTERDYNTRACER_TYPECHECKER_H */
