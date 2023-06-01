#pragma once
#include "PointD.h"
class Point3D : public PointD
{
public:
	double Z;
	double M;
public:
    Point3D();

    /**
     * Construction
     * @param x X
     * @param y Y
     * @param z Z
     */
    Point3D(double x, double y, double z);

    /**
     * Construction
     * @param x X
     * @param y Y
     * @param z Z
     * @param m M
     */
    Point3D(double x, double y, double z, double m);

    /**
     * Calculate distance to a point
     * @param p The point
     * @return Distance
     */
    double distance(Point3D p);

    /**
     * Dot product to a point
     * @param p The point
     * @return Dot product result
     */
    double dot(Point3D p);

    /**
     * Add to a point
     * @param p The point
     * @return Add result
     */
    Point3D add(Point3D p);

    /**
     * Subtract to a point
     * @param p The point
     * @return Subtract result
     */
    Point3D sub(Point3D p);

    /**
     * Multiply
     * @param v The value
     * @return Multiply result
     */
    Point3D mul(double v);

    /**
     * Divide
     * @param v The value
     * @return Divide result
     */
    Point3D div(double v);


    Point3D& operator=(const Point3D& other);

    Point3D Clone();
};

