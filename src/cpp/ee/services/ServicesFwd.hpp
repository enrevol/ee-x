#ifndef EE_X_SERVICES_FWD_HPP
#define EE_X_SERVICES_FWD_HPP

#ifdef __cplusplus

#include <ee/ad_mob/AdMobFwd.hpp>
#include <ee/app_lovin/AppLovinFwd.hpp>
#include <ee/facebook_ads/FacebookAdsFwd.hpp>
#include <ee/iron_source/IronSourceFwd.hpp>
#include <ee/unity_ads/UnityAdsFwd.hpp>
#include <ee/vungle/VungleFwd.hpp>

namespace ee {
namespace services {
enum class AdResult;

class AdsConfig;
class AppOpenConfig;
class BannerConfig;
class InterstitialConfig;
class RewardedConfig;
class AdInstanceConfig;
class SingleInstanceConfig;
class WaterfallInstanceConfig;

class IAdsManager;
class DefaultAdsManager;
} // namespace services

using services::AdResult;
using services::DefaultAdsManager;
using services::IAdsManager;
} // namespace ee

#endif // __cplusplus

#endif // EE_X_SERVICES_FWD_HPP
