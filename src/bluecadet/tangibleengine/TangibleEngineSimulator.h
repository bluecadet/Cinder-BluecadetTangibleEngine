#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/params/Params.h"

#include "ideum/tangibleengine/tangible_tracker.h"

#include "bluecadet/views/TouchView.h"
#include "bluecadet/touch/TouchManager.h"

#include "TangibleEnginePlugin.h"
#include "VirtualTangible.h"

namespace bluecadet {
namespace tangibleengine {

typedef std::shared_ptr<class TangibleEngineSimulator> TangibleEngineSimulatorRef;

class TangibleEngineSimulator {

public:
	TangibleEngineSimulator();
	~TangibleEngineSimulator();

	void setup(TangibleEnginePluginRef plugin, bluecadet::touch::TouchManagerRef touchManager);
	void destroy();

	ci::params::InterfaceGlRef getParams() const { return mParams; }
	bluecadet::views::BaseViewRef getView() const { return mView; }
	std::vector<VirtualTangibleRef> getTangibles() const { return mTangibles; }

protected:

	void setupTangibles();
	void setupParams();
	void handleUpdate();

	ci::signals::Connection mUpdateSignalConnection;

	TangibleEnginePluginRef mPlugin;
	bluecadet::touch::TouchManagerRef mTouchManager;
	bluecadet::views::BaseViewRef mView;
	std::vector<VirtualTangibleRef> mTangibles;
	
	ci::params::InterfaceGlRef mParams;
};

}
}
