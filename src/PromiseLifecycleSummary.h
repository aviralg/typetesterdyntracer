#ifndef TYPETESTERDYNTRACER_PROMISE_LIFECYCLE_SUMMARY_H
#define TYPETESTERDYNTRACER_PROMISE_LIFECYCLE_SUMMARY_H

class PromiseLifecycleSummary {
  public:
    explicit PromiseLifecycleSummary(bool local,
                                     bool argument,
                                     bool escaped,
                                     const PromiseLifecycle& promise_lifecycle)
        : local_(local)
        , argument_(argument)
        , escaped_(escaped)
        , promise_lifecycle_(promise_lifecycle)
        , promise_count_(1) {
    }

    bool is_local() const {
        return local_;
    }

    bool is_argument() const {
        return argument_;
    }

    bool has_escaped() const {
        return escaped_;
    }

    const PromiseLifecycle& get_promise_lifecycle() const {
        return promise_lifecycle_;
    }

    int get_promise_count() const {
        return promise_count_;
    }

    bool try_to_merge(bool local,
                      bool argument,
                      bool escaped,
                      const PromiseLifecycle& promise_lifecycle) {
        if (is_mergeable_(local, argument, escaped, promise_lifecycle)) {
            promise_count_++;
            return true;
        }
        return false;
    }

  private:
    bool local_;
    bool argument_;
    bool escaped_;
    const PromiseLifecycle promise_lifecycle_;
    int promise_count_;

    bool is_mergeable_(bool local,
                       bool argument,
                       bool escaped,
                       const PromiseLifecycle& promise_lifecycle) {
        return (is_local() == local && is_argument() == argument &&
                has_escaped() == escaped &&
                get_promise_lifecycle() == promise_lifecycle);
    }
};

#endif /* TYPETESTERDYNTRACER_PROMISE_LIFECYCLE_SUMMARY_H */
