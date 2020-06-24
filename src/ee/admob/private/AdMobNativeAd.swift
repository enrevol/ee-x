//
//  AdMobNativeAd.swift
//  ee-x-366f64ba
//
//  Created by eps on 6/24/20.
//

import Foundation
import GoogleMobileAds

/// https://stackoverflow.com/a/24592029
func dictionaryOfNames(_ arr: UIView...) -> [String: UIView] {
    var d = [String: UIView]()
    for (ix, v) in arr.enumerated() {
        d["v\(ix + 1)"] = v
    }
    return d
}

class AdMobNativeAd: NSObject, IAdView,
    GADUnifiedNativeAdLoaderDelegate,
    GADUnifiedNativeAdDelegate {
    private let _bridge: IMessageBridge
    private let _adId: String
    private let _layoutName: String
    private let _messageHelper: MessageHelper
    private var _helper: AdViewHelper?
    private var _isLoaded = false
    private var _ad: GADAdLoader?
    private var _view: UIView?
    private var _adView: GADUnifiedNativeAdView?
    private var _viewHelper: ViewHelper?

    init(_ bridge: IMessageBridge,
         _ adId: String,
         _ layoutName: String) {
        _bridge = bridge
        _adId = adId
        _layoutName = layoutName
        _messageHelper = MessageHelper("AdMobNativeAd", _adId)
        super.init()
        _helper = AdViewHelper(_bridge, self, _messageHelper)
        registerHandlers()
        createInternalAd()
        createView()
    }

    func destroy() {
        deregisterHandlers()
        destroyView()
        destroyInternalAd()
    }

    func registerHandlers() {
        _helper?.registerHandlers()
    }

    func deregisterHandlers() {
        _helper?.deregisterHandlers()
    }

    func createInternalAd() {
        Thread.runOnMainThread {
            if self._ad == nil {
                return
            }
            self._isLoaded = false
            let options = GADVideoOptions()
            options.startMuted = true
            let rootView = Utils.getCurrentRootViewController()
            let ad = GADAdLoader(adUnitID: self._adId,
                                 rootViewController: rootView,
                                 adTypes: [GADAdLoaderAdType.unifiedNative],
                                 options: [options])
            ad.delegate = self
            self._ad = ad
        }
    }

    func destroyInternalAd() {
        Thread.runOnMainThread {
            guard let ad = self._ad else {
                return
            }
            self._isLoaded = false
            ad.delegate = nil
            self._ad = nil
        }
    }

    func createView() {
        Thread.runOnMainThread {
            assert(self._view == nil)
            let view = UIView()
            view.isHidden = true
            let adView = self.createAdView()
            view.addSubview(adView)
            view.translatesAutoresizingMaskIntoConstraints = false
            let dict = dictionaryOfNames(adView)
            view.addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "H:|[_adView]|",
                                                               options: NSLayoutConstraint.FormatOptions(rawValue: 0),
                                                               metrics: nil,
                                                               views: dict))
            view.addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "V:[[_adView]|",
                                                               options: NSLayoutConstraint.FormatOptions(rawValue: 0),
                                                               metrics: nil,
                                                               views: dict))
            self._view = view
            self._adView = adView
            self._viewHelper = ViewHelper(view)
            let rootView = Utils.getCurrentRootViewController()
            rootView?.view.addSubview(view)
        }
    }

    func destroyView() {
        Thread.runOnMainThread {
            guard let view = self._view else {
                return
            }
            view.removeFromSuperview()
            self._view = nil
            self._viewHelper = nil
        }
    }

    var isLoaded: Bool {
        return _isLoaded
    }

    func load() {
        Thread.runOnMainThread {
            guard let ad = self._ad else {
                assert(false, "Ad is not initialized")
            }
            ad.load(GADRequest())
        }
    }

    var position: CGPoint {
        get { return _viewHelper?.position ?? CGPoint.zero }
        set(value) { _viewHelper?.position = value }
    }

    var size: CGSize {
        get { return _viewHelper?.size ?? CGSize.zero }
        set(value) { _viewHelper?.size = value }
    }

    var isVisible: Bool {
        get { return _viewHelper?.isVisible ?? false }
        set(value) {
            _viewHelper?.isVisible = value
            if value {
                _adView?.subviews.forEach { $0.setNeedsDisplay() }
            }
        }
    }

    func createAdView() -> GADUnifiedNativeAdView {
        guard let nibObjects = Bundle(for: GADUnifiedNativeAdView.self).loadNibNamed(_layoutName,
                                                                                     owner: nil,
                                                                                     options: nil) else {
            assert(false, "Invalid layout")
        }
        guard let view = nibObjects[0] as? GADUnifiedNativeAdView else {
            assert(false, "Invalid layout class")
        }
        return view
    }

    /// Gets an image representing the number of stars. Returns nil if rating is
    /// less than 3.5 stars.
    func imageForStars(_ stars: Float) -> UIImage? {
        switch stars {
            case _ where stars >= 5: return UIImage(named: "stars_5")
            case _ where stars >= 4.5: return UIImage(named: "stars_4_5")
            case _ where stars >= 4: return UIImage(named: "stars_4")
            case _ where stars >= 3.5: return UIImage(named: "stars_3_5")
            default: return nil
        }
    }

    func adLoader(_ adLoader: GADAdLoader, didReceive ad: GADUnifiedNativeAd) {
        guard let adView = _adView else {
            assert(false, "Ad view is not initialized")
        }

        // Populate the native ad view with the native ad assets.
        // The headline and mediaContent are guaranteed to be present in every
        // native ad.
        (adView.headlineView as? UILabel)?.text = ad.headline
        adView.mediaView?.mediaContent = ad.mediaContent

        // These assets are not guaranteed to be present. Check that they are before
        // showing or hiding them.
        (adView.bodyView as? UILabel)?.text = ad.body
        adView.bodyView?.isHidden = ad.body == nil

        (adView.callToActionView as? UIButton)?.setTitle(ad.callToAction, for: UIControl.State.normal)
        adView.callToActionView?.isHidden = ad.callToAction == nil

        (adView.iconView as? UIImageView)?.image = ad.icon?.image
        adView.iconView?.isHidden = ad.icon == nil

        (adView.starRatingView as? UIImageView)?.image = imageForStars(ad.starRating?.floatValue ?? 0)
        adView.starRatingView?.isHidden = ad.starRating == nil

        (adView.storeView as? UILabel)?.text = ad.store
        adView.storeView?.isHidden = ad.store == nil

        (adView.priceView as? UILabel)?.text = ad.price
        adView.priceView?.isHidden = ad.price == nil

        (adView.advertiserView as? UILabel)?.text = ad.advertiser
        adView.advertiserView?.isHidden = ad.advertiser == nil

        // In order for the SDK to process touch events properly, user interaction
        // should be disabled.
        adView.callToActionView?.isUserInteractionEnabled = false

        // Set ourselves as the ad delegate to be notified of native ad events.
        ad.delegate = self

        adView.nativeAd = ad

        _isLoaded = true
        _bridge.callCpp(_messageHelper.onLoaded)
    }

    func adLoader(_ adLoader: GADAdLoader, didFailToReceiveAdWithError error: GADRequestError) {
        print("\(#function): \(error.localizedDescription)")
        _bridge.callCpp(_messageHelper.onFailedToLoad, error.localizedDescription)
    }

    func adLoaderDidFinishLoading(_ adLoader: GADAdLoader) {
        print("\(#function)")
    }

    func nativeAdDidRecordImpression(_ nativeAd: GADUnifiedNativeAd) {
        print("\(#function)")
    }

    func nativeAdDidRecordClick(_ nativeAd: GADUnifiedNativeAd) {
        print("\(#function)")
    }

    func nativeAdWillPresentScreen(_ nativeAd: GADUnifiedNativeAd) {
        print("\(#function)")
    }

    func nativeAdWillDismissScreen(_ nativeAd: GADUnifiedNativeAd) {
        print("\(#function)")
    }

    func nativeAdDidDismissScreen(_ nativeAd: GADUnifiedNativeAd) {
        print("\(#function)")
    }

    func nativeAdWillLeaveApplication(_ nativeAd: GADUnifiedNativeAd) {
        print("\(#function)")
        _bridge.callCpp(_messageHelper.onClicked)
    }
}
