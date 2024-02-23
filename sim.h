#pragma once
#include <vector>
#include "types3d.h"

struct point
{
	vec2d pos;
	vec2d prevPos;
	bool locked;

	point(float x, float y, bool locked = false)
		: pos(vec2d(x, y)), prevPos(vec2d(x, y)), locked(locked)
	{}
};

struct stick
{
	point *pA, *pB;
	float length;

	stick(point * pA, point * pB, float length)
		: pA(pA), pB(pB), length(length)
	{}
};


class ClothSim
{
private:
	vector<point*> points;
	vector<stick> sticks;
	int numIterations;
	float drag;
	vec2d gravity = vec2d(0, 100.0f);

public:

	ClothSim(int numIterations = 10, float drag = 0.9999f)
		: numIterations(numIterations), drag(drag)
	{}

	void Clear()
	{
		for (int i = 0; i < points.size(); i++)
		{
			delete points[i];
		}
		points.clear();
		sticks.clear();
	}

	void Simulate(float fElapsedTime)
	{
		for (point* p : points)
		{
			if (!p->locked)
			{
				vec2d temp = p->pos;
				vec2d vel = p->pos - p->prevPos;
				if (vel.length() > 10) { vel = vel.normalized() * 10; }
				p->pos += vel * drag;
				p->pos += gravity * fElapsedTime * fElapsedTime;
				p->prevPos = temp;
			}
		}

		for (int i = 0; i < numIterations; i++)
		{
			for (stick s : sticks)
			{
				vec2d center = (s.pA->pos + s.pB->pos) / 2;
				vec2d dir = (s.pA->pos - s.pB->pos).normalized();
				if (!s.pA->locked)
				{
					s.pA->pos = center + dir * s.length / 2.0f;
				}
				if (!s.pB->locked)
				{
					s.pB->pos = center - dir * s.length / 2.0f;
				}
			}
		}
	}

	point* AddPoint(float x, float y, bool locked = false)
	{
		point* res = new point(x, y, locked);
		points.push_back(res);
		return res;
	}

	void RemoveNearestPoint(vec2d fromPos, float within = -1)
	{
		if (points.size() == 0)
		{
			return;
		}

		int ptIndex = 0;
		float shortestDist = points[0]->pos.dist(fromPos);

		for (int i = 1; i < points.size(); i++)
		{
			float dist = points[i]->pos.dist(fromPos);
			if (dist < shortestDist)
			{
				shortestDist = dist;
				ptIndex = i;
			}
		}
		if (within == -1 || shortestDist < within) {
			//Remove all sticks joined to pt
			sticks.erase(remove_if(begin(sticks), end(sticks), [ptIndex, this](stick const& s)
				{
					return s.pA == points[ptIndex] || s.pB == points[ptIndex];
				}), end(sticks));

			//Delete point
			points.erase(points.begin() + ptIndex);
		}
	}

	void AddStick(point* pA, point* pB)
	{
		//float length = 10;
		float length = pA->pos.dist(pB->pos);
		sticks.push_back(stick(pA, pB, length));
	}

	int GetNumPoints()
	{
		return points.size();
	}
	int GetNumSticks()
	{
		return sticks.size();
	}

	//Returns nullptr if there are no points
	point* GetClosestPoint(vec2d fromPos, float within = -1)
	{
		if (points.size() == 0)
		{
			return nullptr;
		}

		point* res = points[0];
		float shortestDist = points[0]->pos.dist(fromPos);

		for (int i = 1; i < points.size(); i++)
		{
			float dist = points[i]->pos.dist(fromPos);
			if (dist < shortestDist)
			{
				shortestDist = dist;
				res = points[i];
			}
		}
		if (within == -1 || shortestDist < within)
		{
			return res;
		}
		return nullptr;
	}

	void Draw(PixelGameEngine* ge, vec2d offset = vec2d())
	{
		//Sticks
		for (stick s : sticks)
		{
			ge->DrawLine((s.pA->pos - offset).asVf(), (s.pB->pos - offset).asVf(), WHITE);
		}
		//Points
		for(point* p : points)
		{
			//ge->DrawString((points[i].pos + vec2d(0, 10)).asVf(), to_string(i), RED);
			ge->DrawCircle((p->pos - offset).asVf(), 2, p->locked?RED:GREEN);
		}
	}
};
