#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <utility>
#include <fstream>
#include <cmath>

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)
#define abs(a) (a < 0 ? -a : a)
#define sqr(x) ((x) * (x))

using std::pair;

const double EPS = 1e-9;
const double PI = acos(-1);

void normAngle(double& angle);    /// angle to [-PI, PI]

/// point
struct pt {
    pt(double x = 0, double y = 0, double z = 0);

    void read(std::ifstream& in);  /// read point format [x, y, z]
    void getDown(const pt& p);     /// x = min(x, P.x) , y and z analogically
    void getUp(const pt& p);       /// x = max(x, P.x) , y and z analogically

    void rotateLeft(double alpha); /// rotate this vector on alpha angle to left (if alpha < 0 => to right)

    void rotateUp(double alpha, double da = 0);
    /*
        rotate this vector on alpha angle to up (if alpha < 0 => to down)
        da - deltaAngle from board +-PI/2 [-PI/2 + |da|, PI/2 - |da|]
        da for formulas-rotate
        if da == 0 and final angle equal +-PI/2 then non-exist case
    */

    pt operator- (pt other);       /// classical vector+ (x1, y1, z1) + (x2, y2, z2) = (x1 + x2, y1 + y2, z1 + z2)
    pt operator+ (pt other);       /// classical vector- (x1, y1, z1) - (x2, y2, z2) = (x1 - x2, y1 - y2, z1 - z2)

    double x_, y_, z_;
};

/// KD - block
struct block {
    block();
    block(pt be, pt en);

    bool checkInPoint(const pt& p) const;  /// true if point P in block, false if not
    pair<block, block> split();            /// split this block on larges side and return pair blocks

    pt be_, en_; /// left-down point and right-up point [begin, end] -> [be, en]
};

/// KD-tree nodes
struct Node {
    Node();
    Node(int l, int r, block b, pt* dataPoint, int boardCntPoint);
    ~Node();

    int l_, r_; /// idx-segment in dataPoint [l_, r_]
    block b_;   /// kd-block on [l_, r_] elements
    Node* lc;   /// ptr on left child
    Node* rc;   /// ptr on right child
};

class KD {
public:
    KD();
    ~KD();

    void read(std::ifstream& in); /// read n, dataPoint,

    /// true - work, false - ignored
    bool build();       /// return false if existed KD-tree and dataPoint, true if not existed
    bool checkBuild();  /// return false if not started build, true if exist build KD-tree and dataPoint
    bool clear();       /// return false if not exist KD-tree and dataPoint, true if exist

private:
    Node* root_;         /// root KD-tree

    int n_;              /// count point in dataPoint
    pt* dataPoint_;      /// points for KD-tree
    int boardCntPoint_;  /// min board count point in one node KD-tree

    bool flagBuilded_;   /// flag existed build KD-tree
};
