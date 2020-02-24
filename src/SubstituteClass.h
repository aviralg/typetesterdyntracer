#ifndef TYPETESTERDYNTRACER_SUBSTITUTE_CLASS_H
#define TYPETESTERDYNTRACER_SUBSTITUTE_CLASS_H

#include <string>

enum class SubstituteClass {
    Undefined = 0,
    SameScope,
    StaticScope,
    DynamicScope,
    NewScope
};

inline std::string to_string(const SubstituteClass subst_class) {
    switch (subst_class) {
    case SubstituteClass::Undefined:
        return "Undefined";
    case SubstituteClass::SameScope:
        return "SameScope";
    case SubstituteClass::StaticScope:
        return "StaticScope";
    case SubstituteClass::DynamicScope:
        return "DynamicScope";
    case SubstituteClass::NewScope:
        return "NewScope";
    }

    return "Undefined";
}

#endif /* TYPETESTERDYNTRACER_SUBSTITUTE_CLASS_H */
