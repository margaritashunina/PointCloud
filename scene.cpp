#include "scene.hpp"

Scene::Scene() {
    buffSize_ = DEFAULT_BUFF_SIZE;
    tmpBuffSize_ = 0;
    uwpPaint_ = nullptr;
}

Scene::~Scene() {
    ///
}

void Scene::deleteData() {
    pointCloud_.deleteData();
}

const pt* Scene::getData(int& sz) const {
    return pointCloud_.getData(sz);
}

void Scene::randomData(int n, std::mt19937& eng) {
    pointCloud_.randomData(n, eng);
}

void Scene::readScene(std::ifstream& in) {
    pointCloud_.read(in);
}

void Scene::initScene() {
    pointCloud_.build();
}

void Scene::clearScene() {
    pointCloud_.clear();
    viewer_ = Camera();
    buffSize_ = DEFAULT_BUFF_SIZE;
    tmpBuffSize_ = 0;
}

void Scene::changeBuffSize(int newBuffSize) {
    buffSize_ = max(DEFAULT_BUFF_SIZE, min(MAX_BUFF_SIZE, newBuffSize));
}

void Scene::rotateLeftViewer(double alpha) {
    viewer_.rotateLeft(alpha);
}

void Scene::rotateUpViewer(double alpha) {
    viewer_.rotateUp(alpha);
}

void Scene::moveViewer(double dx, double dy, double dz) {
    viewer_.move(dx, dy, dz);
}

void Scene::setUwpPaint(std::function<void(int, int)>& f) {
    uwpPaint_ = f;
}

void Scene::paint(int start, int cntPoint) {
    cntPoint = max(0, min(cntPoint, buffSize_ - tmpBuffSize_));
    if (!cntPoint) {
        return;
    }

    uwpPaint_(start, cntPoint);
    tmpBuffSize_ += cntPoint;
}

bool Scene::calcFrame() {
    if (pointCloud_.checkBuild() == false) {
        return false;
    }

    tmpBuffSize_ = 0;
    std::queue<const Node*> q;
    q.push(pointCloud_.getRoot());

    int cntWave = 0;
    double maxDist = pointCloud_.getMaxDist();
    double minDist = pointCloud_.getMinDist();
    pt posViewer = viewer_.getPos();

    /// bfs-algorithm
    while (q.empty() == false && tmpBuffSize_ < buffSize_) {
        int waveSize = q.size();

        for (int i = 0; i < waveSize && tmpBuffSize_ < buffSize_; ++i) {
            const Node* u = q.front();
            q.pop();

            if (viewer_.intersectBlock(u->b_) == false) {
                continue;
            }

            if (cntWave < DEPTH_WAVES) { /// if firsts waves
                if (u->checkLeftChild()) {
                    q.push(u->getLchild());
                }
                if (u->checkRightChild()) {
                    q.push(u->getRchild());
                }
                continue;
            }

            pt centerBlock = u->getCenter();
            double distManhattan = (centerBlock - posViewer).manhattan();

            if (distManhattan > maxDist || u->isLeaf()) { /// if the block is far or leaf
                double hu = ceil(log2(distManhattan / minDist));
                double proportion = pointCloud_.getProportion(hu);

                int cntPoint = u->r_ - u->l_ + 1;
                int cntPointWithProportion = min(cntPoint, int(proportion * cntPoint)); /// for proportion > 1 (eps)

                paint(u->l_, cntPointWithProportion);
            } else { /// if the block is close
                if (u->checkLeftChild()) {
                    q.push(u->getLchild());
                }
                if (u->checkRightChild()) {
                    q.push(u->getRchild());
                }
            }
        }

        if (cntWave >= DEPTH_WAVES) {
            maxDist /= 2;
        }
        cntWave++;
    }

    return true;
}
