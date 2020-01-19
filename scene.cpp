#include "scene.hpp"

Scene::Scene() {
    buffSize_ = DEFAULT_BUFF_SIZE;
    buffPoint_ = new pt [MAX_BUFF_SIZE];
    tmpBuffSize_ = 0;
}

Scene::~Scene() {
    delete[] buffPoint_;
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

void Scene::addToBuffer(const Node*& u, int cntPoint) {
    cntPoint = min(cntPoint, buffSize_ - tmpBuffSize_);
    if (!cntPoint) {
        return;
    }

    const pt* startDataPointer = pointCloud_.getData(u->l_);
    memcpy(buffPoint_ + tmpBuffSize_, startDataPointer, cntPoint * sizeof(pt));
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

                addToBuffer(u, cntPointWithProportion);
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
