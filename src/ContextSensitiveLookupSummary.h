#ifndef TYPETESTERDYNTRACER_CONTEXT_SENSITIVE_LOOKUP_SUMMARY_H
#define TYPETESTERDYNTRACER_CONTEXT_SENSITIVE_LOOKUP_SUMMARY_H

class ContextSensitiveLookupSummary {
  public:
    explicit ContextSensitiveLookupSummary(
        bool local,
        bool argument,
        sexptype_t expression_type,
        sexptype_t value_type,
        const function_id_t& function_id,
        const std::string& function_namespace,
        const std::string& function_names,
        int formal_parameter_position,
        int actual_argument_position,
        const std::string& symbol,
        bool forced)
        : local_(local)
        , argument_(argument)
        , expression_type_(expression_type)
        , value_type_(value_type)
        , function_id_(function_id)
        , function_namespace_(function_namespace)
        , function_names_(function_names)
        , formal_parameter_position_(formal_parameter_position)
        , actual_argument_position_(actual_argument_position)
        , symbol_(symbol)
        , forced_(forced)
        , binding_lookup_count_(1) {
    }

    bool is_local() const {
        return local_;
    }

    bool is_argument() const {
        return argument_;
    }

    sexptype_t get_expression_type() const {
        return expression_type_;
    }

    sexptype_t get_value_type() const {
        return value_type_;
    }
    const function_id_t& get_function_id() const {
        return function_id_;
    }

    const std::string& get_function_namespace() const {
        return function_namespace_;
    }

    const std::string& get_function_names() const {
        return function_names_;
    }

    int get_formal_parameter_position() const {
        return formal_parameter_position_;
    }

    int get_actual_argument_position() const {
        return actual_argument_position_;
    }

    const std::string& get_symbol() const {
        return symbol_;
    }

    bool is_forced() const {
        return forced_;
    }

    int get_binding_lookup_count() const {
        return binding_lookup_count_;
    }

    bool try_to_merge(bool local,
                      bool argument,
                      sexptype_t expression_type,
                      sexptype_t value_type,
                      const function_id_t& function_id,
                      int formal_parameter_position,
                      int actual_argument_position,
                      const std::string& symbol,
                      bool forced) {
        if (is_mergeable_(local,
                          argument,
                          expression_type,
                          value_type,
                          function_id,
                          formal_parameter_position,
                          actual_argument_position,
                          symbol,
                          forced)) {
            binding_lookup_count_++;
            return true;
        }
        return false;
    }

  private:
    bool local_;
    bool argument_;
    sexptype_t expression_type_;
    sexptype_t value_type_;
    const function_id_t function_id_;
    const std::string function_namespace_;
    const std::string function_names_;
    int formal_parameter_position_;
    int actual_argument_position_;
    const std::string symbol_;
    bool forced_;
    int binding_lookup_count_;

    bool is_mergeable_(bool local,
                       bool argument,
                       sexptype_t expression_type,
                       sexptype_t value_type,
                       const function_id_t& function_id,
                       int formal_parameter_position,
                       int actual_argument_position,
                       const std::string& symbol,
                       bool forced) {
        return (is_local() == local && is_argument() == argument &&
                get_expression_type() == expression_type &&
                get_value_type() == value_type &&
                get_function_id() == function_id &&
                get_formal_parameter_position() == formal_parameter_position &&
                get_actual_argument_position() == actual_argument_position &&
                get_symbol() == symbol && is_forced() == forced);
    }
};

#endif /* TYPETESTERDYNTRACER_CONTEXT_SENSITIVE_LOOKUP_SUMMARY_H */
