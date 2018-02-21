/*****************************************************************
 MapNode.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#ifndef MAPNODE_H_
#define MAPNODE_H_

//#include <stdlib.h>

class MapNode {
public:
	MapNode();
	virtual ~MapNode();

	MapNode* first_child;
	MapNode* next_sibling;
	MapNode* parent;
	int data_ref;
};


#endif /* MAPNODE_H_ */
