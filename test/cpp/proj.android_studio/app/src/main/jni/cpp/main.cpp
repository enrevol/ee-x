#include <jni.h>

#include "AppDelegate.hpp"

namespace {
std::unique_ptr<eetest::AppDelegate> appDelegate;
} // namespace

void cocos_android_app_init(JNIEnv* env) {
    appDelegate = std::make_unique<eetest::AppDelegate>();
}
