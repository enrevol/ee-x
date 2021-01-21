//
//  jsb_ifacebook_login_delegate.cpp
//  ee-x
//
//  Created by senspark-dev on 12/12/18.
//

#include "ee/facebook/private/JsbIFacebookLoginDelegate.hpp"

#include <ee/core/JsbUtils.hpp>
#include <ee/core/internal/SharedPtrHandler.hpp>

#include "ee/facebook/private/FacebookLoginDelegate.hpp"

using Self = ee::facebook::ILoginDelegate;

namespace ee {
namespace core {
namespace {
std::unique_ptr<SharedPtrHandler<Self>> handler;
} // namespace

template <>
std::shared_ptr<Self> get_value(const se::Value& value) {
    return handler->getValue(value);
}

template <>
void set_value(se::Value& value, std::shared_ptr<Self>& input) {
    handler->setValue(value, input);
}

template <>
void set_value(se::Value& value, Self& input) {
    set_value_from_pointer(value, &input);
}

template <>
bool makeFinalize<Self>(se::State& state) {
    return handler->finalize(state);
}
} // namespace core

namespace facebook {
namespace {
// clang-format off
constexpr auto finalize  = &core::makeFinalize<Self>;
constexpr auto onSuccess = &core::makeMethod<&Self::onSuccess>;
constexpr auto onFailure = &core::makeMethod<&Self::onFailure>;
constexpr auto onCancel  = &core::makeMethod<&Self::onCancel>;
// clang-format on

SE_BIND_FINALIZE_FUNC(finalize);
SE_BIND_FUNC(onSuccess);
SE_BIND_FUNC(onFailure);
SE_BIND_FUNC(onCancel);
} // namespace

bool registerJsbILoginDelegate(se::Object* global) {
    auto scope = JsbUtils::getPath(global, "ee");
    auto cls =
        se::Class::create("IFacebookLoginDelegate", scope, nullptr, nullptr);
    cls->defineFinalizeFunction(_SE(finalize));

    EE_JSB_DEFINE_FUNCTION(cls, onSuccess);
    EE_JSB_DEFINE_FUNCTION(cls, onFailure);
    EE_JSB_DEFINE_FUNCTION(cls, onCancel);

    cls->install();

    JSBClassType::registerClass<Self>(cls);
    core::handler = core::SharedPtrHandler<Self>::create(cls);

    se::ScriptEngine::getInstance()->clearException();
    return true;
}
} // namespace facebook
} // namespace ee
