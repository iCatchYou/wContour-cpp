#include "pch.h"
#include "PointF.h"



PointF::PointF() : X(0.0f), Y(0.0f) {}
PointF::PointF(float _x, float _y) : X(_x), Y(_y) {}

float PointF::getX() const { return X; }
float PointF::getY() const { return Y; }

void PointF::setX(float _x) { X = _x; }
void PointF::setY(float _y) { Y = _y; }

void PointF::set(float _x, float _y) { X = _x; Y = _y; }