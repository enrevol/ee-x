import {
    AdResult,
    IAd,
    IBannerAd,
    IFullScreenAd,
} from "../../ads";
import {
    DefaultBannerAd,
    DefaultFullScreenAd,
    GuardedBannerAd,
    GuardedFullScreenAd,
    IAsyncHelper,
    MediationManager,
} from "../../ads/internal";
import {
    ILogger,
    IMessageBridge,
    Utils,
} from "../../core";
import { FacebookBannerAdSize } from "../FacebookBannerAdSize";
import { IFacebookAds } from "../IFacebookAds";

type Destroyer = () => void;

export class FacebookAds implements IFacebookAds {
    private readonly kTag = `FacebookAds`;
    private readonly kPrefix = "FacebookAdsBridge";
    private readonly kInitialize = this.kPrefix + "Initialize";
    private readonly kGetTestDeviceHash = this.kPrefix + "GetTestDeviceHash";
    private readonly kAddTestDevice = this.kPrefix + "AddTestDevice";
    private readonly kClearTestDevices = this.kPrefix + "ClearTestDevices";
    private readonly kGetBannerAdSize = this.kPrefix + "GetBannerAdSize";
    private readonly kCreateBannerAd = this.kPrefix + "CreateBannerAd";
    private readonly kCreateNativeAd = this.kPrefix + "CreateNativeAd";
    private readonly kCreateInterstitialAd = this.kPrefix + "CreateInterstitialAd";
    private readonly kCreateRewardedAd = this.kPrefix + "CreateRewardedAd";
    private readonly kDestroyAd = this.kPrefix + "DestroyAd";

    private readonly _bridge: IMessageBridge;
    private readonly _logger: ILogger;
    private readonly _destroyer: Destroyer;
    private readonly _network: string;
    private readonly _ads: { [index: string]: IAd };
    private readonly _displayer: IAsyncHelper<AdResult>;

    public constructor(bridge: IMessageBridge, logger: ILogger, destroyer: Destroyer) {
        this._bridge = bridge;
        this._logger = logger;
        this._destroyer = destroyer;
        this._logger.debug(`${this.kTag}: constructor`);
        this._network = `facebook_ads`;
        this._ads = {};
        this._displayer = MediationManager.getInstance().adDisplayer;
    }

    public destroy(): void {
        this._logger.debug(`${this.kTag}: destroy`);
        for (const id in this._ads) {
            const ad = this._ads[id];
            ad.destroy();
            delete this._ads[id];
        }
        this._destroyer();
    }

    public async initialize(): Promise<boolean> {
        const response = await this._bridge.callAsync(this.kInitialize);
        return Utils.toBool(response);
    }

    public getTestDeviceHash(): string {
        return this._bridge.call(this.kGetTestDeviceHash);
    }

    public addTestDevice(hash: string): void {
        this._bridge.call(this.kAddTestDevice, hash);
    }

    public clearTestDevices(): void {
        this._bridge.call(this.kClearTestDevices);
    }


    private getBannerAdSize(adSize: FacebookBannerAdSize): [number, number] {
        const response = this._bridge.call(this.kGetBannerAdSize, `${adSize}`);
        const json: {
            width: number,
            height: number,
        } = JSON.parse(response);
        return [json.width, json.height];
    }

    public createBannerAd(adId: string, adSize: FacebookBannerAdSize): IBannerAd {
        this._logger.debug(`${this.kTag}: createBannerAd: id = ${adId} size = ${adSize}`);
        if (this._ads[adId]) {
            return this._ads[adId] as IBannerAd;
        }
        const request = {
            [`adId`]: adId,
            [`adSize`]: parseInt(`${adSize}`),
        };
        const response = this._bridge.call(this.kCreateBannerAd, JSON.stringify(request));
        if (!Utils.toBool(response)) {
            throw new Error(`Failed to create banner ad: ${adId}`);
        }
        const size = this.getBannerAdSize(adSize);
        const ad = new GuardedBannerAd(
            new DefaultBannerAd("FacebookBannerAd", this._bridge, this._logger,
                () => this.destroyAd(adId),
                this._network, adId, size));
        this._ads[adId] = ad;
        return ad;
    }

    public createInterstitialAd(adId: string): IFullScreenAd {
        return this.createFullScreenAd(this.kCreateInterstitialAd, adId, () =>
            new DefaultFullScreenAd("FacebookInterstitialAd", this._bridge, this._logger, this._displayer,
                () => this.destroyAd(adId),
                _ => AdResult.Completed,
                this._network, adId))
    }

    public createRewardedAd(adId: string): IFullScreenAd {
        return this.createFullScreenAd(this.kCreateRewardedAd, adId, () =>
            new DefaultFullScreenAd("FacebookRewardedAd", this._bridge, this._logger, this._displayer,
                () => this.destroyAd(adId),
                message => Utils.toBool(message)
                    ? AdResult.Completed
                    : AdResult.Canceled,
                this._network, adId))
    }

    private createFullScreenAd(handlerId: string, adId: string, creator: () => IFullScreenAd): IFullScreenAd {
        this._logger.debug(`${this.kTag}: createFullScreenAd: id = ${adId}`);
        if (this._ads[adId]) {
            return this._ads[adId] as IFullScreenAd;
        }
        const response = this._bridge.call(handlerId, adId);
        if (!Utils.toBool(response)) {
            throw new Error(`Failed to create fullscreen ad: ${adId}`);
        }
        const ad = new GuardedFullScreenAd(creator());
        this._ads[adId] = ad;
        return ad;
    }

    private destroyAd(adId: string): boolean {
        this._logger.debug(`${this.kTag}: destroyAd: id = ${adId}`);
        if (!this._ads[adId]) {
            return false;
        }
        const response = this._bridge.call(this.kDestroyAd, adId);
        if (!Utils.toBool(response)) {
            // Assert.
            return false;
        }
        delete this._ads[adId];
        return true;
    }
}