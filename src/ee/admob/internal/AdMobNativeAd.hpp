//
//  AdMobNativeAd.hpp
//  ee_x
//
//  Created by Zinge on 10/13/17.
//
//

#ifndef EE_X_ADMOB_NATIVE_AD_HPP
#define EE_X_ADMOB_NATIVE_AD_HPP

#include "ee/AdMobFwd.hpp"
#include "ee/ads/IAdView.hpp"
#include "ee/ads/internal/AdViewBridgeHelper.hpp"
#include "ee/ads/internal/AdViewHelper.hpp"

namespace ee {
namespace admob {
class NativeAd : public IAdView {
private:
    using Self = NativeAd;
    using Super = IAdView;

public:
    virtual ~NativeAd() override;

    /// @see Super.
    virtual bool isLoaded() const override;

    /// @see Super.
    virtual void load() override;

    /// @see Super.
    virtual std::pair<float, float> getAnchor() const override;

    /// @see Super.
    virtual void setAnchor(float x, float y) override;

    /// @see Super.
    virtual std::pair<int, int> getPosition() const override;

    /// @see Super.
    virtual void setPosition(int x, int y) override;

    /// @see Super.
    virtual std::pair<int, int> getSize() const override;

    /// @see Super.
    virtual void setSize(int width, int height) override;

    /// @see Super.
    virtual void setVisible(bool visible) override;

protected:
    friend AdMob;

    explicit NativeAd(AdMob* plugin, const std::string& adId);

private:
    void onLoaded();
    void onFailedToLoad(const std::string& message);

    bool loading_;
    std::string adId_;
    AdMob* plugin_;
    ads::AdViewHelper helper_;
    ads::AdViewBridgeHelper bridgeHelper_;
};
} // namespace admob
} // namespace ee

#endif /* EE_X_ADMOB_NATIVE_AD_HPP */
