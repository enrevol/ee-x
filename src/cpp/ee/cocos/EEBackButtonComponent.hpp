//
//  EEBackButtonComponent.hpp
//  ee-library
//
//  Created by Zinge on 12/27/16.
//
//

#ifndef EE_LIBRARY_BACK_BUTTON_COMPONENT_HPP_
#define EE_LIBRARY_BACK_BUTTON_COMPONENT_HPP_

#ifdef __cplusplus

#include "ee/cocos/CocosFwd.hpp"

#ifdef EE_X_COCOS_CPP
#include <2d/CCComponent.h>

namespace ee {
class BackButtonComponent : public cocos2d::Component {
private:
    using Self = BackButtonComponent;
    using Super = cocos2d::Component;

public:
    using BackButtonCallback = std::function<void()>;

    static Self* create(const BackButtonCallback& callback);

    virtual void onAdd() override;
    virtual void onRemove() override;

protected:
    BackButtonComponent();

    virtual bool initWithCallback(const BackButtonCallback& callback);

private:
    /// Keyboard event callback.
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode,
                       cocos2d::Event* event);

    cocos2d::EventListenerKeyboard* listener_;

    BackButtonCallback callback_;
};
} // namespace ee

#endif // EE_X_COCOS_CPP
#endif // __cplusplus

#endif /* EE_LIBRARY_BACK_BUTTON_COMPONENT_HPP_ */
