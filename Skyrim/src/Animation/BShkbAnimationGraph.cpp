#include "Skyrim/Animation/BShkbAnimationGraph.h"

BSTEventSource<BSTransformDeltaEvent>* BShkbAnimationGraph::GetBSTransformDeltaEventSource()
{
	return static_cast<BSTEventSource<BSTransformDeltaEvent>*>(this);
}


BSTEventSource<BSAnimationGraphEvent>* BShkbAnimationGraph::GetBSAnimationGraphEventSource()
{
	return static_cast<BSTEventSource<BSAnimationGraphEvent>*>(this);
}