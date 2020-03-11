#include "typechecker.h"

using tastr::ast::GroupTypeNode;
using tastr::ast::ListTypeNode;
using tastr::ast::NullableTypeNode;
using tastr::ast::ScalarTypeNode;
using tastr::ast::StructTypeNode;
using tastr::ast::TagTypePairNode;
using tastr::ast::TagTypePairNodeSequenceNode;
using tastr::ast::TypeNode;
using tastr::ast::TypeNodeSequenceNode;
using tastr::ast::UnionTypeNode;
using tastr::ast::VectorTypeNode;

static Typecheck to_typecheck(bool result) {
    return result ? Typecheck::Match : Typecheck::Mismatch;
}

std::ostream& operator<<(std::ostream& os, const Typecheck& typecheck) {
    os << to_string(typecheck);
    return os;
}

std::string to_string(const Typecheck& typecheck) {
    switch (typecheck) {
    case Typecheck::Undefined:
        return "Undefined";
        break;
    case Typecheck::Mismatch:
        return "Mismatch";
        break;
    case Typecheck::Match:
        return "Match";
        break;
    case Typecheck::NotAvailable:
        return "NotAvailable";
        break;
    }
}

Typecheck satisfies_character(const TypeNode& type, int length) {
    if (length == 1 && type.is_character_scalar_type_node()) {
        return Typecheck::Match;
    } else if (type.is_vector_type_node()) {
        const ScalarTypeNode& inner_type =
            static_cast<const VectorTypeNode&>(type).get_scalar_type();
        if (inner_type.is_character_scalar_type_node()) {
            return Typecheck::Match;
        }
    }
    return Typecheck::Mismatch;
}

Typecheck satisfies_logical(const TypeNode& type, int length) {
    if (length == 1 && type.is_logical_scalar_type_node()) {
        return Typecheck::Match;
    } else if (type.is_vector_type_node()) {
        const ScalarTypeNode& inner_type =
            static_cast<const VectorTypeNode&>(type).get_scalar_type();
        if (inner_type.is_logical_scalar_type_node()) {
            return Typecheck::Match;
        }
    }
    return Typecheck::Mismatch;
}

Typecheck satisfies_integer(const TypeNode& type, int length) {
    if (length == 1 && type.is_integer_scalar_type_node()) {
        return Typecheck::Match;
    } else if (type.is_vector_type_node()) {
        const ScalarTypeNode& inner_type =
            static_cast<const VectorTypeNode&>(type).get_scalar_type();
        if (inner_type.is_integer_scalar_type_node()) {
            return Typecheck::Match;
        }
    }
    return Typecheck::Mismatch;
}

Typecheck satisfies_double(const TypeNode& type, int length) {
    if (length == 1 && type.is_double_scalar_type_node()) {
        return Typecheck::Match;
    } else if (type.is_vector_type_node()) {
        const ScalarTypeNode& inner_type =
            static_cast<const VectorTypeNode&>(type).get_scalar_type();
        if (inner_type.is_double_scalar_type_node()) {
            return Typecheck::Match;
        }
    }
    return Typecheck::Mismatch;
}

Typecheck satisfies_complex(const TypeNode& type, int length) {
    if (length == 1 && type.is_complex_scalar_type_node()) {
        return Typecheck::Match;
    } else if (type.is_vector_type_node()) {
        const ScalarTypeNode& inner_type =
            static_cast<const VectorTypeNode&>(type).get_scalar_type();
        if (inner_type.is_complex_scalar_type_node()) {
            return Typecheck::Match;
        }
    }
    return Typecheck::Mismatch;
}

Typecheck satisfies_raw(const TypeNode& type, int length) {
    if (length == 1 && type.is_raw_scalar_type_node()) {
        return Typecheck::Match;
    } else if (type.is_vector_type_node()) {
        const ScalarTypeNode& inner_type =
            static_cast<const VectorTypeNode&>(type).get_scalar_type();
        if (inner_type.is_raw_scalar_type_node()) {
            return Typecheck::Match;
        }
    }
    return Typecheck::Mismatch;
}

Typecheck satisfies_list(SEXP value, const ListTypeNode& type) {
    TypeNodeSequenceNode& element_types(type.get_element_types());
    int element_count = LENGTH(value);
    if (element_types.size() != element_count) {
        return Typecheck::Mismatch;
    }
    for (int i = 0; i < element_count; ++i) {
        Typecheck result =
            satisfies(VECTOR_ELT(value, i), *element_types.at(i).get());

        if (result != Typecheck::Match) {
            return result;
        }
    }
    return Typecheck::Match;
}

Typecheck satisfies_struct(SEXP value, const StructTypeNode& type) {
    TagTypePairNodeSequenceNode& element_types(type.get_element_types());
    int element_count = LENGTH(value);
    SEXP names = getAttrib(value, R_NamesSymbol);
    int names_count = LENGTH(names);

    if (element_types.size() != element_count ||
        element_types.size() != names_count) {
        return Typecheck::Mismatch;
    }

    for (int i = 0; i < element_count; ++i) {
        TagTypePairNode& node(*element_types.at(i).get());
        std::string name = CHAR(STRING_ELT(names, i));
        if (node.get_identifier().get_name() != name) {
            return Typecheck::Mismatch;
        }
        Typecheck result = satisfies(VECTOR_ELT(value, i), node.get_type());

        if (result != Typecheck::Match) {
            return result;
        }
    }
    return Typecheck::Match;
}

Typecheck satisfies(SEXP value, const TypeNode& type) {
    SEXPTYPE sexptype = TYPEOF(value);

    if (type.is_union_type_node()) {
        const UnionTypeNode& union_type =
            static_cast<const UnionTypeNode&>(type);
        Typecheck result = satisfies(value, union_type.get_first_type());
        if (result == Typecheck::Match) {
            return result;
        }
        return satisfies(value, union_type.get_second_type());
    }

    if (type.is_group_type_node()) {
        const GroupTypeNode& group_type =
            static_cast<const GroupTypeNode&>(type);
        return satisfies(value, group_type.get_inner_type());
    }

    if (type.is_nullable_type_node() && sexptype != NILSXP) {
        const NullableTypeNode& nullable_type =
            static_cast<const NullableTypeNode&>(type);
        return satisfies(value, nullable_type.get_inner_type());
    }

    Typecheck result;

    switch (sexptype) {
    case NILSXP: /* null */
        result = to_typecheck(type.is_null_type_node());
        if (result == Typecheck::Match) {
            return result;
        }
        return to_typecheck(type.is_nullable_type_node());
        break;

    case LGLSXP: /* logical */
        return satisfies_logical(type, LENGTH(value));
        break;

    case INTSXP: /* integer */
        return satisfies_integer(type, LENGTH(value));
        break;

    case RAWSXP: /* raw */
        return satisfies_raw(type, LENGTH(value));
        break;

    case REALSXP: /* numeric */ /* double */
        return satisfies_double(type, LENGTH(value));
        break;

    case CPLXSXP: /* complex */
        return satisfies_complex(type, LENGTH(value));
        break;

    case STRSXP: /* character */
        return satisfies_character(type, LENGTH(value));
        break;

    case S4SXP: /* S4 */
        return to_typecheck(type.is_s4_type_node());
        break;

    case SYMSXP: /* symbol */ /* name */
        return to_typecheck(type.is_symbol_type_node());
        break;

    case LISTSXP: /* pairlist */
        return to_typecheck(type.is_pairlist_type_node());
        break;

    case ENVSXP: /* environment */
        return to_typecheck(type.is_environment_type_node());
        break;

    case LANGSXP: /* language */
        return to_typecheck(type.is_language_type_node());
        break;

    case EXPRSXP: /* expression */
        return to_typecheck(type.is_expression_type_node());
        break;

    case VECSXP: /* list */
        if (type.is_list_type_node()) {
            return satisfies_list(value,
                                  static_cast<const ListTypeNode&>(type));
        }
        if (type.is_struct_type_node()) {
            return satisfies_struct(value,
                                    static_cast<const StructTypeNode&>(type));
        }
        return Typecheck::Mismatch;
        break;

    case EXTPTRSXP: /* externalptr */
        return to_typecheck(type.is_external_pointer_type_node());
        break;

    case BCODESXP: /* bytecode */
        return to_typecheck(type.is_bytecode_type_node());
        break;

    case WEAKREFSXP:
        return to_typecheck(type.is_weak_reference_type_node());
        break;

    case DOTSXP:
        return to_typecheck(type.is_vararg_type_node());
        break;

    case CLOSXP: /* closure */ /* TODO */
        return Typecheck::Mismatch;
        break;

    case SPECIALSXP: /* special */ /* TODO */
        return Typecheck::Mismatch;
        break;

    case BUILTINSXP: /* builtin */ /* TODO */
        return Typecheck::Mismatch;
        break;

    case PROMSXP: /* promise */ /* TODO */
        return Typecheck::Mismatch;
        break;

    case ANYSXP: /* any */ /* TODO */
        return Typecheck::Mismatch;
        break;

    case CHARSXP: /* char */ /* TODO */
        return Typecheck::Mismatch;
        break;

    default:
        return to_typecheck(type.is_any_type_node());
        break;
    }
}

// function_id, call_id, parameter_id, expected, actual, match_fail_index,
// reason
