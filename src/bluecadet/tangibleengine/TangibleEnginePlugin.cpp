#include "TangibleEnginePlugin.h"

#include "cinder/Log.h"

#include "ideum/tangibleengine/tangible_tracker.h"

#include "bluecadet/touch/TouchManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace tangibleengine {

TangibleEnginePlugin::TangibleEnginePlugin(ci::fs::path patternsFilePath) : bluecadet::touch::TouchManagerPlugin(),
	mPatternsFilePath(patternsFilePath)
{
	mPatternsFilePath = patternsFilePath;
}

TangibleEnginePlugin::~TangibleEnginePlugin() {
}

void TangibleEnginePlugin::wasAddedTo(bluecadet::touch::TouchManager * manager) {
	try {
		TE_Init();

		if (!fs::exists(mPatternsFilePath)) {
			CI_LOG_E("Patterns file could not be found: '" + mPatternsFilePath.string() + "'");
		}

		const int status = TE_SetPatternsFromFile((char *)mPatternsFilePath.string().c_str());

		if (status == 0) {
			CI_LOG_E("Could not load patterns from '" << mPatternsFilePath << "'");
		}

		if (manager->getDiscardMissedTouches()) {
			CI_LOG_I("Configuring touch manager to not discard missed touches (required for Tangible Engine to work)");
			manager->setDiscardMissedTouches(false);
		}

		updateTangibles();
		updatePatterns();

	} catch (Exception e) {
		CI_LOG_EXCEPTION("Can't initialize tangible engine", e);
	}
}

void TangibleEnginePlugin::willBeRemovedFrom(bluecadet::touch::TouchManager * manager) {
	try {
		TE_Deinit();
	} catch (Exception e) {
		CI_LOG_EXCEPTION("Can't deinitialize tangible engine", e);
	}
}

void TangibleEnginePlugin::preUpdate(bluecadet::touch::TouchManager * manager, std::deque<bluecadet::touch::Touch> & touches) {
	// forward touches to TE
	for (auto & touch : touches) {
		forwardTouch(touch);
	}

	updateTangibles();

	// updpate touches that were captured by TE
	for (auto & touch : touches) {
		updateTouchType(touch);
	}
}



void TangibleEnginePlugin::forwardTouch(bluecadet::touch::Touch & touch) {
	// ignore mouse or other touches
	if (touch.type != touch::TouchType::Fiducial &&
		touch.type != touch::TouchType::Touch &&
		touch.type != touch::TouchType::Simulator) {
		return;
	}

	// see http://tangibleengine.com/documentation/tangible_tracker/html/tangible__tracker_8h.html#a4aabe096dc7ed004a98ab6d4f81560fe
	const int downState = touch.phase == bluecadet::touch::TouchPhase::Ended ? 0 : 1;
	TE_ProcessTouchEvent(downState, touch.id, touch.appPosition.x, touch.appPosition.y);
}

inline void TangibleEnginePlugin::updateTouchType(bluecadet::touch::Touch & touch) {
	const int teStatus = TE_IsTouchPointOwned(touch.id);

	if (teStatus != 0) {
		touch.type = bluecadet::touch::TouchType::Fiducial;
	}
}

void TangibleEnginePlugin::updateTangibles() {
	const int maxNumTangibles = 16;
	int numTangibles = maxNumTangibles;
	TE_Tangible tangibles[maxNumTangibles];
	TE_GetTangibles(tangibles, &numTangibles);

	set<int> activeIds;
	for (int i = 0; i < numTangibles; ++i) {
		activeIds.insert(tangibles[i].id);
	}

	// check for lifted tangibles
	for (auto tangibleIt = mTangiblesById.begin(); tangibleIt != mTangiblesById.end();) {
		auto activeIdIt = activeIds.find(tangibleIt->first);
		if (activeIdIt == activeIds.end()) {
			mSignalTangibleLifted.emit(tangibleIt->second);
			tangibleIt = mTangiblesById.erase(tangibleIt);
		} else {
			++tangibleIt;
		}
	}

	// check for new and updated tangibles
	for (int i = 0; i < numTangibles; ++i) {
		const auto & tangible = tangibles[i];
		auto existingIt = mTangiblesById.find(tangible.id);

		if (existingIt == mTangiblesById.end()) {
			// new tangible
			mTangiblesById[tangible.id] = tangible;
			mSignalTangiblePlaced.emit(tangible);

		} else {
			// update tangible
			mTangiblesById[tangible.id] = tangible;
			mSignalTangibleUpdated.emit(tangible);
		}
	}
	
}

void TangibleEnginePlugin::updatePatterns() {
	// TODO: move to heap/store permanently
	const int maxNumPatterns = 128;
	int numPatterns = maxNumPatterns;
	TE_Pattern patterns[maxNumPatterns];
	TE_GetPatterns(patterns, &numPatterns);

	mPatternsByNames.clear();

	for (int i = 0; i < numPatterns; ++i) {
		const auto & pattern = patterns[i];
		mPatternsByNames[string(pattern.name)] = pattern;
	}
}

}
}
