# Cinder-BluecadetTangibleEngine

Tangible engine plugin for Cinder-BluecadetViews block.

## Features

- Reads patterns json for pucks and initializes TangibleEngine
- Forwards BluecadetViews touch events to TangibleEngine
- Marks puck touch points as `touch::TouchType::Fiducial`
- Provides optional debug layer to visualize placed pucks
- Provides optional puck simulator that allows virtual pucks to be moved and placed/lifted (works with the same patterns JSON as the plugin and generates simulated touches, which in turn are recognized as pucks)

## Licensed Files

Tangible Engine comes licensed headers and libraries that we can't include in this repo. In order to get this block running, you'll need to:

- Place `tangible_tracker.h` in [`include/idem/tangibleengine/`](`include/idem/tangibleengine/`)
- Place your x64 `tangible_tracker.lib` and `tangible_tracker.dll` in [`lib/x64/`](`lib/x64/`)
- Place your x86 `tangible_tracker.lib` and `tangible_tracker.dll` in [`lib/x64/`](`lib/x86/`)

This repo comes with a sample patterns file at `assets/bluecadet_tangible_patterns.json`, which is specific to patterns used by Bluecadet on previous projects. You will likely need to replace this with your own file.

## Simple Example

```c++
// Initialize the touch manager plugin with your patterns file
const ci::fs::path patternsPath = getAssetPath("bluecadet_tangible_patterns.json");
mPlugin = make_shared<TangibleEnginePlugin>(patternsPath);
TouchManager::getInstance()->addPlugin(mPlugin);

// Optional debug layer to visualize recognized tangibles
mDebugLayer = make_shared<TangibleDebugLayerView>(mPlugin);
getRootView()->addChild(mDebugLayer);

// Optional simulator to produce draggable tangibles on non-touch devices
mSimulator = make_shared<TangibleEngineSimulator>();
mSimulator->setup(mPlugin, TouchManager::getInstance());
getRootView()->addChild(mSimulator->getView());

// Optionally lay out your virtual tangibles
auto simulatedTangibles = mSimulator->getTangibles();

for (int i = 0; i < simulatedTangibles.size(); ++i) {
	auto simulatedTangible = simulatedTangibles[i];
	simulatedTangible->setPosition(vec2(0.5f, (0.5f + i)) * simulatedTangible->getSize());
}
```

## Full Setup Example

The following example is a full Cinder app with touch simulator and gesture works support.

```c++
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "bluecadet/core/BaseApp.h"
#include "bluecadet/views/TouchView.h"

#include "bluecadet/gestureworks/GestureWorksPlugin.h"

#include "bluecadet/tangibleengine/TangibleEnginePlugin.h"
#include "bluecadet/tangibleengine/TangibleDebugLayerView.h"
#include "bluecadet/tangibleengine/TangibleEngineSimulator.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;
using namespace bluecadet::gestureworks;
using namespace bluecadet::tangibleengine;

class TangibleSampleApp : public BaseApp {
public:
    static void prepareSettings(ci::app::App::Settings* settings);
    void setup() override;
    void update() override;
    void draw() override;

    TangibleDebugLayerViewRef mDebugLayer;
    TangibleEngineSimulatorRef mSimulator;
};

void TangibleSampleApp::prepareSettings(ci::app::App::Settings* settings) {
    SettingsManager::getInstance()->setup(settings, ci::app::getAssetPath("settings.json"), [](SettingsManager * manager) {
        manager->mCollapseParams = true;
    });
}

void TangibleSampleApp::setup() {
    BaseApp::setup();

    fs::path gmlPath = getAssetPath("bluecadet_gestures.gml");
    GestureWorksPlugin::getInstance()->setGmlPath(gmlPath);
    TouchManager::getInstance()->addPlugin(GestureWorksPlugin::getInstance());

    const ci::fs::path patternsPath = getAssetPath("bluecadet_tangible_patterns.json");
    TangibleEnginePlugin::getInstance()->setPatternsFilePath(patternsPath);
    TouchManager::getInstance()->addPlugin(TangibleEnginePlugin::getInstance());

    mDebugLayer = make_shared<TangibleDebugLayerView>(TangibleEnginePlugin::getInstance());
    getRootView()->addChild(mDebugLayer);

    mSimulator = make_shared<TangibleEngineSimulator>();
    mSimulator->setup(TangibleEnginePlugin::getInstance(), TouchManager::getInstance());
    getRootView()->addChild(mSimulator->getView());

    // Lay out simulated pucks
    auto tangibles = mSimulator->getTangibles();
    const float height = (float)ScreenLayout::getInstance()->getAppSize().y;
    const float numPucks = (float)tangibles.size();
    float maxWidth = 0;

    for (int i = 0; i < tangibles.size(); ++i) {
        maxWidth = glm::max(tangibles[i]->getSize()().x, maxWidth);
    }
    for (int i = 0; i < tangibles.size(); ++i) {
        auto & tangible = tangibles[i];
        auto & pattern = tangible->getPattern();
        auto color = hsvToRgb(vec3((float) i / (float) tangibles.size(), 1.0f, 1.0f));
        tangible->setPosition(vec2(-maxWidth * 0.5f, height * ((float)i + 0.5f) / numPucks));
        tangible->setColor(color);
    }
}

void TangibleSampleApp::update() {
    BaseApp::update();
}

void TangibleSampleApp::draw() {
    BaseApp::draw();
}

CINDER_APP(TangibleSampleApp, RendererGl(RendererGl::Options().msaa(4)), TangibleSampleApp::prepareSettings);

```
