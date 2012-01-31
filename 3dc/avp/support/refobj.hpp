/*
	
	refobj.hpp

	Created by DHM for Headhunter; copied into AVP

	Reference-counted base class to be used by e.g. strings

*/

#ifndef _refobj
#define _refobj 1

#include "fail.h"

#ifdef __cplusplus
	extern "C" {
#endif


	extern char const* refobj_fail_addref;
	extern char const* refobj_fail_release;
	extern char const* refobj_fail_destructor;


	class R_DumpContext; // fully declared in DCONTEXT.HPP

	class RefCountObject_TrackData; // fully declared within REFOBJ.CPP

	class RefCountObject
	{
	// {{{ Private data
	private:
		int RefCount;


	// }}}


	// {{{ Private functions
	private:
		// Private fns for the tracking system are complex and defined in REFOBJ.CPP
	// }}}

	public:
		RefCountObject() :
			RefCount(1)
		{
		}

		void R_AddRef(void)
		{
			if ( RefCount <= 0)
			{
				fail(refobj_fail_addref);
			}
			RefCount++;
		}

		void R_Release(void)
		{
			if ( RefCount <= 0)
			{
				fail(refobj_fail_release);
			}

			if ( (--RefCount) == 0 )
			{
				delete this;
			}
		}

		int CheckRef() const
		{
			return RefCount;
		}
			// Handy way to examine reference count for debugging only


	protected:
		// Destructors must be protected; only derived classes may use the
		// destructor and (we hope) only in the R_Release() method:
		virtual ~RefCountObject()
		{
			if ( RefCount != 0 )
			{
				fail(refobj_fail_destructor);
			}
		}
	};




#ifdef __cplusplus
	};
#endif

#endif
