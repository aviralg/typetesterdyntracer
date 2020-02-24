#ifndef TYPETESTERDYNTRACER_PROMISE_GC_SUMMARY_H
#define TYPETESTERDYNTRACER_PROMISE_GC_SUMMARY_H

class PromiseGcSummary {
  public:
    explicit PromiseGcSummary(bool local,
                              bool forced,
                              sexptype_t expression_type,
                              sexptype_t value_type,
                              bool escaped,
                              bool argument,
                              gc_cycle_t gc_cycle_count)
        : local_(local)
        , forced_(forced)
        , expression_type_(expression_type)
        , value_type_(value_type)
        , escaped_(escaped)
        , argument_(argument)
        , gc_cycle_count_(gc_cycle_count)
        , promise_count_(1) {
    }

    bool is_local() const {
        return local_;
    }

    bool is_forced() const {
        return forced_;
    }

    sexptype_t get_expression_type() const {
        return expression_type_;
    }

    sexptype_t get_value_type() const {
        return value_type_;
    }

    bool has_escaped() const {
        return escaped_;
    }

    bool is_argument() const {
        return argument_;
    }

    int get_gc_cycle_count() const {
        return gc_cycle_count_;
    }

    int get_promise_count() const {
        return promise_count_;
    }

    bool try_to_merge(bool local,
                      bool forced,
                      sexptype_t expression_type,
                      sexptype_t value_type,
                      bool escaped,
                      bool argument,
                      gc_cycle_t gc_cycle_count) {
        if (is_mergeable_(local,
                          forced,
                          expression_type,
                          value_type,
                          escaped,
                          argument,
                          gc_cycle_count)) {
            promise_count_++;
            return true;
        }
        return false;
    }

  private:
    bool local_;
    bool forced_;
    sexptype_t expression_type_;
    sexptype_t value_type_;
    bool escaped_;
    bool argument_;
    gc_cycle_t gc_cycle_count_;
    int promise_count_;

    bool is_mergeable_(bool local,
                       bool forced,
                       sexptype_t expression_type,
                       sexptype_t value_type,
                       bool escaped,
                       bool argument,
                       gc_cycle_t gc_cycle_count) {
        return (is_local() == local && is_forced() == forced &&
                get_expression_type() == expression_type &&
                get_value_type() == value_type && has_escaped() == escaped &&
                is_argument() == argument &&
                get_gc_cycle_count() == gc_cycle_count);
    }
};

#endif /* TYPETESTERDYNTRACER_PROMISE_GC_SUMMARY_H */
