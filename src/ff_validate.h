/************************************************
 ff_validate.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Block Validation Routines
 ************************************************/
#ifndef FF_VALIDATE_H_
#define FF_VALIDATE_H_


/************************************************
 Includes
 ************************************************/

#include "ff_registry.h"

/************************************************
 Public Data Structures
 ************************************************/



/************************************************
 Function prototypes
 ************************************************/

void DumpBlock(BlockNode* block);

void Validate(BlockNode* b);


#endif /* FF_VALIDATE_H_ */
