#pragma once

#include <pnapi/pnapi.h>
using namespace pnapi;

template <class PetriNetImpl>
struct OwnedNode
{
		OwnedNode(typename PetriNetImpl::node_type* n , unsigned int i):
			node(n),
			id(i)
		{
		}

	typename PetriNetImpl::node_type* node{nullptr};
	unsigned int id{0};
};

template <>
struct OwnedNode<pnapi::PetriNet>
{
		OwnedNode(pnapi::Node* n , unsigned int i):
			node(n),
			id(i)
		{
		}

	typename pnapi::Node* node{nullptr};
	unsigned int id{0};
};
