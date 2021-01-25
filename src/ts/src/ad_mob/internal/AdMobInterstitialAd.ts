import {
    AdObserver,
    FullScreenAdResult,
    IFullScreenAd,
} from "../../ads";
import {
    AsyncHelper,
    IAsyncHelper,
    MessageHelper,
} from "../../ads/internal";
import {
    IMessageBridge,
    ObserverManager,
    Utils,
} from "../../core";
import { AdMob } from "./AdMob";

export class AdMobInterstitialAd extends ObserverManager<AdObserver> implements IFullScreenAd {
    private readonly _bridge: IMessageBridge;
    private readonly _displayer: IAsyncHelper<FullScreenAdResult>;
    private readonly _plugin: AdMob;
    private readonly _adId: string;
    private readonly _messageHelper: MessageHelper;
    private readonly _loader: IAsyncHelper<boolean>;

    public constructor(
        bridge: IMessageBridge, displayer: IAsyncHelper<FullScreenAdResult>, plugin: AdMob, adId: string) {
        super();
        this._bridge = bridge;
        this._displayer = displayer;
        this._plugin = plugin;
        this._adId = adId;
        this._messageHelper = new MessageHelper("AdMobInterstitialAd", adId);
        this._loader = new AsyncHelper<boolean>();

        this._bridge.registerHandler(_ => this.onLoaded(), this._messageHelper.onLoaded);
        this._bridge.registerHandler(this.onFailedToLoad, this._messageHelper.onFailedToLoad);
        this._bridge.registerHandler(this.onFailedToShow, this._messageHelper.onFailedToShow);
        this._bridge.registerHandler(_ => this.onClicked(), this._messageHelper.onClicked);
        this._bridge.registerHandler(_ => this.onClosed(), this._messageHelper.onClosed);
    }

    public destroy(): void {
        this._bridge.deregisterHandler(this._messageHelper.onLoaded);
        this._bridge.deregisterHandler(this._messageHelper.onFailedToLoad);
        this._bridge.deregisterHandler(this._messageHelper.onFailedToShow);
        this._bridge.deregisterHandler(this._messageHelper.onClicked);
        this._bridge.deregisterHandler(this._messageHelper.onClosed);
        this._plugin.destroyInterstitialAd(this._adId);
    }

    public get isLoaded(): boolean {
        const response = this._bridge.call(this._messageHelper.isLoaded);
        return Utils.toBool(response);
    }

    public load(): Promise<boolean> {
        return this._loader.process(
            () => this._bridge.call(this._messageHelper.load),
            result => {
                // OK.
            });
    }

    public show(): Promise<FullScreenAdResult> {
        return this._displayer.process(
            () => this._bridge.call(this._messageHelper.show),
            result => {
                // OK.
            });
    }

    private onLoaded(): void {
        if (this._loader.isProcessing) {
            this._loader.resolve(true);
        } else {
            // Assert.
        }
        this.dispatchEvent(observer => observer.onLoaded && observer.onLoaded());
    }

    private onFailedToLoad(message: string): void {
        if (this._loader.isProcessing) {
            this._loader.resolve(false);
        } else {
            // Assert.
        }
    }

    private onFailedToShow(message: string): void {
        if (this._displayer.isProcessing) {
            this._displayer.resolve(FullScreenAdResult.Failed);
        } else {
            // Assert.
        }
    }

    private onClicked(): void {
        this.dispatchEvent(observer => observer.onClicked && observer.onClicked());
    }

    private onClosed(): void {
        if (this._displayer.isProcessing) {
            this._displayer.resolve(FullScreenAdResult.Completed);
        } else {
            // Assert..
        }
    }
}