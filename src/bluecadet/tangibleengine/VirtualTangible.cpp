#include "cinder/Log.h"
#include "cinder/Color.h"

#include "bluecadet/core/ScreenCamera.h"

#include "VirtualTangible.h"


using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::gestureworks;
using namespace bluecadet::text;
using namespace bluecadet::touch;
using namespace bluecadet::views;

namespace bluecadet {
namespace tangibleengine {

int VirtualTangible::sTouchId = -1000;

VirtualTangible::VirtualTangible(TE_Pattern pattern, ci::ColorA color, bool draggingEnabled) : TouchView(),
	mPattern(pattern),
	mLabelView(new TextView()),
	mDraggingEnabled(draggingEnabled),
	mGestureHandler(nullptr)
{
	const float touchPointRadius = 32.0f;

	mPoints = TangibleUtils::getLocalizedPatternPoints(pattern);
	mRadius = TangibleUtils::getPatternRadius(pattern) + touchPointRadius;

	// bg view
	mBackgroundView = EllipseViewRef(new EllipseView());
	mBackgroundView->setBackgroundColor(color);
	mBackgroundView->setRadius(mRadius);
	addChild(mBackgroundView);

	// touch points
	const ColorA touchOverlayColor;
	for (const auto & point : mPoints) {
		EllipseViewRef touchPointView(new EllipseView());
		touchPointView->setRadius(touchPointRadius);
		touchPointView->setBackgroundColor(ColorA(1.0f, 1.0f, 1.0f, 0.5f));
		touchPointView->setPosition(point);
		addChild(touchPointView);

		Touch touch = Touch(sTouchId--, vec2(), TouchType::Simulator, TouchPhase::Ended);
		mTouches.push_back(touch);
	}

	// crosshair
	const vec2 crosshairSize(mRadius * 2.0f, 2);
	auto crossH = make_shared<BaseView>();
	crossH->setBackgroundColor(ColorA(1.0f, 1.0f, 1.0f, 0.25f));
	crossH->setSize(crosshairSize);
	crossH->setPosition(-crossH->getSize() * 0.5f);
	mBackgroundView->addChild(crossH);

	auto crossV = make_shared<BaseView>();
	crossV->setBackgroundColor(crossH->getBackgroundColor());
	crossV->setSize(vec2(crosshairSize.y, crosshairSize.x));
	crossV->setPosition(-crossV->getSize() * 0.5f);
	mBackgroundView->addChild(crossV);

	// label view
	mLabelView->setFontSize(40.0f);
	mLabelView->setFontWeight(FontWeight::Medium);
	mLabelView->setTextAlign(TextAlign::Center);
	mLabelView->setTextColor(ColorAf(0.0f, 0.0f, 0.0f, 0.85f));
	mLabelView->setText(string(pattern.name));
	mLabelView->setPosition(-vec2(mLabelView->getTextSize()) * 0.5f);
	mBackgroundView->addChild(mLabelView);

	setPosition(TangibleUtils::getPatternCenter(pattern));
	setSize(mBackgroundView->getSize());
	updateTouches();
}

bool VirtualTangible::canAcceptTouch(const Touch & touch) const {
	return
		touch.type != TouchType::Fiducial &&
		touch.type != TouchType::Simulator &&
		TouchView::canAcceptTouch(touch);
}

bool VirtualTangible::containsPoint(const ci::vec2 & point) {
	return glm::length2(point) < mRadius * mRadius;
}

//void VirtualTangible::handleTouchMoved(const bluecadet::touch::TouchEvent & event) {
//	if (mDraggingEnabled) {
//		setGlobalPosition(event.globalPosition - mInitialLocalTouchPos);
//	}
//}

void VirtualTangible::handleTouchTapped(const bluecadet::touch::TouchEvent & event) {
	mWasPlaced = mIsPlaced;
	mIsPlaced = !mIsPlaced;

	if (mIsPlaced) {
		moveToFront();
	}
}

void VirtualTangible::updateTouches() {
	vec2 scale = core::ScreenCamera::getInstance()->getScale();
	auto transform = core::ScreenCamera::getInstance()->getTransform();

	for (int i = 0; i < mPoints.size(); ++i) {
		auto & touch = mTouches[i];
		
		vec2 globalPoint = this->convertLocalToGlobal(mPoints[i]);
		vec4 windowPos = transform * vec4(globalPoint, 0, 1);
		touch.windowPosition = vec2(windowPos);

		if (!mWasPlaced && mIsPlaced) {
			touch.phase = TouchPhase::Began;
		} else if (mWasPlaced && mIsPlaced) {
			touch.phase = TouchPhase::Moved;
		} else {
			touch.phase = TouchPhase::Ended;
		}
	}
	mWasPlaced = mIsPlaced;
	setAlpha(mIsPlaced ? 1.0f : 0.5f);
}

void VirtualTangible::didMoveToView(bluecadet::views::BaseView * parent) {
	// add gesture handler
	if (!mGestureHandler) {
		mGestureHandler = make_shared<handlers::ManipulateGestureHandler>(getSharedTouchViewPtr(), true, false, true);
	}
}

void VirtualTangible::willMoveFromView(bluecadet::views::BaseView * parent) {
	mGestureHandler = nullptr;
}

}
}
