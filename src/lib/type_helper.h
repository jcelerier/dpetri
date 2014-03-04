#pragma once

#if defined(USE_PNAPI)
#include <pnapi/pnapi.h>
using transition_type = pnapi::Transition;
using place_type = pnapi::Place;
using arc_type = pnapi::Arc;
using node_type = pnapi::Node;
#else
template<typename T>
using transition_type = T::transition_type;
template<typename T>
using place_type = T::place_type;
template<typename T>
using arc_type = T::arc_type;
#endif
