#ifndef EE_X_OBSERVER_HANDLE_HPP
#define EE_X_OBSERVER_HANDLE_HPP

#ifdef __cplusplus

#include <functional>
#include <vector>

#include "ee/core/CoreFwd.hpp"

namespace ee {
namespace core {
class ObserverHandle {
public:
    ObserverHandle();
    ~ObserverHandle();

    template <class Observer>
    ObserverBinder<Observer> bind(IObserverManager<Observer>& manager);

    void clear();

private:
    template <class Observer>
    friend class ObserverBinder;

    void add(const std::function<void()>& rollback);

    std::vector<ScopeGuard> guards_;
};
} // namespace core
} // namespace ee

#include "ee/core/ObserverHandle.inl"

#endif // __cplusplus

#endif // EE_X_OBSERVER_HANDLE_HPP
