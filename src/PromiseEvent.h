#ifndef TYPETESTERDYNTRACER_PROMISE_EVENT_H
#define TYPETESTERDYNTRACER_PROMISE_EVENT_H

#include "utilities.h"

#include <vector>

class PromiseEvent {
  public:
    enum class Type {
        Allocate = 0,
        Deallocate,
        Lookup,
        Preforce,
        Force,
        Metaprogram,
        S3Dispatch,
        S4Dispatch,
        Bound,
        Unbound,
        ValueAssign,
        Escape,
        Deserialize,
        Count
    };

    explicit PromiseEvent(const Type type, const std::size_t count)
        : type_(type), count_(count) {
    }

    Type get_type() const noexcept {
        return type_;
    }

    std::string get_type_code() const noexcept {
        std::vector<std::string> codes{"C",
                                       "D",
                                       "L",
                                       "P",
                                       "F",
                                       "M",
                                       "3",
                                       "4",
                                       "B",
                                       "U",
                                       "A",
                                       "E",
                                       "S",
                                       "T"};
        return codes[to_underlying(get_type())];
    }

    std::size_t get_count() const noexcept {
        return count_;
    }

    void increment_count(const std::size_t increment = 1) noexcept {
        count_ = count_ + increment;
    }

    bool operator==(const PromiseEvent& event) const noexcept {
        return (event.get_type() == get_type() &&
                event.get_count() == get_count());
    }

    bool operator!=(const PromiseEvent& event) const noexcept {
        return !operator==(event);
    }

    bool is_same_event(const PromiseEvent& other) const noexcept {
        return other.get_type() == get_type();
    }

  private:
    Type type_;
    std::size_t count_;
};

using promise_event_sequence_t = std::vector<PromiseEvent>;

std::string to_string(const PromiseEvent::Type type);

#endif /* TYPETESTERDYNTRACER_PROMISE_EVENT_H */
