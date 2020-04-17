#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <utility>
#include <fstream>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>

#define sqr(x) ((x) * (x))

using std::min;
using std::max;
using std::abs;

const double EPS = 1e-9;
const double  PI = acos(-1);

int sgn(double x);  /// return sign x

/// point
struct pt {
    pt(double x = 0, double y = 0, double z = 0);

    void randomPoint(std::mt19937& eng); /// (x_, y_, z_) -> (eng, eng, eng)
    void read(std::ifstream& in);        /// read point format [x, y, z]
    void getDown(const pt& p);           /// x = min(x, P.x) , y and z analogically
    void getUp(const pt& p);             /// x = max(x, P.x) , y and z analogically
    void setOptimalNorm();               /// convert to: max(x, y, z) <= 1
    double dist(const pt& p) const;      /// return dist(this, p) [euclid]
    double manhattan() const;            /// return manhattan norm

    void rotateLeft(double alpha); /// rotates this vector through alpha degrees to the left (or ti the right if alpha < 0)

    double getRotateUp(double alpha, double da); /// return true angle after rotateUp
    void rotateUp(double alpha, double da);
    /*
        rotates this vector through alpha degrees upwards (if alpha < 0 => downwards)
        da - deltaAngle from board +-PI/2 [-PI/2 + |da|, PI/2 - |da|]
        da for formulas-rotate
        if da == 0 and final angle equal +-PI/2 then non-exist case
    */

    pt operator- (pt other) const; /// classical vector+ (x1, y1, z1) + (x2, y2, z2) = (x1 + x2, y1 + y2, z1 + z2)
    pt operator+ (pt other) const; /// classical vector- (x1, y1, z1) - (x2, y2, z2) = (x1 - x2, y1 - y2, z1 - z2)
    pt operator* (double k) const; /// classical vector* (x1, y1, z1) * k = (k * x1, k * y1, k * z1)

    double x_, y_, z_;
};

pt cross(pt u, pt v);       /// cross-mult
double dot(pt u, pt v);     /// dot-mult

struct Line {
    Line(pt u, pt v);      /// line on two point U and V, if U == V => bad
    void setOptimalNorm(); /// vector-rotate convert to: max(x, y, z) <= 1

    pt s_; /// start point
    pt p_; /// vector-rotate
};

struct Plane {
    Plane();                                         /// default (0, 0, 0, 0)
    Plane(pt u, pt v, pt w);                         /// create a plane at three points [u, v, w]
    Plane(pt start, pt norm);                        /// create a plane at point and normal
    void invertNormal();                             /// (a_, b_, c_, _d) -> (-a_, -b_, -c_, -d_) and norm_ *= -1
    int getPositionPoint(const pt& p) const ;        /// return sgn(plane(p))
    void setNormal(const pt& p);                     /// set this plane to : plane(p) > 0 (point out of plane)

    pt     norm_;          /// normal-vector plane
    double a_, b_, c_, d_; /// formula: x * a_ + y * b_ + z * c_ + d_ = 0
};

struct Rect {
    Rect();
    Rect(pt lup, pt rup, pt ldown, pt rdown);

    void rotateLeft(double alpha);                 /// rotate rectangle in left on alpha (if alpha < 0 => rotate right)
    double trueRotateUp(double alpha, double da);  /// return true angle, check board[-pi/2 + |da|l pi.2 - |da|]
    void rotateUp(double alpha, double da);
    /*
        rotate rectangle in up on alpha (if alpha < 0 => rotate down)
        da - delta angle from Camera option da_
    */

    pt getCenter() const; /// return center

    pt lup_, rup_;     /// left up and right up vector on rectangle
    pt ldown_, rdown_; /// left down and right down vector on rectangle
};

/// KD - block
struct block {
    block();
    block(pt be, pt en);

    bool checkInPoint(const pt& p) const;  /// returns true if point P is in the block, returns false otherwise
    std::pair<block, block> split() const; /// splits this block on the larger side and returns pair of blocks
    pt getCenter() const;                  /// return center block (be_ + en_) * 0.5

    pt be_, en_;    /// left-down point and right-up point [begin, end] -> [be, en]
    pt boardPt_[6]; /// board-point box without two points (be_, en_)
};
