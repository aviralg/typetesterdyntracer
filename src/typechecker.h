#ifndef TYPETESTERDYNTRACER_TYPECHECKER_H
#define TYPETESTERDYNTRACER_TYPECHECKER_H

#include "stdlibs.h"

#include <tastr/ast/ast.hpp>

bool satisfies(SEXP value, const tastr::ast::TypeNode& type);

#endif /* TYPETESTERDYNTRACER_TYPECHECKER_H */
