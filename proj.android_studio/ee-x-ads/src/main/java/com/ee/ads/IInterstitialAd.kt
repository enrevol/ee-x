package com.ee.ads

/**
 * Created by Zinge on 10/13/17.
 */
interface IInterstitialAd {
    val isLoaded: Boolean
    fun load()
    fun show()
}