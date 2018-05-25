#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "ideum/tangibleengine/tangible_tracker.h"
#include "bluecadet/touch/TouchManagerPlugin.h"

namespace bluecadet {
namespace tangibleengine {

typedef std::shared_ptr<class TangibleEnginePlugin> TangibleEnginePluginRef;

class TangibleEnginePlugin : public bluecadet::touch::TouchManagerPlugin {

public:

	typedef ci::signals::Signal<void(const TE_Tangible & tangible)> TangibleSignal;

	static TangibleEnginePluginRef getInstance() {
		static TangibleEnginePluginRef instance = nullptr;
		if (!instance) {
			instance = TangibleEnginePluginRef(new TangibleEnginePlugin());
		}
		return instance;
	}
	
	TangibleEnginePlugin(ci::fs::path patternsFilePath = "");
	~TangibleEnginePlugin();

	ci::fs::path getPatternsFilePath() const { return mPatternsFilePath; }
	void setPatternsFilePath(const ci::fs::path value) { mPatternsFilePath = value; }

	void wasAddedTo(bluecadet::touch::TouchManager * manager) override;
	void willBeRemovedFrom(bluecadet::touch::TouchManager * manager) override;

	void preUpdate(bluecadet::touch::TouchManager * manager, std::deque<bluecadet::touch::Touch> & touches) override;

	inline const std::map<int, TE_Tangible> & getTangiblesById() const { return mTangiblesById; };
	inline const std::map<std::string, TE_Pattern> & getPatternsByNames() const { return mPatternsByNames; }

	TangibleSignal & getSignalTangiblePlaced() { return mSignalTangiblePlaced; }
	TangibleSignal & getSignalTangibleUpdated() { return mSignalTangibleUpdated; }
	TangibleSignal & getSignalTangibleLifted() { return mSignalTangibleLifted; }

protected:

	inline void forwardTouch(bluecadet::touch::Touch & touch);
	inline void updateTouchType(bluecadet::touch::Touch & touch);

	inline void updateTangibles();
	inline void updatePatterns();

	TangibleSignal mSignalTangiblePlaced;
	TangibleSignal mSignalTangibleUpdated;
	TangibleSignal mSignalTangibleLifted;

	ci::fs::path mPatternsFilePath;
	std::map<int, TE_Tangible> mTangiblesById;
	std::map<std::string, TE_Pattern> mPatternsByNames;

};

}
}