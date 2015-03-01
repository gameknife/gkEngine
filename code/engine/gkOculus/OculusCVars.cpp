#include "gkPlatform.h"
#include "OculusCVars.h"
#include "ISystem.h"

//gkOculusCVars* g_pOculusCVars=0;

gkOculusCVars::gkOculusCVars()
{
	REGISTER_CVARINT_DESC(r_aux, 
		_T("Toggles Aux Helper Rendering.\n")
		_T("Usage: r_aux [0/1]\n")
		_T("Default is 1 (on). Set to 0 to diable aux rendering."), 1);

	REGISTER_CVARFLOAT_DESC( r_SSAOScale,
		_T("SSAO对比度.\n")
		_T("Usage: r_SSAOConstract [0.0 - 2.0]\n")
		_T("默认为1.0"), 0.1f, 10.0f, 1.0f);
}

gkOculusCVars::~gkOculusCVars()
{

}