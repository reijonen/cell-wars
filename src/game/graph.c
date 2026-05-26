#include <string.h>

#include "graph.h"
#include "game_render_ids.h"
#include "engine/renderer.h"

void graph_init_nodes(Node *nodes, size_t count)
{
	memset(nodes, 0, sizeof(Node) * count);
	for (size_t i = 0; i < count; i++)
	{
		nodes[i].id = (NID)i;
	}
}

void graph_init_edges(Edge *edges, size_t count)
{
	memset(edges, 0, sizeof(Edge) * count);
	for (size_t i = 0; i < count; i++)
	{
		edges[i].id = (EID)i;
	}
}

void graph_build_default(Node *nodes, Edge *edges)
{
	graph_connect_undirected(nodes, edges, 0, 0, 1);
	graph_connect_undirected(nodes, edges, 1, 0, 2);
	graph_connect_undirected(nodes, edges, 2, 0, 3);
	graph_connect_undirected(nodes, edges, 3, 1, 2);
	graph_connect_undirected(nodes, edges, 4, 2, 3);
	graph_connect_undirected(nodes, edges, 5, 1, 4);
	graph_connect_undirected(nodes, edges, 6, 2, 4);
	graph_connect_undirected(nodes, edges, 7, 3, 4);
}

void graph_build_edge_vertices(Vertex *vertices, const Base *bases, const Edge *edges, unsigned edge_count)
{
	Color c = {255, 255, 255, 255};
	unsigned vertex_idx = 0;
	for (unsigned e = 0; e < edge_count; e++)
	{
		NID left = edges[e].endpoints[0];
		NID right = edges[e].endpoints[1];
		const Base *left_base = &bases[left];
		const Base *right_base = &bases[right];

		vertices[vertex_idx].pos.x = left_base->pos.x + left_base->size / 2;
		vertices[vertex_idx].pos.y = left_base->pos.y - left_base->size / 2;
		vertices[vertex_idx].color = c;
		vertex_idx++;

		vertices[vertex_idx].pos.x = right_base->pos.x + left_base->size / 2;
		vertices[vertex_idx].pos.y = right_base->pos.y - left_base->size / 2;
		vertices[vertex_idx].color = c;
		vertex_idx++;
	}
}

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

void edges_render(void)
{
	renderer_draw(EDGE_PIPELINE_IDX, EDGE_BUFF_IDX, EDGE_COUNT * 2, 1);
}
