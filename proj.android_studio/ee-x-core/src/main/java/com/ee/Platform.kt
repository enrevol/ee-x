package com.ee

import android.annotation.SuppressLint
import android.app.Activity
import android.content.ActivityNotFoundException
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.content.res.Configuration
import android.content.res.Resources
import android.graphics.Point
import android.net.ConnectivityManager
import android.net.Uri
import android.os.Build
import android.util.DisplayMetrics
import android.util.Log
import android.view.Surface
import android.view.WindowManager
import com.android.installreferrer.api.InstallReferrerClient
import com.android.installreferrer.api.InstallReferrerStateListener
import com.ee.internal.InstallReferrerBridge
import com.ee.internal.deserialize
import com.ee.internal.serialize
import com.google.android.gms.ads.identifier.AdvertisingIdClient
import com.google.android.gms.common.wrappers.InstantApps
import com.google.common.truth.Truth.assertThat
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.async
import kotlinx.coroutines.withContext
import kotlinx.coroutines.withTimeout
import kotlinx.serialization.InternalSerializationApi
import kotlinx.serialization.Serializable
import java.lang.reflect.InvocationTargetException
import java.net.InetAddress
import java.net.UnknownHostException
import java.security.MessageDigest
import java.util.concurrent.CancellationException

object Platform {
    private val _logger = Logger(Platform::class.java.name)
    private const val kPrefix = "Platform_"
    private const val kIsApplicationInstalled = kPrefix + "isApplicationInstalled"
    private const val kOpenApplication = kPrefix + "openApplication"
    private const val kGetApplicationId = kPrefix + "getApplicationId"
    private const val kGetApplicationName = kPrefix + "getApplicationName"
    private const val kGetVersionName = kPrefix + "getVersionName"
    private const val kGetVersionCode = kPrefix + "getVersionCode"
    private const val kGetApplicationSignatures = kPrefix + "getApplicationSignatures"
    private const val kIsInstantApp = kPrefix + "isInstantApp"
    private const val kIsTablet = kPrefix + "isTablet"
    private const val kGetDensity = kPrefix + "getDensity"
    private const val kGetViewSize = kPrefix + "getViewSize"
    private const val kGetScreenSize = kPrefix + "getScreenSize"
    private const val kGetDeviceId = kPrefix + "getDeviceId"
    private const val kGetSafeInset = kPrefix + "getSafeInset"
    private const val kSendMail = kPrefix + "sendMail"
    private const val kTestConnection = kPrefix + "testConnection"
    private const val kShowInstallPrompt = kPrefix + "showInstallPrompt"
    private const val kGetInstallReferrerUrl = kPrefix + "getInstallReferrerUrl"

    @InternalSerializationApi
    fun registerHandlers(bridge: IMessageBridge, context: Context) {
        bridge.registerHandler(kIsApplicationInstalled) { message ->
            Utils.toString(isApplicationInstalled(context, message))
        }
        bridge.registerHandler(kOpenApplication) { message ->
            Utils.toString(openApplication(context, message))
        }
        bridge.registerHandler(kGetApplicationId) {
            getApplicationId(context)
        }
        bridge.registerHandler(kGetApplicationName) {
            getApplicationName(context)
        }
        bridge.registerHandler(kGetVersionName) {
            getVersionName(context)
        }
        bridge.registerHandler(kGetVersionCode) {
            getVersionCode(context)
        }
        bridge.registerHandler(kGetApplicationSignatures) { message ->
            @Serializable
            class Request(
                val algorithm: String
            )

            @Serializable
            @Suppress("unused")
            class Response(
                val signatures: List<String>
            )

            val request = deserialize<Request>(message)
            val signatures = getApplicationSignatures(context, request.algorithm)
            val response = Response(signatures)
            response.serialize()
        }
        bridge.registerHandler(kIsInstantApp) {
            Utils.toString(isInstantApp(context))
        }
        bridge.registerHandler(kIsTablet) {
            Utils.toString(isTablet())
        }
        bridge.registerHandler(kGetDensity) {
            getDensity().toString()
        }
        bridge.registerHandler(kGetViewSize) {
            @Serializable
            @Suppress("unused")
            class Response(
                val width: Int,
                val height: Int
            )

            val activity = PluginManager.getInstance().getActivity()
            if (activity == null) {
                assertThat(false).isTrue()
                return@registerHandler ""
            }
            val size = getViewSize(activity)
            val response = Response(size.x, size.y)
            response.serialize()
        }
        bridge.registerHandler(kGetScreenSize) {
            @Serializable
            @Suppress("unused")
            class Response(
                val width: Int,
                val height: Int
            )

            val size = getScreenSize(context)
            val response = Response(size.x, size.y)
            response.serialize()
        }
        bridge.registerAsyncHandler(kGetDeviceId) {
            getDeviceId(context)
        }
        bridge.registerHandler(kGetSafeInset) {
            @Serializable
            @Suppress("unused")
            class Response(
                val left: Int,
                val right: Int,
                val top: Int,
                val bottom: Int
            )

            val activity = PluginManager.getInstance().getActivity()
            if (activity == null) {
                assertThat(false).isTrue()
                return@registerHandler ""
            }
            val inset = getSafeInset(activity)
            val response = Response(
                inset.left,
                inset.right,
                inset.top,
                inset.bottom
            )
            response.serialize()
        }
        bridge.registerHandler(kSendMail) { message: String ->
            @Serializable
            class Request(
                val recipient: String,
                val subject: String,
                val body: String
            )

            val request = deserialize<Request>(message)
            Utils.toString(sendMail(context, request.recipient, request.subject, request.body))
        }
        bridge.registerAsyncHandler(kTestConnection) { message ->
            @Serializable
            class Request(
                val host_name: String,
                val time_out: Float
            )

            val request = deserialize<Request>(message)
            val result = testConnection(context, request.host_name, request.time_out)
            Utils.toString(result)
        }
        bridge.registerAsyncHandler(kGetInstallReferrerUrl) {
            getInstallReferrerUrl();
        }
        bridge.registerHandler(kShowInstallPrompt) { message ->
            @Serializable
            class Request(
                val url: String,
                val referrer: String
            )

            val request = deserialize<Request>(message)
            val activity = PluginManager.getInstance().getActivity()
            if (activity == null) {
                assertThat(false).isTrue()
                return@registerHandler ""
            }
            assertThat(activity).isNotNull()
            showInstantPrompt(request.url, request.referrer, activity)
            ""
        }
    }

    fun deregisterHandlers(bridge: IMessageBridge) {
        bridge.deregisterHandler(kIsApplicationInstalled)
        bridge.deregisterHandler(kOpenApplication)
        bridge.deregisterHandler(kGetApplicationId)
        bridge.deregisterHandler(kGetVersionName)
        bridge.deregisterHandler(kGetVersionCode)
        bridge.deregisterHandler(kGetApplicationSignatures)
        bridge.deregisterHandler(kIsInstantApp)
        bridge.deregisterHandler(kIsTablet)
        bridge.deregisterHandler(kGetDensity)
        bridge.deregisterHandler(kGetScreenSize)
        bridge.deregisterHandler(kGetDeviceId)
        bridge.deregisterHandler(kGetSafeInset)
        bridge.deregisterHandler(kSendMail)
        bridge.deregisterHandler(kTestConnection)
        bridge.deregisterHandler(kShowInstallPrompt)
    }

    fun isApplicationInstalled(context: Context,
                               applicationId: String): Boolean {
        val packetManager = context.packageManager
        var installed = false
        try {
            packetManager.getPackageInfo(applicationId, PackageManager.GET_ACTIVITIES)
            installed = true
        } catch (ignored: PackageManager.NameNotFoundException) {
        }
        return installed
    }

    private fun openApplication(context: Context, applicationId: String): Boolean {
        val packetManager = context.packageManager
        val launchIntent = packetManager.getLaunchIntentForPackage(applicationId)
        if (launchIntent != null) {
            // Null pointer check in case package name was not found.
            context.startActivity(launchIntent)
            return true
        }
        return false
    }

    private fun getApplicationId(context: Context): String {
        return context.packageName
    }

    private fun getApplicationName(context: Context): String {
        val pm = context.packageManager
        return try {
            val ai = pm.getApplicationInfo(context.packageName, 0)
            pm.getApplicationLabel(ai) as String
        } catch (e: PackageManager.NameNotFoundException) {
            ""
        }
    }

    private fun getVersionName(context: Context): String {
        val packetManager = context.packageManager
        var versionName = ""
        try {
            val info = packetManager.getPackageInfo(context.packageName, 0)
            versionName = info.versionName
        } catch (e: PackageManager.NameNotFoundException) {
            e.printStackTrace()
        }
        return versionName
    }

    fun getVersionCode(context: Context): String {
        val packetManager = context.packageManager
        var versionCode = ""
        try {
            val info = packetManager.getPackageInfo(context.packageName, 0)
            versionCode = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                info.longVersionCode.toString()
            } else {
                info.versionCode.toString()
            }
        } catch (e: PackageManager.NameNotFoundException) {
            e.printStackTrace()
        }
        return versionCode
    }

    // https://stackoverflow.com/questions/52041805/how-to-use-packageinfo-get-signing-certificates-in-api-28/53407183
    private fun getApplicationSignatures(context: Context, algorithm: String): List<String> {
        var signatures = emptyList<String>()
        try {
            val digest = MessageDigest.getInstance(algorithm)
            val packetManager = context.packageManager
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                val flags = PackageManager.GET_SIGNING_CERTIFICATES
                val info = packetManager.getPackageInfo(context.packageName, flags)
                signatures = if (info.signingInfo.hasMultipleSigners()) {
                    info.signingInfo.apkContentsSigners.map {
                        toHexString(digest.digest(it.toByteArray()))
                    }
                } else {
                    info.signingInfo.signingCertificateHistory.map {
                        toHexString(digest.digest(it.toByteArray()))
                    }
                }
            } else {
                val flags = PackageManager.GET_SIGNATURES
                val info = packetManager.getPackageInfo(context.packageName, flags)
                signatures = info.signatures.map {
                    toHexString(digest.digest(it.toByteArray()))
                }
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
        return signatures
    }

    private val HEX_ARRAY = "0123456789ABCDEF".toCharArray()

    private fun toHexString(bytes: ByteArray): String {
        val chars = CharArray(bytes.size * 2)
        for (i in bytes.indices) {
            val v = bytes[i].toInt() and 0xFF
            chars[i * 2] = HEX_ARRAY[v shr 4]
            chars[i * 2 + 1] = HEX_ARRAY[v and 0x0F]
        }
        return String(chars)
    }

    private fun isTablet(): Boolean {
        // Verifies if the Generalized Size of the device is XLARGE to be
        // considered a Tablet
        val screenLayout = Resources.getSystem().configuration.screenLayout
        val xlarge = screenLayout and Configuration.SCREENLAYOUT_SIZE_MASK >=
            Configuration.SCREENLAYOUT_SIZE_LARGE

        // If XLarge, checks if the Generalized Density is at least MDPI
        // (160dpi)
        if (xlarge) {
            val densityDpi = getDensityDpi()

            // MDPI=160, DEFAULT=160, DENSITY_HIGH=240, DENSITY_MEDIUM=160,
            // DENSITY_TV=213, DENSITY_XHIGH=320
            if (densityDpi == DisplayMetrics.DENSITY_DEFAULT ||
                densityDpi == DisplayMetrics.DENSITY_HIGH ||
                densityDpi == DisplayMetrics.DENSITY_MEDIUM ||
                densityDpi == DisplayMetrics.DENSITY_TV ||
                densityDpi == DisplayMetrics.DENSITY_XHIGH) {
                // Yes, this is a tablet!
                return true
            }
        }

        // No, this is not a tablet!
        return false
    }

    private fun isInstantApp(context: Context): Boolean {
        return InstantApps.isInstantApp(context)
    }

    fun getDensity(): Double {
        val metrics = Resources.getSystem().displayMetrics
        return metrics.density.toDouble()
    }

    fun getViewSize(activity: Activity): Point {
        val view = activity.window.decorView
        return Point(view.width, view.height)
    }

    fun getScreenSize(context: Context): Point {
        val manager = context.getSystemService(Context.WINDOW_SERVICE) as WindowManager
        val display = manager.defaultDisplay
        val metrics = DisplayMetrics()
        display.getRealMetrics(metrics)
        return Point(metrics.widthPixels, metrics.heightPixels)
    }

    private fun getDensityDpi(): Int {
        val metrics = Resources.getSystem().displayMetrics
        return metrics.densityDpi
    }

    private suspend fun getDeviceId(context: Context): String {
        val task = GlobalScope.async(Dispatchers.IO) {
            try {
                val info = AdvertisingIdClient.getAdvertisingIdInfo(context.applicationContext)
                if (info.isLimitAdTrackingEnabled) {
                    ""
                } else {
                    info.id
                }
            } catch (ex: Exception) {
                // java.io.IOException: java.util.concurrent.TimeoutException: Timed out waiting for the service connection
                ""
            }
        }
        return task.await()
    }

    class SafeInset {
        var left = 0
        var right = 0
        var top = 0
        var bottom = 0
    }

    private fun calculateSafeInset(activity: Activity, notchSize: Int, inset: SafeInset) {
        // https://stackoverflow.com/questions/3663665/how-can-i-get-the-current-screen-orientation
        when (activity.windowManager.defaultDisplay.rotation) {
            Surface.ROTATION_0 -> inset.top = notchSize
            Surface.ROTATION_90 -> inset.left = notchSize
            Surface.ROTATION_180 -> inset.bottom = notchSize
            Surface.ROTATION_270 -> inset.right = notchSize
        }
    }

    @Throws(IllegalAccessException::class, InvocationTargetException::class, NoSuchMethodException::class)
    private fun getSafeInset_Huawei(activity: Activity, inset: SafeInset): Boolean {
        val class_HwNotchSizeUtil: Class<*>
        try {
            val classLoader = activity.classLoader
            class_HwNotchSizeUtil = classLoader.loadClass("com.huawei.android.util.HwNotchSizeUtil")
            val method_hasNotchInScreen = class_HwNotchSizeUtil.getMethod("hasNotchInScreen")
            val hasFeature = method_hasNotchInScreen.invoke(class_HwNotchSizeUtil) as Boolean
            if (!hasFeature) {
                return false
            }
        } catch (e: Exception) {
            return false
        }
        val method_getNotchSize = class_HwNotchSizeUtil.getMethod("getNotchSize")
        val size = method_getNotchSize.invoke(class_HwNotchSizeUtil) as IntArray
        val notchWidth = size[0]
        val notchHeight = size[1]
        calculateSafeInset(activity, notchHeight, inset)
        return true
    }

    @Throws(ClassNotFoundException::class, IllegalAccessException::class, InstantiationException::class, InvocationTargetException::class, NoSuchMethodException::class)
    private fun getSafeInset_Oppo(activity: Activity, inset: SafeInset): Boolean {
        if (!activity.packageManager.hasSystemFeature("com.oppo.feature.screen.heteromorphism")) {
            return false
        }
        @SuppressLint("PrivateApi") val cls = Class.forName("android.os.SystemProperties")
        val hideMethod = cls.getMethod("get", String::class.java)
        val `object` = cls.newInstance()
        val value = hideMethod.invoke(`object`, "ro.oppo.screen.heteromorphism") as String

        // [378,0:702,80]
        val texts = value.split("[,:]").toTypedArray()
        var values: IntArray? = IntArray(texts.size)
        try {
            for (i in texts.indices) {
                values!![i] = texts[i].toInt()
            }
        } catch (e: NumberFormatException) {
            values = null
        }
        if (values != null && values.size == 4) {
            val notchHeight = values[3]
            calculateSafeInset(activity, notchHeight, inset)
        }
        return true
    }

    private fun getSafeInset(activity: Activity): SafeInset {
        val inset = SafeInset()
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.P) {
            try {
                // https://stackoverflow.com/questions/51743622/how-to-handle-notchdisplay-cutout-in-android-api-lower-than-28
                if (getSafeInset_Oppo(activity, inset)) {
                    return inset
                }
                if (getSafeInset_Huawei(activity, inset)) {
                    return inset
                }
            } catch (ex: Exception) {
                _logger.error("getSafeInset", ex)
            }
        } else {
            val decorView = activity.window.decorView
            val insets = decorView.rootWindowInsets
            val cutout = insets.displayCutout
            if (cutout == null) {
                // Doesn't have cutout.
            } else {
                inset.left = cutout.safeInsetLeft
                inset.right = cutout.safeInsetRight
                inset.top = cutout.safeInsetTop
                inset.bottom = cutout.safeInsetBottom
            }
        }
        return inset
    }

    private fun sendMail(context: Context, recipient: String, subject: String, body: String): Boolean {
        val intent = Intent(Intent.ACTION_SEND) //
            .setType("message/rfc822") //
            .putExtra(Intent.EXTRA_EMAIL, arrayOf(recipient)) //
            .putExtra(Intent.EXTRA_SUBJECT, subject) //
            .putExtra(Intent.EXTRA_TEXT, body)
        try {
            // Fix Calling startActivity() from outside of an Activity  context requires the
            // FLAG_ACTIVITY_NEW_TASK flag. Is this really what you want?
            val chooserIntent = Intent.createChooser(intent, "Send mail...")
            chooserIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(chooserIntent)
            return true
        } catch (ex: ActivityNotFoundException) {
            // There are no email clients installed.
            ex.printStackTrace()
        }
        return false
    }

    /// This methods only test if internet is connected or connecting.
    private fun isInternetAvailable(context: Context): Boolean {
        val connectivityManager = (context.getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager)
        val info = connectivityManager.activeNetworkInfo
        return info != null && info.isConnectedOrConnecting
    }

    /// https://stackoverflow.com/questions/9570237/android-check-internet-connection
    /// https://stackoverflow.com/questions/2758612/executorservice-that-interrupts-tasks-after-a-timeout
    private suspend fun testConnection(context: Context, hostName: String, timeOut: Float): Boolean {
        var result = false
        try {
            result = withContext(Dispatchers.IO) {
                withTimeout((timeOut * 1000).toLong()) {
                    try {
                        val isAvailable = isInternetAvailable(context)
                        if (!isAvailable) {
                            return@withTimeout false
                        }
                        @Suppress("BlockingMethodInNonBlockingContext")
                        val address = InetAddress.getByName(hostName)
                        address.toString() != ""
                    } catch (ex: UnknownHostException) {
                        false
                    }
                }
            }
        } catch (ex: CancellationException) {
            // Time-out.
        }
        return result
    }

    private fun showInstantPrompt(url: String, referrer: String, activity: Activity) {
        val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url)).addCategory(Intent.CATEGORY_BROWSABLE)
        val requestCode = 10;
        com.google.android.gms.instantapps.InstantApps.showInstallPrompt(activity, intent, requestCode, referrer)
    }

    private suspend fun getInstallReferrerUrl(): String {
        return InstallReferrerBridge.getUrl()
    }
}