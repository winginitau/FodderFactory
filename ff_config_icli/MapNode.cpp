/*****************************************************************
 MapNode.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#include "MapNode.h"
#include <stdlib.h>

MapNode::MapNode() {
	first_child = NULL;
	next_sibling = NULL;
	parent = NULL;
	data_ref = 0;
}

MapNode::~MapNode() {
	// Auto-generated destructor stub
}

