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

bool satisfies_character(const TypeNode& type, int length) {
    if (length == 1 && type.is_character_scalar_type_node()) {
        return true;
    } else if (type.is_vector_type_node()) {
        const ScalarTypeNode& inner_type =
            static_cast<const VectorTypeNode&>(type).get_scalar_type();
        if (inner_type.is_character_scalar_type_node()) {
            return true;
        }
    }
    return false;
}

bool satisfies_logical(const TypeNode& type, int length) {
    if (length == 1 && type.is_logical_scalar_type_node()) {
        return true;
    } else if (type.is_vector_type_node()) {
        const ScalarTypeNode& inner_type =
            static_cast<const VectorTypeNode&>(type).get_scalar_type();
        if (inner_type.is_logical_scalar_type_node()) {
            return true;
        }
    }
    return false;
}

bool satisfies_integer(const TypeNode& type, int length) {
    if (length == 1 && type.is_integer_scalar_type_node()) {
        return true;
    } else if (type.is_vector_type_node()) {
        const ScalarTypeNode& inner_type =
            static_cast<const VectorTypeNode&>(type).get_scalar_type();
        if (inner_type.is_integer_scalar_type_node()) {
            return true;
        }
    }
    return false;
}

bool satisfies_double(const TypeNode& type, int length) {
    if (length == 1 && type.is_double_scalar_type_node()) {
        return true;
    } else if (type.is_vector_type_node()) {
        const ScalarTypeNode& inner_type =
            static_cast<const VectorTypeNode&>(type).get_scalar_type();
        if (inner_type.is_double_scalar_type_node()) {
            return true;
        }
    }
    return false;
}

bool satisfies_complex(const TypeNode& type, int length) {
    if (length == 1 && type.is_complex_scalar_type_node()) {
        return true;
    } else if (type.is_vector_type_node()) {
        const ScalarTypeNode& inner_type =
            static_cast<const VectorTypeNode&>(type).get_scalar_type();
        if (inner_type.is_complex_scalar_type_node()) {
            return true;
        }
    }
    return false;
}

bool satisfies_raw(const TypeNode& type, int length) {
    if (length == 1 && type.is_raw_scalar_type_node()) {
        return true;
    } else if (type.is_vector_type_node()) {
        const ScalarTypeNode& inner_type =
            static_cast<const VectorTypeNode&>(type).get_scalar_type();
        if (inner_type.is_raw_scalar_type_node()) {
            return true;
        }
    }
    return false;
}

bool satisfies_list(SEXP value, const ListTypeNode& type) {
    TypeNodeSequenceNode& element_types(type.get_element_types());
    int element_count = LENGTH(value);
    if (element_types.size() != element_count) {
        return false;
    }
    for (int i = 0; i < element_count; ++i) {
        if (!satisfies(VECTOR_ELT(value, i), *element_types.at(i).get())) {
            return false;
        }
    }
    return true;
}

bool satisfies_struct(SEXP value, const StructTypeNode& type) {
    TagTypePairNodeSequenceNode& element_types(type.get_element_types());
    int element_count = LENGTH(value);
    SEXP names = getAttrib(value, R_NamesSymbol);
    int names_count = LENGTH(names);

    if (element_types.size() != element_count ||
        element_types.size() != names_count) {
        return false;
    }

    for (int i = 0; i < element_count; ++i) {
        TagTypePairNode& node(*element_types.at(i).get());
        std::string name = CHAR(STRING_ELT(names, i));
        if (node.get_identifier().get_name() != name) {
            return false;
        }
        if (!satisfies(VECTOR_ELT(value, i), node.get_type())) {
            return false;
        }
    }
    return true;
}

bool satisfies(SEXP value, const TypeNode& type) {
    SEXPTYPE sexptype = TYPEOF(value);

    if (type.is_union_type_node()) {
        const UnionTypeNode& union_type =
            static_cast<const UnionTypeNode&>(type);
        return satisfies(value, union_type.get_first_type()) ||
               satisfies(value, union_type.get_second_type());
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

    switch (sexptype) {
    case NILSXP: /* null */
        return (type.is_null_type_node() || type.is_nullable_type_node());
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
        return type.is_s4_type_node();
        break;

    case SYMSXP: /* symbol */ /* name */
        return type.is_symbol_type_node();
        break;

    case LISTSXP: /* pairlist */
        return type.is_pairlist_type_node();
        break;

    case ENVSXP: /* environment */
        return type.is_environment_type_node();
        break;

    case LANGSXP: /* language */
        return type.is_language_type_node();
        break;

    case EXPRSXP: /* expression */
        return type.is_expression_type_node();
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
        return false;
        break;

    case EXTPTRSXP: /* externalptr */
        return type.is_external_pointer_type_node();
        break;

    case BCODESXP: /* bytecode */
        return type.is_bytecode_type_node();
        break;

    case WEAKREFSXP:
        return type.is_weak_reference_type_node();
        break;

    case DOTSXP:
        return type.is_vararg_type_node();
        break;

    case CLOSXP: /* closure */ /* TODO */
        return false;
        break;

    case SPECIALSXP: /* special */ /* TODO */
        return false;
        break;

    case BUILTINSXP: /* builtin */ /* TODO */
        return false;
        break;

    case PROMSXP: /* promise */ /* TODO */
        return false;
        break;

    case ANYSXP: /* any */ /* TODO */
        return false;
        break;

    case CHARSXP: /* char */ /* TODO */
        return false;
        break;

    default:
        return type.is_any_type_node();
        break;
    }
}

// function_id, call_id, parameter_id, expected, actual, match_fail_index,
// reason
