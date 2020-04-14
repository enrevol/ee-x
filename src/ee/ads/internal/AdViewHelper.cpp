//
//  AdViewHelper.cpp
//  ee_x
//
//  Created by Zinge on 10/12/17.
//
//

#include "ee/ads/internal/AdViewHelper.hpp"

#include <ee/nlohmann/json.hpp>

#include <ee/core/Utils.hpp>
#include <ee/core/internal/IMessageBridge.hpp>

namespace ee {
namespace ads {
using Self = AdViewHelper;

Self::AdViewHelper(IMessageBridge& bridge, const std::string& prefix,
                   const std::string& adId)
    : bridge_(bridge)
    , prefix_(prefix)
    , adId_(adId)
    , anchorX_(0.0f)
    , anchorY_(0.0f) {}

bool Self::isLoaded() const {
    auto response = bridge_.call(k__isLoaded());
    return core::toBool(response);
}

void Self::load() {
    auto response = bridge_.call(k__load());
}

std::pair<float, float> Self::getAnchor() const {
    return std::tie(anchorX_, anchorY_);
}

void Self::setAnchor(float x, float y) {
    int width, height;
    std::tie(width, height) = getSize();
    int positionX, positionY;
    std::tie(positionX, positionY) = getPositionTopLeft();
    setPositionTopLeft(positionX - static_cast<int>((x - anchorX_) * width),
                       positionY - static_cast<int>((y - anchorY_) * height));
    anchorX_ = x;
    anchorY_ = y;
}

std::pair<int, int> Self::getPosition() const {
    int width, height;
    std::tie(width, height) = getSize();
    float anchorX, anchorY;
    std::tie(anchorX, anchorY) = getAnchor();
    int x, y;
    std::tie(x, y) = getPositionTopLeft();
    return std::make_pair(x + anchorX * width, y + anchorY * height);
}

void Self::setPosition(int x, int y) {
    int width, height;
    std::tie(width, height) = getSize();
    float anchorX, anchorY;
    std::tie(anchorX, anchorY) = getAnchor();
    setPositionTopLeft(x - static_cast<int>(anchorX * width),
                       y - static_cast<int>(anchorY * height));
}

std::pair<int, int> Self::getPositionTopLeft() const {
    auto response = bridge_.call(k__getPosition());
    auto json = nlohmann::json::parse(response);
    auto x = json["x"].get<int>();
    auto y = json["y"].get<int>();
    return std::make_pair(x, y);
}

void Self::setPositionTopLeft(int x, int y) {
    nlohmann::json json;
    json["x"] = x;
    json["y"] = y;
    bridge_.call(k__setPosition(), json.dump());
}

std::pair<int, int> Self::getSize() const {
    auto response = bridge_.call(k__getSize());
    auto json = nlohmann::json::parse(response);
    auto width = json["width"].get<int>();
    auto height = json["height"].get<int>();
    return std::make_pair(width, height);
}

void Self::setSize(int width, int height) {
    int currentWidth, currentHeight;
    std::tie(currentWidth, currentHeight) = getSize();
    float anchorX, anchorY;
    std::tie(anchorX, anchorY) = getAnchor();
    int x, y;
    std::tie(x, y) = getPositionTopLeft();
    setPositionTopLeft(
        x - static_cast<int>((width - currentWidth) * anchorX),
        y - static_cast<int>((height - currentHeight) * anchorY));

    nlohmann::json json;
    json["width"] = width;
    json["height"] = height;
    bridge_.call(k__setSize(), json.dump());
}

bool Self::isVisible() const {
    auto response = bridge_.call(k__isVisible());
    return core::toBool(response);
}

void Self::setVisible(bool visible) {
    bridge_.call(k__setVisible(), core::toString(visible));
}

std::string Self::k__isLoaded() const {
    return prefix_ + "_isLoaded_" + adId_;
}

std::string Self::k__load() const {
    return prefix_ + "_load_" + adId_;
}

std::string Self::k__getPosition() const {
    return prefix_ + "_getPosition_" + adId_;
}

std::string Self::k__setPosition() const {
    return prefix_ + "_setPosition_" + adId_;
}

std::string Self::k__getSize() const {
    return prefix_ + "_getSize_" + adId_;
}

std::string Self::k__setSize() const {
    return prefix_ + "_setSize_" + adId_;
}

std::string Self::k__isVisible() const {
    return prefix_ + "_isVisible_" + adId_;
}

std::string Self::k__setVisible() const {
    return prefix_ + "_setVisible_" + adId_;
}
} // namespace ads
} // namespace ee
