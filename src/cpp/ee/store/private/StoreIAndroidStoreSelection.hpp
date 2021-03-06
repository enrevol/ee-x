//
//  StoreIAndroidStoreSelection.hpp
//  Pods
//
//  Created by eps on 7/2/20.
//

#ifndef EE_X_STORE_I_ANDROID_STORE_SELECTION_HPP
#define EE_X_STORE_I_ANDROID_STORE_SELECTION_HPP

#include "ee/store/private/StoreIStoreConfiguration.hpp"

namespace ee {
namespace store {
class IAndroidStoreSelection : public virtual IStoreConfiguration {
public:
    virtual AppStore appStore() const = 0;
};
} // namespace store
} // namespace ee

#endif /* EE_X_STORE_I_ANDROID_STORE_SELECTION_HPP */
