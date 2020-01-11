#include "camera.hpp"

Line::Line(pt u, pt v) {
    s_ = u;
    p_ = v - u;
    getNorm();
}

void Line::getNorm() {
    double d = max(abs(p_.x_), max(abs(p_.y_), abs(p_.z_)));
    p_.x_ /= d;
    p_.y_ /= d;
    p_.z_ /= d;
}

Rect::Rect() {
    /// default all point is (0, 0, 0)
}

Rect::Rect(pt lup, pt rup, pt ldown, pt rdown) {
    lup_ = lup;
    rup_ = rup;
    ldown_ = ldown;
    rdown_ = rdown;
}

void Rect::rotateLeft(double alpha) {
    lup_.rotateLeft(alpha);
    rup_.rotateLeft(alpha);
    ldown_.rotateLeft(alpha);
    rdown_.rotateLeft(alpha);
}

void Rect::rotateUp(double alpha, double da) {
    lup_.rotateUp(alpha, da);
    rup_.rotateUp(alpha, da);
    ldown_.rotateUp(alpha, da);
    rdown_.rotateUp(alpha, da);
}

Camera::Camera(double h, double w) {
    w *= SIDE;
    h *= SIDE;

    distToRect_ = w * cos(PI / 6);

    pos_ = pt(-distToRect_, 0, 0);

    pt lup = pt(0, w / 2, h / 2);
    pt rup = pt(0, -w / 2, h / 2);
    pt ldown = pt(0, w / 2, -h / 2);
    pt rdown = pt(0, -w / 2, -h / 2);

    rect_ = Rect(lup, rup, ldown, rdown);
    da_ = DEFAULT_DA;
}

void Camera::move(double dx, double dy, double dz) {
    pt dpos = pt(dx, dy, dz);
    pos_ = pos_ + dpos;
}

void Camera::rotateLeft(double alpha) {
    rect_.rotateLeft(alpha);
}

void Camera::rotateUp(double alpha) {
    rect_.rotateUp(alpha, da_);
}
