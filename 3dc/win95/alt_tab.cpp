#include "db.h"
#include "hash_tem.hpp"
//#include <typeinfo>


#include "awtexld.h"
#include "alt_tab.h"

template <class DX_PTR>
class AltTabRestore
{
	public:
		virtual ~AltTabRestore(){}
		virtual void DoRestore(DX_PTR * pDxGraphic) = 0;
};

template <class DX_PTR>
class AltTabAwRestore : public AltTabRestore<DX_PTR>
{
	public:
		AltTabAwRestore(AW_BACKUPTEXTUREHANDLE hBackup) : m_hBackup(hBackup) {}
	private:
		AW_BACKUPTEXTUREHANDLE m_hBackup;
	protected:
		virtual void DoRestore(DX_PTR * pDxGraphic);
};

template <>
void AltTabAwRestore<DDSurface>::DoRestore(DDSurface * pSurface)
{
	DDSurface * pNewSurface = AwCreateSurface("rtf",m_hBackup,pSurface,AW_TLF_PREVSRCALL);
	// should have succeeded
	db_assert1(pNewSurface);
	// should return the same pointer!
	db_assert1(pNewSurface == pSurface);
	// don't need both references
	pNewSurface->Release();
}

template <>
void AltTabAwRestore<D3DTexture>::DoRestore(D3DTexture * pTexture)
{
	D3DTexture * pNewTexture = AwCreateTexture("rtf",m_hBackup,pTexture,AW_TLF_PREVSRCALL);
	// should have succeeded
	db_assert1(pNewTexture);
	// should return the same pointer!
	db_assert1(pNewTexture == pTexture);
	// don't need both references
	pNewTexture->Release();
}

template <class DX_PTR>
class AltTabUserRestore : public AltTabRestore<DX_PTR>
{
	public:
		typedef void (* PFN_RESTORE) (DX_PTR * pDxGraphic, void * pUser);
			AltTabUserRestore(PFN_RESTORE pfnRestore, void * pUser, char const * pszFuncName) : m_pfnRestore(pfnRestore), m_pUser(pUser), m_pszFuncName(pszFuncName) {}
	private:
		PFN_RESTORE m_pfnRestore;
		void * m_pUser;
		char const * m_pszFuncName;
	protected:
		virtual void DoRestore(DX_PTR * pDxGraphic);
};

template <class DX_PTR>
void AltTabUserRestore<DX_PTR>::DoRestore(DX_PTR * pDxGraphic)
{
		db_logf4(("\t\tCalling User Restore Function %s = %p (%s * = %p, void * = %p)",m_pszFuncName,m_pfnRestore,typeid(*pDxGraphic).name(),pDxGraphic,m_pUser));
	
	m_pfnRestore(pDxGraphic, m_pUser);
}

template <class DX_PTR>
struct AltTabEntry
{
	DX_PTR * m_pDxGraphic;
	AltTabRestore<DX_PTR> * m_pRestore;

		char const * m_pszFile;
		unsigned m_nLine;
		char * m_pszDebugString;
	

	inline bool operator == (AltTabEntry const & rEntry) const
		{ return m_pDxGraphic == rEntry.m_pDxGraphic; }
	inline bool operator != (AltTabEntry const & rEntry) const
		{ return ! operator == (rEntry); }
		
	friend inline HashFunction(AltTabEntry const & rEntry)
		{ return HashFunction(rEntry.m_pDxGraphic); }
};

struct AltTabLists
{
	HashTable<AltTabEntry<D3DTexture> > m_listTextures;
	HashTable<AltTabEntry<DDSurface> > m_listSurfaces;
};

static
struct AltTabDebugLists : AltTabLists
{
	~AltTabDebugLists()
	{
		// destructor for derived class will be called before destructor
		// for base class, so we can make assersions about the base class:
		// everything *should* have been removed from these lists before exiting
		if (m_listSurfaces.Size())
		{
			db_logf1(("ERROR: AltTab lists still referring to %u surface(s) on exiting",m_listSurfaces.Size()));
			unsigned i = 1;
			for
			(
				HashTable<AltTabEntry<DDSurface> >::ConstIterator itSurfaces(m_listSurfaces);
				!itSurfaces.Done(); itSurfaces.Next()
			)
			{
				db_logf1(("\t%u. Included in line %u of %s (details: %s)",i++,itSurfaces.Get().m_nLine,itSurfaces.Get().m_pszFile,itSurfaces.Get().m_pszDebugString ? itSurfaces.Get().m_pszDebugString : "n/a"));
			}
			
		}
		else
		{
			db_logf1(("OK on exiting: AltTab surface lists are clean"));
		}
		if (m_listTextures.Size())
		{
			db_logf1(("ERROR: AltTab lists still referring to %u texture(s) on exiting",m_listTextures.Size()));
			unsigned i = 1;
			for
			(
				HashTable<AltTabEntry<D3DTexture> >::ConstIterator itTextures(m_listTextures);
				!itTextures.Done(); itTextures.Next()
			)
			{
				db_logf1(("\t%u. Included in line %u of %s (details: %s)",i++,itTextures.Get().m_nLine,itTextures.Get().m_pszFile,itTextures.Get().m_pszDebugString ? itTextures.Get().m_pszDebugString : "n/a"));
			}
		}
		else
		{
			db_logf1(("OK on exiting: AltTab texture lists are clean"));
		}
	}
}
	g_atlists;

	void _ATIncludeTexture(D3DTexture * pTexture, AW_BACKUPTEXTUREHANDLE hBackup, char const * pszFile, unsigned nLine, char const * pszDebugString)

{
	db_assert1(pTexture);
	db_assert1(hBackup);
	HashTable<AltTabEntry<D3DTexture> >::Node * pNewNode = g_atlists.m_listTextures.NewNode();
	pNewNode->d.m_pDxGraphic = pTexture;
	pNewNode->d.m_pRestore = new AltTabAwRestore<D3DTexture>(hBackup);

		pNewNode->d.m_pszFile = pszFile;
		pNewNode->d.m_nLine = nLine;
		if (pszDebugString)
		{
			pNewNode->d.m_pszDebugString = new char [strlen(pszDebugString)+1];
			strcpy(pNewNode->d.m_pszDebugString,pszDebugString);
		}
		else
			pNewNode->d.m_pszDebugString = NULL;

	g_atlists.m_listTextures.AddAsserted(pNewNode);
}

	void _ATIncludeSurface(DDSurface * pSurface, AW_BACKUPTEXTUREHANDLE hBackup, char const * pszFile, unsigned nLine, char const * pszDebugString)
{
	db_assert1(pSurface);
	db_assert1(hBackup);
	HashTable<AltTabEntry<DDSurface> >::Node * pNewNode = g_atlists.m_listSurfaces.NewNode();
	pNewNode->d.m_pDxGraphic = pSurface;
	pNewNode->d.m_pRestore = new AltTabAwRestore<DDSurface>(hBackup);
		pNewNode->d.m_pszFile = pszFile;
		pNewNode->d.m_nLine = nLine;
		if (pszDebugString)
		{
			pNewNode->d.m_pszDebugString = new char [strlen(pszDebugString)+1];
			strcpy(pNewNode->d.m_pszDebugString,pszDebugString);
		}
		else
			pNewNode->d.m_pszDebugString = NULL;


	g_atlists.m_listSurfaces.AddAsserted(pNewNode);
}

	void _ATIncludeTextureEx(D3DTexture * pTexture, AT_PFN_RESTORETEXTURE pfnRestore, void * pUser, char const * pszFile, unsigned nLine, char const * pszFuncName, char const * pszDebugString)

{
	db_assert1(pTexture);
	db_assert1(pfnRestore);
	HashTable<AltTabEntry<D3DTexture> >::Node * pNewNode = g_atlists.m_listTextures.NewNode();
	pNewNode->d.m_pDxGraphic = pTexture;
		pNewNode->d.m_pRestore = new AltTabUserRestore<D3DTexture>(pfnRestore,pUser,pszFuncName);
		pNewNode->d.m_pszFile = pszFile;
		pNewNode->d.m_nLine = nLine;
		if (pszDebugString)
		{
			pNewNode->d.m_pszDebugString = new char [strlen(pszDebugString)+1];
			strcpy(pNewNode->d.m_pszDebugString,pszDebugString);
		}
		else
			pNewNode->d.m_pszDebugString = NULL;
	
	g_atlists.m_listTextures.AddAsserted(pNewNode);
}

	void _ATIncludeSurfaceEx(DDSurface * pSurface, AT_PFN_RESTORESURFACE pfnRestore, void * pUser, char const * pszFile, unsigned nLine, char const * pszFuncName, char const * pszDebugString)
{
	db_assert1(pSurface);
	db_assert1(pfnRestore);
	HashTable<AltTabEntry<DDSurface> >::Node * pNewNode = g_atlists.m_listSurfaces.NewNode();
	pNewNode->d.m_pDxGraphic = pSurface;
		pNewNode->d.m_pRestore = new AltTabUserRestore<DDSurface>(pfnRestore,pUser,pszFuncName);
		pNewNode->d.m_pszFile = pszFile;
		pNewNode->d.m_nLine = nLine;
		if (pszDebugString)
		{
			pNewNode->d.m_pszDebugString = new char [strlen(pszDebugString)+1];
			strcpy(pNewNode->d.m_pszDebugString,pszDebugString);
		}
		else
			pNewNode->d.m_pszDebugString = NULL;

	g_atlists.m_listSurfaces.AddAsserted(pNewNode);
}

void ATRemoveTexture(D3DTexture * pTexture)
{
	db_assert1(pTexture);
	AltTabEntry<D3DTexture> ateRemove;
	ateRemove.m_pDxGraphic = pTexture;
	AltTabEntry<D3DTexture> const * pEntry = g_atlists.m_listTextures.Contains(ateRemove);
	db_assert1(pEntry);
	db_assert1(pEntry->m_pRestore);
	delete pEntry->m_pRestore;
	g_atlists.m_listTextures.RemoveAsserted(ateRemove);
}

void ATRemoveSurface(DDSurface * pSurface)
{
	db_assert1(pSurface);
	AltTabEntry<DDSurface> ateRemove;
	ateRemove.m_pDxGraphic = pSurface;
	AltTabEntry<DDSurface> const * pEntry = g_atlists.m_listSurfaces.Contains(ateRemove);
	db_assert1(pEntry);
	db_assert1(pEntry->m_pRestore);
	delete pEntry->m_pRestore;
		if (pEntry->m_pszDebugString)
			delete[] pEntry->m_pszDebugString;

	g_atlists.m_listSurfaces.RemoveAsserted(ateRemove);
}

void ATOnAppReactivate()
{
	db_log1("ATOnAppReactivate() called");
	// surfaces
	for
	(
		HashTable<AltTabEntry<DDSurface> >::ConstIterator itSurfaces(g_atlists.m_listSurfaces);
		!itSurfaces.Done(); itSurfaces.Next()
	)
	{
		DDSurface * pSurface = itSurfaces.Get().m_pDxGraphic;
		db_logf5(("\tIs surface %p lost?? [included at line %u of %s (details: %s)]",pSurface,itSurfaces.Get().m_nLine,itSurfaces.Get().m_pszFile,itSurfaces.Get().m_pszDebugString ? itSurfaces.Get().m_pszDebugString : "n/a"));
		HRESULT hResult = pSurface->IsLost();
		if (DDERR_SURFACELOST == hResult)
		{
			db_logf4(("\t\tSurface %p is lost - restoring",pSurface));
			hResult = pSurface->Restore();
			if (DD_OK == hResult)
			{
				db_logf5(("\t\tRestore() returned DD_OK",pSurface));
				db_assert1(itSurfaces.Get().m_pRestore);
				itSurfaces.Get().m_pRestore->DoRestore(pSurface);
			}
			else
			{
				db_logf1(("\t\tERROR [%p->Restore()] %s",pSurface,AwDxErrorToString(hResult)));
			}
		}
		else if (DD_OK != hResult)
		{
			db_logf1(("\t\tERROR [%p->IsLost()] %s",pSurface,AwDxErrorToString(hResult)));
		}
		else
		{
			db_logf5(("\t\tSurface %p wasn't lost",pSurface));
		}
	}
		
	// textures
	for
	(
		HashTable<AltTabEntry<D3DTexture> >::ConstIterator itTextures(g_atlists.m_listTextures);
		!itTextures.Done(); itTextures.Next()
	)
	{
		D3DTexture * pTexture = itTextures.Get().m_pDxGraphic;
		db_logf5(("\tIs texture %p lost?? [included at line %u of %s (details: %s)]",pTexture,itTextures.Get().m_nLine,itTextures.Get().m_pszFile,itTextures.Get().m_pszDebugString ? itTextures.Get().m_pszDebugString : "n/a"));
		DDSurface * pSurface;
		HRESULT hResult = pTexture->QueryInterface(GUID_DD_SURFACE,reinterpret_cast<void * *>(&pSurface));
		if (DD_OK != hResult)
		{
			db_logf1(("\t\tERROR [%p->QueryInterface(GUID_DD_SURFACE,%p)] %s",pTexture,&pSurface,AwDxErrorToString(hResult)));
		}
		else
		{
			hResult = pSurface->IsLost();
			if (DDERR_SURFACELOST == hResult)
			{
				db_logf4(("\t\tTexture %p is lost - restoring",pTexture));
				hResult = pSurface->Restore();
				if (DD_OK == hResult)
				{
					db_logf5(("\t\tRestore() returned DD_OK",pTexture));
					db_assert1(itTextures.Get().m_pRestore);
					itTextures.Get().m_pRestore->DoRestore(pTexture);
				}
				else
				{
					db_logf1(("\t\tERROR [%p->Restore()] %s",pTexture,AwDxErrorToString(hResult)));
				}
			}
			else if (DD_OK != hResult)
			{
				db_logf1(("\t\tERROR [%p->IsLost()] %s",pTexture,AwDxErrorToString(hResult)));
			}
			else
			{
				db_logf5(("\t\tTexture %p wasn't lost",pTexture));
			}
			// don't need this reference anymore
			pSurface->Release();
		}
	}
		
	db_log1("ATOnAppReactivate() returning");
}


