#ifndef ABSTRACT_PATHER
#define ABSTRACT_PATHER
/*
Copyright (C) 2013 Anders Musikka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <vector>
#include "rheap.h"
#include <algorithm>

template<
	class World,
	class Point,
	class Edge,
	class VisitTracker>
class AbstractPather
{
	World& world;
	VisitTracker visited;
	struct State
	{
		Point point;
		unsigned int burnedCost;
		unsigned int nominalCost;
	};

public:

	friend unsigned int costof(const State& o)
	{
		return o.nominalCost;
	}

	AbstractPather(World& world,const VisitTracker& visited=VisitTracker()) : world(world),visited(visited)
	{
	}

	std::vector<Point> solve(Point p1,Point p2)
	{
		RHeap<State> stack;
		State state;
		state.burnedCost=0;
		state.nominalCost=0+world.estimate(p1,p2);
		state.point=p1;
		stack.push(state);

		while(!stack.empty())
		{
			const State top=stack.pop();

			Point toppoint=top.point;
			if (toppoint==p2)
				return getPath(p2,p1);

			auto edgeIterator=world.getEdges(toppoint);

			for(;;)
			{
				auto edge=edgeIterator.next();
				if (!edge.valid)
					break;
				Point newPoint=edge.destination;
				unsigned int edgecost=edge.cost;
				unsigned int estimateRemaining=world.estimate(toppoint,newPoint);
				unsigned int burnedCost=edgecost+top.burnedCost;
				unsigned int nominalCost=burnedCost+estimateRemaining;
				auto& visitPlace=visited.get(newPoint);
				unsigned int alreadyCost=visitPlace.getCost();

				if (alreadyCost<=burnedCost)
					continue;

				State newstate;
				newstate.burnedCost=burnedCost;
				newstate.nominalCost=nominalCost;
				newstate.point=newPoint;
				visitPlace.visit(burnedCost,toppoint,newPoint);
				stack.push(newstate);
			}
		}
		return std::vector<Point>(); //no solution

	}
private:
	std::vector<Point> getPath(Point p2,Point p1)
	{
		Point cur=p2;
		std::vector<Point> ret;
		ret.push_back(cur);
		while(!(cur==p1))
		{
			cur=visited.get(cur).getPrevPoint(cur);
			ret.push_back(cur);
		}
		std::reverse(ret.begin(),ret.end());
		return ret;
	}

};

#endif

