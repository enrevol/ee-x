//
//  NullAdView.hpp
//  ee_x
//
//  Created by Zinge on 10/27/17.
//
//

#ifndef EE_X_NULL_BANNER_AD_HPP
#define EE_X_NULL_BANNER_AD_HPP

#ifdef __cplusplus

#include "ee/ads/NullAd.hpp"
#include "ee/ads/IBannerAd.hpp"

namespace ee {
namespace ads {
class NullBannerAd : public IBannerAd, public NullAd {
public:
    NullBannerAd();

    virtual bool isVisible() const override;
    virtual void setVisible(bool visible) override;

    virtual std::pair<float, float> getAnchor() const override;
    virtual void setAnchor(float x, float y) override;

    virtual std::pair<float, float> getPosition() const override;
    virtual void setPosition(float x, float y) override;

    virtual std::pair<float, float> getSize() const override;
    virtual void setSize(float width, float height) override;

private:
    bool visible_;
    int positionX_;
    int positionY_;
    float anchorX_;
    float anchorY_;
    int width_;
    int height_;
};
} // namespace ads
} // namespace ee

#endif // __cplusplus

#endif /* EE_X_NULL_BANNER_AD_HPP */
