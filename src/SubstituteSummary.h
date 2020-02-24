#ifndef TYPETESTERDYNTRACER_SUBSTITUTE_SUMMARY_H
#define TYPETESTERDYNTRACER_SUBSTITUTE_SUMMARY_H

#include "Call.h"
#include "SubstituteClass.h"

class SubstituteSummary {
  public:
    explicit SubstituteSummary(const Call* const call,
                               SubstituteClass subst_class);

    const function_id_t& get_function_id() const {
        return function_id_;
    }

    const std::string& get_function_namespace() const {
        return function_namespace_;
    }

    const std::string& get_function_names() const {
        return function_names_;
    }

    SubstituteClass get_substitute_class() const {
        return subst_class_;
    }

    int get_call_count() const {
        return call_count_;
    }

    bool try_to_merge(const Call* const call, SubstituteClass subst_class) {
        if (is_mergeable_(call, subst_class)) {
            call_count_++;
            return true;
        }
        return false;
    }

  private:
    function_id_t function_id_;
    std::string function_namespace_;
    std::string function_names_;
    SubstituteClass subst_class_;
    int call_count_;

    bool is_mergeable_(const Call* const call, SubstituteClass subst_class);
};

#endif /* TYPETESTERDYNTRACER_SUBSTITUTE_SUMMARY_H */
