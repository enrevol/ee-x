#include "ee/ads/internal/GuardedRewardedAd.hpp"

#include <ee/core/NoAwait.hpp>
#include <ee/core/ObserverHandle.hpp>
#include <ee/core/Task.hpp>
#include <ee/core/Utils.hpp>

#include "ee/ads/internal/Retrier.hpp"

namespace ee {
namespace ads {
using Self = GuardedRewardedAd;

Self::GuardedRewardedAd(const std::shared_ptr<IRewardedAd>& ad)
    : ad_(ad) {
    loading_ = false;
    loaded_ = false;
    displaying_ = false;

    handle_ = std::make_unique<ObserverHandle>();
    handle_->bind(*ad_).addObserver({
        .onLoaded =
            [this] {
                // Propagation.
                dispatchEvent([](auto&& observer) {
                    if (observer.onLoaded) {
                        observer.onLoaded();
                    }
                });
            },
        .onClicked =
            [this] {
                // Propagation.
                dispatchEvent([](auto&& observer) {
                    if (observer.onClicked) {
                        observer.onClicked();
                    }
                });
            },
    });

    retrier_ = std::make_unique<Retrier>(1, 2, 64);
}

Self::~GuardedRewardedAd() = default;

void Self::destroy() {
    ad_->destroy();
    handle_->clear();
    retrier_->stop();
}

bool Self::isLoaded() const {
    return loaded_;
}

Task<bool> Self::load() {
    if (loaded_) {
        co_return true;
    }
    if (displaying_) {
        co_return false;
    }
    if (loading_) {
        co_return false;
    }
    loading_ = true;
    loaded_ = co_await ad_->load();
    /*
    if (loaded_) {
        retrier_->stop();
    } else {
        noAwait([this]() -> Task<> {
            co_await retrier_->process([this]() -> Task<bool> {
                co_return loaded_ = co_await ad_->load();
            });
        });
    }
    */
    loading_ = false;
    co_return loaded_;
}

Task<IRewardedAdResult> Self::show() {
    if (not loaded_) {
        co_return IRewardedAdResult::Failed;
    }
    if (loading_) {
        co_return IRewardedAdResult::Failed;
    }
    if (displaying_) {
        co_return IRewardedAdResult::Failed;
    }
    displaying_ = true;
    loaded_ = false;
    auto result = co_await ad_->show();
    displaying_ = false;
    co_return result;
}
} // namespace ads
} // namespace ee
