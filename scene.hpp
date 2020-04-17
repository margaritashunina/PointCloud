#pragma once

#include "primitives.hpp"
#include "camera.hpp"
#include "kdTree.hpp"
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <queue>
#include <functional>

const int DEFAULT_BUFF_SIZE = 1e6;
const int     MAX_BUFF_SIZE = 1e7;
const int       DEPTH_WAVES = 6;

class Scene {
public:
    Scene();
    ~Scene();

    void deleteData();                                  /// delete data in KD
    void randomData(int n, std::mt19937& eng);          /// generate random pointCloud (n point with eng)
    void readScene(std::ifstream& in);                  /// read data from file
    void initScene();                                   /// initial Scene (build KD-tree)
    void clearScene();                                  /// clear KD-tree, viewer = default, buffSize_ = DEFAULT_BUFF_SIZE
    void changeBuffSize(int newBuffSize);               /// changed buffSize_ = max(default, min(max, new))

    void rotateLeftViewer(double alpha);              /// rotate viewer in left on alpha (if alpha < 0 => rotate right)
    void rotateUpViewer(double alpha);                /// rotate viewer in up on alpha (if alpha < 0 => rotate down)
    void moveViewer(double dx, double dy, double dz); /// move viewer on vector(dx, dy, dz)

    bool calcFrame(); /// calculate visible points and save point in the buffer [return true if existed KD.build]

    void setViewVector(pt v);         /// set view vector

    const pt* getData(int& sz) const; /// pointer to data in KD
    pt getPosViewer() const;          /// return pos camera
    pt getViewVector() const;         /// return view vector

private:
    void paint(int start, int cntPoint); /// painted dataPoint{board(u, cntPoint)}

    Camera viewer_;
    KD     pointCloud_;
    int    buffSize_;    /// size buffPoint_
    int    tmpBuffSize_; /// size buffer for specific frame
};
