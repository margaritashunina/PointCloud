#pragma once

#include "primitives.hpp"
#include "camera.hpp"
#include "kdTree.hpp"
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <queue>

const int DEFAULT_BUFF_SIZE = 1e6;
const int     MAX_BUFF_SIZE = 1e7;
const int       DEPTH_WAVES = 6;

class Scene {
public:
    Scene();
    ~Scene();

    void readScene(std::ifstream& in);    /// read data from file
    void initScene();                     /// initial Scene (build KD-tree)
    void clearScene();                    /// clear KD-tree, viewer = default, buffSize_ = DEFAULT_BUFF_SIZE
    void changeBuffSize(int newBuffSize); /// changed buffSize_ = max(default, min(max, new))

    void rotateLeftViewer(double alpha);              /// rotate viewer in left on alpha (if alpha < 0 => rotate right)
    void rotateUpViewer(double alpha);                /// rotate viewer in up on alpha (if alpha < 0 => rotate down)
    void moveViewer(double dx, double dy, double dz); /// move viewer on vector(dx, dy, dz)

    bool calcFrame(); /// calculate visible points and save point in the buffer [return true if existed KD.build]

private:
    void addToBuffer(const Node*& u, int cntPoint); /// add points to buffer from dataPoint{board(u, cntPoint)}

    Camera viewer_;
    KD     pointCloud_;
    pt*    buffPoint_;   /// saved point from pointCloud_
    int    buffSize_;    /// size buffPoint_
    int    tmpBuffSize_; /// size buffer for specific frame
};
