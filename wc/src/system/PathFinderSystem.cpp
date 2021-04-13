/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#include <Config.h>
#include <system/PathFinderSystem.h>

#include <component/TerrainComponent.h>
#include <component/RenderComponent.h>

#include <abstract_pather.h>

#include <micropather.h>

using namespace wc;
using namespace std;

namespace {
	class PathFinderGraph : public micropather::Graph {
		int _width;
		artemis::ComponentMapper<ObstructionComponent> obstructionMapper;
		std::set<std::shared_ptr<artemis::Entity> >* _entities;

	public:

		void setEntities(std::set<std::shared_ptr<artemis::Entity> >* entities) {
			_entities = entities;
		}
		void setWidth(int width) {
			_width = width;
		}

		void NodeToXY(void* node, int* x, int* y) {
			intptr_t index = (intptr_t)node;
			*y = index / _width;
			*x = index - *y * _width;

			*x -= _width / 2;
			*y -= _width / 2;
		}

		void* XYToNode(int x, int y) {
			x += _width / 2;
			y += _width / 2;

			return (void*)(y*_width + x);
		}

		void* XYToNode(float x, float  y) {
			return XYToNode((int)x, (int)y);
		}

		virtual float LeastCostEstimate(void* nodeStart, void* nodeEnd) {
			int xStart, yStart, xEnd, yEnd;
			NodeToXY(nodeStart, &xStart, &yStart);
			NodeToXY(nodeEnd, &xEnd, &yEnd);

			/* Compute the minimum path cost using distance measurement. It is possible
			to compute the exact minimum path using the fact that you can move only
			on a straight line or on a diagonal, and this will yield a better result.
			*/
			int dx = xStart - xEnd;
			int dy = yStart - yEnd;
			return (float)sqrt((double)(dx*dx) + (double)(dy*dy));
		}

		virtual bool Passable(int x, int y) {
			for (auto p : *_entities) {
				ObstructionComponent* obstruction = obstructionMapper.get(p);
				if (!obstruction->enabled) {
					continue;
				}

				glm::vec3 center = obstruction->bounds.getCenter();
				glm::vec3 extents = obstruction->bounds.getExtents();

				if (fabs(center.x - x) < (extents.x) && fabs(center.z - y) < (extents.z)) {
					return false;
				}
			}
			return true;
		}

		virtual void AdjacentCost(void* node, MP_VECTOR< micropather::StateCost > *adjacent) {
			int x, y;
			const int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
			const int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
			const float cost[8] = { 1.0f, 1.41f, 1.0f, 1.41f, 1.0f, 1.41f, 1.0f, 1.41f };

			NodeToXY(node, &x, &y);

			for (int i = 0; i < 8; ++i) {
				int nx = x + dx[i];
				int ny = y + dy[i];

				bool pass = Passable(nx, ny);
				if (pass) {
					micropather::StateCost nodeCost = { XYToNode(nx, ny), cost[i] };
					adjacent->push_back(nodeCost);
				}
				else {
					micropather::StateCost nodeCost = { XYToNode(nx, ny), FLT_MAX };
					adjacent->push_back(nodeCost);
				}
			}
		}

		virtual void PrintStateInfo(void* state) {

		}
	};

	PathFinderGraph* graph;
	micropather::MicroPather* pather;
}

// TODO: This is for testing. Although it is supposed to be faster it is substantially slower. Why is that?
namespace {
	struct Point
	{
		int x, y;
		Point()
		{
			x = y = 0;
		}
		Point(int x, int y) :x(x), y(y){
		}
		bool operator==(const Point& o)const
		{
			return x == o.x && y == o.y;
		}
		Point operator+(const Point& o)const
		{
			return Point(x + o.x, y + o.y);
		}
		bool operator<(const Point& o)const
		{
			if (x < o.x) return true;
			if (x > o.x) return false;
			if (y < o.y) return true;
			return false;
		}
	};

	struct Edge
	{
		int dx, dy;
		int cost;
		Point destination;
		bool valid;
		Edge()
		{
			dx = dy = 0;
			cost = 1;
			valid = false;
		}
		Edge(int dx, int dy, int cost, Point dest) :dx(dx), dy(dy), cost(cost), destination(dest)
		{
			valid = true;
		}
	};

	struct VisitTracker
	{
		struct VisitPlace
		{
			unsigned int cost;
			Point prev;
			VisitPlace()
			{
				cost = UINT_MAX;
			}
			unsigned int getCost()const
			{
				return cost;
			}

			void visit(unsigned int pcost, Point pprev, Point curp)
			{
				prev = pprev;
				cost = pcost;
			}
			Point getPrevPoint(Point)const
			{
				return prev;
			}
		};
		std::map<Point, VisitPlace> visited;

		VisitPlace& get(Point p)
		{
			return visited[p];
		}
	};

	struct Labyrinth
	{
		TerrainComponent* terrain;
		struct EdgeIterator
		{
			std::vector<Edge> edges;
			int idx;
			Edge next()
			{
				if (idx >= (int)edges.size())
				{
					return Edge();
				}
				return edges[idx++];
			}
		};

		struct EdgeIterator getEdges(const Point& p)
		{
			std::vector<Edge> edges;
			for (int dy = -1; dy <= 1; ++dy)
			{
				for (int dx = -1; dx <= 1; ++dx)
				{
					if (dx != 0 || dy != 0)
					{
						auto np = p + (Point(dx, dy));
						if (np.x >= -terrain->extends.x && np.x <= terrain->extends.x &&
							np.y >= -terrain->extends.z && np.y <= terrain->extends.z) {
							if (graph->Passable(np.x, np.y)) {
								int cost = 1;
								edges.push_back(Edge(dx, dy, cost, np));
							}
						}
					}
				}
			}

			EdgeIterator ei;
			ei.idx = 0;
			ei.edges = edges;
			return ei;
		}

		unsigned int estimate(const Point& p1, const Point& p2)
		{
			return std::abs(p2.x - p1.x) + std::abs(p2.y - p1.y);
		}
	};

	Labyrinth* labyrinth;
}

PathFinderSystem::PathFinderSystem() : EntityProcessingSystem(artemis::Aspect::getAspectFor<ObstructionComponent>()) {
	labyrinth = new Labyrinth();
	graph = new PathFinderGraph();
	pather = new micropather::MicroPather(graph);
}

void PathFinderSystem::initialize() {
}

void PathFinderSystem::processEntity(const std::shared_ptr<artemis::Entity>& e) {
	ObstructionComponent* obstruction = mapper.get(e);
	RenderComponent* render = renderMapper.get(e);
	if (render) {
		ge::Bounds aabb;
		// XXX: Combine the bounds.
		for (auto p = render->renderable.begin(); p != render->renderable.end(); ++p) {
			aabb = (*p)->getBounds();
			break;
		}
		obstruction->bounds = aabb;
	}
}

void PathFinderSystem::begin() {
	pather->Reset();
	graph->setEntities(&_entities);
}

void PathFinderSystem::inserted(const std::shared_ptr<artemis::Entity>& e) {
	_entities.insert(e);
}

void PathFinderSystem::removed(const std::shared_ptr<artemis::Entity>& e) {
	_entities.erase(e);
}

std::vector<glm::vec3> PathFinderSystem::solve(const glm::vec3& from, const glm::vec3& to) {
	sf::Clock clock;
	std::vector<glm::vec3> result;
#ifndef never
	void* startState = graph->XYToNode(from.x, from.z);
	void* endState = graph->XYToNode(to.x, to.z);

	float totalCost;
	micropather::MPVector< void* > path;
	int v = pather->Solve(startState, endState, &path, &totalCost);
	if (v != micropather::MicroPather::SOLVED) {
		// TODO: We don't want to path find here too often. The current code will find each frame.
		// Exactly what to do depends on the entity.
		//
		// Note also this pathfind is a direct find. It won't find the closest spot which is
		// appropriate for players entities.
		return result;
	}

	for (size_t i = 0; i < path.size(); ++i) {
		if (i == 0) {
			result.push_back(glm::vec3(from.x, 0, from.z));
		}
		else if (i == path.size() - 1) {
			result.push_back(glm::vec3(to.x, 0, to.z));
		}
		else {
			int x, y;
			graph->NodeToXY(path[i], &x, &y);
			result.push_back(glm::vec3(x, 0, y));
		}
	}
#else
	AbstractPather<Labyrinth, Point, Edge, VisitTracker> pather(*labyrinth);
	for (auto p : pather.solve(Point(from.x, from.z), Point(to.x, to.z))) {
		result.push_back(glm::vec3(p.x, 0, p.y));
	}
#endif
	sf::Time t = clock.getElapsedTime();
	printf("elapsed: %.8f\n", t.asSeconds());

	return result;
}
void PathFinderSystem::setTerrain(const std::shared_ptr<artemis::Entity>& terrain) {
	TerrainComponent* terrainComponent = terrain->getComponent<TerrainComponent>();
	labyrinth->terrain = terrainComponent;
	graph->setWidth(terrainComponent->heightmap->getSize().x);
}
