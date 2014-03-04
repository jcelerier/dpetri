#pragma once
#include "type_helper.h"

struct OwnedNode
{
		OwnedNode(node_type* n , unsigned int i):
			node(n),
			id(i)
		{
		}

	node_type* node{nullptr};
	unsigned int id{0};
};

