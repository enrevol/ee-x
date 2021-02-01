package com.ee

import android.app.Activity
import android.app.Application
import android.os.Bundle
import androidx.annotation.AnyThread
import com.ee.internal.deserialize
import com.google.firebase.analytics.FirebaseAnalytics
import com.google.firebase.analytics.ktx.analytics
import com.google.firebase.analytics.ktx.logEvent
import com.google.firebase.ktx.Firebase
import kotlinx.serialization.Contextual
import kotlinx.serialization.InternalSerializationApi
import kotlinx.serialization.Serializable

@InternalSerializationApi
class FirebaseAnalyticsBridge(
    private val _bridge: IMessageBridge,
    private val _logger: ILogger,
    private val _application: Application,
    private var _activity: Activity?) : IPlugin {
    companion object {
        private val kTag = FirebaseAnalyticsBridge::class.java.name
        private const val kPrefix = "FirebaseAnalyticsBridge"
        private const val kSetUserProperty = "${kPrefix}SetUserProperty"
        private const val kTrackScreen = "${kPrefix}TrackScreen"
        private const val kLogEvent = "${kPrefix}LogEvent"
    }

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

    @AnyThread
    private fun registerHandlers() {
        _bridge.registerHandler(kSetUserProperty) { message ->
            @Serializable
            class Request(
                val key: String,
                val value: String,
            )

            val request = deserialize<Request>(message)
            setUserProperty(request.key, request.value)
            ""
        }
        _bridge.registerHandler(kTrackScreen) { message ->
            trackScreen(message)
            ""
        }
        _bridge.registerHandler(kLogEvent) { message ->
            @Serializable
            class Request(
                val name: String,
                val parameters: Map<String, @Contextual FirebaseEventParameter>
            )

            val request = deserialize<Request>(message)
            logEvent(request.name, request.parameters)
            ""
        }
    }

    @AnyThread
    private fun deregisterHandlers() {
        _bridge.deregisterHandler(kSetUserProperty)
        _bridge.deregisterHandler(kLogEvent)
        _bridge.deregisterHandler(kTrackScreen)
    }

    @AnyThread
    private fun setUserProperty(key: String, value: String) {
        Thread.runOnMainThread {
            Firebase.analytics.setUserProperty(key, value)
        }
    }

    @AnyThread
    private fun trackScreen(name: String) {
        Thread.runOnMainThread {
            Firebase.analytics.logEvent(FirebaseAnalytics.Event.SCREEN_VIEW) {
                param(FirebaseAnalytics.Param.SCREEN_NAME, name)
            }
        }
    }

    @AnyThread
    private fun logEvent(name: String, parameters: Map<String, FirebaseEventParameter>) {
        Thread.runOnMainThread {
            val bundle = Bundle()
            for ((key, value) in parameters) {
                if (value.type == FirebaseEventType.Long) {
                    bundle.putLong(key, value.longValue)
                }
                if (value.type == FirebaseEventType.Double) {
                    bundle.putDouble(key, value.doubleValue)
                }
                if (value.type == FirebaseEventType.String) {
                    bundle.putString(key, value.stringValue)
                }
            }
            Firebase.analytics.logEvent(name, bundle)
        }
    }
}