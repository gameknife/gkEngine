//////////////////////////////////////////////////////////////////////////
/*
Copyright (c) 2011-2015 Kaiming Yi
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
	
*/
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	IInputManager.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2011/07/20
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _IInputManager_h_
#define _IInputManager_h_

#include "gkPlatform.h"

	/**
	 @brief 输入状态
	 @remark 
	*/
enum EInputState
{
	eIS_Unknown		= 0,
	eIS_Pressed		= (1 << 0),
	eIS_Released	= (1 << 1),
	eIS_Down			= (1 << 2),
	eIS_Changed		= (1 << 3),
	eIS_UI				= (1 << 4),
};
	/**
	 @brief 修改器状态
	 @remark 
	*/
enum EModifierMask
{
	eMM_None				= 0,
	eMM_LCtrl				= (1 << 0),
	eMM_LShift			= (1 << 1),
	eMM_LAlt				= (1 << 2),
	eMM_LWin				= (1 << 3),
	eMM_RCtrl				= (1 << 4),
	eMM_RShift			= (1 << 5),
	eMM_RAlt				= (1 << 6),
	eMM_RWin				= (1 << 7),
	eMM_NumLock			= (1 << 8),
	eMM_CapsLock		= (1 << 9),
	eMM_ScrollLock	= (1 << 10),

	eMM_Ctrl				= (eMM_LCtrl | eMM_RCtrl),
	eMM_Shift				= (eMM_LShift | eMM_RShift),
	eMM_Alt					= (eMM_LAlt | eMM_RAlt),
	eMM_Win					= (eMM_LWin | eMM_RWin),
	eMM_Modifiers		= (eMM_Ctrl | eMM_Shift | eMM_Alt | eMM_Win),
	eMM_LockKeys		= (eMM_CapsLock | eMM_NumLock | eMM_ScrollLock)
};


#define KI_KEYBOARD_BASE	0
#define KI_MOUSE_BASE			256
#define KI_XINPUT_BASE		512
#define KI_PS3_BASE				1024
#define KI_SYS_BASE				2048

	/**
	 @brief 键值的定义
	 @remark 
	*/
enum EKeyId
{
	eKI_Escape = KI_KEYBOARD_BASE,
	eKI_1,
	eKI_2,
	eKI_3,
	eKI_4,
	eKI_5,
	eKI_6,
	eKI_7,
	eKI_8,
	eKI_9,
	eKI_0,
	eKI_Minus,
	eKI_Equals,
	eKI_Backspace,
	eKI_Tab,
	eKI_Q,
	eKI_W,
	eKI_E,
	eKI_R,
	eKI_T,
	eKI_Y,
	eKI_U,
	eKI_I,
	eKI_O,
	eKI_P,
	eKI_LBracket,
	eKI_RBracket,
	eKI_Enter,
	eKI_LCtrl,
	eKI_A,
	eKI_S,
	eKI_D,
	eKI_F,
	eKI_G,
	eKI_H,
	eKI_J,
	eKI_K,
	eKI_L,
	eKI_Semicolon,
	eKI_Apostrophe,
	eKI_Tilde,
	eKI_LShift,
	eKI_Backslash,
	eKI_Z,
	eKI_X,
	eKI_C,
	eKI_V,
	eKI_B,
	eKI_N,
	eKI_M,
	eKI_Comma,
	eKI_Period,
	eKI_Slash,
	eKI_RShift,
	eKI_NP_Multiply,
	eKI_LAlt,
	eKI_Space,
	eKI_CapsLock,
	eKI_F1,
	eKI_F2,
	eKI_F3,
	eKI_F4,
	eKI_F5,
	eKI_F6,
	eKI_F7,
	eKI_F8,
	eKI_F9,
	eKI_F10,
	eKI_NumLock,
	eKI_ScrollLock,
	eKI_NP_7,
	eKI_NP_8,
	eKI_NP_9,
	eKI_NP_Substract,
	eKI_NP_4,
	eKI_NP_5,
	eKI_NP_6,
	eKI_NP_Add,
	eKI_NP_1,
	eKI_NP_2,
	eKI_NP_3,
	eKI_NP_0,
	eKI_F11,
	eKI_F12,
	eKI_F13,
	eKI_F14,
	eKI_F15,
	eKI_Colon,
	eKI_Underline,
	eKI_NP_Enter,
	eKI_RCtrl,
	eKI_NP_Period,
	eKI_NP_Divide,
	eKI_Print,
	eKI_RAlt,
	eKI_Pause,
	eKI_Home,
	eKI_Up,
	eKI_PgUp,
	eKI_Left,
	eKI_Right,
	eKI_End,
	eKI_Down,
	eKI_PgDn,
	eKI_Insert,
	eKI_Delete,
	eKI_LWin,
	eKI_RWin,
	eKI_Apps,
	eKI_OEM_102,

	// Mouse.
	eKI_Mouse1 = KI_MOUSE_BASE,
	eKI_Mouse2,
	eKI_Mouse3,
	eKI_Mouse4,
	eKI_Mouse5,
	eKI_Mouse6,
	eKI_Mouse7,
	eKI_Mouse8,
	eKI_MouseWheelUp, 
	eKI_MouseWheelDown,
	eKI_MouseX,
	eKI_MouseY,
	eKI_MouseZ,
	eKI_MouseLast,

	// X360 controller.
	eKI_Xbox_DPadUp = KI_XINPUT_BASE,
	eKI_Xbox_DPadDown,
	eKI_Xbox_DPadLeft,
	eKI_Xbox_DPadRight,
	eKI_Xbox_Start,
	eKI_Xbox_Back,
	eKI_Xbox_ThumbL,
	eKI_Xbox_ThumbR,
	eKI_Xbox_ShoulderL,
	eKI_Xbox_ShoulderR,
	eKI_Xbox_A,
	eKI_Xbox_B,
	eKI_Xbox_X,
	eKI_Xbox_Y,
	eKI_Xbox_TriggerL,
	eKI_Xbox_TriggerR,
	eKI_Xbox_ThumbLX,
	eKI_Xbox_ThumbLY,
	eKI_Xbox_ThumbLUp,
	eKI_Xbox_ThumbLDown,
	eKI_Xbox_ThumbLLeft,
	eKI_Xbox_ThumbLRight,
	eKI_Xbox_ThumbRX,
	eKI_Xbox_ThumbRY,
	eKI_Xbox_ThumbRUp,
	eKI_Xbox_ThumbRDown,
	eKI_Xbox_ThumbRLeft,
	eKI_Xbox_ThumbRRight,
	eKI_Xbox_TriggerLBtn,
	eKI_Xbox_TriggerRBtn,

	// PS3 controller.
	eKI_PS3_Select = KI_PS3_BASE,
	eKI_PS3_L3,
	eKI_PS3_R3,
	eKI_PS3_Start,
	eKI_PS3_Up,
	eKI_PS3_Right,
	eKI_PS3_Down,
	eKI_PS3_Left,
	eKI_PS3_L2,
	eKI_PS3_R2,
	eKI_PS3_L1,
	eKI_PS3_R1,
	eKI_PS3_Triangle,
	eKI_PS3_Circle,
	eKI_PS3_Cross,
	eKI_PS3_Square,
	eKI_PS3_StickLX,
	eKI_PS3_StickLY,
	eKI_PS3_StickRX,
	eKI_PS3_StickRY,
	eKI_PS3_RotX,
	eKI_PS3_RotY,
	eKI_PS3_RotZ,
	eKI_PS3_RotX_KeyL,
	eKI_PS3_RotX_KeyR,
	eKI_PS3_RotZ_KeyD,
	eKI_PS3_RotZ_KeyU,

	eKI_Android_Touch,
	eKI_Android_DragX,
	eKI_Android_DragY,
    
    eKI_Gesture_Zoom,
    eKI_Gesture_Pan,
    eKI_Gesture_Rotate,
    eKI_Gesture_DoubleTap,

	eKI_SYS_Commit = KI_SYS_BASE,
	eKI_SYS_ConnectDevice,
	eKI_SYS_DisconnectDevice,

	// Terminator.
	eKI_Unknown		= 0xffffffff,
};

	/**
	 @brief 设备ID定义
	 @remark 
	*/
enum EDeviceId
{
	eDI_Keyboard			= 0,			///! 键盘
	eDI_Mouse				= 1,			///! 鼠标
	eDI_XBox				= 2,			///! XBOX
	eDI_Kinect				= 3,			///! KINECT
	eDI_Android				= 4,			///! 安卓
	eDI_IOS					= 5,			///! IOS
	eDI_Unknown				= 0xff,
};

struct SInputSymbol;

	/**
	 @brief 输入事件
	 @remark 每一次输入会转发一个事件到所有激活的监听器
	*/
struct SInputEvent
{

	EDeviceId		deviceId;		///! 设备ID，该消息由哪个设备发出
	EInputState		state;			///! 输入状态，该消息的状态
	EKeyId			keyId;			///! 键值，输入键
	uint32			timestamp;		// Timestamp of the event, (GetTickCount compatible).
	int				modifiers;	// Key modifiers enabled at the time of this event.
	float			value;		// Value associated with the event.
	float			value2;		// Value associated with the event.
	SInputSymbol*	pSymbol;		// Input symbol the event originated from.
	uint8			deviceIndex;	//Controller index

	SInputEvent()
	{
		deviceId		= eDI_Unknown;
		state			= eIS_Unknown;
		keyId			= eKI_Unknown;
		modifiers		= eMM_None;
		timestamp		= 0;
		value			= 0;
		//keyName   = "";
		pSymbol			= 0;
		deviceIndex		= 0;
	}
};

struct SInputSymbol
{
	// Summary:
	//	 Input symbol types.
	enum EType
	{
		Button,		// 按钮，state=release,down,press value = 0/1
		Toggle,		// 开关
		RawAxis,	// 源轴数据 value = 实际数据
		Axis,		// 轴数据 value = -1.0 ~ 1.0
		Trigger,	// 扳机 value = 0.0 - 1.0
	};

	SInputSymbol(uint32 devSpecId_, EKeyId keyId_,/* const TKeyName& name_,*/ EType type_, uint32 user_ = 0)
		: devSpecId(devSpecId_)
		, keyId(keyId_)//, name(name_)
		, state(eIS_Unknown)
		, type(type_)
		, value(0.0f)
		, value2(0.0f)
		, user(user_)
		, deviceIndex(0)
	{
	}

	void PressEvent(bool pressed)
	{
		if (pressed)
		{
			state = eIS_Pressed;
			value = 1.0f;
		}
		else
		{
			state = eIS_Released;
			value = 0.0f;
		}
	}
	void ChangeEvent(float v)
	{
		state = eIS_Changed;
		value = v;
	}
	void AssignTo(SInputEvent& event, int modifiers=0)
	{
		event.pSymbol = this;
		event.deviceId = deviceId;
		event.modifiers= modifiers;
		event.state		= state;
		event.value		= value;
		event.value2	= value2;
		event.keyId		= keyId;
		//event.deviceIndex = deviceIndex;
	}

	const EKeyId		keyId;			
	const uint32		devSpecId;		
	EInputState			state;			
	const EType			type;			
	float				value;	
	float				value2;	
	uint32				user;				// userdata, 用于modifer的生成
	EDeviceId			deviceId;	
	uint8				deviceIndex; 

};

	/**
	 @brief 输入监听器接口
	 @remark 继承自此接口，并实现onInputEvent函数来实现监听回调。
	*/
struct IInputEventListener
{
	/**
	 @brief 监听回调
	 @return 
	 @param const SInputEvent & event
	 @remark 
	*/
	virtual bool OnInputEvent( const SInputEvent &event ) = 0;
	virtual bool OnInputEventUI( const SInputEvent &event ) {	return false;	}
};


#ifdef OS_ANDROID
typedef int32_t (*Android_InputHandler)(struct android_app* app, struct AInputEvent* event);
#endif

struct IInputDevice
{
	// Summary:
	//	 Implements virtual destructor just for safety.
	virtual ~IInputDevice(){}

	virtual const TCHAR* GetDeviceName() const	= 0;
	virtual EDeviceId GetDeviceId() const	= 0;
	// Initialization.
	virtual bool	Init() = 0;
	virtual void	PostInit() = 0;
	// Update.
	virtual void	Update(bool bFocus) = 0;
	// Summary:
	//	 Checks for key pressed and held.
	//virtual bool	InputState(const TKeyName& key, EInputState state) = 0;
	// Summary:
	//	 Sets/unsets DirectInput to exclusive mode.
	virtual bool	SetExclusiveMode(bool value) = 0;
	// Summary:
	//	 Clears the key (pressed) state.
	virtual void	ClearKeyState() = 0;
	// Summary:
	//	 Clears analog position state.
	virtual void	ClearAnalogKeyState() = 0;

	virtual SInputSymbol* LookupSymbol(EKeyId id) const = 0;
	//virtual bool IsOfDeviceType( EInputDeviceType type ) const = 0;
	virtual void Enable(bool enable) = 0;
	virtual bool IsEnabled() const = 0;

	virtual void SetHwnd(HWND hwnd) {}

	virtual const char* GetKeyName(const SInputEvent& event, bool bGUI=0) =0;

#ifdef OS_ANDROID	
	virtual Android_InputHandler getAndroidHandler() =0;
#endif
};


	/**
	 @ingroup CoreModuleAPI
	 @brief 输入管理模块
	 @remark 输入管理模块是所有输入设备的管理器，其内封装各平台的输入设备实现

	 @sa IInputDevice 输入设备抽象对象
	*/
struct UNIQUE_IFACE IInputManager
{
public:
	virtual ~IInputManager(void) {}

	/**
	 @brief 初始化输入模块
	 @return 
	 @param HWND hDlg
	 @remark 
	*/
	virtual HRESULT Init( HWND hDlg ) = 0;
	/**
	 @brief 销毁输入模块
	 @return 
	 @remark 
	*/
	virtual void Destroy() = 0;


	/**
	 @brief 每帧更新接口
	 @return 
	 @param float fElapsedTime
	 @remark 
	*/
	virtual void Update(float fElapsedTime) = 0;

	/**
	 @brief 设置输入模块的WINDOWS句柄
	 @return 
	 @param HWND hWnd
	 @remark 仅针对WNDOWS平台
	*/
	virtual void setHWND(HWND hWnd) = 0;


	/**
	 @brief 添加输入监听器
	 @return 
	 @param IInputEventListener * listener
	 @remark 
	*/
	virtual void addInputEventListener(IInputEventListener* listener) =0;
	/**
	 @brief 移除输入监听器
	 @return 
	 @param IInputEventListener * listener
	 @remark 
	*/
	virtual void removeEventListener( IInputEventListener* listener ) = 0;

	/**
	 @brief 设置为独占监听器
	 @return 
	 @param IInputEventListener * pListener
	 @remark 
	*/
	virtual void setExclusiveListener( IInputEventListener *pListener ) = 0;
	/**
	 @brief 取得当前的独占监听器
	 @return 如果不存在，返回NULL
	 @remark 
	*/
	virtual IInputEventListener *getExclusiveListener() = 0;

	/**
	 @brief 通过event取得键名
	 @return 
	 @param const SInputEvent & event
	 @param bool bGUI
	 @remark 
	*/
	virtual const char* GetKeyName(const SInputEvent& event, bool bGUI=0) =0;
    
    virtual void PostInputEvent( const SInputEvent &event, bool bForce = false) =0;

#ifdef OS_ANDROID	
	virtual Android_InputHandler getAndroidHandler() =0;
#endif

};

#endif