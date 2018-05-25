#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "bluecadet/core/BaseApp.h"
#include "bluecadet/core/ScreenCamera.h"
#include "bluecadet/views/TouchView.h"

#include "bluecadet/gestureworks/GestureWorksPlugin.h"

#include "bluecadet/tangibleengine/TangibleEnginePlugin.h"
#include "bluecadet/tangibleengine/TangibleEngineSimulator.h"
#include "bluecadet/tangibleengine/TangibleDebugLayerView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;
using namespace bluecadet::gestureworks;
using namespace bluecadet::tangibleengine;

class BasicTangibleSampleApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;
	void update() override;
	void draw() override;

	TangibleEngineSimulatorRef mSimulator;
	TangibleDebugLayerViewRef mDebugLayer;
};

void BasicTangibleSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	SettingsManager::getInstance()->setup(settings, "", [=](SettingsManager * manager) {
		manager->mWindowSize = ivec2(1280, 720);
		manager->mDisplaySize = ivec2(3840, 2160);
		manager->mFullscreen = false;
		manager->mBorderless = false;
		manager->mConsole = false;
		manager->mMinimizeParams = true;
		manager->mShowTouches = true;
		manager->mNativeTouchEnabled = true;
		manager->mMouseEnabled = true;
		manager->mShowCursor = true;
	});

	ScreenLayout::getInstance()->setDisplaySize(ivec2(3840, 2160));
	ScreenLayout::getInstance()->setNumRows(1);
	ScreenLayout::getInstance()->setNumColumns(1);
}

void BasicTangibleSampleApp::setup() {
	BaseApp::setup();

	fs::path gmlPath = getAssetPath("bluecadet_gestures.gml");
	GestureWorksPlugin::getInstance()->setGmlPath(gmlPath);
	TouchManager::getInstance()->addPlugin(GestureWorksPlugin::getInstance());

	const ci::fs::path patternsPath = getAssetPath("bluecadet_tangible_patterns_three_points.json");
	TangibleEnginePlugin::getInstance()->setPatternsFilePath(patternsPath);
	TouchManager::getInstance()->addPlugin(TangibleEnginePlugin::getInstance());

	mDebugLayer = make_shared<TangibleDebugLayerView>(TangibleEnginePlugin::getInstance());
	getRootView()->addChild(mDebugLayer);

	mSimulator = make_shared<TangibleEngineSimulator>();
	mSimulator->setup(TangibleEnginePlugin::getInstance(), TouchManager::getInstance());
	getRootView()->addChild(mSimulator->getView());

	auto tangibles = mSimulator->getTangibles();
	const float height = ScreenLayout::getInstance()->getAppSize().y;
	const float numPucks = tangibles.size();
	float maxWidth = 0;

	for (int i = 0; i < tangibles.size(); ++i) {
		maxWidth = glm::max(tangibles[i]->getSize().x, maxWidth);
	}
	for (int i = 0; i < tangibles.size(); ++i) {
		auto tangible = tangibles[i];
		tangible->setPosition(vec2(maxWidth * 0.5f, height * ((float)i + 0.5f) / numPucks));
	}
}

void BasicTangibleSampleApp::update() {
	BaseApp::update();
}

void BasicTangibleSampleApp::draw() {
	gl::drawStringCentered("Tap pucks to toggle between placed/lifted", getWindowCenter(), Color::white(), Font("Arial", 30));
	BaseApp::draw();
}

CINDER_APP(BasicTangibleSampleApp, RendererGl(RendererGl::Options().msaa(4)), BasicTangibleSampleApp::prepareSettings);