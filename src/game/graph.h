#pragma once

typedef unsigned NID;
typedef unsigned EID;

typedef struct Edge
{
	EID id;
	NID endpoints[2];
} Edge;

typedef struct Node
{
	NID id;
	EID edges[4];
	unsigned char edge_count;
} Node;

void new_edge(Edge *edge, unsigned *edge_count, EID from, EID to);