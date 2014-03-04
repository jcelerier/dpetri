#pragma once

#if defined(USE_PNAPI)
#include <pnapi/pnapi.h>
using transition_type = pnapi::Transition;
using place_type = pnapi::Place;
using arc_type = pnapi::Arc;
using node_type = pnapi::Node;
#else
#include <simpleNet/simplePetriNet.h>
using transition_type = SimplePetriNet::transition_type;
using place_type = SimplePetriNet::place_type;
using arc_type = SimplePetriNet::arc_type;
using node_type = SimplePetriNet::node_type;
#endif
