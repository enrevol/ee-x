//
//  AdViewHelper.cpp
//  ee_x
//
//  Created by Zinge on 10/12/17.
//
//

#include "ee/ads/internal/AdViewHelper.hpp"

#include <ee/nlohmann/json.hpp>

#include <ee/core/IMessageBridge.hpp>
#include <ee/core/Utils.hpp>

namespace ee {
namespace ads {
using Self = AdViewHelper;

Self::AdViewHelper(IMessageBridge& bridge, const MessageHelper& helper,
                   const std::pair<int, int>& size)
    : bridge_(bridge)
    , helper_(helper)
    , anchor_(0, 0)
    , position_(0, 0)
    , size_(size)
    , visible_(false) {}

const std::pair<int, int> Self::getSizeInternal() const {
    return size_;
}

void Self::setSizeInternal(int width, int height) {
    size_ = std::pair(width, height);
}

const std::pair<float, float> Self::getAnchorInternal() const {
    return anchor_;
}

void Self::setAnchorInternal(float x, float y) {
    anchor_ = std::pair(x, y);
}

const std::pair<int, int> Self::getPositionInternal() const {
    return position_;
}

void Self::setPositionInternal(int x, int y) {
    position_ = std::pair(x, y);
}

bool Self::isVisibleInternal() const {
    return visible_;
}
void Self::setVisibleInternal(bool visible) {
    visible_ = visible;
}

bool Self::isLoaded() const {
    auto response = bridge_.call(helper_.isLoaded());
    return core::toBool(response);
}

void Self::load() {
    bridge_.call(helper_.load());
}

std::pair<float, float> Self::getAnchor() const {
    return getAnchorInternal();
}

void Self::setAnchor(float x, float y) {
    auto [width, height] = getSizeInternal();
    auto [anchorX, anchorY] = getAnchorInternal();
    auto [positionX, positionY] = getPositionInternal();
    setPositionTopLeft(positionX - static_cast<int>((x - anchorX) * width),
                       positionY - static_cast<int>((y - anchorY) * height));
    setAnchorInternal(x, y);
}

std::pair<int, int> Self::getPosition() const {
    auto [width, height] = getSizeInternal();
    auto [anchorX, anchorY] = getAnchorInternal();
    auto [x, y] = getPositionInternal();
    return std::pair(x + anchorX * width, y + anchorY * height);
}

void Self::setPosition(int x, int y) {
    auto [width, height] = getSizeInternal();
    auto [anchorX, anchorY] = getAnchorInternal();
    setPositionTopLeft(x - static_cast<int>(anchorX * width),
                       y - static_cast<int>(anchorY * height));
}

std::pair<int, int> Self::getPositionTopLeft() const {
    return getPositionInternal();
    /* Direct call.
    assert(isMainThread());
    auto response = bridge_.call(helper_.getPosition());
    auto json = nlohmann::json::parse(response);
    auto x = json["x"].get<int>();
    auto y = json["y"].get<int>();
    return std::pair(x, y);
     */
}

void Self::setPositionTopLeft(int x, int y) {
    nlohmann::json json;
    json["x"] = x;
    json["y"] = y;
    bridge_.call(helper_.setPosition(), json.dump());
    setPositionInternal(x, y);
}

std::pair<int, int> Self::getSize() const {
    return getSizeInternal();
    /* Direct call.
    assert(isMainThread());
    auto response = bridge_.call(helper_.getSize());
    auto json = nlohmann::json::parse(response);
    auto width = json["width"].get<int>();
    auto height = json["height"].get<int>();
    return std::make_pair(width, height);
     */
}

void Self::setSize(int width, int height) {
    auto [currentWidth, currentHeight] = getSizeInternal();
    auto [anchorX, anchorY] = getAnchorInternal();
    auto [x, y] = getPositionInternal();
    setPositionTopLeft(
        x - static_cast<int>((width - currentWidth) * anchorX),
        y - static_cast<int>((height - currentHeight) * anchorY));

    nlohmann::json json;
    json["width"] = width;
    json["height"] = height;
    bridge_.call(helper_.setSize(), json.dump());
    setSizeInternal(width, height);
}

bool Self::isVisible() const {
    return isVisibleInternal();
    /* Direct call.
    assert(isMainThread());
    auto response = bridge_.call(helper_.isVisible());
    return core::toBool(response);
     */
}

void Self::setVisible(bool visible) {
    bridge_.call(helper_.setVisible(), core::toString(visible));
    setVisibleInternal(visible);
}
} // namespace ads
} // namespace ee
