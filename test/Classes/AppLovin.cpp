//
//  AppLovin.cpp
//  ee_x_test
//
//  Created by Zinge on 10/16/17.
//
//

#include "AppLovin.hpp"

#include <ee/Coroutine.hpp>

#include "Utils.hpp"

namespace eetest {
ee::AppLovin* getAppLovin() {
    static auto plugin = ee::AppLovin();
    static bool initialized;
    if (not initialized) {
        ee::runOnUiThreadAndWait([] {
            plugin.initialize(
                R"(gG8pkErh1_fQo-4cNDXGnxGyb9H4qz6VDEJyS8eU8IvxH-XeB4wy0BubKAg97neL0yIT4xyDEs8WqfA0l4zlGr)");
            plugin.setTestAdsEnabled(true);
            plugin.setVerboseLogging(true);
        });
        initialized = true;
    }
    return &plugin;
}

void testAppLovinRewardedAd() {
    auto ad =
        ee::runOnUiThreadAndWaitResult<std::shared_ptr<ee::IRewardedAd>>([] { //
            return std::make_shared<ee::GuardedRewardedAd>(
                getAppLovin()->createRewardedAd());
        });

    float delay = 0.0f;
    scheduleForever(delay += 1.0f, 3.0f, [ad] {
        ee::runOnUiThread(ee::makeAwaiter([ad]() -> ee::Task<> {
            getLogger().info("Load AppLovin rewarded ad");
            co_await ad->load();
        }));
    });

    scheduleForever(delay += 1.0f, 3.0f, [ad] {
        ee::runOnUiThread(ee::makeAwaiter([ad]() -> ee::Task<> {
            getLogger().info("Show AppLovin rewarded ad");
            auto result = co_await ad->show();
            logCurrentThread();
            getLogger().info("Result = %d", static_cast<int>(result));
        }));
    });
}
} // namespace eetest
