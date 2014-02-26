#pragma once

#include <pnapi/pnapi.h>
using namespace pnapi;
struct OwnedNode
{
		OwnedNode(Node* n , unsigned int i):
			node(n),
			id(i)
		{
		}

	Node* node{nullptr};
	unsigned int id{0};
};
