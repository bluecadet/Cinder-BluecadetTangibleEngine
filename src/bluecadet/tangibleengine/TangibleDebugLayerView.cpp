#include "TangibleEngineSimulator.h"

#include "cinder/Log.h"

#include "bluecadet/views/EllipseView.h"
#include "TangibleDebugLayerView.h"
#include "TangibleUtils.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace tangibleengine {

//==================================================
// Main Class
// 

TangibleDebugLayerView::TangibleDebugLayerView(TangibleEnginePluginRef plugin) : 
	mPlugin(plugin)
{
	mSignalConnections += mPlugin->getSignalTangiblePlaced().connect(bind(&TangibleDebugLayerView::handleTangiblePlaced, this, std::placeholders::_1));
	mSignalConnections += mPlugin->getSignalTangibleUpdated().connect(bind(&TangibleDebugLayerView::handleTangibleUpdated, this, std::placeholders::_1));
	mSignalConnections += mPlugin->getSignalTangibleLifted().connect(bind(&TangibleDebugLayerView::handleTangibleLifted, this, std::placeholders::_1));
}

TangibleDebugLayerView::~TangibleDebugLayerView() {
}

void TangibleDebugLayerView::handleTangiblePlaced(const TE_Tangible & tangible) {
	const auto & patterns = mPlugin->getPatternsByNames();
	const string patternName = string(tangible.name);
	const auto patternIt = patterns.find(patternName);

	if (patternIt == patterns.end()) {
		CI_LOG_W("Tangible '" + to_string(tangible.id) + "' is using an unrecognized pattern '" + patternName + "'");
		return;
	}

	// create new view
	auto view = acquireView();
	view->setup(tangible, patternIt->second);
	mActiveViewsById[tangible.id] = view;
	addChild(view);
	view->animateIn();

	view->setGlobalPosition(vec2(tangible.x, tangible.y));
}

void TangibleDebugLayerView::handleTangibleUpdated(const TE_Tangible & tangible) {
	auto viewIt = mActiveViewsById.find(tangible.id);
	if (viewIt == mActiveViewsById.end()) {
		CI_LOG_W("No view found for updated tangible id '" + to_string(tangible.id) + "'");
		return;
	}

	vec2 pos(tangible.x, tangible.y);
	viewIt->second->setGlobalPosition(pos);
}

void TangibleDebugLayerView::handleTangibleLifted(const TE_Tangible & tangible) {
	auto viewIt = mActiveViewsById.find(tangible.id);

	if (viewIt == mActiveViewsById.end()) {
		CI_LOG_W("No view found for lifted tangible '" + to_string(tangible.id) + "'");
		return;
	}

	auto view = viewIt->second;
	viewIt = mActiveViewsById.erase(viewIt);
	view->animateOut([=] {
		App::get()->dispatchAsync([=] {
			removeChild(view);
			recycleView(view);
		});
	});
}

InfoViewRef TangibleDebugLayerView::acquireView() {
	InfoViewRef view = nullptr;
	if (mInactiveViews.empty()) {
		view = InfoViewRef(new InfoView());
	} else {
		view = mInactiveViews.back();
		mInactiveViews.pop_back();
	}
	return view;
}

void TangibleDebugLayerView::recycleView(InfoViewRef view) {
	mInactiveViews.push_back(view);
}

//==================================================
// Helper Class
// 

InfoView::InfoView() :
	mLabelView(new bluecadet::views::TextView())
{
	setBackgroundColor(ColorAf(1.0f, 1.0f, 1.0f, 0.5f));
	mLabelView->setPadding(8.0f, 8.0f);
	mLabelView->setFontSize(40.0f);
	mLabelView->setTextAlign(bluecadet::text::TextAlign::Center);
	mLabelView->setTextColor(ColorAf(0.0f, 0.0f, 0.0f, 0.85f));
	addChild(mLabelView);
}

InfoView::~InfoView() {
}

void InfoView::setup(TE_Tangible tangible, TE_Pattern pattern) {
	mTangible = tangible;
	mPattern = pattern;

	const float padding = 128.0f;
	const float radius = TangibleUtils::getPatternRadius(pattern);

	setRadius(radius + padding);
	setScale(0);
	setAlpha(0);

	mLabelView->setText(string(pattern.name) + " (" + to_string(tangible.id) + ")" );
	mLabelView->setPosition(vec2(-mLabelView->getTextWidth() * 0.5f, - radius - padding * 0.5f -mLabelView->getTextHeight() * 0.5f));
}

void InfoView::animateIn(std::function<void()> callback) {
	const float duration = 0.33f;
	getTimeline()->apply(&getAlpha(), 1.0f, duration, easeOutQuart);
	getTimeline()->apply(&getScale(), vec2(1.0f), duration, ci::EaseOutBack());
}

void InfoView::animateOut(std::function<void()> callback) {
	const float duration = 0.33f;
	getTimeline()->apply(&getAlpha(), 0.0f, duration, easeInQuart);
	getTimeline()->apply(&getScale(), vec2(0.0f), duration, easeInQuart).finishFn(callback);
}

}
}
