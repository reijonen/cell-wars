#include "graph.h"

// TODO: TMP
#include <stdio.h>

void new_edge(Edge *edge, unsigned *edge_count, EID from, EID to)
{
	edge->endpoints[0] = from;
	edge->endpoints[1] = to;
	*edge_count++;
}
