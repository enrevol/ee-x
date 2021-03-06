package com.ee

import android.app.Activity
import android.app.Application
import androidx.annotation.AnyThread
import com.appsflyer.AppsFlyerConversionListener
import com.appsflyer.AppsFlyerLib
import com.ee.internal.deserialize
import kotlinx.serialization.Contextual
import kotlinx.serialization.Serializable

class AppsFlyerBridge(
    private val _bridge: IMessageBridge,
    private val _logger: ILogger,
    private val _application: Application,
    private var _activity: Activity?) : IPlugin {
    companion object {
        private val kTag = AppsFlyerBridge::class.java.name
        private const val kPrefix = "AppsFlyerBridge"
        private const val kInitialize = "${kPrefix}Initialize"
        private const val kStartTracking = "${kPrefix}StartTracking"
        private const val kGetDeviceId = "${kPrefix}GetDeviceId"
        private const val kSetDebugEnabled = "${kPrefix}SetDebugEnabled"
        private const val kSetStopTracking = "${kPrefix}SetStopTracking"
        private const val kTrackEvent = "${kPrefix}TrackEvent"
    }

    private val _tracker = AppsFlyerLib.getInstance()

    init {
        _logger.info("$kTag: constructor begin: application = $_application activity = $_activity")
        registerHandlers()
        _logger.info("$kTag: constructor end")
    }

    override fun onCreate(activity: Activity) {
        _activity = activity
    }

    override fun onStart() {}
    override fun onStop() {}
    override fun onResume() {}
    override fun onPause() {}

    override fun onDestroy() {
        _activity = null
    }

    override fun destroy() {
        deregisterHandlers()
    }

    @Serializable
    private class TrackEventRequest(
        val name: String,
        val values: Map<String, @Contextual Any>
    )

    @AnyThread
    private fun registerHandlers() {
        _bridge.registerHandler(kInitialize) { message ->
            initialize(message)
            ""
        }
        _bridge.registerHandler(kStartTracking) {
            startTracking()
            ""
        }
        _bridge.registerHandler(kGetDeviceId) {
            deviceId
        }
        _bridge.registerHandler(kSetDebugEnabled) { message ->
            setDebugEnabled(Utils.toBoolean(message))
            ""
        }
        _bridge.registerHandler(kSetStopTracking) { message ->
            setStopTracking(Utils.toBoolean(message))
            ""
        }
        _bridge.registerHandler(kTrackEvent) { message ->
            val request = deserialize<TrackEventRequest>(message)
            trackEvent(request.name, request.values)
            ""
        }
    }

    @AnyThread
    private fun deregisterHandlers() {
        _bridge.deregisterHandler(kInitialize)
        _bridge.deregisterHandler(kStartTracking)
        _bridge.deregisterHandler(kGetDeviceId)
        _bridge.deregisterHandler(kSetDebugEnabled)
        _bridge.deregisterHandler(kSetStopTracking)
        _bridge.deregisterHandler(kTrackEvent)
    }

    @AnyThread
    fun initialize(devKey: String) {
        Thread.runOnMainThread {
            val listener = object : AppsFlyerConversionListener {
                override fun onConversionDataSuccess(conversionData: Map<String, Any>) {
                    for (key in conversionData.keys) {
                        _logger.debug("$kTag: ${this::onConversionDataSuccess.name}: $key = ${conversionData[key]}")
                    }
                }

                override fun onConversionDataFail(errorMessage: String) {
                    _logger.debug("$kTag: ${this::onConversionDataFail.name}: $errorMessage")
                }

                override fun onAppOpenAttribution(conversionData: Map<String, String>) {
                    for (key in conversionData.keys) {
                        _logger.debug("$kTag: ${this::onAppOpenAttribution.name}: $key = ${conversionData[key]}")
                    }
                }

                override fun onAttributionFailure(errorMessage: String) {
                    _logger.debug("$kTag: ${this::onAttributionFailure.name}: $errorMessage")
                }
            }
            _tracker.init(devKey, listener, _application)
            _tracker.setDeviceTrackingDisabled(true)
            _tracker.enableLocationCollection(true)
        }
    }

    @AnyThread
    fun startTracking() {
        Thread.runOnMainThread {
            _tracker.startTracking(_application)
        }
    }

    val deviceId: String
        @AnyThread get() = _tracker.getAppsFlyerUID(_application)

    @AnyThread
    fun setDebugEnabled(enabled: Boolean) {
        Thread.runOnMainThread {
            _tracker.setDebugLog(enabled)
        }
    }

    @AnyThread
    fun setStopTracking(enabled: Boolean) {
        Thread.runOnMainThread {
            _tracker.stopTracking(enabled, _application)
        }
    }

    @AnyThread
    fun trackEvent(name: String, values: Map<String, Any>) {
        Thread.runOnMainThread {
            _tracker.trackEvent(_application, name, values)
        }
    }
}