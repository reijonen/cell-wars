#pragma once

typedef unsigned NID;
typedef unsigned EID;

#define NODE_MAX_LINKS 4
#define INVALID_EID ((EID)-1)

typedef struct Edge
{
	EID id;
	NID endpoints[2];
} Edge;

typedef struct NodeLink
{
	NID to;
	EID edge;
} NodeLink;

typedef struct Node
{
	NID id;
	NodeLink links[NODE_MAX_LINKS];
	unsigned char link_count;
} Node;

void new_edge(Edge *edge, EID from, EID to);
void add_node_link(Node *node, NID to, EID edge);
void graph_connect_undirected(Node *nodes, Edge *edges, EID edge_id, NID from, NID to);
EID graph_find_edge_between(const Node *node, NID to);
void edges_render(void);
