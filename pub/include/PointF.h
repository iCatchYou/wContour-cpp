#pragma once
class  PointF {
public:
	PointF();
	PointF(float _x, float _y);

	float getX() const;
	float getY() const;

	void setX(float _x);
	void setY(float _y);

	void set(float _x, float _y);

public:
	float X;
	float Y;
};
