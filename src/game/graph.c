#include "graph.h"

void new_edge(Edge *edge, EID from, EID to)
{
	edge->endpoints[0] = from;
	edge->endpoints[1] = to;
}

void add_node_link(Node *node, NID to, EID edge)
{
	node->links[node->link_count].to = to;
	node->links[node->link_count].edge = edge;
	node->link_count++;
}

void graph_connect_undirected(Node *nodes, Edge *edges, EID edge_id, NID from, NID to)
{
	new_edge(&edges[edge_id], from, to);
	add_node_link(&nodes[from], to, edge_id);
	add_node_link(&nodes[to], from, edge_id);
}

EID graph_find_edge_between(const Node *node, NID to)
{
	for (unsigned i = 0; i < node->link_count; i++)
	{
		if (node->links[i].to == to)
		{
			return node->links[i].edge;
		}
	}

	return INVALID_EID;
}
