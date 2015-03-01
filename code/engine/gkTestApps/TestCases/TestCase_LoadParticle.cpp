#include "TestCaseFramework.h"
#include "TestCase_Template.h"
#include "ICamera.h"
#include "I3DEngine.h"
#include "TestCaseUtil_Cam.h"
#include "IFont.h"

TEST_CASE_FASTIMPL_HEAD( TestCase_LoadParticle, eTcc_Loading )

	IGameObject* m_particle;
	IFtFont* font;

virtual void OnInit() 
{
	ICamera* maincam = gEnv->p3DEngine->getMainCamera();
	maincam->setPosition(0, -20, 2);
	maincam->setOrientation( Quat::CreateRotationXYZ( Ang3( DEG2RAD(-5), 0,0 ) ) );

	m_particle = gEnv->pGameObjSystem->CreateParticleGameObject( _T("objects/particle/default.ptc"), Vec3(0,0,0), Quat::CreateIdentity() );



	font = gEnv->pFont->CreateFont( _T("engine/fonts/msyh.ttf"), 16, GKFONT_OUTLINE_0PX);
	//font->precache();
}

virtual bool OnUpdate() 
{
	gEnv->pFont->DrawString(  _T("gkRendererD3D9\n")
		_T("test中文的测试 空格\n")
		_T("日本語を書く改行する\n")
		_T("都暗红色的近几年了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("阿斯顿飞洒的丰盛的了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("时发生的发生发生地方了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("阿萨德发生的发生大发了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("二品娇嫩人就该了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("的收费的收费狂怒了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("玛丽苏对啊合肥南方了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("摩卡壶水电费约翰今年地方了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("刻录机哦I汗水呢哇额\n了看见没呢人abcdopjpasjnoweorqwer")
		_T("阿萨德发生地方了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("鞥月的那款，了，的萨芬得分了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("在校门口的腰围哦了看见没呢人abcdopjpasjnoweorqwer\n")

		_T("摩卡壶水电费约翰今年地方了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("刻录机哦I汗水呢哇额了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("阿萨德发生地方了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("鞥月的那款，了，的萨芬得分了看见没呢人abcdopjpasjnoweorqwer\n")
		_T("在校门口的腰围哦了看见没呢人abcdopjpasjnoweorqwer\n")
// 
// _T("日本語を書く改行する了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("都暗红色的近几年了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("阿斯顿飞洒的丰盛的了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("时发生的发生发生地方了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("阿萨德发生的发生大发了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("二品娇嫩人就该了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("的收费的收费狂怒了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("玛丽苏对啊合肥南方了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("摩卡壶水电费约翰今年地方了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("刻录机哦I汗水呢哇额了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("阿萨德发生地方了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("鞥月的那款，了，的萨芬得分了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("在校门口的腰围哦了看见没呢人abcdopjpasjnoweorqwer\n")
// // 
// _T("统一印刷的明年发牢骚了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("一年年开始的买了发射了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("亚特阿伯的少年方开始考虑了看见没呢人abcdopjpasjnoweorqwern")
// _T("有本事弄丢了发生辽阔的了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("说的话饭也不能卡迪夫了看见没呢人abcdopjpasjnoweorqwer\n")
// _T("拉素昏头脑卡萨诺改变哈迪斯了看见没呢人abcdopjpasjnoweorqwer\n")
		, font, Vec2(640,360), ColorB(255,255,255,255), eFA_HCenter | eFA_VCenter );


	return true;
}

virtual void OnDestroy() 
{
	gEnv->pGameObjSystem->DestoryGameObject( m_particle );
	gEnv->pSystem->cleanGarbage();
}

virtual void OnInputEvent( const SInputEvent &event ) 
{
	HandleFreeCam( event );
}

TEST_CASE_FASTIMPL_TILE( TestCase_LoadParticle )