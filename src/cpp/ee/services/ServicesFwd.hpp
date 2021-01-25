#ifndef EE_X_SERVICES_FWD_HPP
#define EE_X_SERVICES_FWD_HPP

#ifdef __cplusplus

#include <ee/core/CoreFwd.hpp>

namespace ee {
namespace services {
enum class AdFormat;
enum class AdResult;

class AdsConfig;
class LazyBannerAd;
class GenericAd;
class IAdsManager;
class DefaultAdsManager;
} // namespace services

using services::AdResult;
using services::DefaultAdsManager;
using services::IAdsManager;
} // namespace ee

#endif // __cplusplus

#endif // EE_X_SERVICES_FWD_HPP
