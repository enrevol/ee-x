Pod::Spec.new do |spec|
  spec.name           = 'ee-x'
  spec.version        = '0.1.5'
  spec.summary        = 'ee-x'
  spec.description    = 'ee-x'

  spec.homepage       = 'https://github.com/Senspark/ee-x'

  # spec.license        = { :type => 'MIT', :file => 'FILE_LICENSE' }
  spec.author         = 'Hai Hoang'

  spec.ios.deployment_target = '9.0'
  spec.osx.deployment_target = '10.8'

  spec.source = {
    :git => 'https://github.com/Senspark/ee-x.git',
    :branch => 'master'
  }

  spec.framework = 'Foundation'

  spec.requires_arc = false

  spec.header_mappings_dir = 'src'

  spec.subspec 'json' do |s|
    # Don't use source_files: all dirs may be included in header map.
    # s.source_files = 'third_party/nlohmann/include/**/*'
    s.preserve_path = 'third_party/nlohmann/include'
    s.public_header_files = 'third_party/nlohmann/include/**/*'
    s.header_mappings_dir = 'third_party/nlohmann/include'
  end

  spec.subspec 'core' do |s|
    s.source_files =
      'src/ee/Macro.hpp',
      'src/ee/Core*',
      'src/ee/core/**/*'

    s.private_header_files =
      'src/ee/core/private/**'
    
    s.exclude_files =
      'src/ee/core/Android.mk',
      'src/ee/core/CMakeLists.txt',
      'src/ee/core/**/*Android*',
      'src/ee/core/**/Jni*'

    s.xcconfig = {
      'CLANG_CXX_LANGUAGE_STANDARD' => 'c++17'
    }

    s.dependency 'ee-x/json'
  end

  spec.subspec 'ads' do |s|
    s.source_files =
      'src/ee/Ads*',
      'src/ee/ads/**/*'

    s.private_header_files =
      'src/ee/ads/private/*'

    s.exclude_files =
      'src/ee/ads/CMakeLists.txt'

    s.dependency 'ee-x/core'
  end

  spec.subspec 'admob' do |s|
    s.source_files =
      'src/ee/AdMob*',
      'src/ee/admob/**/*'

    s.private_header_files =
      'src/ee/admob/private/*'

    s.exclude_files =
      'src/ee/admob/CMakeLists.txt'

    s.resources = 'res/*'
    s.dependency 'ee-x/ads'
    s.dependency 'Google-Mobile-Ads-SDK'
  end

  spec.subspec 'applovin-base' do |s|
    s.source_files =
      'src/ee/AppLovin*',
      'src/ee/applovin/**/*'

    s.private_header_files =
      'src/ee/applovin/private/*'
    
    s.exclude_files =
      'src/ee/applovin/CMakeLists.txt'

    s.dependency 'ee-x/ads'
  end

  spec.subspec 'applovin' do |s|
    s.dependency 'ee-x/applovin-base'
    s.dependency 'AppLovinSDK'
  end

  spec.subspec 'applovin-mediation' do |s|
    s.dependency 'ee-x/applovin-base'
    s.dependency 'ee-x/ironsource-mediation-base'

    # AppLovinSDK is included in IronSourceAppLovinAdapter.
    s.dependency 'IronSourceAppLovinAdapter'
  end

  spec.subspec 'facebook-ads' do |s|
    s.source_files =
      'src/ee/FacebookAds*',
      'src/ee/facebookads/**/*'

    s.private_header_files =
      'src/ee/facebookads/private/*'

    s.exclude_files =
      'src/ee/facebookads/CMakeLists.txt'
      
    s.dependency 'ee-x/ads'
    s.dependency 'FBAudienceNetwork'
  end

  spec.subspec 'ironsource-mediation-base' do |s|
    s.preserve_path = 'dummy_path'
    s.xcconfig = {
      'GCC_PREPROCESSOR_DEFINITIONS' => [
        'EE_X_USE_IRON_SOURCE_MEDIATION'
      ].join(' ')
    }
  end

  spec.subspec 'ironsource' do |s|
    s.source_files =
      'src/ee/IronSource*',
      'src/ee/ironsource/**/*'

    s.private_header_files =
      'src/ee/ironsource/private/*'

    s.exclude_files =
      'src/ee/ironsource/CMakeLists.txt'

    s.dependency 'ee-x/ads'
    s.dependency 'IronSourceSDK'
  end

  spec.subspec 'unity-ads-base' do |s|
    s.source_files =
      'src/ee/UnityAds*',
      'src/ee/unityads/**/*'

    s.private_header_files =
      'src/ee/unityads/private/*'

    s.exclude_files =
      'src/ee/unityads/CMakeLists.txt'

    s.dependency 'ee-x/ads'
  end

  spec.subspec 'unity-ads' do |s|
    s.dependency 'ee-x/unity-ads-base'
    s.dependency 'UnityAds'
  end

  spec.subspec 'unity-ads-mediation' do |s|
    s.dependency 'ee-x/unity-ads-base'
    s.dependency 'ee-x/ironsource-mediation-base'

    # UnityAds is included in IronSourceUnityAdsAdapter.
    s.dependency 'IronSourceUnityAdsAdapter'
  end

  spec.subspec 'vungle-base' do |s|
    s.source_files =
      'src/ee/Vungle*',
      'src/ee/vungle/**/*'

    s.private_header_files =
      'src/ee/vungle/private/*'

    s.exclude_files =
      'src/ee/vungle/CMakeLists.txt'

    s.dependency 'ee-x/ads'
  end

  spec.subspec 'vungle' do |s|
    s.dependency 'ee-x/vungle-base'
    s.dependency 'VungleSDK-iOS'
  end

  spec.subspec 'vungle-mediation' do |s|
    s.dependency 'ee-x/vungle-base'
    s.dependency 'ee-x/ironsource-mediation-base'

    # VungleSDK-iOS is included in IronSourceVungleAdapter.
    s.dependency 'IronSourceVungleAdapter'
  end

  spec.subspec 'appsflyer' do |s|
    s.source_files =
      'src/ee/AppsFlyer*',
      'src/ee/appsflyer/**/*'

    s.private_header_files =
      'src/ee/appsflyer/private/*'

    s.exclude_files =
      'src/ee/appsflyer/CMakeLists.txt'

    s.dependency 'ee-x/core'
    s.dependency 'AppsFlyerFramework'
  end

  spec.subspec 'campaign-receiver' do |s|
    s.source_files =
      'src/ee/CampaignReceiver*',
      'src/ee/campaignreceiver/**/*'

    s.exclude_files =
      'src/ee/campaignreceiver/CMakeLists.txt'

    s.dependency 'ee-x/core'
  end

  spec.subspec 'crashlytics' do |s|
    s.source_files =
      'src/ee/Crashlytics*',
      'src/ee/crashlytics/**/*'

    s.exclude_files =
      'src/ee/crashlytics/CMakeLists.txt'

    s.dependency 'ee-x/core'
    s.dependency 'Crashlytics'
    s.dependency 'Fabric'
  end

  spec.subspec 'cocos' do |s|
    s.source_files =
      'src/ee/cocos/*'

    s.xcconfig = {
      'HEADER_SEARCH_PATHS' => [
        '${PODS_ROOT}/../../cocos2d',
        '${PODS_ROOT}/../../cocos2d/cocos',
        '${PODS_ROOT}/../../cocos2d/cocos/editor-support',
        '${PODS_ROOT}/../../cocos2d/external',
        '${PODS_ROOT}/../../cocos2d/extensions',

        # For macos.
        '${PODS_ROOT}/../../cocos2d/external/mac/include/glfw3', # Cocos Creator
        '${PODS_ROOT}/../../cocos2d/external/glfw3/include/mac' # Cocos2d-x
      ].join(' ')
    }

    s.ios.library = 'iconv'
    s.ios.framework = 'OpenAL'
  end

  spec.subspec 'facebook' do |s|
    s.source_files =
      'src/ee/Facebook{,Fwd}.*',
      'src/ee/facebook/**/*'

    s.private_header_files =
      'src/ee/facebook/private/*'

    s.exclude_files =
      'src/ee/facebook/CMakeLists.txt'

    s.dependency 'ee-x/core'
    s.dependency 'FBSDKCoreKit'
    s.dependency 'FBSDKLoginKit'
    s.dependency 'FBSDKShareKit'
  end

  spec.subspec 'firebase-headers' do |s|
    # Don't use source_files: all dirs may be included in header map.
    # Conflict google_play_services/availability.h and macOS Availability.h
    # s.source_files = 'third_party/firebase_cpp_sdk/include/**/*'
    s.preserve_path = 'third_party/firebase_cpp_sdk/include'
    s.public_header_files = 'third_party/firebase_cpp_sdk/include/**/*'
    s.header_mappings_dir = 'third_party/firebase_cpp_sdk/include'
    s.platform = :ios
  end

  spec.subspec 'firebase-core' do |s|
    s.source_files =
      'src/ee/Firebase*',
      'src/ee/firebase/core/*'

    s.exclude_files =
      'src/ee/firebase/**/*Android*'

    s.ios.vendored_library = 'third_party/firebase_cpp_sdk/libs/ios/universal/libfirebase_app.a'
    s.dependency 'ee-x/core'
    s.ios.dependency 'ee-x/firebase-headers'
    s.ios.dependency 'Firebase/Core'
  end

  spec.subspec 'firebase-analytics' do |s|
    s.source_files = 'src/ee/firebase/analytics/*'
    s.ios.vendored_library = 'third_party/firebase_cpp_sdk/libs/ios/universal/libfirebase_analytics.a'
    s.dependency 'ee-x/firebase-core'
  end

  spec.subspec 'firebase-dynamic-link' do |s|
    s.source_files = 'src/ee/firebase/dynamiclink/*'
    s.ios.vendored_library = 'third_party/firebase_cpp_sdk/libs/ios/universal/libfirebase_dynamic_links.a'
    s.dependency 'ee-x/firebase-core'
    s.ios.dependency 'Firebase/DynamicLinks'
  end

  spec.subspec 'firebase-messaging' do |s|
    s.source_files = 'src/ee/firebase/messaging/*'
    s.ios.vendored_library = 'third_party/firebase_cpp_sdk/libs/ios/universal/libfirebase_messaging.a'
    s.dependency 'ee-x/firebase-core'
    s.ios.dependency 'Firebase/Messaging'
  end

  spec.subspec 'firebase-remote-config' do |s|
    s.source_files = 'src/ee/firebase/remoteconfig/*'
    s.ios.vendored_library = 'third_party/firebase_cpp_sdk/libs/ios/universal/libfirebase_remote_config.a'
    s.dependency 'ee-x/firebase-core'
    s.ios.dependency 'Firebase/RemoteConfig'
  end

  spec.subspec 'firebase-storage' do |s|
    s.source_files = 'src/ee/firebase/storage/*'
    s.ios.vendored_library = 'third_party/firebase_cpp_sdk/libs/ios/universal/libfirebase_storage.a'
    s.dependency 'ee-x/firebase-core'
    s.ios.dependency 'Firebase/Storage'
  end

  spec.subspec 'firebase-performance' do |s|
    s.source_files = 'src/ee/firebase/performance/**/*'
    s.private_header_files = 'src/ee/firebase/performance/private/*'
    s.dependency 'ee-x/firebase-core'
    s.ios.dependency 'Firebase/Performance'
  end

  spec.subspec 'google-analytics' do |s|
    s.source_files =
      'src/ee/GoogleAnalytics*',
      'src/ee/google/**/*'

    s.private_header_files =
      'src/ee/google/private/*'

    s.exclude_files =
      'src/ee/google/CMakeLists.txt'

    s.platform = :ios
    s.dependency 'ee-x/core'
    s.dependency 'GoogleAnalytics'
  end

  spec.subspec 'notification' do |s|
    s.source_files =
      'src/ee/Notification*',
      'src/ee/notification/**/*'

    s.exclude_files =
      'src/ee/notification/CMakeLists.txt'

    s.platform = :ios
    s.dependency 'ee-x/core'
  end

  spec.subspec 'play' do |s|
    s.source_files =
      'src/ee/Play*',
      'src/ee/play/*'

    s.exclude_files =
      'src/ee/play/CMakeLists.txt'

    s.dependency 'ee-x/core'
  end

  spec.subspec 'recorder' do |s|
    s.source_files =
      'src/ee/Recorder*',
      'src/ee/recorder/*'

    s.dependency 'ee-x/core'
  end

  spec.subspec 'tenjin' do |s|
    s.source_files =
      'src/ee/Tenjin*',
      'src/ee/tenjin/**/*'

    s.dependency 'ee-x/core'
    s.dependency 'TenjinSDK'
  end

  spec.subspec 'twitter' do |s|
    s.source_files =
      'src/ee/Twitter*',
      'src/ee/twitter/**/*'

    s.dependency 'ee-x/core'
    s.dependency 'TwitterKit'
  end

  spec.subspec 'jansson' do |s| 
    s.source_files = 'third_party/jansson/*'
    s.header_mappings_dir = 'third_party'
  end

  spec.subspec 'keeva' do |s|
    s.source_files = 'third_party/keeva/*'
    s.header_mappings_dir = 'third_party'
  end

  spec.subspec 'soomla-ios-core' do |s|
    s.source_files = 'third_party/soomla/SoomlaiOSCore/**/*'
    s.header_mappings_dir = 'third_party/soomla'
    s.dependency 'ee-x/keeva'
  end

  spec.subspec 'soomla-cocos2dx-core' do |s|
    s.source_files = 'third_party/soomla/SoomlaCocos2dxCore/**/*'
    s.exclude_files = 'third_party/soomla/SoomlaCocos2dxCore/Soomla/jsb/*'
    s.header_mappings_dir = 'third_party/soomla'

    s.xcconfig = {
      'HEADER_SEARCH_PATHS' => [
        '${PODS_ROOT}/../../cocos2d',
        '${PODS_ROOT}/../../cocos2d/cocos',
        '${PODS_ROOT}/../../../cocos2d-x',
        '${PODS_ROOT}/../../../cocos2d-x/cocos',
        "${PODS_ROOT}/Headers/Public/#{spec.name}/SoomlaCocos2dxCore/Soomla/**"
      ].join(' ')
    }
    
    s.dependency 'ee-x/json'
    s.dependency 'ee-x/jansson'
    s.dependency 'ee-x/keeva'
    s.dependency 'ee-x/soomla-ios-core'
  end

  spec.subspec 'soomla-ios-store' do |s| 
    s.source_files = 'third_party/soomla/SoomlaiOSStore/**/*'
    s.header_mappings_dir = 'third_party/soomla'
    s.dependency 'ee-x/keeva'
    s.dependency 'ee-x/soomla-ios-core'
  end

  spec.subspec 'soomla-cocos2dx-store' do |s| 
    s.source_files = 'third_party/soomla/SoomlaCocos2dxStore/**/*'
    s.header_mappings_dir = 'third_party/soomla'
    s.dependency 'ee-x/soomla-cocos2dx-core'
    s.dependency 'ee-x/soomla-ios-store'
  end

  spec.subspec 'jsb-core' do |s|
    s.source_files =
      'src/ee/jsb/jsb_core*',
      'src/ee/jsb/jsb_fwd.hpp',
      'src/ee/jsb/core/*'

    s.xcconfig = {
      'HEADER_SEARCH_PATHS' => [
        '${PODS_ROOT}/../../../cocos2d-x',
        '${PODS_ROOT}/../../../cocos2d-x/cocos',
        '${PODS_ROOT}/../../../cocos2d-x/cocos/editor-support',
        '${PODS_ROOT}/../../../cocos2d-x/external/sources',

        # For macos.
        '${PODS_ROOT}/../../../cocos2d-x/external/mac/include/v8'
      ].join(' ')
    }
    
    s.dependency 'ee-x/core'
  end

  spec.subspec 'jsb-ads' do |s|
    s.source_files =
      'src/ee/jsb/jsb_ads*',
      'src/ee/jsb/ads/*'

    s.dependency 'ee-x/jsb-core'
    s.dependency 'ee-x/ads'
  end

  spec.subspec 'jsb-admob' do |s|
    s.source_files =
      'src/ee/jsb/jsb_admob*',
      'src/ee/jsb/admob/*'

    s.dependency 'ee-x/jsb-core'
    s.dependency 'ee-x/admob'
  end

  spec.subspec 'jsb-facebook-ads' do |s|
    s.source_files =
      'src/ee/jsb/jsb_facebook_ads*',
      'src/ee/jsb/facebookads/*'

    s.dependency 'ee-x/jsb-core'
    s.dependency 'ee-x/facebook-ads'
  end

  spec.subspec 'jsb-ironsource' do |s|
    s.source_files =
      'src/ee/jsb/jsb_ironsource*',
      'src/ee/jsb/ironsource/*'

    s.dependency 'ee-x/jsb-core'
    s.dependency 'ee-x/ironsource'
  end

  spec.subspec 'jsb-unity-ads' do |s|
    s.source_files =
      'src/ee/jsb/jsb_unity_ads*',
      'src/ee/jsb/unityads/*'

    s.dependency 'ee-x/jsb-core'
    s.dependency 'ee-x/unity-ads'
  end

  spec.subspec 'jsb-vungle' do |s|
    s.source_files =
      'src/ee/jsb/jsb_vungle*',
      'src/ee/jsb/vungle/*'

    s.dependency 'ee-x/jsb-core'
    s.dependency 'ee-x/vungle'
  end

  spec.subspec 'jsb-crashlytics' do |s|
    s.source_files =
      'src/ee/jsb/jsb_crashlytics*',
      'src/ee/jsb/crashlytics/*'
    
    s.dependency 'ee-x/jsb-core'
    s.dependency 'ee-x/crashlytics'
  end

  spec.subspec 'jsb-facebook' do |s|
    s.source_files =
      'src/ee/jsb/jsb_facebook*',
      'src/ee/jsb/facebook/*'

    s.dependency 'ee-x/jsb-core'
    s.dependency 'ee-x/facebook'
  end

  spec.subspec 'jsb-firebase' do |s|
    s.source_files =
      'src/ee/jsb/jsb_firebase*',
      'src/ee/jsb/firebase/*'

    s.dependency 'ee-x/jsb-core'
    s.dependency 'ee-x/firebase-analytics'
    s.dependency 'ee-x/firebase-performance'
    s.dependency 'ee-x/firebase-remote-config'
  end

  spec.subspec 'jsb-google-analytics' do |s|
    s.source_files =
      'src/ee/jsb/jsb_google_analytics*',
      'src/ee/jsb/google/*'
    
    s.dependency 'ee-x/jsb-core'
    s.dependency 'ee-x/google-analytics'
  end
  
  spec.subspec 'jsb-notification' do |s|
    s.source_files =
      'src/ee/jsb/jsb_notification*',
      'src/ee/jsb/notification/*'

    s.dependency 'ee-x/jsb-core'
    s.dependency 'ee-x/notification'
  end

  spec.subspec 'jsb-recorder' do |s|
    s.source_files =
      'src/ee/jsb/jsb_recorder.*',
      'src/ee/jsb/recorder/*'

    s.dependency 'ee-x/jsb-core'
    s.dependency 'ee-x/recorder'
  end
  
  spec.subspec 'jsb-soomla-store' do |s|
    s.source_files =
      'src/ee/jsb/jsb_soomla*',
      'src/ee/jsb/soomla/*'
    
    s.dependency 'ee-x/jsb-core'
    s.dependency 'ee-x/soomla-cocos2dx-store'
  end
end

#  post_install do |installer_representation|
#    installer_representation.project.targets.each do |target|
#      if target.name == "FBSDKShareKit"
#        target.build_configurations.each do |config|
#          config.build_settings['GCC_PREPROCESSOR_DEFINITIONS'] ||= ['$(inherited)', 'BUCK']
#        end
#      end
#    end
#  end
