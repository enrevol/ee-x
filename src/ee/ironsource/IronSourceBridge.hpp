#ifndef EE_X_IRON_SOURCE_BRIDGE_HPP
#define EE_X_IRON_SOURCE_BRIDGE_HPP

#include <map>

#include "ee/IronSourceFwd.hpp"
#include "ee/ads/IRewardedVideo.hpp"

namespace ee {
namespace ironsource {
class IronSource final {
public:
    IronSource();
    ~IronSource();

    /// Initializes ironSource with the specified game ID.
    void initialize(const std::string& gameId);

    /// Creates a rewarded vided with the specifie placement ID.
    std::shared_ptr<IRewardedVideo>
    createRewardedVideo(const std::string& placementId);

private:
    friend RewardedVideo;

    bool destroyRewardedVideo(const std::string& placementId);

    bool hasRewardedVideo() const;
    bool showRewardedVideo(const std::string& placementId);

    void onRewarded(const std::string& placementId);
    void onFailed();
    void onOpened();
    void onClosed();

    bool errored_;
    bool rewarded_;

    std::map<std::string, RewardedVideo*> rewardedVideos_;
};
} // namespace ironsource
} // namespace ee

#endif /* EE_X_IRON_SOURCE_BRIDGE_HPP */
