using UnityEngine;

namespace EETest {
    public class ClickEvent : EE.DynamicAnalyticsEvent {
        public override string EventName => "click";

        [EE.AnalyticsParameter("button")]
        public string Button;
    }

    public class MainScene : MonoBehaviour {
        private bool _initialized;
        private EE.IAnalyticsManager _analyticsManager;
        private EE.IAudioManager _audioManager;
        private EE.ISceneLoader _sceneLoader;
        private EE.IAdsManager _adsManager;

        private void Awake() {
            EE.Utils.NoAwait(async () => {
                await ServiceUtils.Initialize();
                _analyticsManager = EE.ServiceLocator.Resolve<EE.IAnalyticsManager>();
                _audioManager = EE.ServiceLocator.Resolve<EE.IAudioManager>();
                _sceneLoader = EE.ServiceLocator.Resolve<EE.ISceneLoader>();
                _adsManager = EE.ServiceLocator.Resolve<EE.IAdsManager>();
                _initialized = true;
            });
        }

        public void OnAudioButtonPressed() {
            if (!_initialized) {
                return;
            }
            _audioManager.IsMusicEnabled = !_audioManager.IsMusicEnabled;
            _audioManager.IsSoundEnabled = !_audioManager.IsSoundEnabled;
        }

        public void OnOpenTestSuiteButtonPressed() {
            if (!_initialized) {
                return;
            }
            _analyticsManager.LogEvent(new ClickEvent {
                Button = "open_test_suite"
            });
            _adsManager.OpenTestSuite();
        }

        public void OnTestBannerAdButtonPressed() {
            if (!_initialized) {
                return;
            }
            _analyticsManager.LogEvent(new ClickEvent {
                Button = "test_banner_ad"
            });
            EE.Utils.NoAwait(async () => {
                var scene = await _sceneLoader.LoadScene<BannerAdScene>(nameof(BannerAdScene));
            });
        }

        public void OnTestFullScreenAdButtonPressed() {
            if (!_initialized) {
                return;
            }
            _analyticsManager.LogEvent(new ClickEvent {
                Button = "test_full_screen_ad"
            });
            EE.Utils.NoAwait(async () => {
                var scene = await _sceneLoader.LoadScene<FullScreenAdScene>(nameof(FullScreenAdScene));
            });
        }
    }
}