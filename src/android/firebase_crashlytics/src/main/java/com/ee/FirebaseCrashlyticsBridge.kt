package com.ee

import android.app.Activity
import android.app.Application
import androidx.annotation.AnyThread
import com.google.firebase.crashlytics.FirebaseCrashlytics

class FirebaseCrashlyticsBridge(
    private val _bridge: IMessageBridge,
    private val _logger: ILogger,
    private val _application: Application,
    private var _activity: Activity?) : IPlugin {
    companion object {
        private val kTag = FirebaseCrashlyticsBridge::class.java.name
        private const val kPrefix = "FirebaseCrashlyticsBridge"
        private const val kInitialize = "${kPrefix}Initialize"
        private const val kLog = "${kPrefix}Log"
    }

    private var _plugin: FirebaseCrashlytics? = null

    init {
        _logger.info("$kTag: constructor begin: application = $_application activity = $_activity")
        registerHandlers()
        _logger.info("$kTag: constructor end.")
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
        _bridge.registerAsyncHandler(kInitialize) {
            Utils.toString(initialize())
        }
        _bridge.registerHandler(kLog) { message ->
            log(message)
            ""
        }
    }

    @AnyThread
    private fun deregisterHandlers() {
        _bridge.deregisterHandler(kInitialize)
        _bridge.deregisterHandler(kLog)
    }

    @AnyThread
    private suspend fun initialize(): Boolean {
        if (FirebaseInitializer.instance.initialize(false)) {
            _plugin = FirebaseCrashlytics.getInstance()
            return true
        }
        return false
    }

    @AnyThread
    fun log(message: String) {
        Thread.runOnMainThread {
            val plugin = _plugin ?: throw IllegalStateException("Please call initialize() first")
            plugin.log(message)
        }
    }
}