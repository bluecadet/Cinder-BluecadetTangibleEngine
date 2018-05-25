#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "bluecadet/views/BaseView.h"
#include "bluecadet/views/EllipseView.h"
#include "bluecadet/views/TextView.h"

#include "TangibleEnginePlugin.h"

namespace bluecadet {
namespace tangibleengine {

typedef std::shared_ptr<class TangibleDebugLayerView> TangibleDebugLayerViewRef;
typedef std::shared_ptr<class InfoView> InfoViewRef;

//==================================================
// Main Class
// 

class TangibleDebugLayerView : public bluecadet::views::BaseView {

public:
	TangibleDebugLayerView(TangibleEnginePluginRef plugin);
	~TangibleDebugLayerView();

protected:
	void handleTangiblePlaced(const TE_Tangible & tangible);
	void handleTangibleUpdated(const TE_Tangible & tangible);
	void handleTangibleLifted(const TE_Tangible & tangible);

	InfoViewRef acquireView();
	void recycleView(InfoViewRef view);

	TangibleEnginePluginRef mPlugin;
	std::map<int, InfoViewRef> mActiveViewsById;
	std::deque<InfoViewRef> mInactiveViews;

	ci::signals::ConnectionList mSignalConnections;

};

//==================================================
// Helper Class
// 

class InfoView : public bluecadet::views::EllipseView {

public:
	InfoView();
	~InfoView();

	void setup(TE_Tangible tangible, TE_Pattern pattern);
	void animateIn(std::function<void()> callback = nullptr);
	void animateOut(std::function<void()> callback = nullptr);

	const TE_Tangible & getTangible() const { return mTangible; }

protected:
	bluecadet::views::TextViewRef mLabelView;
	TE_Tangible mTangible;
	TE_Pattern mPattern;
};

}
}
