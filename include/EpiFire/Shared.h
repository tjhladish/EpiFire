#ifndef EF_SHARED_H
#define EF_SHARED_H

#include <map>

//predeclare classes
class Edge;
class Node;
class Network;
typedef int stateType;
struct MapNodeComp { bool operator() (const Node* const& lhs, const Node* const& rhs) const; };
typedef std::map<const Node*, double, MapNodeComp> DistanceMatrix;
typedef std::map<const Node*, DistanceMatrix, MapNodeComp> PairwiseDistanceMatrix;

#endif // EF_SHARED_H
