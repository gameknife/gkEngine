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
// Name:   	ISystem.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2011/07/20
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _ISystem_h_
#define _ISystem_h_

/** 
	@defgroup CoreModuleAPI 核心模块定义
	@brief 核心模块接口
	@remark 
*/

#include "gkPlatform.h"
#include "ILog.h"
#include "MathLib/gk_Vector2.h"
#include "MathLib/gk_Color.h"

struct ISystem;
struct IInputManager;
struct INetworkLayer;
struct IRenderer;
struct I3DEngine;
class IResourceManager;
struct ITimer;
struct ISystemProfiler;
struct IPhysics;
struct IGameObjectSystem;
struct IGameFramework;
struct IResFileManager;
struct IAnimation;
struct IMeshLoader;
struct IInGUI;
struct IFileChangeMonitor;
struct ITaskDispatcher;
struct IFontModule;
struct ITrackBus;

struct ISound;
struct IVideo;
struct IStereoDevice;

#define GK_CVARTYPE_FLOAT 0
#define GK_CVARTYPE_INT 1

struct gkCVar
{
	#ifndef _PUREGAME
	gkCVar(byte type, void* ptr, const gkStdString& desc, double fMin, double fMax) : m_type(type), m_ptr(ptr), m_wstrDesc(desc), m_dMin(fMin), m_dMax(fMax) {}
	#else
	gkCVar(byte type, void* ptr) : m_type(type), m_ptr(ptr) {}
	#endif
	~gkCVar() {}
	uint8 m_type;
	void* m_ptr;

	int getInt() {return *((int*)(m_ptr));}
	float getFloat() {return (float)(*((double*)(m_ptr)));}

	#ifndef _PUREGAME
	gkStdString m_wstrDesc;
	double m_dMin;
	double m_dMax;
	#endif
};

typedef std::map<gkStdString, gkCVar>  CVarNameMap;


#define REGISTER_CVAR(_name,_type)	(gEnv->pCVManager->registerVar(_T(#_name) , _type , &_name))
#define REGISTER_CVARFLOAT(_name)	(gEnv->pCVManager->registerVar(_T(#_name) , 0 , &_name))
#define REGISTER_CVARINT(_name)	(gEnv->pCVManager->registerVar(_T(#_name) , 1 , &_name))

#define REGISTER_CVARFLOAT_DESC(_name, _desc, _min, _max, _default)	(gEnv->pCVManager->registerVar(_T(#_name), 0 , &_name, _desc, _min, _max, _default))
#define REGISTER_CVARINT_DESC(_name, _desc, _default)	(gEnv->pCVManager->registerVar(_T(#_name), 1 , &_name, _desc, 0, 100, _default))

struct IConsoleVariablesManager
{
	gkStdString m_wstrPath;

	virtual void reloadAllParameters() =0;
	virtual void saveAllParameters() =0;

	virtual void PrintLine(const TCHAR *s)=0;

	virtual void PrintLinePlus(const TCHAR *s)=0;

	virtual CVarNameMap& getCVarMap()=0;
	virtual gkCVar* getCVar(const gkStdString& name)=0;

	virtual void render() =0;

	virtual bool isConsoleEnable() =0;

	virtual void executeCommand(const TCHAR* wszCommandline) =0;
	virtual void registerVar( const gkStdString& name, uint8 type, void* var, const gkStdString& desc = _T(""), double fMin = 0.0, double fMax = 100.0, double defvalue = 0.0) =0;
};

// Summary:
//	 Simple logs of data with low verbosity.
void gkLogMessage(const TCHAR *, ...);
inline void gkLogMessage( const TCHAR *format,... )
{
	// Fran: we need these guards for the testing framework to work
	if (gEnv && gEnv->pSystem)		
	{
		va_list args;
		va_start(args,format);
		gEnv->pLog->LogV(ILog::eMessage, format,args );
		va_end(args);
	}
}

void gkLogInput(const TCHAR *, ...);
inline void gkLogInput( const TCHAR *format,... )
{
	// Fran: we need these guards for the testing framework to work
	if (gEnv && gEnv->pSystem)		
	{
		va_list args;
		va_start(args,format);
		gEnv->pLog->LogV(ILog::eInput, format,args );
		va_end(args);
	}
}


void gkLogWarning(const TCHAR *, ...);
inline void gkLogWarning( const TCHAR *format,... )
{
	// Fran: we need these guards for the testing framework to work
	if (gEnv && gEnv->pSystem)		
	{
		va_list args;
		va_start(args,format);
		gEnv->pLog->LogV(ILog::eWarning, format, args );
		va_end(args);
	}
}

void gkLogError(const TCHAR *, ...);
inline void gkLogError( const TCHAR *format,... )
{
	// Fran: we need these guards for the testing framework to work
	if (gEnv && gEnv->pSystem)		
	{
		va_list args;
		va_start(args,format);
		gEnv->pLog->LogV( ILog::eError, format, args );
		va_end(args);
	}
}

struct IProgressCallback
{
	virtual void OnUpdateProgress( int nProgress ) = 0;
	virtual void OnInitProgress( const TCHAR* progressText ) =0;
};

struct android_app;

struct ISystemInitInfo
{
	HINSTANCE hInstance;
	IProgressCallback* pProgressCallBack;
	int fWidth;
	int fHeight;
    void* appPtr;
	const char* rootDir;
    
#ifdef OS_ANDROID
	void* nativeWindowPTR;
	android_app* android_states;
#endif

	ISystemInitInfo(){
		hInstance = NULL;
		pProgressCallBack = NULL;
		fWidth = 0;
		fHeight = 0;
        appPtr = NULL;
		rootDir = NULL;
		#ifdef OS_ANDROID
		nativeWindowPTR = 0;
		android_states = 0;
		#endif
	}
};

	/**
	 @ingroup CoreModuleAPI
	 @brief 游戏系统模块
	 @remark 整个游戏系统的管理对象
				\n负责游戏系统的 建立，更新，摧毁
				\n负责各个核心模块的加载，错误管理，更新，摧毁
	*/
struct UNIQUE_IFACE ISystem
{
public:

	virtual ~ISystem(void) {}

	// System Management

	/**
	 @brief 系统的初始化接口
	 @return 
	 @param ISystemInitInfo & sii 初始化参数集
	 @remark 
	*/
	virtual bool Init(ISystemInitInfo& sii) =0;
	/**
	 @brief 
	 @return 后初始化，传入hwnd
	 @param HWND hWnd
	 @param ISystemInitInfo & sii
	 @remark 如果由外部创建窗口句柄（编辑器/MAX插件集成等），需要由这里传入，否则输入设备会发生紊乱
			 客户端系统，hwnd传空即可，引擎会创建好窗口。
	*/
	virtual bool PostInit(HWND hWnd, ISystemInitInfo& sii) =0;
	/**
	 @brief 摧毁系统
	 @return 
	 @remark 
	*/
	virtual bool Destroy() =0;
	/**
	 @brief 系统更新
	 @return 
	 @remark 每帧调用一次，和PostUpdate配对
			 \n在这里处理渲染器的启动
	*/
	virtual bool Update() =0;
	/**
	 @brief 系统后更新
	 @return 
	 @remark 每帧调用一次，和Update配对
			 \n在这里处理渲染器多线程等待或present
	*/
	virtual bool PostUpdate() =0;

	/**
	 @brief 取得全局环境gEnv
	 @return 
	 @remark 
	*/
	virtual SSystemGlobalEnvironment* getENV() =0;

	/**
	 @brief 设置编辑器的窗口句柄
	 @return 
	 @param HWND drawHwnd 绘制的窗口句柄
	 @param HWND mainHwnd 采集输入消息的窗口句柄
	 @remark 
	*/
	virtual void SetEditorHWND( HWND drawHwnd, HWND mainHwnd) =0;
	/**
	 @brief 是否为编辑器模式
	 @return 
	 @remark 
	*/
	virtual bool IsEditor() =0;

	/**
	 @brief 线程安全的加载场景
	 @return 
	 @param const TCHAR * filename
	 @param IProgressCallback * pCallback
	 @remark 此接口调用后，系统会在下一次更新前的线程保护时间进行场景加载
	*/
	virtual bool LoadScene_s(const TCHAR* filename, IProgressCallback* pCallback) =0;
	/**
	 @brief 存储场景到指定文件
	 @return 
	 @param const TCHAR * filename
	 @remark 
	*/
	virtual bool SaveScene(const TCHAR* filename) =0;
	/**
	 @brief 判断场景是否已经加载
	 @return 
	 @remark 
	*/
	virtual bool IsSceneEmtpy() =0;

	/**
	 @brief 线程安全的重载资源
	 @return 
	 @param uint8 type
	 @param const gkStdString & name
	 @remark 系统会将重载的资源加入重载队列，在下一次更新的线程保护期重新加载
	*/
	virtual bool reloadResource_s(uint8 type, const gkStdString& name) =0;
	/**
	 @brief 更新进度
	 @return 
	 @param int progress
	 @remark 通知进度更新，调用loadingProgressCallback来通知外部进度
	*/
	virtual void updateProgress(int progress) =0;

	/**
	 @brief 清理垃圾
	 @return 
	 @remark 清理资源管理器中引用计数为3的垃圾，因为引用计数为3时，不在有任何外部对象持有资源
	*/
	virtual void cleanGarbage() =0;

	// Resource Management
	// 资源管理
	/**
	 @brief 获取纹理资源管理器
	 @return 
	 @remark 
	*/
	virtual IResourceManager* getTextureMngPtr() =0;
	/**
	 @brief 获取材质资源管理器
	 @return 
	 @remark 
	*/
	virtual IResourceManager* getMaterialMngPtr() =0;
	/**
	 @brief 获取模型资源管理器
	 @return 
	 @remark 
	*/
	virtual IResourceManager* getMeshMngPtr() =0;
	/**
	 @brief 获取shader资源管理器
	 @return 
	 @remark 
	*/
	virtual IResourceManager* getShaderMngPtr() =0;

	/**
	 @brief 获取一个新的obj/gmf模型加载器
	 @return 
	 @remark FIXME
	*/
	virtual IMeshLoader* getOBJMeshLoader() =0;
	/**
	 @brief 获取一个新的hkx模型加载器
	 @return 
	 @remark FIXME
	*/
	virtual IMeshLoader* getHKXMeshLoader() =0;

	virtual void ReturnMeshLoader(IMeshLoader* target) =0;

	virtual HWND getHWnd() =0;

};

struct IFtFont;

	/**
	 @brief 内部GUI系统接口
	 @return 
	 @remark 提供简单的GUI功能，实现profiler等交互功能
	*/
struct IInGUI
{
	virtual ~IInGUI() {}
	virtual uint32 gkGUITab(const TCHAR* title, const Vec2& pos, int width, int height, const ColorB& textColor, const ColorB& bgColor,int selection = 0, IFtFont* font = NULL) =0;
	virtual bool gkGUIButton(const TCHAR* title, const Vec2& pos, int width, int height, const ColorB& textColor, const ColorB& bgColor, IFtFont* font = NULL) =0;
	virtual void gkPopupMsg(const TCHAR* title) =0;
};

	/**
	 @brief 文件修改监听器
	 @return 
	 @remark 集成此接口以获取文件修改回调
	*/
struct IFileChangeMonitorListener
{
	virtual void OnFileChange(const TCHAR* filename) =0;
};
	/**
	 @brief 文件修改监视器
	 @return 
	 @remark 向监视器添加/删除监听器以获取和取消对文件修改的监听
	*/
struct IFileChangeMonitor
{
	virtual void AddListener( IFileChangeMonitorListener* listener ) =0;
	virtual void RemoveListener( IFileChangeMonitorListener* listener ) =0;
};

#endif