#include "RendererD3D9StableHeader.h"
#include "gkStateManager.h"

//--------------------------------------------------------------------------------------
// Base implementation of a custom ID3DXEffectStateManager interface
// This implementation does nothing more than forward all state change commands to the
// appropriate D3D handler.
// An interface that implements custom state change handling may be derived from this
// (such as statistical collection, or filtering of redundant state change commands for
// a subset of states)
//--------------------------------------------------------------------------------------
class CBaseStateManager :
	public gkStateManager
{
protected:
	LPDIRECT3DDEVICE9 m_pDevice;
	LONG m_lRef;
	UINT m_nTotalStateChanges;
	UINT m_nTotalStateChangesPerFrame;
	WCHAR m_wszFrameStats[256];
public:
	CBaseStateManager( LPDIRECT3DDEVICE9 pDevice )
		: m_lRef( 1UL ),
		m_pDevice( pDevice ),
		m_nTotalStateChanges( 0 ),
		m_nTotalStateChangesPerFrame( 0 )
	{
		// Increment the reference count on the device, because a pointer to it has
		// been copied for later use
		m_pDevice->AddRef();
		m_wszFrameStats[0] = 0;
	}

	virtual ~CBaseStateManager()
	{
		// Release the reference count held from the constructor
		m_pDevice->Release();
	}

	// Must be invoked by the application anytime it allows state to be
	// changed outside of the D3DX Effect system.
	// An entry-point for this should be provided if implementing custom filtering of redundant
	// state changes.
	virtual void DirtyCachedValues()
	{
	}

	virtual LPCWSTR EndFrameStats()
	{
		if( m_nTotalStateChangesPerFrame != m_nTotalStateChanges )
		{
			swprintf_s( m_wszFrameStats,
				256,
				L"Frame Stats:\nTotal State Changes (Per Frame): %d",
				m_nTotalStateChanges );

			m_wszFrameStats[255] = 0;
			m_nTotalStateChangesPerFrame = m_nTotalStateChanges;
		}

		m_nTotalStateChanges = 0;

		return m_wszFrameStats;
	}

	// methods inherited from ID3DXEffectStateManager
	STDMETHOD( QueryInterface )( THIS_ REFIID iid, LPVOID *ppv )
	{
		if( iid == IID_IUnknown || iid == IID_ID3DXEffectStateManager )
		{
			*ppv = static_cast<ID3DXEffectStateManager*>( this );
		}
		else
		{
			*ppv = NULL;
			return E_NOINTERFACE;
		}

		reinterpret_cast<IUnknown*>( this )->AddRef();
		return S_OK;
	}
	STDMETHOD_( ULONG, AddRef )( THIS )
	{
		return ( ULONG )InterlockedIncrement( &m_lRef );
	}
	STDMETHOD_( ULONG, Release )( THIS )
	{
		if( 0L == InterlockedDecrement( &m_lRef ) )
		{
			delete this;
			return 0L;
		}

		return m_lRef;
	}
	STDMETHOD( SetRenderState )( THIS_ D3DRENDERSTATETYPE d3dRenderState, DWORD dwValue )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetRenderState( d3dRenderState, dwValue );
	}
	STDMETHOD( SetSamplerState )( THIS_ DWORD dwStage, D3DSAMPLERSTATETYPE d3dSamplerState, DWORD dwValue )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetSamplerState( dwStage, d3dSamplerState, dwValue );
	}
	STDMETHOD( SetTextureStageState )( THIS_ DWORD dwStage, D3DTEXTURESTAGESTATETYPE d3dTextureStageState, DWORD dwValue )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetTextureStageState( dwStage, d3dTextureStageState, dwValue );
	}
	STDMETHOD( SetTexture )( THIS_ DWORD dwStage, LPDIRECT3DBASETEXTURE9 pTexture )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetTexture( dwStage, pTexture );
	}
	STDMETHOD( SetVertexShader )( THIS_ LPDIRECT3DVERTEXSHADER9 pShader )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetVertexShader( pShader );
	}
	STDMETHOD( SetPixelShader )( THIS_ LPDIRECT3DPIXELSHADER9 pShader )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetPixelShader( pShader );
	}
	STDMETHOD( SetFVF )( THIS_ DWORD dwFVF )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetFVF( dwFVF );
	}
	STDMETHOD( SetTransform )( THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetTransform( State, pMatrix );
	}
	STDMETHOD( SetMaterial )( THIS_ CONST D3DMATERIAL9 *pMaterial )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetMaterial( pMaterial );
	}
	STDMETHOD( SetLight )( THIS_ DWORD Index, CONST D3DLIGHT9 *pLight )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetLight( Index, pLight );
	}
	STDMETHOD( LightEnable )( THIS_ DWORD Index, BOOL Enable )
	{
		m_nTotalStateChanges++;
		return m_pDevice->LightEnable( Index, Enable );
	}
	STDMETHOD( SetNPatchMode )( THIS_ FLOAT NumSegments )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetNPatchMode( NumSegments );
	}
	STDMETHOD( SetVertexShaderConstantF )( THIS_ UINT RegisterIndex,
		CONST FLOAT *pConstantData,
		UINT RegisterCount )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetVertexShaderConstantF( RegisterIndex,
			pConstantData,
			RegisterCount );
	}
	STDMETHOD( SetVertexShaderConstantI )( THIS_ UINT RegisterIndex,
		CONST INT *pConstantData,
		UINT RegisterCount )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetVertexShaderConstantI( RegisterIndex,
			pConstantData,
			RegisterCount );
	}
	STDMETHOD( SetVertexShaderConstantB )( THIS_ UINT RegisterIndex,
		CONST BOOL *pConstantData,
		UINT RegisterCount )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetVertexShaderConstantB( RegisterIndex,
			pConstantData,
			RegisterCount );
	}
	STDMETHOD( SetPixelShaderConstantF )( THIS_ UINT RegisterIndex,
		CONST FLOAT *pConstantData,
		UINT RegisterCount )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetPixelShaderConstantF( RegisterIndex,
			pConstantData,
			RegisterCount );
	}
	STDMETHOD( SetPixelShaderConstantI )( THIS_ UINT RegisterIndex,
		CONST INT *pConstantData,
		UINT RegisterCount )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetPixelShaderConstantI( RegisterIndex,
			pConstantData,
			RegisterCount );
	}
	STDMETHOD( SetPixelShaderConstantB )( THIS_ UINT RegisterIndex,
		CONST BOOL *pConstantData,
		UINT RegisterCount )
	{
		m_nTotalStateChanges++;
		return m_pDevice->SetPixelShaderConstantB( RegisterIndex,
			pConstantData,
			RegisterCount );
	}
};


//--------------------------------------------------------------------------------------
// Templated class for caching and duplicate state change filtering of paired types
// (such as D3DRENDERSTATETYPE/DWORD value types)
//--------------------------------------------------------------------------------------
template <typename _Kty, typename _Ty> class multicache
{
protected:
	std::map <_Kty, _Ty> cache;         // A map provides a fast look-up of contained states
	// and furthermore ensures that duplicate key values
	// are not present.
	// Additionally, dirtying the cache can be done by
	// clear()ing the container.
public:
	// Command to dirty all cached values
	inline void dirtyall()
	{
		cache.clear();
	}

	// Command to dirty one key value
	inline void dirty( _Kty key )
	{
		std::map <_Kty, _Ty>::iterator it = cache.find( key );
		if( cache.end() != it )
			cache.erase( it );
	}

	// Called to update the cache
	// The return value indicates whether or not the update was a redundant change.
	// A value of 'true' indicates the new state was unique, and must be submitted
	// to the D3D Runtime.
	inline bool set_val( _Kty key, _Ty value )
	{
		std::map <_Kty, _Ty>::iterator it = cache.find( key );
		if( cache.end() == it )
		{
			cache.insert( std::map <_Kty, _Ty>::value_type( key, value ) );
			return true;
		}
		if( it->second == value )
			return false;
		it->second = value;
		return true;
	}
};


#define CACHED_STAGES 288       // The number of stages to cache

class CPureDeviceStateManager :
	public CBaseStateManager
{
protected:
	typedef multicache <D3DSAMPLERSTATETYPE, DWORD> samplerStageCache;
	typedef multicache <D3DTEXTURESTAGESTATETYPE, DWORD> textureStateStageCache;
protected:
	multicache <D3DRENDERSTATETYPE, DWORD> cacheRenderStates;    // cached Render-States
	std::vector <samplerStageCache> vecCacheSamplerStates;       // cached Sampler States
	std::vector <textureStateStageCache> vecCacheTextureStates;       // cached Texture Stage States
	std::vector <IDirect3DBaseTexture9*> vecCacheTexture;				// cached Texture

	UINT m_nFilteredStateChanges;                            // Statistics -- # of redundant
	// states actually filtered
	UINT m_nFilteredStateChangesPerFrame;

public:
	CPureDeviceStateManager( LPDIRECT3DDEVICE9 pDevice )
		: CBaseStateManager( pDevice ),
		cacheRenderStates(),
		vecCacheSamplerStates( CACHED_STAGES ),
		vecCacheTextureStates( CACHED_STAGES ),
		vecCacheTexture( CACHED_STAGES ),
		m_nFilteredStateChanges( 0 ),
		m_nFilteredStateChangesPerFrame( 0 )
	{
	}
	virtual LPCWSTR EndFrameStats()
	{
		// If either the 'total' state changes or the 'filtered' state changes
		// has changed, re-compute the frame statistics string
		if( 0 != ( ( m_nTotalStateChangesPerFrame - m_nTotalStateChanges )
			| ( m_nFilteredStateChangesPerFrame - m_nFilteredStateChanges ) ) )
		{
			swprintf_s( m_wszFrameStats,
				256,
				L"Frame Stats:\nTotal State Changes (Per Frame): %d\nRedundants Filtered (Per Frame): %d",
				m_nTotalStateChanges, m_nFilteredStateChanges );

			m_wszFrameStats[255] = 0;

			m_nTotalStateChangesPerFrame = m_nTotalStateChanges;
			m_nFilteredStateChangesPerFrame = m_nFilteredStateChanges;
		}

		m_nTotalStateChanges = 0;
		m_nFilteredStateChanges = 0;

		return m_wszFrameStats;
	}
	// More targeted 'Dirty' commands may be useful.
	void DirtyCachedValues()
	{
		cacheRenderStates.dirtyall();

		std::vector <samplerStageCache>::iterator it_samplerStages;
		for( it_samplerStages = vecCacheSamplerStates.begin();
			it_samplerStages != vecCacheSamplerStates.end();
			it_samplerStages++ )
			( *it_samplerStages ).dirtyall();

		std::vector <textureStateStageCache>::iterator it_textureStages;
		for( it_textureStages = vecCacheTextureStates.begin();
			it_textureStages != vecCacheTextureStates.end();
			it_textureStages++ )
			( *it_textureStages ).dirtyall();
	}

	// methods inherited from ID3DXEffectStateManager
	STDMETHOD( SetRenderState )( THIS_ D3DRENDERSTATETYPE d3dRenderState, DWORD dwValue )
	{
		m_nTotalStateChanges++;

		// Update the render state cache
		// If the return value is 'true', the command must be forwarded to
		// the D3D Runtime.
		if( cacheRenderStates.set_val( d3dRenderState, dwValue ) )
			return m_pDevice->SetRenderState( d3dRenderState, dwValue );

		m_nFilteredStateChanges++;

		return S_OK;
	}

	STDMETHOD( SetSamplerState )( THIS_ DWORD dwStage, D3DSAMPLERSTATETYPE d3dSamplerState, DWORD dwValue )
	{
		m_nTotalStateChanges++;

		// If this dwStage is not cached, pass the value through and exit.
		// Otherwise, update the sampler state cache and if the return value is 'true', the 
		// command must be forwarded to the D3D Runtime.
		if( dwStage >= CACHED_STAGES || vecCacheSamplerStates[dwStage].set_val( d3dSamplerState, dwValue ) )
			return m_pDevice->SetSamplerState( dwStage, d3dSamplerState, dwValue );

		m_nFilteredStateChanges++;

		return S_OK;
	}

	STDMETHOD( SetTextureStageState )( THIS_ DWORD dwStage, D3DTEXTURESTAGESTATETYPE d3dTextureStageState, DWORD dwValue )
	{
		m_nTotalStateChanges++;

		// If this dwStage is not cached, pass the value through and exit.
		// Otherwise, update the texture stage state cache and if the return value is 'true', the 
		// command must be forwarded to the D3D Runtime.
		if( dwStage >= CACHED_STAGES || vecCacheTextureStates[dwStage].set_val( d3dTextureStageState, dwValue ) )
			return m_pDevice->SetTextureStageState( dwStage, d3dTextureStageState, dwValue );

		m_nFilteredStateChanges++;

		return S_OK;
	}

	STDMETHOD( SetTexture )( THIS_ DWORD dwStage, LPDIRECT3DBASETEXTURE9 pTexture )
	{
		m_nTotalStateChanges++;

		if ( dwStage >= CACHED_STAGES || vecCacheTexture[dwStage] != pTexture)
		{
			vecCacheTexture[dwStage] = pTexture;
			return m_pDevice->SetTexture( dwStage, pTexture );
		}

		m_nFilteredStateChanges++;

		return S_OK;
	}
};


//--------------------------------------------------------------------------------------
// Create an extended ID3DXEffectStateManager instance
//--------------------------------------------------------------------------------------
gkStateManager* gkStateManager::Create( LPDIRECT3DDEVICE9 pDevice )
{
	gkStateManager* pStateManager = NULL;

	D3DDEVICE_CREATION_PARAMETERS cp;
	memset( &cp, 0, sizeof cp );

	if( SUCCEEDED( pDevice->GetCreationParameters( &cp ) ) )
	{
		// A PURE device does not attempt to filter duplicate state changes (with some
		// exceptions) from the driver.  Such duplicate state changes can be expensive
		// on the CPU.  To create the proper state manager, the application determines
		// whether or not it is executing on a PURE device.
		bool bPureDevice = ( cp.BehaviorFlags & D3DCREATE_PUREDEVICE ) != 0;

		//if( bPureDevice )
			pStateManager = new CPureDeviceStateManager( pDevice );
		//else
		//	pStateManager = new CBaseStateManager( pDevice );
	}

	if( NULL == pStateManager )
	{
		//DXUT_ERR_MSGBOX( L"Failed to Create State Manager", E_OUTOFMEMORY );
	}

	return pStateManager;
}
