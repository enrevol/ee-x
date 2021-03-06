//
//  StorePurchaseProcessingResult.hpp
//  Pods
//
//  Created by eps on 6/26/20.
//

#ifndef EE_X_STORE_PURCHASE_PROCESSING_RESULT_HPP
#define EE_X_STORE_PURCHASE_PROCESSING_RESULT_HPP

#include "ee/store/StoreFwd.hpp"

namespace ee {
namespace store {
enum class PurchaseProcessingResult {
    Complete,
    Pending,
};
} // namespace store
} // namespace ee

#endif /* EE_X_STORE_PURCHASE_PROCESSING_RESULT_HPP */
