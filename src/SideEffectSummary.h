#ifndef TYPETESTERDYNTRACER_SIDE_EFFECT_SUMMARY_H
#define TYPETESTERDYNTRACER_SIDE_EFFECT_SUMMARY_H

#include "SideEffectMode.h"

// TODO - add argument field
class SideEffectSummary {
  public:
    explicit SideEffectSummary(const function_id_t& function_id,
                               const std::string& function_namespace,
                               const std::string& function_names,
                               int formal_parameter_position,
                               int actual_argument_position,
                               const std::string& expression,
                               const std::string& symbol,
                               bool side_effect_creator,
                               SideEffectMode& side_effect_mode,
                               bool direct)
        : function_id_(function_id)
        , function_namespace_(function_namespace)
        , function_names_(function_names)
        , formal_parameter_position_(formal_parameter_position)
        , actual_argument_position_(actual_argument_position)
        , expression_(expression)
        , symbol_(symbol)
        , side_effect_creator_(side_effect_creator)
        , side_effect_mode_(side_effect_mode)
        , direct_(direct)
        , side_effect_count_(1) {
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

    const std::string& get_expression() const {
        return expression_;
    }

    const std::string& get_symbol() const {
        return symbol_;
    }

    bool is_side_effect_creator() const {
        return side_effect_creator_;
    }

    SideEffectMode get_side_effect_mode() const {
        return side_effect_mode_;
    }

    bool is_direct() const {
        return direct_;
    }

    int get_side_effect_count() const {
        return side_effect_count_;
    }

    bool try_to_merge(const function_id_t& function_id,
                      int formal_parameter_position,
                      int actual_argument_position,
                      const std::string& expression,
                      const std::string& symbol,
                      bool side_effect_creator,
                      SideEffectMode& side_effect_mode,
                      bool direct) {
        if (is_mergeable_(function_id,
                          formal_parameter_position,
                          actual_argument_position,
                          expression,
                          symbol,
                          side_effect_creator,
                          side_effect_mode,
                          direct)) {
            side_effect_count_++;
            return true;
        }
        return false;
    }

  private:
    function_id_t function_id_;
    std::string function_namespace_;
    std::string function_names_;
    int formal_parameter_position_;
    int actual_argument_position_;
    std::string expression_;
    std::string symbol_;
    bool side_effect_creator_;
    SideEffectMode side_effect_mode_;
    bool direct_;
    int side_effect_count_;

    bool is_mergeable_(const function_id_t& function_id,
                       int formal_parameter_position,
                       int actual_argument_position,
                       const std::string& expression,
                       const std::string& symbol,
                       bool side_effect_creator,
                       SideEffectMode& side_effect_mode,
                       bool direct) {
        return (get_function_id() == function_id &&
                get_formal_parameter_position() == formal_parameter_position &&
                get_actual_argument_position() == actual_argument_position &&
                get_expression() == expression && get_symbol() == symbol &&
                is_side_effect_creator() == side_effect_creator &&
                get_side_effect_mode() == side_effect_mode &&
                is_direct() == direct);
    }
};

#endif /* TYPETESTERDYNTRACER_SIDE_EFFECT_SUMMARY_H */
