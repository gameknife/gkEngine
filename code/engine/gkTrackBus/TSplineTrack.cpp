#include "StableHeader.h"

#include "TSplineTrack.h"
#include "I2DSpline.h"


CSplineVec3Track::CSplineVec3Track(float default_value )
{
	// clear
	for ( int i=0; i < 3; ++i )
	{
		m_splines[i] = NULL;
	}

	// create
	for ( int i=0; i < 3; ++i )
	{
		m_splines[i] = new I2DSpline<float, BezierBasis>(default_value);
	}
}

CSplineVec3Track::~CSplineVec3Track( void )
{
	for ( int i=0; i < 3; ++i )
	{
		if( m_splines[i] )
		{
			delete m_splines[i];
		}
	}
}

bool CSplineVec3Track::set_key( IKey* key )
{
	assert( typeid(*key) == typeid(ISplineVec3Key) );
	ISplineVec3Key* Vec3key = (ISplineVec3Key*)key;

	if (Vec3key)
	{
		for (int i=0; i < 3; ++i)
		{
			m_splines[i]->set_key( key->get_time(), Vec3key->m_value[i], Vec3key->m_dd[i], Vec3key->m_ds[i] );
		}
	}
	return TBaseTrack<ISplineVec3Key>::set_key( key );
}

void CSplineVec3Track::remove_key( float time )
{
	for (int i=0; i < 3; ++i)
	{
		m_splines[i]->remove_key( time );
	}
}

void CSplineVec3Track::get_value( float time, Vec3& out )
{
	out.x = m_splines[0]->interpole(time);
	out.y = m_splines[1]->interpole(time);
	out.z = m_splines[2]->interpole(time);
}

void CSplineVec3Track::get_value( float time, float& out )
{
	out = m_splines[0]->interpole(time);
}

CSplineTCBTrack::CSplineTCBTrack( Vec3 default_value )
{
	m_splines[0] = new I2DSpline<Vec3, HermitBasis>(default_value);
}

CSplineTCBTrack::~CSplineTCBTrack( void )
{
	delete m_splines[0];
}

bool CSplineTCBTrack::set_key( IKey* key )
{
	assert( typeid(*key) == typeid(ISplineVec3Key) );
	ISplineVec3Key* Vec3key = (ISplineVec3Key*)key;

	if (Vec3key)
	{
		m_splines[0]->set_key( key->get_time(), Vec3key->m_value, Vec3key->m_dd, Vec3key->m_ds );
	}
	return TBaseTrack<ISplineVec3Key>::set_key( key );
}

void CSplineTCBTrack::remove_key( float time )
{
	m_splines[0]->remove_key( time );
}

void CSplineTCBTrack::get_value( float time, Vec3& out )
{
	out = m_splines[0]->interpole(time);
}

void CSplineTCBTrack::get_value( float time, float& out )
{

}
