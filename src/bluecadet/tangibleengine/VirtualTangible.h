#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/params/Params.h"

#include "TangibleUtils.h"

#include "bluecadet/views/TouchView.h"
#include "bluecadet/views/TextView.h"
#include "bluecadet/views/EllipseView.h"

#include "bluecadet/gestureworks/handlers/ManipulateGestureHandler.h"

namespace bluecadet {
namespace tangibleengine {

typedef std::shared_ptr<class VirtualTangible> VirtualTangibleRef;

class VirtualTangible : public bluecadet::views::TouchView {
public:
	VirtualTangible(TE_Pattern pattern, ci::ColorA color, bool draggingEnabled = true);
	virtual ~VirtualTangible() {};

	bool canAcceptTouch(const bluecadet::touch::Touch & touch) const override;
	bool containsPoint(const ci::vec2 & point) override;
	//void handleTouchMoved(const bluecadet::touch::TouchEvent & event) override;
	void handleTouchTapped(const bluecadet::touch::TouchEvent & event) override;
	void updateTouches();

	inline const std::vector<bluecadet::touch::Touch> & getTouches() { return mTouches; }

	bool getIsPlaced() const { return mIsPlaced; }
	void setIsPlaced(const bool value) { mIsPlaced = value; }

	bool getWasPlaced() const { return mWasPlaced; }
	void setWasPlaced(const bool value) { mWasPlaced = value; }

	ci::ColorA getColor() const { return mBackgroundView->getBackgroundColor(); }
	void setColor(const ci::ColorA value) { mBackgroundView->setBackgroundColor(value); }

	const TE_Pattern & getPattern() const { return mPattern; }

protected:
	static int sTouchId;

	void didMoveToView(bluecadet::views::BaseView* parent) override;
	void willMoveFromView(bluecadet::views::BaseView* parent) override;

	TE_Pattern mPattern;
	
	std::vector<ci::vec2> mPoints;
	std::vector<bluecadet::touch::Touch> mTouches;

	bluecadet::views::TextViewRef mLabelView;
	bluecadet::views::EllipseViewRef mBackgroundView;
	bluecadet::gestureworks::handlers::ManipulateGestureHandlerRef mGestureHandler;

	float mRadius = 0;
	bool mWasPlaced = false;
	bool mIsPlaced = false;
	bool mDraggingEnabled = true;

};

}
}
