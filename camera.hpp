#pragma once

#include "kdTree.hpp"

const double SIDE = 1e3;
const double DEFAULT_DA = PI / 18;

struct Line {
    Line(pt u, pt v); /// line on two point U and V, if U == V => bad
    void getNorm();   /// vector-rotate convert to: max(x, y, z) <= 1

    pt s_; /// start point
    pt p_; /// vector-rotate
};

struct Rect {
    Rect();
    Rect(pt lup, pt rup, pt ldown, pt rdown);

    void rotateLeft(double alpha);             /// rotate rectangle in left on alpha (if alpha < 0 => rotate right)
    void rotateUp(double alpha, double da);
    /*
        rotate rectangle in up on alpha (if alpha < 0 => rotate down)
        da - delta angle from Camera option da_
    */

    pt lup_, rup_;      /// left up and right up point on rectangle
    pt ldown_, rdown_;  /// left down and right down point on rectangle
};

class Camera {
public:
    Camera(double h = 9, double w = 16);
    /*
        default w/h = 16/9 and pyramid with
        triangle (w * SIDE, w * SIDE, w * SIDE) on Oxy
        and position camera is (-w * SIZE * cos(30*), 0, 0)
        and lup_, .., rdown_ in Oyz
    */

    /// all rotate saving pos_, only rotate rectangle item in camera
    void rotateLeft(double alpha);              /// rotate camera in left on alpha (if alpha < 0 => rotate right)
    void rotateUp(double alpha);                /// rotate camera in up on alpha (if alpha < 0 => rotate down)
    void move(double dx, double dy, double dz); /// move Camera on vector(dx, dy, dz)
private:
    pt pos_;            /// point position
    Rect rect_;         /// rectangle item in camera
    double distToRect_; /// distToRect_ = dist(pos_, plane(rect_))
    double da_;         /// delta-angle for camera up-down rotate [-PI/2 + |da_|, PI/2 - |da|]
};
