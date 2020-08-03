/*
 * graph.h
 *
 *  Created on: 27 Jul 2020
 *      Author: sergeynasekin
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#pragma once

#include "general_utils.h"

#include <cstdlib>
#include <deque>
#include <vector>

namespace Graph {

using VertexId = size_t;
using EdgeId = size_t;

template<typename Weight>
struct Edge {
	VertexId from;
	VertexId to;
	Weight weight;
};

template<typename Weight>
class DirectedWeightedGraph {
private:
	using EdgeList = std::vector<EdgeId>;
	using EdgesRange = Range<typename EdgeList::const_iterator>;

public:
	DirectedWeightedGraph(size_t vertex_count = 0);
	EdgeId AddEdge(const Edge<Weight>& edge);

	size_t GetVertexCount() const;
	size_t GetEdgeCount() const;
	const Edge<Weight>& GetEdge(EdgeId edge_id) const;
	EdgesRange GetVertexEdges(VertexId vertex) const;

private:
	std::vector<Edge<Weight>> edges_;
	// vector of vectors of EdgeIds which puts into correspondence to each vertex related edges
	std::vector<EdgeList> vertices_to_edge_lists_;
};

template<typename Weight>
DirectedWeightedGraph<Weight>::DirectedWeightedGraph(size_t vertex_count) :
	vertices_to_edge_lists_(vertex_count) {
}

template<typename Weight>
EdgeId DirectedWeightedGraph<Weight>::AddEdge(const Edge<Weight>& edge) {
	edges_.push_back(edge);
	const EdgeId id = edges_.size() - 1;
	vertices_to_edge_lists_[edge.from].push_back(id);
	return id;
}

template<typename Weight>
size_t DirectedWeightedGraph<Weight>::GetVertexCount() const {
	return vertices_to_edge_lists_.size();
}

template<typename Weight>
size_t DirectedWeightedGraph<Weight>::GetEdgeCount() const {
	return edges_.size();
}

template<typename Weight>
const Edge<Weight>& DirectedWeightedGraph<Weight>::GetEdge(
		EdgeId edge_id) const {
	// get edge by id
	return edges_[edge_id];
}

template<typename Weight>
typename DirectedWeightedGraph<Weight>::EdgesRange DirectedWeightedGraph<
		Weight>::GetVertexEdges(VertexId vertex) const {
	// get an iterators' range for edges corresponding to the given vertex
	const auto& edges = vertices_to_edge_lists_[vertex];
	return {std::begin(edges), std::end(edges)};
}
}

#endif /* GRAPH_H_ */
