#include "StableHeader.h"

#include "SplineFloatTrack.h"
#include "I2DSpline.h"


CSplineFloatTrack::CSplineFloatTrack(float default_value)
{
	// clear
	for ( int i=0; i < 1; ++i )
	{
		m_splines[i] = NULL;
	}

	// create
	for ( int i=0; i < 1; ++i )
	{
		m_splines[i] = new I2DSpline<float, BezierBasis>(default_value);
	}
}


CSplineFloatTrack::~CSplineFloatTrack(void)
{
}

bool CSplineFloatTrack::set_key( IKey* key )
{
	assert( typeid(*key) == typeid(ISplineFloatKey) );
	ISplineFloatKey* vec3key = (ISplineFloatKey*)key;

	if (vec3key)
	{
		m_splines[0]->set_key( key->get_time(), vec3key->m_value, vec3key->m_dd, vec3key->m_ds);
	}
	return TBaseTrack<ISplineFloatKey>::set_key( key );
}

void CSplineFloatTrack::remove_key( float time )
{
	for (int i=0; i < 1; ++i)
	{
		m_splines[i]->remove_key( time );
	}
}

void CSplineFloatTrack::get_value( float time, float& out )
{
	out = m_splines[0]->interpole(time);
}
