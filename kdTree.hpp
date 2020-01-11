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

    void rotateLeft(double alpha); /// rotates this vector through alpha degrees to the left (or ti the right if alpha < 0)

    void rotateUp(double alpha, double da = 0);
    /*
        rotates this vector through alpha degrees upwards (if alpha < 0 => downwards)
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

    bool checkInPoint(const pt& p) const;  /// returns true if point P is in the block, returns false otherwise
    pair<block, block> split();            /// splits this block on the larger side and returns pair of blocks

    pt be_, en_; /// left-down point and right-up point [begin, end] -> [be, en]
};

/// KD-tree nodes
struct Node {
    Node();
    Node(int l, int r, block b, pt* dataPoint, int boardCntPoint);
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
    bool build();       /// returns false if KD-tree and dataPoint existed, returns false otherwise
    bool checkBuild();  /// returns false if build hasn't started yet, returns true if KD-tree and dataPoint existed 
    bool clear();       /// returns false if KD-tree and dataPoint don't exist, returns true if KD-tree and dataPoint existed and was cleared

private:
    Node* root_;         /// KD-tree root

    int n_;              /// number of points in dataPoint
    pt* dataPoint_;      /// points for KD-tree
    int boardCntPoint_;  /// minimal allowed number of points in one block of KD-tree

    bool flagBuilded_;   /// flag of existence of KD-tree
};
