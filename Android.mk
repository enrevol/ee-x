LOCAL_PATH := $(call my-dir)

$(call import-add-path, $(LOCAL_PATH))

$(call import-module, src/ee/adjust)
$(call import-module, src/ee/admob)
$(call import-module, src/ee/ads)
$(call import-module, src/ee/app_lovin)
$(call import-module, src/ee/apps_flyer)
$(call import-module, src/ee/campaign_receiver)
$(call import-module, src/ee/cocos)
$(call import-module, src/ee/core)
$(call import-module, src/ee/facebook)
$(call import-module, src/ee/facebook_ads)
$(call import-module, src/ee/firebase/analytics)
$(call import-module, src/ee/firebase/core)
$(call import-module, src/ee/firebase/crashlytics)
$(call import-module, src/ee/firebase/dynamic_link)
$(call import-module, src/ee/firebase/messaging)
$(call import-module, src/ee/firebase/performance)
$(call import-module, src/ee/firebase/remote_config)
$(call import-module, src/ee/firebase/storage)
$(call import-module, src/ee/google)
$(call import-module, src/ee/iron_source)
$(call import-module, src/ee/jsb/admob)
$(call import-module, src/ee/jsb/ads)
$(call import-module, src/ee/jsb/apps_flyer)
$(call import-module, src/ee/jsb/core)
$(call import-module, src/ee/jsb/crashlytics)
$(call import-module, src/ee/jsb/facebook)
$(call import-module, src/ee/jsb/facebook_ads)
$(call import-module, src/ee/jsb/firebase/analytics)
$(call import-module, src/ee/jsb/firebase/performance)
$(call import-module, src/ee/jsb/firebase/remote_config)
$(call import-module, src/ee/jsb/google)
$(call import-module, src/ee/jsb/iron_source)
$(call import-module, src/ee/jsb/notification)
$(call import-module, src/ee/jsb/recorder)
$(call import-module, src/ee/jsb/soomla)
$(call import-module, src/ee/jsb/unity_ads)
$(call import-module, src/ee/jsb/vungle)
$(call import-module, src/ee/notification)
$(call import-module, src/ee/play)
$(call import-module, src/ee/recorder)
$(call import-module, src/ee/store)
$(call import-module, src/ee/unity_ads)
$(call import-module, src/ee/vungle)
$(call import-module, third_party/firebase_cpp_sdk)
$(call import-module, third_party/jansson)
$(call import-module, third_party/soomla/core)
$(call import-module, third_party/soomla/store)
