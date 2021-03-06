//
//  IronSourceInterstitialAd.cpp
//  ee_x
//
//  Created by Zinge on 10/10/17.
//
//

#include "ee/iron_source/private/IronSourceInterstitialAd.hpp"

#include <cassert>

#include <ee/ads/internal/AsyncHelper.hpp>
#include <ee/core/ILogger.hpp>
#include <ee/core/Utils.hpp>

#include "ee/iron_source/private/IronSourceBridge.hpp"

namespace ee {
namespace iron_source {
using Self = InterstitialAd;

Self::InterstitialAd(
    ILogger& logger,
    const std::shared_ptr<ads::IAsyncHelper<AdResult>>& displayer,
    Bridge* plugin, const std::string& network, const std::string& adId)
    : logger_(logger)
    , displayer_(displayer)
    , plugin_(plugin)
    , network_(network)
    , adId_(adId) {
    logger_.debug("%s: adId = %s", __PRETTY_FUNCTION__, adId_.c_str());
    loader_ = std::make_unique<ads::AsyncHelper<bool>>();
}

Self::~InterstitialAd() = default;

void Self::destroy() {
    logger_.debug("%s: adId = %s", __PRETTY_FUNCTION__, adId_.c_str());
    plugin_->destroyInterstitialAd(adId_);
}

bool Self::isLoaded() const {
    return plugin_->hasInterstitialAd();
}

Task<bool> Self::load() {
    logger_.debug("%s: adId = %s loading = %s", __PRETTY_FUNCTION__,
                  adId_.c_str(),
                  core::toString(loader_->isProcessing()).c_str());
    auto result = co_await loader_->process(
        [this] { //
            plugin_->loadInterstitialAd();
        },
        [](bool result) {
            // OK.
        });
    co_return result;
}

Task<AdResult> Self::show() {
    logger_.debug("%s: adId = %s displaying = %s", __PRETTY_FUNCTION__,
                  adId_.c_str(),
                  core::toString(displayer_->isProcessing()).c_str());
    auto result = co_await displayer_->process(
        [this] { //
            plugin_->showInterstitialAd(adId_);
        },
        [](AdResult result) {
            // OK
        });
    co_return result;
}

void Self::onLoaded() {
    logger_.debug("%s: adId = %s loading = %s", __PRETTY_FUNCTION__,
                  adId_.c_str(),
                  core::toString(loader_->isProcessing()).c_str());
    if (loader_->isProcessing()) {
        loader_->resolve(true);
        dispatchEvent([this](auto&& observer) {
            if (observer.onLoadResult) {
                observer.onLoadResult({
                    .network = network_,
                    .result = true,
                });
            }
        });
    } else {
        logger_.error("%s: this ad is expected to be loading",
                      __PRETTY_FUNCTION__);
        assert(false);
    }
    dispatchEvent([](auto&& observer) {
        if (observer.onLoaded) {
            observer.onLoaded();
        }
    });
}

void Self::onFailedToLoad(int code, const std::string& message) {
    logger_.debug("%s: adId = %s loading = %s message = %s",
                  __PRETTY_FUNCTION__, adId_.c_str(),
                  core::toString(loader_->isProcessing()).c_str(),
                  message.c_str());
    if (loader_->isProcessing()) {
        loader_->resolve(false);
        dispatchEvent([this, code, message](auto&& observer) {
            if (observer.onLoadResult) {
                observer.onLoadResult({
                    .network = network_,
                    .result = false,
                    .errorCode = code,
                    .errorMessage = message,
                });
            }
        });
    } else {
        // IronSource may dispatches multiple interstitialDidFailToLoadWithError
        // events due to mediation.
    }
}

void Self::onFailedToShow(int code, const std::string& message) {
    logger_.debug("%s: adId = %s displaying = %s message = %s",
                  __PRETTY_FUNCTION__, adId_.c_str(),
                  core::toString(displayer_->isProcessing()).c_str(),
                  message.c_str());
    if (displayer_->isProcessing()) {
        displayer_->resolve(AdResult::Failed);
    } else {
        logger_.error("%s: this ad is expected to be displaying",
                      __PRETTY_FUNCTION__);
    }
}

void Self::onClicked() {
    logger_.debug("%s: adId = %s", __PRETTY_FUNCTION__, adId_.c_str());
    dispatchEvent([](auto&& observer) {
        if (observer.onClicked) {
            observer.onClicked();
        }
    });
}

void Self::onClosed() {
    logger_.debug("%s: adId = %s displaying = %s", __PRETTY_FUNCTION__,
                  adId_.c_str(),
                  core::toString(displayer_->isProcessing()).c_str());
    if (displayer_->isProcessing()) {
        displayer_->resolve(AdResult::Completed);
    } else {
        logger_.error("%s: this ad is expected to be displaying",
                      __PRETTY_FUNCTION__);
    }
}
} // namespace iron_source
} // namespace ee
