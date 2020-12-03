using System.Collections.Generic;
using System.IO;
using System.Xml.Linq;

using UnityEditor;

using UnityEngine;

namespace EE.Editor {
    internal class LibrarySettings : ScriptableObject {
        private const string LibraryVersion = "1.3.6";

        private const string LibrarySettingsDir = "Assets/EE";
        private const string LibrarySettingsResDir = "Assets/EE/Resources";
        private const string LibrarySettingsFile = "Assets/EE/Resources/EESettings.asset";

        private const string LibrarySettingsEditorDir = "Assets/EE/Editor";
        private const string LibraryDependenciesFile = "Assets/EE/Editor/Dependencies.xml";

        private static LibrarySettings _sharedInstance;

        [SerializeField]
        private bool _isDeveloperModeEnabled = false;

        [SerializeField]
        private string _libraryPath = "ee-x";

        [SerializeField]
        private bool _isMultiDexEnabled = false;

        [SerializeField]
        private bool _isCoreEnabled = true;

        [SerializeField]
        private bool _isAdjustEnabled = false;

        [SerializeField]
        private bool _isAdColonyEnabled = false;

        [SerializeField]
        private bool _isAdMobEnabled = false;

        [SerializeField]
        private bool _isAdMobMediationEnabled = false;

        [SerializeField]
        private string _adMobAndroidAppId;

        [SerializeField]
        private string _adMobIosAppId;

        [SerializeField]
        private bool _isFacebookAdsEnabled = false;

        [SerializeField]
        private bool _isIronSourceEnabled = false;

        [SerializeField]
        private bool _isIronSourceMediationEnabled = false;

        [SerializeField]
        private bool _isUnityAdsEnabled = false;

        public bool IsDeveloperModeEnabled {
            get => _isDeveloperModeEnabled;
            set => _isDeveloperModeEnabled = value;
        }

        public string LibraryPath {
            get => _libraryPath;
            set => _libraryPath = value;
        }

        public bool IsMultiDexEnabled {
            get => _isMultiDexEnabled;
            set => _isMultiDexEnabled = value;
        }

        public bool IsCoreEnabled {
            get => _isCoreEnabled;
            set => _isCoreEnabled = value;
        }

        public bool IsAdColonyEnabled {
            get => _isAdColonyEnabled;
            set => _isAdColonyEnabled = value;
        }

        public bool IsAdMobEnabled {
            get => _isAdMobEnabled;
            set => _isAdMobEnabled = value;
        }

        public bool IsAdMobMediationEnabled {
            get => _isAdMobMediationEnabled;
            set => _isAdMobMediationEnabled = value;
        }

        public string AdMobAndroidAppId {
            get => _adMobAndroidAppId;
            set => _adMobAndroidAppId = value;
        }

        public string AdMobIosAppId {
            get => _adMobIosAppId;
            set => _adMobIosAppId = value;
        }

        public bool IsAdjustEnabled {
            get => _isAdjustEnabled;
            set => _isAdjustEnabled = value;
        }

        public bool IsFacebookAdsEnabled {
            get => _isFacebookAdsEnabled;
            set => _isFacebookAdsEnabled = value;
        }

        public bool IsIronSourceEnabled {
            get => _isIronSourceEnabled;
            set => _isIronSourceEnabled = value;
        }

        public bool IsIronSourceMediationEnabled {
            get => _isIronSourceMediationEnabled;
            set => _isIronSourceMediationEnabled = value;
        }

        public bool IsUnityAdsEnabled {
            get => _isUnityAdsEnabled;
            set => _isUnityAdsEnabled = value;
        }

        public static LibrarySettings Instance {
            get {
                if (_sharedInstance == null) {
                    if (!AssetDatabase.IsValidFolder(LibrarySettingsDir)) {
                        AssetDatabase.CreateFolder("Assets", "EE");
                    }
                    if (!AssetDatabase.IsValidFolder(LibrarySettingsResDir)) {
                        AssetDatabase.CreateFolder(LibrarySettingsDir, "Resources");
                    }
                    _sharedInstance = AssetDatabase.LoadAssetAtPath<LibrarySettings>(LibrarySettingsFile);
                    if (_sharedInstance == null) {
                        _sharedInstance = CreateInstance<LibrarySettings>();
                        AssetDatabase.CreateAsset(_sharedInstance, LibrarySettingsFile);
                    }
                }
                return _sharedInstance;
            }
        }

        public void WriteSettingsToFile() {
            UpdateDependencies();
            AssetDatabase.SaveAssets();
        }

        private void UpdateDependencies() {
            if (!AssetDatabase.IsValidFolder(LibrarySettingsEditorDir)) {
                AssetDatabase.CreateFolder(LibrarySettingsDir, "Editor");
            }
            XDocument document;
            try {
                document = XDocument.Load(LibraryDependenciesFile);
            } catch (IOException exception) {
                document = new XDocument();
            }
            var dependencies = document.Element("dependencies");
            if (dependencies == null) {
                dependencies = new XElement("dependencies");
                document.Add(dependencies);
            }
            var iosPods = dependencies.Element("iosPods");
            if (iosPods == null) {
                iosPods = new XElement("iosPods");
                dependencies.Add(iosPods);
            }
            var androidPackages = dependencies.Element("androidPackages");
            if (androidPackages == null) {
                androidPackages = new XElement("androidPackages");
                dependencies.Add(androidPackages);
            }
            iosPods.RemoveAll();
            androidPackages.RemoveAll();
            var androidLibraries = new List<string>();
            var androidRepositories = new List<string>();
            var iosLibraries = new List<string>();
            if (IsMultiDexEnabled) {
                androidLibraries.Add("androidx.multidex:multidex:[2.0.1]");
            }
            if (IsCoreEnabled) {
                androidRepositories.Add("https://dl.bintray.com/enrevol/ee-x");
                androidLibraries.Add($"com.senspark.ee:core:[{LibraryVersion}]");
                iosLibraries.Add("ee-x/cs-core");
                if (IsAdjustEnabled) {
                    androidLibraries.Add($"com.senspark.ee:adjust:[{LibraryVersion}]");
                    iosLibraries.Add("ee-x/cs-adjust");
                    iosPods.Add(new XElement("iosPod", new XAttribute("name", "Adjust"),
                        new XAttribute("modular_headers", true)));
                }
                if (IsAdColonyEnabled) {
                    androidRepositories.Add("https://adcolony.bintray.com/AdColony");
                    androidLibraries.Add($"com.senspark.ee:ad-colony:[{LibraryVersion}]");
                    iosLibraries.Add("ee-x/cs-ad-colony");
                }
                if (IsAdMobEnabled) {
                    if (IsAdMobMediationEnabled) {
                        androidRepositories.Add("https://adcolony.bintray.com/AdColony");
                        androidLibraries.Add($"com.senspark.ee:admob-mediation:[{LibraryVersion}]");
                        iosLibraries.Add("ee-x/cs-admob-mediation");
                    } else {
                        androidLibraries.Add($"com.senspark.ee:admob:[{LibraryVersion}]");
                        iosLibraries.Add("ee-x/cs-admob");
                    }
                }
                if (IsFacebookAdsEnabled) {
                    androidLibraries.Add($"com.senspark.ee:facebook-ads:[{LibraryVersion}]");
                    iosLibraries.Add("ee-x/cs-facebook-ads");
                }
                if (IsIronSourceEnabled) {
                    androidRepositories.Add("https://dl.bintray.com/ironsource-mobile/android-sdk");
                    if (IsIronSourceMediationEnabled) {
                        androidRepositories.Add("https://dl.bintray.com/ironsource-mobile/android-adapters");
                        androidRepositories.Add("https://adcolony.bintray.com/AdColony");
                        androidLibraries.Add($"com.senspark.ee:iron-source-mediation:[{LibraryVersion}]");
                        iosLibraries.Add("ee-x/cs-iron-source-mediation");
                    } else {
                        androidLibraries.Add($"com.senspark.ee:iron-source:[{LibraryVersion}]");
                        iosLibraries.Add("ee-x/cs-iron-source");
                    }
                }
                if (IsUnityAdsEnabled) {
                    androidLibraries.Add($"com.senspark.ee:unity-ads:[{LibraryVersion}]");
                    iosLibraries.Add("ee-x/cs-unity-ads");
                }
            }
            var repositories = new XElement("repositories");
            foreach (var repository in androidRepositories) {
                repositories.Add(new XElement("repository", repository));
            }
            androidPackages.Add(repositories);
            foreach (var library in androidLibraries) {
                androidPackages.Add(new XElement("androidPackage", new XAttribute("spec", library)));
            }
            foreach (var library in iosLibraries) {
                var pod = new XElement("iosPod", new XAttribute("name", library));
                if (IsDeveloperModeEnabled) {
                    pod.Add(new XAttribute("path", LibraryPath));
                } else {
                    pod.Add(new XAttribute("version", LibraryVersion));
                }
                iosPods.Add(pod);
            }
            document.Save(LibraryDependenciesFile);
        }
    }
}