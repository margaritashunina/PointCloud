#include "pch.h"
#include "primitives.hpp"

int sgn(double x) {
    if (abs(x) < EPS) {
        return 0;
    }
    if (x < 0) {
        return -1;
    }
    return 1;
}

pt::pt(double x, double y, double z) {
    x_ = x;
    y_ = y;
    z_ = z;
}

void pt::randomPoint(std::mt19937& eng) {
    x_ = eng() / 1e9;
    y_ = eng() / 1e9;
    z_ = eng() / 1e9;
}

void pt::read(std::ifstream& in) {
    in >> x_ >> y_ >> z_;
}

void pt::getDown(const pt& p) {
    x_ = min(x_, p.x_);
    y_ = min(y_, p.y_);
    z_ = min(z_, p.z_);
}

void pt::getUp(const pt& p) {
    x_ = max(x_, p.x_);
    y_ = max(y_, p.y_);
    z_ = max(z_, p.z_);
}

void pt::setOptimalNorm() {
    double d = max(abs(x_), max(abs(y_), abs(z_)));
    x_ /= d;
    y_ /= d;
    z_ /= d;
}

void pt::norm() {
    double d = sqrt(sqr(x_) + sqr(y_) + sqr(z_));
    x_ /= d;
    y_ /= d;
    z_ /= d;
}

double pt::dist(const pt& p) const {
    return sqrt(sqr(p.x_ - x_) + sqr(p.y_ - y_) + sqr(p.z_ - z_));
}

double pt::manhattan() const {
    return abs(x_) + abs(y_) + abs(z_);
}

void pt::rotateLeft(double alpha) {
    double r = sqrt(sqr(x_) + sqr(z_));
    if (abs(r) < EPS) {
        return;
    }

    double phi = atan2(z_, x_);
    phi += alpha;

    x_ = r * cos(phi);
    z_ = r * sin(phi);
}

double pt::getRotateUp(double alpha, double da) {
    double r = sqrt(sqr(x_) + sqr(y_) + sqr(z_));

    double teta = asin(y_ / r);

    double newTeta = teta + alpha;

    da = abs(da);

    newTeta = max(-PI / 2 + da, newTeta);
    newTeta = min(PI / 2 - da, newTeta);

    return newTeta - teta;
}

void pt::rotateUp(double alpha, double da) {
    double r = sqrt(sqr(x_) + sqr(y_) + sqr(z_));

    double teta = asin(y_ / r);

    double newTeta = teta + alpha;

    da = abs(da);
    newTeta = max(-PI / 2 + da, newTeta);
    newTeta = min(PI / 2 - da, newTeta);

    /// exist phi if |da| > 0 then in Camera all correct
    double phi = atan2(z_, x_);

    x_ = r * cos(phi) * cos(newTeta);
    z_ = r * sin(phi) * cos(newTeta);
    y_ = r * sin(newTeta);
}

pt pt::operator+ (pt other) const {
    return pt(x_ + other.x_, y_ + other.y_, z_ + other.z_);
}

pt pt::operator- (pt other) const {
    return pt(x_ - other.x_, y_ - other.y_, z_ - other.z_);
}

pt pt::operator* (double k) const {
    return pt(x_ * k, y_ * k, z_ * k);
}

pt cross(pt u, pt v) {
    return pt(u.y_ * v.z_ - u.z_ * v.y_, -(u.x_ * v.z_ - u.z_ * v.x_), u.x_ * v.y_ - u.y_ * v.x_);
}

double dot(pt u, pt v) {
    return u.x_ * v.x_ + u.y_ * v.y_ + u.z_ * v.z_;
}

Line::Line(pt u, pt v) {
    s_ = u;
    p_ = v - u;
    setOptimalNorm();
}

void Line::setOptimalNorm() {
    p_.setOptimalNorm();
}

Plane::Plane() {
    /// norm_ : default pt constructor
    a_ = b_ = c_ = d_ = 0;
}

Plane::Plane(pt u, pt v, pt w) {
    u = u - w;
    v = v - w;

    norm_ = cross(u, v);
    norm_.setOptimalNorm();

    a_ = norm_.x_;
    b_ = norm_.y_;
    c_ = norm_.z_;
    d_ = -dot(norm_, w);
}

Plane::Plane(pt start, pt norm) {
    norm_ = norm;
    norm_.setOptimalNorm();

    a_ = norm_.x_;
    b_ = norm_.y_;
    c_ = norm_.z_;
    d_ = -dot(norm_, start);
}

void Plane::invertNormal() {
    norm_.x_ *= -1;
    norm_.y_ *= -1;
    norm_.z_ *= -1;
    a_ = -a_;
    b_ = -b_;
    c_ = -c_;
    d_ = -d_;
}

int Plane::getPositionPoint(const pt& p) const {
    int res = sgn(a_ * p.x_ + b_ * p.y_ + c_ * p.z_ + d_);
    return res;
}

void Plane::setNormal(const pt& p) {
    if (getPositionPoint(p) != 1) {
        invertNormal();
    }
}

Rect::Rect() {
    /// default all point is [default constructor]
}

Rect::Rect(pt lup, pt rup, pt ldown, pt rdown) {
    lup_ = lup;
    rup_ = rup;
    ldown_ = ldown;
    rdown_ = rdown;
    center_ = (lup_ + rdown_) * 0.5;
}

void Rect::initRect() {
    pt du = pt(center_.z_, 0, -center_.x_);
    pt dv = cross(du, center_) * (-1);

    du.norm();
    dv.norm();

    double w = W_DEF * 0.5;
    double h = H_DEF * 0.5;

    du = du * w;
    dv = dv * h;

    rup_ = center_ + du + dv;
    lup_ = center_ - du + dv;
    rdown_ = center_ + du - dv;
    ldown_ = center_ - du - dv;
}

void Rect::rotateLeft(double alpha) {
    center_.rotateLeft(alpha);
    initRect();
}

void Rect::rotateUp(double alpha, double da) {
    center_.rotateUp(alpha, da);
    initRect();
}

pt Rect::getCenter() const {
    return center_;
}

block::block() {
    /// default constructor be_, en_
}

block::block(pt be, pt en) {
    be_.x_ = min(be.x_, en.x_);
    be_.y_ = min(be.y_, en.y_);
    be_.z_ = min(be.z_, en.z_);

    en_.x_ = max(be.x_, en.x_);
    en_.y_ = max(be.y_, en.y_);
    en_.z_ = max(be.z_, en.z_);

    boardPt_[0] = pt(be_.x_, be_.y_, en_.z_);
    boardPt_[1] = pt(be_.x_, en_.y_, en_.z_);
    boardPt_[2] = pt(be_.x_, en_.y_, be_.z_);

    boardPt_[3] = pt(en_.x_, en_.y_, be_.z_);
    boardPt_[4] = pt(en_.x_, be_.y_, be_.z_);
    boardPt_[5] = pt(en_.x_, be_.y_, en_.z_);
}

bool block::checkInPoint(const pt& p) const {
    return p.x_ >= be_.x_ && p.x_ <= en_.x_ &&
        p.y_ >= be_.y_ && p.y_ <= en_.y_ &&
        p.z_ >= be_.z_ && p.z_ <= en_.z_;
}

std::pair<block, block> block::split() const {
    double dx = abs(be_.x_ - en_.x_);
    double dy = abs(be_.y_ - en_.y_);
    double dz = abs(be_.z_ - en_.z_);

    pt delta; /// = default constructor

    /// init delta
    if (dx > max(dy, dz)) {
        delta.x_ = dx / 2;
    }
    if (dy > max(dx, dz)) {
        delta.y_ = dy / 2;
    }
    if (dz > max(dx, dy)) {
        delta.z_ = dz / 2;
    }

    pt upBe = en_ - delta;
    pt downEn = be_ + delta;

    return { block(be_, upBe), block(downEn, en_) };
}

pt block::getCenter() const {
    return (be_ + en_) * 0.5;
}