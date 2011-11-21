/*******************************************************************
 *
 *    DESCRIPTION: 	refobj.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 15/9/97
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "refobj.hpp"
#include "list_tem.hpp"

#define UseLocalAssert Yes
#include "ourasert.h"


/* Exported globals ************************************************/
	char const* refobj_fail_addref = "Failure in R_AddRef()\n";
	char const* refobj_fail_release = "Failure in R_Release()\n";
	char const* refobj_fail_destructor = "Failure in Destructor()\n";



/* adj  delete me */
