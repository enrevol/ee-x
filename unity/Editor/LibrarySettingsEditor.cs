#if UNITY_EDITOR

using UnityEditor;

using UnityEngine;

namespace EE.Editor {
    [CustomEditor(typeof(LibrarySettings))]
    public class LibrarySettingsEditor : UnityEditor.Editor {
        [MenuItem("Assets/Senspark EE-x/Settings")]
        public static void OpenInspector() {
            Selection.activeObject = LibrarySettings.Instance;
        }

        public override void OnInspectorGUI() {
            var settings = LibrarySettings.Instance;
            EditorGUILayout.LabelField("Utilities", EditorStyles.boldLabel);
            settings.IsMultiDexEnabled =
                EditorGUILayout.Toggle(new GUIContent("Use MultiDex"), settings.IsMultiDexEnabled);
            EditorGUILayout.Separator();
            EditorGUILayout.LabelField("Modules", EditorStyles.boldLabel);

            // Core plugin is always enabled.
            // Prevent user from changing.
            EditorGUI.BeginDisabledGroup(true);
            settings.IsCoreEnabled = EditorGUILayout.Toggle(new GUIContent("Core"), settings.IsCoreEnabled);
            EditorGUI.EndDisabledGroup();

            // Adjust plugin.
            settings.IsAdjustEnabled = EditorGUILayout.Toggle(new GUIContent("Adjust"), settings.IsAdjustEnabled);

            // AdMob plugin.
            settings.IsAdMobEnabled = EditorGUILayout.Toggle(new GUIContent("AdMob"), settings.IsAdMobEnabled);
            EditorGUI.BeginDisabledGroup(!settings.IsAdMobEnabled);
            ++EditorGUI.indentLevel;
            settings.IsAdMobMediationEnabled =
                EditorGUILayout.Toggle(new GUIContent("Use Mediation"), settings.IsAdMobMediationEnabled);
            settings.AdMobAndroidAppId = EditorGUILayout.TextField("Android App ID", settings.AdMobAndroidAppId);
            settings.AdMobIosAppId = EditorGUILayout.TextField("iOS App ID", settings.AdMobIosAppId);
            --EditorGUI.indentLevel;
            EditorGUI.EndDisabledGroup();

            // IronSource plugin.
            settings.IsIronSourceEnabled =
                EditorGUILayout.Toggle(new GUIContent("IronSource"), settings.IsIronSourceEnabled);
            EditorGUI.BeginDisabledGroup(!settings.IsIronSourceEnabled);
            ++EditorGUI.indentLevel;
            settings.IsIronSourceMediationEnabled =
                EditorGUILayout.Toggle(new GUIContent("Use Mediation"), settings.IsIronSourceMediationEnabled);
            --EditorGUI.indentLevel;
            EditorGUI.EndDisabledGroup();

            if (GUI.changed) {
                OnSettingsChanged();
            }
        }

        private void OnSettingsChanged() {
            EditorUtility.SetDirty(target);
            LibrarySettings.Instance.WriteSettingsToFile();
        }
    }
}

#endif // UNITY_EDITOR