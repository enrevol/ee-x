//
//  FacebookBannerAd.cpp
//  ee_x
//
//  Created by Zinge on 10/6/17.
//
//

#include "ee/facebook_ads/private/FacebookBannerAd.hpp"

#include <cassert>

#include <ee/nlohmann/json.hpp>

#include <ee/core/internal/IMessageBridge.hpp>

#include "ee/facebook_ads/FacebookAdsBridge.hpp"

namespace ee {
namespace facebook_ads {
using Self = BannerAd;

namespace {
auto k__onLoaded(const std::string& id) {
    return "FacebookBannerAd_onLoaded_" + id;
}

auto k__onFailedToLoad(const std::string& id) {
    return "FacebookBannerAd_onFailedToLoad_" + id;
}

auto k__onClicked(const std::string& id) {
    return "FacebookBannerAd_onClicked_" + id;
}
} // namespace

Self::BannerAd(IMessageBridge& bridge, Bridge* plugin, const std::string& adId)
    : Super()
    , adId_(adId)
    , bridge_(bridge)
    , plugin_(plugin)
    , helper_(bridge, "FacebookBannerAd", adId) {
    loading_ = false;

    bridge_.registerHandler(
        [this](const std::string& message) {
            onLoaded();
            return "";
        },
        k__onLoaded(adId_));
    bridge_.registerHandler(
        [this](const std::string& message) {
            onFailedToLoad(message);
            return "";
        },
        k__onFailedToLoad(adId_));
    bridge_.registerHandler(
        [this](const std::string& message) {
            onClicked();
            return "";
        },
        k__onClicked(adId_));
}

Self::~BannerAd() {
    bool succeeded = plugin_->destroyBannerAd(adId_);
    assert(succeeded);

    bridge_.deregisterHandler(k__onLoaded(adId_));
    bridge_.deregisterHandler(k__onFailedToLoad(adId_));
    bridge_.deregisterHandler(k__onClicked(adId_));
}

bool Self::isLoaded() const {
    return helper_.isLoaded();
}

void Self::load() {
    if (loading_) {
        return;
    }
    loading_ = true;
    helper_.load();
}

std::pair<float, float> Self::getAnchor() const {
    return helper_.getAnchor();
}

void Self::setAnchor(float x, float y) {
    helper_.setAnchor(x, y);
}

std::pair<int, int> Self::getPosition() const {
    return helper_.getPosition();
}

void Self::setPosition(int x, int y) {
    helper_.setPosition(x, y);
}

std::pair<int, int> Self::getSize() const {
    return helper_.getSize();
}

void Self::setSize(int width, int height) {
    helper_.setSize(width, height);
}

bool Self::isVisible() const {
    return helper_.isVisible();
}

void Self::setVisible(bool visible) {
    helper_.setVisible(visible);
}

void Self::onLoaded() {
    // Facebook banner is auto-loading.
    // assert(loading_);
    loading_ = false;
    dispatchEvent([](auto&& observer) {
        if (observer.onLoaded) {
            observer.onLoaded();
        }
    });
}

void Self::onFailedToLoad(const std::string& message) {
    // Facebook banner is auto-loading.
    // assert(loading_);
    loading_ = false;
    dispatchEvent([](auto&& observer) {
        if (observer.onFailedToLoad) {
            observer.onFailedToLoad();
        }
    });
}

void Self::onClicked() {
    dispatchEvent([](auto&& observer) {
        if (observer.onClicked) {
            observer.onClicked();
        }
    });
}
} // namespace facebook_ads
} // namespace ee
