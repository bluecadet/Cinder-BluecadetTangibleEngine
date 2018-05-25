#include "TangibleEngineSimulator.h"

#include "cinder/Log.h"
#include "cinder/Color.h"

#include "bluecadet/core/ScreenCamera.h"
#include "bluecadet/views/EllipseView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace tangibleengine {

//==================================================
// Public
// 

TangibleEngineSimulator::TangibleEngineSimulator() :
	mView(new views::BaseView())
{
	//mParams = params::InterfaceGl::create("Tangible Engine Simulator", ivec2(200, 250));
}

TangibleEngineSimulator::~TangibleEngineSimulator() {
}

void TangibleEngineSimulator::setup(TangibleEnginePluginRef plugin, bluecadet::touch::TouchManagerRef touchManager) {
	destroy();

	mPlugin = plugin;
	mTouchManager = touchManager;

	setupTangibles();

	mUpdateSignalConnection = App::get()->getSignalUpdate().connect(bind(&TangibleEngineSimulator::handleUpdate, this));
}

void TangibleEngineSimulator::destroy() {
	mUpdateSignalConnection.disconnect();

	//mParams->clear();

	mPlugin = nullptr;
	mTouchManager = nullptr;
}

//==================================================
// Protected
// 

void TangibleEngineSimulator::setupTangibles() {
	// TODO: move to heap/store permanently
	const int maxNumPatterns = 128;
	int numPatterns = maxNumPatterns;
	TE_Pattern patterns[maxNumPatterns];
	TE_GetPatterns(patterns, &numPatterns);

	if (numPatterns <= 0) {
		CI_LOG_W("No patterns configured with Tangible Engine. Make sure to call setup() after Tangible Engine has been configured.");
	}

	mView->removeAllChildren();

	for (int i = 0; i < numPatterns; ++i) {
		vec3 hsv = vec3((float)i / (float) numPatterns, 1.0f, 1.0f);
		ColorA color = ColorA(hsvToRgb(hsv), 0.5f);
		VirtualTangibleRef tangible(new VirtualTangible(patterns[i], color));
		mTangibles.push_back(tangible);
		mView->addChild(tangible);
	}
}

void TangibleEngineSimulator::setupParams() {

}

void TangibleEngineSimulator::handleUpdate() {
	for (auto tangible : mTangibles) {
		tangible->updateTouches();
		for (auto touch : tangible->getTouches()) {
			mTouchManager->addTouch(touch);
		}
	}
}

}
}
