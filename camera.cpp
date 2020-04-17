#include "camera.hpp"

void Camera::initPlane() {
    /// real points rectangle (rect_)
    pt ptLup = rect_.lup_ + pos_;
    pt ptRup = rect_.rup_ + pos_;
    pt ptLdown = rect_.ldown_ + pos_;
    pt ptRdown = rect_.rdown_ + pos_;

    /// created planes pyramid
    up_ = Plane(pos_, ptLup, ptRup);
    down_ = Plane(pos_, ptLdown, ptRdown);
    left_ = Plane(pos_, ptLup, ptLdown);
    right_ = Plane(pos_, ptRup, ptRdown);
    front_ = Plane(pos_, cross(ptLup - ptLdown, ptRdown - ptLdown));

    ///create reality center [pos_ + center(rect_)]
    pt center = (ptLup + ptRdown) * 0.5;

    ///normalization normals
    up_.setNormal(center);
    down_.setNormal(center);
    left_.setNormal(center);
    right_.setNormal(center);
    front_.setNormal(center);
}

Camera::Camera(double h, double w) {
    w *= SIDE;
    h *= SIDE;

    distToRect_ = w * cos(PI / 6);

    pos_ = pt(0, 0, 0);

    pt lup = pt(-w / 2, h / 2, distToRect_);
    pt rup = pt(w / 2, h / 2, distToRect_);
    pt ldown = pt(-w / 2, -h / 2, distToRect_);
    pt rdown = pt(w / 2, -h / 2, distToRect_);

    rect_ = Rect(lup, rup, ldown, rdown);
    da_ = DEFAULT_DA;

    initPlane();
}

void Camera::rotateLeft(double alpha) {
    rect_.rotateLeft(alpha);
    initPlane();
}

void Camera::rotateUp(double alpha) {
    rect_.rotateUp(alpha, da_);
    initPlane();
}

void Camera::move(double dx, double dy, double dz) {
    pt dpos = pt(dx, dy, dz);
    pos_ = pos_ + dpos;
    initPlane();
}

bool Camera::visiblePoint(const pt& p) const {
    return up_.getPositionPoint(p)    >= 0 &&
           down_.getPositionPoint(p)  >= 0 &&
           left_.getPositionPoint(p)  >= 0 &&
           right_.getPositionPoint(p) >= 0;
}

/// if there are points in front and behind the plane
bool Camera::checkLocation(const Plane& plane, const block& b) const {
    if (plane.getPositionPoint(b.be_) != -1 || plane.getPositionPoint(b.en_) != -1) {
        return true;
    }
    for (int i = 0; i < 6; ++i) {
        if (plane.getPositionPoint(b.boardPt_[i]) != -1) {
            return true;
        }
    }
    return false;
}

bool Camera::intersectBlock(const block& b) const {
    if (b.checkInPoint(pos_)) {
        return true;
    }

    if (visiblePoint(b.be_) || visiblePoint(b.en_)) {
        return true;
    }

    for (int i = 0; i < 6; ++i) {
        if (visiblePoint(b.boardPt_[i])) {
            return true;
        }
    }

    if (!checkLocation(front_, b)) {
        return false;
    }

    return (checkLocation(left_, b) && checkLocation(right_, b) && checkLocation(up_, b) && checkLocation(down_, b));
}

pt Camera::getPos() const {
    return pos_;
}
