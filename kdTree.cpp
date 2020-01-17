#include "kdTree.hpp"

Node::Node() {
    l_ = r_ = -1;
    lc = rc = nullptr;
}

Node::Node(int l, int r, block b, pt* dataPoint, int boardCntPoint, int depht, double& minDist) {
    l_ = l;
    r_ = r;
    b_ = b;
    lc = rc = nullptr;

    if (depht == MAX_DEPHT || r_ - l_ + 1 <= boardCntPoint) { /// check leaf
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();    /// SEED for shuffle
        shuffle (dataPoint + l_, dataPoint + r_ + 1, std::default_random_engine(seed)); /// shuffle data in leaf
        minDist = min(minDist, (b.en_ - b.be_).manhattan());
        return;
    }

    std::pair<block, block> newBloks = b_.split();
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

    if (l <= rit) {
        lc = new Node(l, rit, newBloks.first, dataPoint, boardCntPoint, depht + 1, minDist);  /// created left child
    }
    if (lit <= r) {
        rc = new Node(lit, r, newBloks.second, dataPoint, boardCntPoint, depht + 1, minDist); /// created right child
    }
}

Node::~Node() {
    if (lc != nullptr) {
        delete lc;
    }
    if (rc != nullptr) {
        delete rc;
    }
}

const Node* Node::getLchild() const {
    return lc;
}

const Node* Node::getRchild() const {
    return rc;
}

pt Node::getCenter() const {
    return b_.getCenter();
}

bool Node::isLeaf() const {
    return lc == nullptr && rc == nullptr;
}

bool Node::checkLeftChild() const {
    return lc != nullptr;
}

bool Node::checkRightChild() const {
    return rc != nullptr;
}

KD::KD() {
    boardCntPoint_ = n_ = 0;
    flagBuilded_ = false;
    root_ = nullptr;
    dataPoint_ = nullptr;
    minDist_ = maxDist_ = depth_ = -1;
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

    boardCntPoint_ = cbrt(n_);
}

bool KD::checkBuild() const {
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

    maxDist_ = (rootBe - rootEn).manhattan();
    minDist_ = 1e18;

    /// created KD-tree
    root_ = new Node(0, n_ - 1, block(rootBe, rootEn), dataPoint_, boardCntPoint_, 0, minDist_);

    depth_ = log2(maxDist_ / minDist_) - 1;
    depth_ -= (depth_ == 20); /// special case of further formulas

    return true;
}

const pt* KD::getData(int index) const {
    return dataPoint_ + index;
}

const Node* KD::getRoot() const {
    return root_;
}

double KD::getMaxDist() const {
    return maxDist_;
}

double KD::getMinDist() const {
    return minDist_;
}

double KD::getProportion(int x) const {
    double h = depth_;
    return (x * (sqr(h) - 20) + sqr(x) * (20 - h)) / (20*h*(h - 1));
}
