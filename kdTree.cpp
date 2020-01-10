#include "kdTree.hpp"

pt::pt(double x, double y, double z) {
    x_ = x;
    y_ = y;
    z_ = z;
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

pt pt::operator+ (pt other) {
    return pt(x_ + other.x_, y_ + other.y_, z_ + other.z_);
}

pt pt::operator- (pt other) {
    return pt(x_ - other.x_, y_ - other.y_, z_ - other.z_);
}

block::block() {
    /// default be_ = en_ = (0, 0, 0)
}

block::block(pt be, pt en) {
    be_.x_ = min(be.x_, en.x_);
    be_.y_ = min(be.y_, en.y_);
    be_.z_ = min(be.z_, en.z_);

    en_.x_ = max(be.x_, en.x_);
    en_.y_ = max(be.y_, en.y_);
    en_.z_ = max(be.z_, en.z_);
}

bool block::checkInPoint(const pt& p) const {
    return p.x_ >= be_.x_ && p.x_ <= en_.x_ &&
           p.y_ >= be_.y_ && p.y_ <= en_.y_ &&
           p.z_ >= be_.z_ && p.z_ <= en_.z_;
}

pair<block, block> block::split() {
    double dx = abs(be_.x_ - en_.x_);
    double dy = abs(be_.y_ - en_.y_);
    double dz = abs(be_.z_ - en_.z_);

    pt delta; /// = default(0, 0, 0)

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

    return {block(be_, upBe), block(downEn, en_)};
}

Node::Node() {
    l_ = r_ = -1;
    lc = rc = nullptr;
}

Node::Node(int l, int r, block b, pt* dataPoint, int boardCntPoint) {
    l_ = l;
    r_ = r;
    b_ = b;
    lc = rc = nullptr;

    if (r_ - l_ + 1 <= boardCntPoint) { /// check leaf
        return;
    }

    pair<block, block> newBloks = b_.split();
    int lit = l_; /// it in point for check in newBlock.first
    int rit = r_; /// it on non-used point

    /// finaly: [l, r] => [l, rit] in left child, [lit, r] in right child
    while (lit <= rit) {
        if (newBloks.first.checkInPoint(dataPoint[lit])) {
            ++lit;
        } else {
            std::swap(dataPoint[lit], dataPoint[rit]);
            --rit;
        }
    }

    lc = new Node(l, rit, newBloks.first, dataPoint, boardCntPoint);  /// created left child
    rc = new Node(lit, r, newBloks.second, dataPoint, boardCntPoint); /// created right child
}

Node::~Node() {
    if (lc != nullptr) {
        delete lc;
    }
    if (rc != nullptr) {
        delete rc;
    }
}

KD::KD() {
    boardCntPoint_ = n_ = 0;
    flagBuilded_ = false;
    root_ = nullptr;
    dataPoint_ = nullptr;
}

KD::~KD() {
    delete root_;
    delete[] dataPoint_;
}

void KD::read(std::ifstream& in) {
/*
    format:
    n - count point
    n string: {xi, yi, zi}
*/

    in >> n_;
    dataPoint_ = new pt[n_];

    for (int i = 0; i < n_; ++i) {
        dataPoint_[i].read(in);
    }

    boardCntPoint_ = sqrt(n_);
}

bool KD::checkBuild() {
    return flagBuilded_;
}

bool KD::clear() {
    if (checkBuild()) {
        flagBuilded_ = false;

        delete root_;
        delete[] dataPoint_;
        n_ = 0;
        boardCntPoint_ = 0;
        root_ = nullptr;
        dataPoint_ = nullptr;

        return true;
    }

    return false;
}

bool KD::build() {
    if (checkBuild()) {
        return false;
    }

    flagBuilded_ = true;

    /// box for dataPoint in next KD-tree
    pt rootBe;
    pt rootEn;

    /// init start value
    rootBe = rootEn = dataPoint_[0];

    /// find box
    for (int i = 1; i < n_; ++i) {
        rootBe.getDown(dataPoint_[i]);
        rootEn.getUp(dataPoint_[i]);
    }

    /// created KD-tree
    root_ = new Node(0, n_ - 1, block(rootBe, rootEn), dataPoint_, boardCntPoint_);

    return true;
}
