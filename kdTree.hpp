#pragma once

#include "primitives.hpp"
#include <algorithm>
#include <random>
#include <chrono>

const int               MAX_DEPHT = 20;
const int DEFAULT_BOARD_CNT_POINT = 1000;

/// KD-tree nodes
struct Node {
    Node();
    Node(int l, int r, block b, pt* dataPoint, int boardCntPoint, int depht, double& minDist);
    ~Node();

    const Node* getLchild() const; /// return lc
    const Node* getRchild() const; /// return rc
    pt getCenter() const;          /// return center block b_
    bool isLeaf() const;           /// return true if this node is leaf
    bool checkLeftChild() const;   /// return true if leftChild exist
    bool checkRightChild() const;  /// return true if rightChild exist

    int   l_, r_; /// idx-segment in dataPoint [l_, r_]
    block b_;     /// kd-block on [l_, r_] elements
    Node* lc;     /// left child pointer
    Node* rc;     /// right child pointer
};

class KD {
public:
    KD();
    ~KD();

    void read(std::ifstream& in); /// read n, dataPoint,

    /// true - work, false - ignored
    bool build();            /// returns false if KD-tree and dataPoint existed, returns false otherwise
    bool checkBuild() const; /// returns false if build hasn't started yet, returns true if KD-tree and dataPoint existed
    bool clear();            /// returns false if KD-tree and dataPoint don't exist, returns true if KD-tree and dataPoint existed and was cleared

    const pt* getData(int index) const; /// return (dataPoint_ + index), if index >= n_ => bad situation
    const Node* getRoot() const;        /// return root_
    double getMaxDist() const;          /// return maxDist_
    double getMinDist() const;          /// return minDist_
    double getProportion(int x) const;  /// return (x * (h^2 - 20) + x^2 * (20 - h)) / (20h(h - 1)), h = depth_
private:
    Node*  root_;          /// KD-tree root
    int    n_;             /// number of points in dataPoint
    pt*    dataPoint_;     /// points for KD-tree
    int    boardCntPoint_; /// minimal allowed number of points in one block of KD-tree
    bool   flagBuilded_;   /// flag of existence of KD-tree
    double minDist_;       /// minimum |dx| + |dy| + |dz| for all block-leaf
    double maxDist_;       /// |dx| + |dy| + |dz| for root_ block
    int    depth_;         /// lvl depth vision [depth_ = (int)(log2(maxDist_ / minDist_) - 1)]
};
