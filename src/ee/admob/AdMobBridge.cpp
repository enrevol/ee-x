//
//  AdMob.cpp
//  ee_x
//
//  Created by Zinge on 10/11/17.
//
//

#include "ee/admob/AdMobBridge.hpp"

#include <ee/nlohmann/json.hpp>

#include <ee/ads/internal/MediationManager.hpp>
#include <ee/core/Logger.hpp>
#include <ee/core/Utils.hpp>
#include <ee/core/internal/MessageBridge.hpp>

#include "ee/admob/AdMobNativeAdLayout.hpp"
#include "ee/admob/private/AdMobBannerAd.hpp"
#include "ee/admob/private/AdMobInterstitialAd.hpp"
#include "ee/admob/private/AdMobNativeAd.hpp"
#include "ee/admob/private/AdMobRewardedAd.hpp"

namespace ee {
namespace admob {
using Self = Bridge;

namespace {
// clang-format off
const std::string kPrefix               = "AdMob";

const auto k__initialize                = kPrefix + "_initialize";
    
const auto k__getEmulatorTestDeviceHash = kPrefix + "_getEmulatorTestDeviceHash";
const auto k__addTestDevice             = kPrefix + "_addTestDevice";
    
const auto k__createBannerAd            = kPrefix + "_createBannerAd";
const auto k__destroyBannerAd           = kPrefix + "_destroyBannerAd";

const auto k__createNativeAd            = kPrefix + "_createNativeAd";
const auto k__destroyNativeAd           = kPrefix + "_destroyNativeAd";

const auto k__createInterstitialAd      = kPrefix + "_createInterstitialAd";
const auto k__destroyInterstitialAd     = kPrefix + "_destroyInterstitialAd";

const auto k__createRewardedAd          = kPrefix + "_createRewardedAd";
const auto k__destroyRewardedAd         = kPrefix + "_destroyRewardedAd";
// clang-format on
} // namespace

namespace {
// clang-format off
constexpr auto k__ad_id                 = "ad_id";
constexpr auto k__ad_size               = "ad_size";
constexpr auto k__layout_name           = "layout_name";
constexpr auto k__identifiers           = "identifiers";
// clang-format on
} // namespace

Self::Bridge()
    : Self(Logger::getSystemLogger()) {}

Self::Bridge(const Logger& logger)
    : bridge_(MessageBridge::getInstance())
    , logger_(logger) {
    logger_.debug("%s", __PRETTY_FUNCTION__);
    auto&& mediation = ads::MediationManager::getInstance();
    interstitialAdDisplayer_ = mediation.getInterstitialAdDisplayer();
    rewardedAdDisplayer_ = mediation.getRewardedAdDisplayer();
}

Self::~Bridge() {
    logger_.debug("%s", __PRETTY_FUNCTION__);
}

void Self::initialize(const std::string& applicationId) {
    runOnUiThread([this, applicationId] { //
        bridge_.call(k__initialize, applicationId);
    });
}

std::string Self::getEmulatorTestDeviceHash() const {
    return bridge_.call(k__getEmulatorTestDeviceHash);
}

void Self::addTestDevice(const std::string& hash) {
    runOnUiThread([this, hash] { //
        bridge_.call(k__addTestDevice, hash);
    });
}

std::shared_ptr<IAdView> Self::createBannerAd(const std::string& adId,
                                              BannerAdSize adSize) {
    logger_.debug("%s: id = %s size = %d", __PRETTY_FUNCTION__, adId.c_str(),
                  static_cast<int>(adSize));
    auto iter = bannerAds_.find(adId);
    if (iter != bannerAds_.cend()) {
        return iter->second;
    }
    runOnUiThread([this, adId, adSize] {
        nlohmann::json json;
        json[k__ad_id] = adId;
        json[k__ad_size] = static_cast<int>(adSize);
        auto response = bridge_.call(k__createBannerAd, json.dump());
        if (core::toBool(response)) {
            // OK.
            return;
        }
        assert(false);
        logger_.error("%s: There was an error when attempt to create an ad.",
                      __PRETTY_FUNCTION__);
    });
    auto ad =
        std::shared_ptr<IAdView>(new BannerAd(bridge_, logger_, this, adId));
    bannerAds_.emplace(adId, ad);
    return ad;
}

bool Self::destroyBannerAd(const std::string& adId) {
    logger_.debug("%s: id = %s", __PRETTY_FUNCTION__, adId.c_str());
    auto iter = bannerAds_.find(adId);
    if (iter == bannerAds_.cend()) {
        return false;
    }
    runOnUiThread([this, adId] {
        auto response = bridge_.call(k__destroyBannerAd, adId);
        if (core::toBool(response)) {
            // OK.
            return;
        }
        assert(false);
        logger_.error("%s: There was an error when attempt to destroy an ad.",
                      __PRETTY_FUNCTION__);
    });
    bannerAds_.erase(iter);
    return true;
}

std::shared_ptr<IAdView>
Self::createNativeAd(const std::string& adId, const std::string& layoutName,
                     const NativeAdLayout& identifiers) {
    logger_.debug("%s: id = %s", __PRETTY_FUNCTION__, adId.c_str());
    auto iter = nativeAds_.find(adId);
    if (iter != nativeAds_.cend()) {
        return iter->second;
    }
    runOnUiThread([this, adId, layoutName, identifiers] {
        nlohmann::json json;
        json[k__ad_id] = adId;
        json[k__layout_name] = layoutName;
        json[k__identifiers] = identifiers.params_;
        auto&& response = bridge_.call(k__createNativeAd, json.dump());
        if (core::toBool(response)) {
            // OK.
            return;
        }
        assert(false);
        logger_.error("%s: There was an error when attempt to create an ad.",
                      __PRETTY_FUNCTION__);
    });
    auto ad =
        std::shared_ptr<IAdView>(new NativeAd(bridge_, logger_, this, adId));
    nativeAds_.emplace(adId, ad);
    return ad;
}

bool Self::destroyNativeAd(const std::string& adId) {
    logger_.debug("%s: id = %s", __PRETTY_FUNCTION__, adId.c_str());
    auto iter = nativeAds_.find(adId);
    if (iter == nativeAds_.cend()) {
        return false;
    }
    runOnUiThread([this, adId] {
        auto&& response = bridge_.call(k__destroyNativeAd, adId);
        if (core::toBool(response)) {
            // OK.
            return;
        }
        assert(false);
        logger_.error("%s: There was an error when attempt to destroy an ad.",
                      __PRETTY_FUNCTION__);
    });
    nativeAds_.erase(iter);
    return true;
}

std::shared_ptr<IInterstitialAd>
Self::createInterstitialAd(const std::string& adId) {
    logger_.debug("%s: id = %s", __PRETTY_FUNCTION__, adId.c_str());
    auto iter = interstitialAds_.find(adId);
    if (iter != interstitialAds_.cend()) {
        return iter->second;
    }
    runOnUiThread([this, adId] {
        auto response = bridge_.call(k__createInterstitialAd, adId);
        if (core::toBool(response)) {
            // OK.
            return;
        }
        assert(false);
        logger_.error("%s: There was an error when attempt to create an ad.",
                      __PRETTY_FUNCTION__);
    });
    auto ad = std::shared_ptr<IInterstitialAd>(new InterstitialAd(
        bridge_, logger_, interstitialAdDisplayer_, this, adId));
    interstitialAds_.emplace(adId, ad);
    return ad;
}

bool Self::destroyInterstitialAd(const std::string& adId) {
    logger_.debug("%s: id = %s", __PRETTY_FUNCTION__, adId.c_str());
    auto iter = interstitialAds_.find(adId);
    if (iter == interstitialAds_.cend()) {
        return false;
    }
    runOnUiThread([this, adId] {
        auto&& response = bridge_.call(k__destroyInterstitialAd, adId);
        if (core::toBool(response)) {
            // OK.
            return;
        }
        assert(false);
        logger_.error("%s: There was an error when attempt to destroy an ad.",
                      __PRETTY_FUNCTION__);
    });
    interstitialAds_.erase(iter);
    return true;
}

std::shared_ptr<IRewardedAd> Self::createRewardedAd(const std::string& adId) {
    logger_.debug("%s: id = %s", __PRETTY_FUNCTION__, adId.c_str());
    auto iter = rewardedAds_.find(adId);
    if (iter != rewardedAds_.cend()) {
        return iter->second;
    }
    runOnUiThread([this, adId] {
        auto response = bridge_.call(k__createRewardedAd, adId);
        if (core::toBool(response)) {
            // OK.
            return;
        }
        assert(false);
        logger_.error("%s: There was an error when attempt to create an ad.",
                      __PRETTY_FUNCTION__);
    });
    auto ad = std::shared_ptr<IRewardedAd>(
        new RewardedAd(bridge_, logger_, rewardedAdDisplayer_, this, adId));
    rewardedAds_.emplace(adId, ad);
    return ad;
}

bool Self::destroyRewardedAd(const std::string& adId) {
    logger_.debug("%s: id = %s", __PRETTY_FUNCTION__, adId.c_str());
    auto iter = rewardedAds_.find(adId);
    if (iter == rewardedAds_.cend()) {
        return false;
    }
    runOnUiThread([this, adId] {
        auto&& response = bridge_.call(k__destroyRewardedAd, adId);
        if (core::toBool(response)) {
            // OK.
            return;
        }
        assert(false);
        logger_.error("%s: There was an error when attempt to destroy an ad.",
                      __PRETTY_FUNCTION__);
    });
    rewardedAds_.erase(iter);
    return true;
}
} // namespace admob
} // namespace ee
