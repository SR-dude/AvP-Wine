/*
	
	command.hpp

	An object for encapsulating requests; see pp233-242 of
 	"Design Patterns"

*/

#ifndef _command
#define _command 1

#include "refobj.hpp"

	
#ifdef __cplusplus
	extern "C" {
#endif

/* Type definitions *****************************************************/
	class Command : public RefCountObject
	{
	public:
		virtual OurBool Execute(void) = 0;
			// return value is "was command completed successfully?"

	protected:
		// Empty constructor:
		Command() : RefCountObject() {}
		
	protected:
		// Protected destructor; Release() is the only method allowed to 
		// delete it...
		virtual ~Command()
		{
		 	// empty
		}
	};
	

#ifdef __cplusplus
	};
#endif

#endif
