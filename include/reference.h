//
// Created by sophomore on 12/22/24.
//

#ifndef REFERENCE_H
#define REFERENCE_H
#include <memory>
#include <variant>

template<typename T>
class reference {
    std::variant<T, std::shared_ptr<T>, T *> value;

    const T *Address() const {
        return std::holds_alternative<T>(value)
                   ? &std::get<T>(value)
                   : std::holds_alternative<T *>(value)
                         ? std::get<T *>(value)
                         : std::get<std::shared_ptr<T> >(value).get();
    }

public:
    reference() = default;

    reference(T t) : value(std::move(t)) {
    }

    reference(T *t) : value(t) {
    }

    reference(std::shared_ptr<T> t) : value(t) {
    }

    reference(const reference &) = default;

    reference &operator=(reference &&) noexcept = default;

    reference(reference &&) noexcept = default;

    ~reference() = default;

    // Accessors:
    const T &operator()() const { return *Address(); }
    const T &operator*() const { return *Address(); }
    const T *operator->() const { return Address(); }
};
#endif //REFERENCE_H
