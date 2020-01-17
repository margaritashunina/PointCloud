#pragma once

#include "primitives.hpp"

const double       SIDE = 1e3;
const double DEFAULT_DA = PI / 18;

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
    void rotateLeft(double alpha);                                /// rotate camera in left on alpha (if alpha < 0 => rotate right)
    void rotateUp(double alpha);                                  /// rotate camera in up on alpha (if alpha < 0 => rotate down)
    void move(double dx, double dy, double dz);                   /// move Camera on vector(dx, dy, dz)
    inline bool visiblePoint(const pt& p) const;                  /// return true if point P is visible
    bool checkLocation(const Plane& plane, const block& b) const; /// check block relative to the plane
    bool intersectBlock(const block& b) const;                    /// block intersection check (return true if intersect)

    pt getPos() const; /// return pos_

private:
    void initPlane();         /// initial planes in pyramid

    pt     pos_;              /// point position
    Rect   rect_;             /// rectangle item in camera [vector-style, this is not real point. Real: pos_ + rect_.()]
    double distToRect_;       /// distToRect_ = dist(pos_, plane(rect_))
    double da_;               /// delta-angle for camera up-down rotate [-PI/2 + |da_|, PI/2 - |da_|]
    Plane  up_, down_;        /// planes up-down pyramid-camera
    Plane  left_, right_;     /// planes left-right pyramid-camera
    Plane  front_;            /// front plane of the pyramid section
};
