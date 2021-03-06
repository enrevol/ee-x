#ifndef EE_X_I_FIREBASE_ANALYTICS_BRIDGE_HPP
#define EE_X_I_FIREBASE_ANALYTICS_BRIDGE_HPP

#ifdef __cplusplus

#include <unordered_map>
#include <variant>

#include <ee/core/IPlugin.hpp>

#include "ee/firebase_analytics/FirebaseAnalyticsFwd.hpp"

namespace ee {
namespace firebase {
namespace analytics {
class IBridge : public IPlugin {
public:
    [[nodiscard]] virtual Task<bool> initialize() = 0;
    virtual void setUserProperty(const std::string& key,
                                 const std::string& value) = 0;
    virtual void trackScreen(const std::string& name) = 0;
    virtual void
    logEvent(const std::string& name,
             const std::unordered_map<
                 std::string, std::variant<std::int64_t, double, std::string>>&
                 parameters) = 0;
};
} // namespace analytics
} // namespace firebase
} // namespace ee

#endif // __cplusplus

#endif // EE_X_I_FIREBASE_ANALYTICS_BRIDGE_HPP
