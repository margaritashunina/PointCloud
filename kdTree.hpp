#pragma once

#include <algorithm>
#include <random>
#include <chrono>
#include "primitives.hpp"

const int MAX_DEPHT = 20;
const int DEFAULT_BOARD_CNT_POINT = 1000;

/// KD-tree nodes
struct Node {
    Node();
    Node(int l, int r, block b, pt* dataPoint, int boardCntPoint, int depht);
    ~Node();

    int l_, r_; /// idx-segment in dataPoint [l_, r_]
    block b_;   /// kd-block on [l_, r_] elements
    Node* lc;   /// left child pointer
    Node* rc;   /// right child pointer
};

class KD {
public:
    KD();
    ~KD();

    void read(std::ifstream& in); /// read n, dataPoint,

    /// true - work, false - ignored
    bool build();             /// returns false if KD-tree and dataPoint existed, returns false otherwise
    bool checkBuild() const;  /// returns false if build hasn't started yet, returns true if KD-tree and dataPoint existed
    bool clear();             /// returns false if KD-tree and dataPoint don't exist, returns true if KD-tree and dataPoint existed and was cleared

private:
    Node* root_;         /// KD-tree root

    int n_;              /// number of points in dataPoint
    pt* dataPoint_;      /// points for KD-tree
    int boardCntPoint_;  /// minimal allowed number of points in one block of KD-tree

    bool flagBuilded_;   /// flag of existence of KD-tree
};
