//
//  EEALovinAds.m
//  ee_x
//
//  Created by Pham Xuan Han on 5/12/17.
//
//

#import "ee/app_lovin/EEAppLovin.h"

#import <AppLovinSDK/ALAdRewardDelegate.h>
#import <AppLovinSDK/ALIncentivizedInterstitialAd.h>
#import <AppLovinSDK/ALInterstitialAd.h>
#import <AppLovinSDK/ALSdk.h>

#import <ee-Swift.h>

#import <ee/core/internal/EEJsonUtils.h>

#define kPrefix @"AppLovinBridge"

// clang-format off
static NSString* const k__initialize         = kPrefix "Initialize";
static NSString* const k__setTestAdsEnabled  = kPrefix "SetTestAdsEnabled";
static NSString* const k__setVerboseLogging  = kPrefix "SetVerboseLogging";
static NSString* const k__setMuted           = kPrefix "SetMuted";

static NSString* const k__hasInterstitialAd  = kPrefix "HasInterstitialAd";
static NSString* const k__loadInterstitialAd = kPrefix "LoadInterstitialAd";
static NSString* const k__showInterstitialAd = kPrefix "ShowInterstitialAd";

static NSString* const k__loadRewardedAd     = kPrefix "LoadRewardedAd";
static NSString* const k__hasRewardedAd      = kPrefix "HasRewardedAd";
static NSString* const k__showRewardedAd     = kPrefix "ShowRewardedAd";

static NSString* const k__onInterstitialAdLoaded       = kPrefix "OnInterstitialAdLoaded";
static NSString* const k__onInterstitialAdFailedToLoad = kPrefix "OnInterstitialAdFailedToLoad";
static NSString* const k__onInterstitialAdClicked      = kPrefix "OnInterstitialAdClicked";
static NSString* const k__onInterstitialAdClosed       = kPrefix "OnInterstitialAdClosed";

static NSString* const k__onRewardedAdLoaded       = kPrefix "OnRewardedAdLoaded";
static NSString* const k__onRewardedAdFailedToLoad = kPrefix "OnRewardedAdFailedToLoad";
static NSString* const k__onRewardedAdClicked      = kPrefix "OnRewardedAdClicked";
static NSString* const k__onRewardedAdClosed       = kPrefix "OnRewardedAdClosed";
// clang-format on

#undef kPrefix

@interface EEAppLovinInterstitialAdDelegate
    : NSObject <ALAdLoadDelegate, ALAdDisplayDelegate>
@end

@implementation EEAppLovinInterstitialAdDelegate {
    id<EEIMessageBridge> bridge_;
}

- (id)initWithBridge:(id<EEIMessageBridge>)bridge {
    NSAssert([EEThread isMainThread], @"");
    self = [super init];
    if (self == nil) {
        return nil;
    }
    bridge_ = bridge;
    return self;
}

- (void)adService:(ALAdService*)adService didLoadAd:(ALAd*)ad {
    NSLog(@"%s", __PRETTY_FUNCTION__);
    [bridge_ callCpp:k__onInterstitialAdLoaded];
}

- (void)adService:(ALAdService*)adService didFailToLoadAdWithError:(int)code {
    NSLog(@"%s", __PRETTY_FUNCTION__);
    [bridge_ callCpp:k__onInterstitialAdFailedToLoad //
                    :[@(code) stringValue]];
}

- (void)ad:(ALAd*)ad wasClickedIn:(UIView*)view {
    NSLog(@"%s", __PRETTY_FUNCTION__);
    [bridge_ callCpp:k__onInterstitialAdClicked];
}

- (void)ad:(ALAd*)ad wasDisplayedIn:(UIView*)view {
    NSLog(@"%s", __PRETTY_FUNCTION__);
}

- (void)ad:(ALAd*)ad wasHiddenIn:(UIView*)view {
    NSLog(@"%s", __PRETTY_FUNCTION__);
    [bridge_ callCpp:k__onInterstitialAdClosed];
}

@end

@interface EEAppLovinRewardedAdDelegate
    : NSObject <ALAdLoadDelegate, ALAdDisplayDelegate, ALAdRewardDelegate>
@end

@implementation EEAppLovinRewardedAdDelegate {
    id<EEIMessageBridge> bridge_;

@public
    BOOL rewarded_;
}

- (id)initWithBridge:(id<EEIMessageBridge>)bridge {
    NSAssert([EEThread isMainThread], @"");
    self = [super init];
    if (self == nil) {
        return nil;
    }
    bridge_ = bridge;
    return self;
}

- (void)adService:(ALAdService*)adService didLoadAd:(ALAd*)ad {
    NSLog(@"%s", __PRETTY_FUNCTION__);
    [bridge_ callCpp:k__onRewardedAdLoaded];
}

- (void)adService:(ALAdService*)adService didFailToLoadAdWithError:(int)code {
    NSLog(@"%s", __PRETTY_FUNCTION__);
    [bridge_ callCpp:k__onRewardedAdFailedToLoad //
                    :[@(code) stringValue]];
}

- (void)ad:(ALAd*)ad wasClickedIn:(UIView*)view {
    NSLog(@"%s", __PRETTY_FUNCTION__);
    [bridge_ callCpp:k__onRewardedAdClicked];
}

- (void)ad:(ALAd*)ad wasDisplayedIn:(UIView*)view {
    NSLog(@"%s", __PRETTY_FUNCTION__);
}

- (void)ad:(ALAd*)ad wasHiddenIn:(UIView*)view {
    NSLog(@"%s", __PRETTY_FUNCTION__);
    [bridge_ callCpp:k__onRewardedAdClosed //
                    :[EEUtils toString:rewarded_]];
}

- (void)rewardValidationRequestForAd:(ALAd*)ad
              didSucceedWithResponse:(NSDictionary*)response {
    NSLog(@"%s: %@", __PRETTY_FUNCTION__, response);
    rewarded_ = YES;
}

- (void)rewardValidationRequestForAd:(ALAd*)ad
          didExceedQuotaWithResponse:(NSDictionary*)response {
    NSLog(@"%s: %@", __PRETTY_FUNCTION__, response);
}

- (void)rewardValidationRequestForAd:(ALAd*)ad
             wasRejectedWithResponse:(NSDictionary*)response {
    NSLog(@"%s: %@", __PRETTY_FUNCTION__, response);
}

- (void)rewardValidationRequestForAd:(ALAd*)ad
                    didFailWithError:(NSInteger)responseCode {
    NSLog(@"%s: code = %ld", __PRETTY_FUNCTION__, responseCode);
}

@end

@implementation EEAppLovin {
    id<EEIMessageBridge> bridge_;
    BOOL initialized_;
    ALSdk* sdk_;
    ALInterstitialAd* interstitialAd_;
    EEAppLovinInterstitialAdDelegate* interstitialAdDelegate_;
    ALIncentivizedInterstitialAd* rewardedAd_;
    EEAppLovinRewardedAdDelegate* rewardedAdDelegate_;
}

- (id)init {
    NSAssert([EEThread isMainThread], @"");
    self = [super init];
    if (self == nil) {
        return self;
    }
    initialized_ = NO;
    bridge_ = [EEMessageBridge getInstance];
    [self registerHandlers];
    return self;
}

- (void)dealloc {
    [super dealloc];
}

- (void)destroy {
    NSAssert([EEThread isMainThread], @"");
    [self deregisterHandlers];
    if (!initialized_) {
        return;
    }
    [interstitialAd_ setAdLoadDelegate:nil];
    [interstitialAd_ setAdDisplayDelegate:nil];
    [interstitialAd_ release];
    interstitialAd_ = nil;
    [interstitialAdDelegate_ release];
    interstitialAdDelegate_ = nil;
    [rewardedAd_ setAdDisplayDelegate:nil];
    [rewardedAd_ release];
    rewardedAd_ = nil;
    [rewardedAdDelegate_ release];
    rewardedAdDelegate_ = nil;
    sdk_ = nil;
}

- (void)registerHandlers {
    [bridge_ registerHandler:
               k__initialize:^(NSString* message) {
                   NSString* key = message;
                   [self initialize:key];
                   return @"";
               }];

    [bridge_ registerHandler:
        k__setTestAdsEnabled:^(NSString* message) {
            [self setTestAdsEnabled:[EEUtils toBool:message]];
            return @"";
        }];

    [bridge_ registerHandler:
        k__setVerboseLogging:^(NSString* message) {
            [self setVerboseLogging:[EEUtils toBool:message]];
            return @"";
        }];

    [bridge_ registerHandler:
                 k__setMuted:^(NSString* message) {
                     [self setMuted:[EEUtils toBool:message]];
                     return @"";
                 }];

    [bridge_ registerHandler:
        k__hasInterstitialAd:^(NSString* message) {
            return [EEUtils toString:[self hasInterstitialAd]];
        }];

    [bridge_ registerHandler:
        k__loadInterstitialAd:^(NSString* message) {
            [self loadInterstitialAd];
            return @"";
        }];

    [bridge_ registerHandler:
        k__showInterstitialAd:^(NSString* message) {
            [self showInterstitialAd];
            return @"";
        }];

    [bridge_ registerHandler:
            k__hasRewardedAd:^(NSString* message) {
                return [EEUtils toString:[self hasRewardedAd]];
            }];

    [bridge_ registerHandler:
           k__loadRewardedAd:^(NSString* message) {
               [self loadRewardedAd];
               return @"";
           }];

    [bridge_ registerHandler:
           k__showRewardedAd:^(NSString* message) {
               [self showRewardedAd];
               return @"";
           }];
}

- (void)deregisterHandlers {
    [bridge_ deregisterHandler:k__initialize];
    [bridge_ deregisterHandler:k__setTestAdsEnabled];
    [bridge_ deregisterHandler:k__setVerboseLogging];
    [bridge_ deregisterHandler:k__setMuted];
    [bridge_ deregisterHandler:k__hasInterstitialAd];
    [bridge_ deregisterHandler:k__loadInterstitialAd];
    [bridge_ deregisterHandler:k__showInterstitialAd];
    [bridge_ deregisterHandler:k__hasRewardedAd];
    [bridge_ deregisterHandler:k__loadRewardedAd];
    [bridge_ deregisterHandler:k__showRewardedAd];
}

- (void)initialize:(NSString* _Nonnull)key {
    NSAssert([EEThread isMainThread], @"");
    if (initialized_) {
        return;
    }
    sdk_ = [ALSdk sharedWithKey:key];
    [sdk_ initializeSdk];

    interstitialAd_ = [[ALInterstitialAd alloc] initWithSdk:sdk_];
    interstitialAdDelegate_ =
        [[EEAppLovinInterstitialAdDelegate alloc] initWithBridge:bridge_];
    [interstitialAd_ setAdLoadDelegate:interstitialAdDelegate_];
    [interstitialAd_ setAdDisplayDelegate:interstitialAdDelegate_];

    rewardedAd_ = [[ALIncentivizedInterstitialAd alloc] initWithSdk:sdk_];
    rewardedAdDelegate_ =
        [[EEAppLovinRewardedAdDelegate alloc] initWithBridge:bridge_];
    [rewardedAd_ setAdDisplayDelegate:rewardedAdDelegate_];

    initialized_ = YES;
}

- (void)setTestAdsEnabled:(BOOL)enabled {
    // Removed.
}

- (void)setVerboseLogging:(BOOL)enabled {
    NSAssert([EEThread isMainThread], @"");
    [[sdk_ settings] setIsVerboseLogging:enabled];
}

- (void)setMuted:(BOOL)enabled {
    NSAssert([EEThread isMainThread], @"");
    [[sdk_ settings] setMuted:enabled];
}

- (BOOL)hasInterstitialAd {
    NSAssert([EEThread isMainThread], @"");
    // FIXME: use boolean variable.
    return [interstitialAd_ isReadyForDisplay];
}

- (void)loadInterstitialAd {
    NSAssert([EEThread isMainThread], @"");
    [[sdk_ adService] loadNextAd:[ALAdSize interstitial]
                       andNotify:interstitialAdDelegate_];
}

- (void)showInterstitialAd {
    NSAssert([EEThread isMainThread], @"");
    [interstitialAd_ show];
}

- (BOOL)hasRewardedAd {
    return [rewardedAd_ isReadyForDisplay];
}

- (void)loadRewardedAd {
    NSAssert([EEThread isMainThread], @"");
    [rewardedAd_ preloadAndNotify:rewardedAdDelegate_];
}

- (void)showRewardedAd {
    NSAssert([EEThread isMainThread], @"");
    rewardedAdDelegate_->rewarded_ = NO;
    [rewardedAd_ showAndNotify:rewardedAdDelegate_];
}

@end
