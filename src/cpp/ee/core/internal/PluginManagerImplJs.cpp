#include "ee/core/internal/PluginManagerImplJs.hpp"

#include "ee/core/JsbUtils.hpp"
#include "ee/core/PluginManager.hpp"
#include "ee/core/internal/MessageBridgeJs.hpp"

namespace ee {
namespace core {
namespace {
bool ee_staticLogLevel_binder(se::State& state) {
    constexpr std::size_t Arity = 1;
    auto&& args = state.args();
    if (Arity != args.size()) {
        SE_REPORT_ERROR("Wrong number of arguments: %zu, was expecting: %zu.",
                        args.size(), Arity);
        return false;
    }
    se::Value jsThis(state.thisObject());
    // FIXME.
    // PluginManager::setLogLevel(args[0].toInt32());
    return true;
}

bool ee_staticAddPlugin_binder(se::State& state) {
    constexpr std::size_t Arity = 1;
    auto&& args = state.args();
    if (Arity != args.size()) {
        SE_REPORT_ERROR("Wrong number of arguments: %zu, was expecting: %zu.",
                        args.size(), Arity);
        return false;
    }
    se::Value jsThis(state.thisObject());
    PluginManager::addPlugin(static_cast<Plugin>(args[0].toInt32()));
    return true;
}

bool ee_staticRemovePlugin_binder(se::State& state) {
    constexpr std::size_t Arity = 1;
    auto&& args = state.args();
    if (Arity != args.size()) {
        SE_REPORT_ERROR("Wrong number of arguments: %zu, was expecting: %zu.",
                        args.size(), Arity);
        return false;
    }
    se::Value jsThis(state.thisObject());
    PluginManager::removePlugin(static_cast<Plugin>(args[0].toInt32()));
    return true;
}

constexpr auto ee_staticLogLevel = &ee_staticLogLevel_binder;
constexpr auto ee_staticAddPlugin = &ee_staticAddPlugin_binder;
constexpr auto ee_staticRemovePlugin = &ee_staticRemovePlugin_binder;

SE_BIND_FUNC(ee_staticLogLevel)
SE_BIND_FUNC(ee_staticAddPlugin)
SE_BIND_FUNC(ee_staticRemovePlugin)

bool registerJsb(se::Object* global) {
    global->defineFunction("ee_staticLogLevel", _SE(ee_staticLogLevel));
    global->defineFunction("ee_staticAddPlugin", _SE(ee_staticAddPlugin));
    global->defineFunction("ee_staticRemovePlugin", _SE(ee_staticRemovePlugin));
    return true;
}
} // namespace

using Self = PluginManagerImplJs;

void Self::initialize() {
    auto&& engine = se::ScriptEngine::getInstance();
    engine->addRegisterCallback(registerJsb);
    bridge_ = std::make_shared<MessageBridgeJs>();
}

IMessageBridge& Self::getBridge() {
    return *bridge_;
}
} // namespace core
} // namespace ee
