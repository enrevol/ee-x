﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

using Jsonite;

namespace EE.Internal {
    internal static class AdsConfigUtils {
        public static AdNetwork ParseNetwork(string id) {
            switch (id) {
                case "ad_mob": return AdNetwork.AdMob;
                case "facebook_ads": return AdNetwork.FacebookAds;
                case "iron_source": return AdNetwork.IronSource;
                case "unity_ads": return AdNetwork.UnityAds;
                default: return AdNetwork.Null;
            }
        }

        public static AdFormat ParseFormat(string id) {
            switch (id) {
                case "banner": return AdFormat.Banner;
                case "rect": return AdFormat.Rectangle;
                case "app_open": return AdFormat.AppOpen;
                case "interstitial": return AdFormat.Interstitial;
                case "rewarded_interstitial": return AdFormat.RewardedInterstitial;
                case "rewarded": return AdFormat.Rewarded;
                default: return AdFormat.Null;
            }
        }
    }

    internal class IntervalConfig {
        private readonly Func<ICapper> _creator;

        public IntervalConfig(JsonObject node, string key) {
            _creator = () => new LockCapper();
            if (!node.TryGetValue(key, out var value)) {
                return;
            }
            _creator = () => new Capper(Convert.ToSingle(value));
        }

        public ICapper Create() {
            return _creator();
        }
    }

    internal class RetrierConfig {
        private readonly Func<IRetrier> _creator;

        public RetrierConfig(JsonObject node, string key) {
            _creator = () => new NullRetrier();
            if (!node.TryGetValue(key, out var value)) {
                return;
            }
            if (!(value is JsonArray array)) {
                return;
            }
            if (array.Count < 3) {
                return;
            }
            _creator = () => new Retrier(
                Convert.ToSingle(array[0]),
                Convert.ToSingle(array[1]),
                Convert.ToSingle(array[2]));
        }

        public IRetrier Create() {
            return _creator();
        }
    }

    internal interface INetworkConfigManager {
        Task Initialize();
        void AddTestDevice(string hash);
        void OpenTestSuite(AdNetwork network);
        IAd CreateAd(AdNetwork network, AdFormat format, string id);
    }

    internal class NetworkConfigManager : INetworkConfigManager {
        private readonly List<INetworkConfig> _networks;

        public NetworkConfigManager(JsonObject node) {
            var networks = node.TryGetValue("networks", out var value)
                ? (JsonArray) value
                : new JsonArray();
            _networks = networks.Select(entry => NetworkConfig.Parse((JsonObject) entry)).ToList();
        }

        public async Task Initialize() {
            foreach (var network in _networks) {
                await network.Initialize();
            }
        }

        public void AddTestDevice(string hash) {
            foreach (var item in _networks) {
                item.AddTestDevice(hash);
            }
        }

        public void OpenTestSuite(AdNetwork network) {
            foreach (var item in _networks) {
                if (item.Network == network) {
                    item.OpenTestSuite();
                    break;
                }
            }
        }

        public IAd CreateAd(AdNetwork network, AdFormat format, string id) {
            foreach (var item in _networks) {
                if (item.Network == network) {
                    return item.CreateAd(format, id);
                }
            }
            return new NullAd();
        }
    }

    internal interface INetworkConfig {
        Task Initialize();
        AdNetwork Network { get; }
        void AddTestDevice(string hash);
        void OpenTestSuite();
        IAd CreateAd(AdFormat format, string id);
    }

    internal static class NetworkConfig {
        public static INetworkConfig Parse(JsonObject node) {
            var network = AdsConfigUtils.ParseNetwork((string) node["network"]);
            switch (network) {
                case AdNetwork.AdMob: return new AdMobConfig(node);
                case AdNetwork.FacebookAds: return new FacebookAdsConfig(node);
                case AdNetwork.IronSource: return new IronSourceConfig(node);
                case AdNetwork.UnityAds: return new UnityAdsConfig(node);
                case AdNetwork.Null: return new NullNetworkConfig();
            }
            throw new ArgumentException();
        }
    }

    internal class AdMobConfig : INetworkConfig {
        private IAdMob _plugin;

        public AdMobConfig(JsonObject node) {
        }

        public async Task Initialize() {
            _plugin = PluginManager.CreatePlugin<IAdMob>();
            await _plugin.Initialize();
        }

        public AdNetwork Network => AdNetwork.AdMob;

        public void AddTestDevice(string hash) {
            _plugin.AddTestDevice(hash);
        }

        public void OpenTestSuite() {
            _plugin.OpenTestSuite();
        }

        public IAd CreateAd(AdFormat format, string id) {
            switch (format) {
                case AdFormat.Banner:
                    return _plugin.CreateBannerAd(id, AdMobBannerAdSize.Normal);
                case AdFormat.Rectangle:
                    return _plugin.CreateBannerAd(id, AdMobBannerAdSize.MediumRectangle);
                case AdFormat.AppOpen:
                    return _plugin.CreateAppOpenAd(id);
                case AdFormat.Interstitial:
                    return _plugin.CreateInterstitialAd(id);
                case AdFormat.RewardedInterstitial:
                    return _plugin.CreateRewardedInterstitialAd(id);
                case AdFormat.Rewarded:
                    return _plugin.CreateRewardedAd(id);
                case AdFormat.Null:
                    return new NullAd();
            }
            throw new ArgumentException();
        }
    }

    internal class FacebookAdsConfig : INetworkConfig {
        private IFacebookAds _plugin;

        public FacebookAdsConfig(JsonObject node) {
        }

        public async Task Initialize() {
            _plugin = PluginManager.CreatePlugin<IFacebookAds>();
            await _plugin.Initialize();
        }

        public AdNetwork Network => AdNetwork.FacebookAds;

        public void AddTestDevice(string hash) {
            _plugin.AddTestDevice(hash);
        }

        public void OpenTestSuite() {
        }

        public IAd CreateAd(AdFormat format, string id) {
            switch (format) {
                case AdFormat.Banner:
                    return _plugin.CreateBannerAd(id, FacebookBannerAdSize.BannerHeight50);
                case AdFormat.Rectangle:
                    return _plugin.CreateBannerAd(id, FacebookBannerAdSize.RectangleHeight250);
                case AdFormat.AppOpen:
                case AdFormat.RewardedInterstitial:
                    return new NullFullScreenAd();
                case AdFormat.Interstitial:
                    return _plugin.CreateInterstitialAd(id);
                case AdFormat.Rewarded:
                    return _plugin.CreateRewardedAd(id);
                case AdFormat.Null:
                    return new NullAd();
            }
            throw new ArgumentException();
        }
    }

    internal class IronSourceConfig : INetworkConfig {
        private IIronSource _plugin;
        private readonly string _appId;

        public IronSourceConfig(JsonObject node) {
            _appId = (string) node["app_id"];
        }

        public async Task Initialize() {
            _plugin = PluginManager.CreatePlugin<IIronSource>();
            await _plugin.Initialize(_appId);
        }

        public AdNetwork Network => AdNetwork.IronSource;

        public void AddTestDevice(string hash) {
        }

        public void OpenTestSuite() {
        }

        public IAd CreateAd(AdFormat format, string id) {
            switch (format) {
                case AdFormat.Banner:
                    return _plugin.CreateBannerAd(id, IronSourceBannerAdSize.Banner);
                case AdFormat.Rectangle:
                    return _plugin.CreateBannerAd(id, IronSourceBannerAdSize.Rectangle);
                case AdFormat.AppOpen:
                case AdFormat.RewardedInterstitial:
                    return new NullFullScreenAd();
                case AdFormat.Interstitial:
                    return _plugin.CreateInterstitialAd(id);
                case AdFormat.Rewarded:
                    return _plugin.CreateRewardedAd(id);
                case AdFormat.Null:
                    return new NullAd();
            }
            throw new ArgumentException();
        }
    }

    internal class UnityAdsConfig : INetworkConfig {
        private IUnityAds _plugin;
        private readonly string _appId;
        private readonly int _timeOut;

        public UnityAdsConfig(JsonObject node) {
            _appId = (string) node["app_id"];
            _timeOut = node.TryGetValue("time_out", out var value) ? (int) value : 30;
        }

        public async Task Initialize() {
            _plugin = PluginManager.CreatePlugin<IUnityAds>();
            await Task.WhenAny(Task.Delay(_timeOut * 1000), _plugin.Initialize(_appId, false));
        }

        public AdNetwork Network => AdNetwork.UnityAds;

        public void AddTestDevice(string hash) {
        }

        public void OpenTestSuite() {
        }

        public IAd CreateAd(AdFormat format, string id) {
            switch (format) {
                case AdFormat.Banner:
                    return _plugin.CreateBannerAd(id, UnityBannerAdSize.Normal);
                case AdFormat.Rectangle:
                    return new NullBannerAd();
                case AdFormat.AppOpen:
                case AdFormat.RewardedInterstitial:
                    return new NullFullScreenAd();
                case AdFormat.Interstitial:
                    return _plugin.CreateInterstitialAd(id);
                case AdFormat.Rewarded:
                    return _plugin.CreateRewardedAd(id);
                case AdFormat.Null:
                    return new NullAd();
            }
            throw new ArgumentException();
        }
    }

    internal class NullNetworkConfig : INetworkConfig {
        public Task Initialize() {
            return Task.FromResult<object>(null);
        }

        public AdNetwork Network => AdNetwork.Null;

        public void AddTestDevice(string hash) {
        }

        public void OpenTestSuite() {
        }

        public IAd CreateAd(AdFormat format, string id) {
            switch (format) {
                case AdFormat.Banner:
                case AdFormat.Rectangle:
                    return new NullBannerAd();
                case AdFormat.AppOpen:
                case AdFormat.RewardedInterstitial:
                case AdFormat.Interstitial:
                case AdFormat.Rewarded:
                    return new NullFullScreenAd();
                case AdFormat.Null:
                    return new NullAd();
            }
            throw new ArgumentException();
        }
    }

    internal interface IAdConfigManager {
        IAd CreateAd(AdFormat format);
    }

    internal class AdConfigManager : IAdConfigManager {
        private readonly INetworkConfigManager _manager;
        private readonly List<IAdConfig> _ads;

        public AdConfigManager(INetworkConfigManager manager, JsonObject node) {
            _manager = manager;
            var ads = node.TryGetValue("ads", out var value)
                ? (JsonArray) value
                : new JsonArray();
            _ads = ads.Select(entry => AdConfig.Parse((JsonObject) entry)).ToList();
        }

        public IAd CreateAd(AdFormat format) {
            foreach (var ad in _ads) {
                if (ad.Format == format) {
                    return ad.CreateAd(_manager);
                }
            }
            return new NullAd();
        }
    }

    internal interface IAdConfig {
        AdFormat Format { get; }
        IAd CreateAd(INetworkConfigManager manager);
    }

    internal static class AdConfig {
        public static IAdConfig Parse(JsonObject node) {
            var format = AdsConfigUtils.ParseFormat((string) node["format"]);
            switch (format) {
                case AdFormat.Banner: return new BannerConfig(node);
                case AdFormat.Rectangle: return new RectangleConfig(node);
                case AdFormat.AppOpen: return new AppOpenConfig(node);
                case AdFormat.Interstitial: return new InterstitialConfig(node);
                case AdFormat.RewardedInterstitial: return new RewardedInterstitialConfig(node);
                case AdFormat.Rewarded: return new RewardedConfig(node);
                case AdFormat.Null: return new NullAdConfig();
            }
            throw new ArgumentException();
        }
    }

    internal class BannerConfig : IAdConfig {
        private readonly IntervalConfig _loadConfig;
        private readonly RetrierConfig _retrierConfig;
        private readonly IAdInstanceConfig<IBannerAd> _instance;

        public BannerConfig(JsonObject node) {
            _loadConfig = new IntervalConfig(node, "load_interval");
            _retrierConfig = new RetrierConfig(node, "reload_params");
            _instance = AdInstanceConfig<IBannerAd>.Parse<MultiBannerAd>(AdFormat.Banner, node["instance"]);
        }

        public AdFormat Format => AdFormat.Banner;

        public IAd CreateAd(INetworkConfigManager manager) {
            var ad = _instance.CreateAd(manager);
            return new GenericBannerAd(ad, _loadConfig.Create(), _retrierConfig.Create());
        }
    }

    internal class RectangleConfig : IAdConfig {
        private readonly IntervalConfig _loadConfig;
        private readonly RetrierConfig _retrierConfig;
        private readonly IAdInstanceConfig<IBannerAd> _instance;

        public RectangleConfig(JsonObject node) {
            _loadConfig = new IntervalConfig(node, "load_interval");
            _retrierConfig = new RetrierConfig(node, "reload_params");
            _instance = AdInstanceConfig<IBannerAd>.Parse<MultiBannerAd>(AdFormat.Rectangle, node["instance"]);
        }

        public AdFormat Format => AdFormat.Rectangle;

        public IAd CreateAd(INetworkConfigManager manager) {
            var ad = _instance.CreateAd(manager);
            return new GenericBannerAd(ad, _loadConfig.Create(), _retrierConfig.Create());
        }
    }

    internal class AppOpenConfig : IAdConfig {
        private readonly IntervalConfig _displayConfig;
        private readonly IntervalConfig _loadConfig;
        private readonly RetrierConfig _retrierConfig;
        private readonly IAdInstanceConfig<IFullScreenAd> _instance;

        public AppOpenConfig(JsonObject node) {
            _displayConfig = new IntervalConfig(node, "interval");
            _loadConfig = new IntervalConfig(node, "load_interval");
            _retrierConfig = new RetrierConfig(node, "reload_params");
            _instance = AdInstanceConfig<IFullScreenAd>.Parse<MultiFullScreenAd>(AdFormat.AppOpen, node["instance"]);
        }

        public AdFormat Format => AdFormat.AppOpen;

        public IAd CreateAd(INetworkConfigManager manager) {
            var ad = _instance.CreateAd(manager);
            return new GenericFullScreenAd(ad, _displayConfig.Create(), _loadConfig.Create(), _retrierConfig.Create());
        }
    }

    internal class InterstitialConfig : IAdConfig {
        private readonly IntervalConfig _displayConfig;
        private readonly IntervalConfig _loadConfig;
        private readonly RetrierConfig _retrierConfig;
        private readonly IAdInstanceConfig<IFullScreenAd> _instance;

        public InterstitialConfig(JsonObject node) {
            _displayConfig = new IntervalConfig(node, "interval");
            _loadConfig = new IntervalConfig(node, "load_interval");
            _retrierConfig = new RetrierConfig(node, "reload_params");
            _instance = AdInstanceConfig<IFullScreenAd>.Parse<MultiFullScreenAd>(AdFormat.Interstitial,
                node["instance"]);
        }

        public AdFormat Format => AdFormat.Interstitial;

        public IAd CreateAd(INetworkConfigManager manager) {
            var ad = _instance.CreateAd(manager);
            return new GenericFullScreenAd(ad, _displayConfig.Create(), _loadConfig.Create(), _retrierConfig.Create());
        }
    }

    internal class RewardedInterstitialConfig : IAdConfig {
        private readonly IntervalConfig _loadConfig;
        private readonly RetrierConfig _retrierConfig;
        private readonly IAdInstanceConfig<IFullScreenAd> _instance;

        public RewardedInterstitialConfig(JsonObject node) {
            _loadConfig = new IntervalConfig(node, "load_interval");
            _retrierConfig = new RetrierConfig(node, "reload_params");
            _instance = AdInstanceConfig<IFullScreenAd>.Parse<MultiFullScreenAd>(AdFormat.RewardedInterstitial,
                node["instance"]);
        }

        public AdFormat Format => AdFormat.RewardedInterstitial;

        public IAd CreateAd(INetworkConfigManager manager) {
            var ad = _instance.CreateAd(manager);
            return new GenericFullScreenAd(ad, new LockCapper(), _loadConfig.Create(), _retrierConfig.Create());
        }
    }

    internal class RewardedConfig : IAdConfig {
        private readonly IntervalConfig _loadConfig;
        private readonly RetrierConfig _retrierConfig;
        private readonly IAdInstanceConfig<IFullScreenAd> _instance;

        public RewardedConfig(JsonObject node) {
            _loadConfig = new IntervalConfig(node, "load_interval");
            _retrierConfig = new RetrierConfig(node, "reload_params");
            _instance = AdInstanceConfig<IFullScreenAd>.Parse<MultiFullScreenAd>(AdFormat.Rewarded, node["instance"]);
        }

        public AdFormat Format => AdFormat.Rewarded;

        public IAd CreateAd(INetworkConfigManager manager) {
            var ad = _instance.CreateAd(manager);
            return new GenericFullScreenAd(ad, new LockCapper(), _loadConfig.Create(), _retrierConfig.Create());
        }
    }

    internal class NullAdConfig : IAdConfig {
        public AdFormat Format => AdFormat.Null;

        public IAd CreateAd(INetworkConfigManager manager) {
            return new NullAd();
        }
    }

    internal interface IAdInstanceConfig<out Ad> where Ad : IAd {
        Ad CreateAd(INetworkConfigManager manager);
    }

    internal static class AdInstanceConfig<Ad> where Ad : class, IAd {
        public static IAdInstanceConfig<Ad> Parse<MultiAd>(AdFormat format, object node)
            where MultiAd : IMultiAd<Ad>, Ad, new() {
            if (node is JsonArray array) {
                return new WaterfallInstanceConfig<Ad, MultiAd>(format, array);
            }
            return new SingleInstanceConfig<Ad>(format, (JsonObject) node);
        }
    }

    internal class SingleInstanceConfig<Ad> : IAdInstanceConfig<Ad> where Ad : class, IAd {
        private readonly AdFormat _format;
        private readonly AdNetwork _network;
        private readonly string _id;

        public SingleInstanceConfig(AdFormat format, JsonObject node) {
            _format = format;
            var network = (string) node["network"];
            _network = AdsConfigUtils.ParseNetwork(network);
            _id = node.TryGetValue("id", out var value) ? (string) value : "";
        }

        public Ad CreateAd(INetworkConfigManager manager) {
            var ad = manager.CreateAd(_network, _format, _id);
            return ad as Ad;
        }
    }

    internal class WaterfallInstanceConfig<Ad, MultiAd> : IAdInstanceConfig<Ad>
        where Ad : class, IAd
        where MultiAd : IMultiAd<Ad>, Ad, new() {
        private readonly List<IAdInstanceConfig<Ad>> _instances = new List<IAdInstanceConfig<Ad>>();

        public WaterfallInstanceConfig(AdFormat format, JsonArray node) {
            foreach (var value in node) {
                _instances.Add(AdInstanceConfig<Ad>.Parse<MultiAd>(format, value));
            }
        }

        public Ad CreateAd(INetworkConfigManager manager) {
            var ad = new MultiAd();
            foreach (var instance in _instances) {
                ad.AddItem(instance.CreateAd(manager));
            }
            return ad;
        }
    }

    internal class AdsConfig {
        private INetworkConfigManager _networkManager;
        private IAdConfigManager _adManager;

        public static AdsConfig Parse(string text) {
            var node = (JsonObject) Json.Deserialize(text);
            return Parse(node);
        }

        private static AdsConfig Parse(JsonObject node) {
            var networkManager = new NetworkConfigManager(node);
            var adManager = new AdConfigManager(networkManager, node);
            var result = new AdsConfig {
                _networkManager = networkManager,
                _adManager = adManager
            };
            return result;
        }

        public async Task Initialize() {
            await _networkManager.Initialize();
        }

        public void AddTestDevice(string hash) {
            _networkManager.AddTestDevice(hash);
        }

        public void OpenTestSuite(AdNetwork network) {
            _networkManager.OpenTestSuite(network);
        }

        public IAd CreateAd(AdFormat format) {
            return _adManager.CreateAd(format);
        }
    }
}