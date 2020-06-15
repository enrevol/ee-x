//
//  EECampaignReceiver.h
//  ee_x
//
//  Created by Pham Xuan Han on 5/12/17.
//
//

@protocol EEIPlugin;

@interface EECampaignReceiver : NSObject <EEIPlugin>

+ (BOOL)application:(UIApplication*)app
            openURL:(nonnull NSURL*)url
            options:(nonnull NSDictionary<NSString*, id>*)options;

@end
