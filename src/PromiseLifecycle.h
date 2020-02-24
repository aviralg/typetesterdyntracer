#ifndef TYPETESTERDYNTRACER_PROMISE_LIFECYCLE_H
#define TYPETESTERDYNTRACER_PROMISE_LIFECYCLE_H

#include "PromiseEvent.h"

//#include <iostream>
#include <vector>

class PromiseLifecycle {
  public:
    using iterator = promise_event_sequence_t::iterator;
    using reverse_iterator = promise_event_sequence_t::reverse_iterator;
    using const_iterator = promise_event_sequence_t::const_iterator;
    using const_reverse_iterator =
        promise_event_sequence_t::const_reverse_iterator;

    PromiseLifecycle(): count_(1) {
        /* we are assuming that most promises encounter 4 events in their life.
           Creation, Becoming an Argument, Getting Looked up, Getting Destroyed.
         */
        events_.reserve(4);
    }

    bool operator==(const PromiseLifecycle& sequence) const {
        if (sequence.size() != size()) {
            return false;
        }

        for (std::size_t i = 0; i < size(); ++i) {
            if (sequence[i] != events_[i]) {
                return false;
            }
        }

        return true;
    }

    const PromiseEvent& operator[](const std::size_t index) const {
        return events_[index];
    }

    bool operator!=(const PromiseLifecycle& sequence) const {
        return !operator==(sequence);
    }

    size_t size() const {
        return events_.size();
    }

    bool is_empty() const {
        return events_.empty();
    }

    iterator begin() {
        return events_.begin();
    }

    iterator end() {
        return events_.end();
    }

    reverse_iterator rbegin() {
        return events_.rbegin();
    }

    reverse_iterator rend() {
        return events_.rend();
    }

    const_iterator cbegin() const {
        return events_.cbegin();
    }

    const_iterator cend() const {
        return events_.cend();
    }

    const_reverse_iterator crbegin() const {
        return events_.crbegin();
    }

    const_reverse_iterator crend() const {
        return events_.crend();
    }

    void add_event(const PromiseEvent::Type type) {
        if (events_.size() == 0 || events_.back().get_type() != type) {
            events_.push_back(PromiseEvent(type, 1));
        } else {
            events_.back().increment_count();
        }
    }

    std::size_t get_count() const {
        return count_;
    }

    void increment_count(const std::size_t increment = 1) {
        count_ += increment;
    }

    std::string get_event_names() const {
        if (events_.size() == 0) {
            return "()";
        }

        std::string str = "(" + events_[0].get_type_code();

        for (auto i = 1; i < events_.size(); ++i) {
            str.append(" ").append(events_[i].get_type_code());
        }

        return str + ")";
    }

    std::string get_event_counts() const {
        if (events_.size() == 0) {
            return "()";
        }

        std::string str = "(" + std::to_string(events_[0].get_count());

        for (auto i = 1; i < events_.size(); ++i) {
            str.append(" ").append(std::to_string(events_[i].get_count()));
        }

        return str + ")";
    }

  private:
    promise_event_sequence_t events_;
    std::size_t count_;
};

using promise_lifecycle_sequence_t = std::vector<PromiseLifecycle>;

#endif /* TYPETESTERDYNTRACER_PROMISE_LIFECYCLE_H */
