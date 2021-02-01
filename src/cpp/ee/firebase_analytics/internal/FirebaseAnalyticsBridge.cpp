//
//  FirebaseAnalytics.cpp
//  ee_x
//
//  Created by Zinge on 5/10/17.
//
//

#include "ee/firebase_analytics/internal/FirebaseAnalyticsBridge.hpp"

#include <ee/core/ILogger.hpp>
#include <ee/core/IMessageBridge.hpp>
#include <ee/nlohmann/json.hpp>

namespace nlohmann {
template <typename... Args>
struct adl_serializer<std::variant<Args...>> {
    static void to_json(json& json, std::variant<Args...> const& value) {
        std::visit(
            [&json](auto&& v) { //
                json = std::forward<decltype(v)>(v);
            },
            value);
    }
};
} // namespace nlohmann

namespace ee {
namespace firebase {
namespace analytics {
namespace {
const std::string kPrefix = "FirebaseAnalyticsBridge";
const std::string kSetUserProperty = kPrefix + "SetUserProperty";
const std::string kTrackScreen = kPrefix + "TrackScreen";
const std::string kLogEvent = kPrefix + "LogEvent";
} // namespace

using Self = Bridge;

Self::Bridge(IMessageBridge& bridge, ILogger& logger,
             const Destroyer& destroyer)
    : bridge_(bridge)
    , logger_(logger)
    , destroyer_(destroyer) {}

Self::~Bridge() = default;

void Self::destroy() {
    destroyer_();
}

void Self::setUserProperty(const std::string& key, const std::string& value) {
    nlohmann::json request;
    request["key"] = key;
    request["value"] = value;
    bridge_.call(kSetUserProperty, request.dump());
}

void Self::trackScreen(const std::string& name) {
    bridge_.call(kTrackScreen, name);
}

void Self::logEvent(
    const std::string& name,
    const std::unordered_map<std::string,
                             std::variant<std::int64_t, double, std::string>>&
        parameters) {
    nlohmann::json request;
    request["name"] = name;
    request["parameters"] = parameters;
    bridge_.call(kLogEvent, request.dump());
}
} // namespace analytics
} // namespace firebase
} // namespace ee
