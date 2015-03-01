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
// Name:   	ITrackBus.h
// Desc:		TrackBus的外部接口
// 	
// 
// Author:  YiKaiming
// Date:	2013/9/3
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _ITrackBus_h_
#define _ITrackBus_h_

#include "gkPlatform.h"

struct IMovieShot;
struct IMovieActorNode;

	/**
	 @brief 轨道类型
	 @remark 
	*/
enum EAnimTrackType
{
	eATT_PositionTrack = 0,
	eATT_RotationTrack,
	eATT_ScalingTrack,
	eATT_CamTargetTrack,
	eATT_SoundTrack,
	eATT_DirectorTrack,
	eATT_VideoTrack,

	eATT_CamFovTrack,

	eATT_CharacterTrack,

	eATT_Max,
};

enum EAnimNodeType
{
	eANT_CamNode,
	eANT_DirectorNode,
	eANT_EntityNode,
	eANT_SoundNode,
};


	/**
	 @brief 影片的运行时上下文
	 @remark 
	*/
struct SMovieContext
{
	float m_curr_time;
	float m_speed_scale;
};
	/**
	 @brief 镜头的开始参数
	 @remark 
	*/
struct SSequenceStartParam
{
	float m_start_time;
	float m_speed_scale;

	SSequenceStartParam()
	{
		m_start_time = 0.f;
		m_speed_scale = 1.f;
	}
};
	/**
	 @brief 镜头参数
	 @remark 
	*/
struct SSequenceParam
{
	float m_length;
	float m_frame_per_second;
	bool m_loop;

	SSequenceParam()
	{
		m_length = 10.f; // 10 seconds
		m_frame_per_second = 30.f; // NTSC fps
		m_loop = false;
	}
};

struct IKey;
typedef std::vector<IKey*> KeyArray;

struct IKey
{
	IKey(float time):m_time(time){}
	virtual ~IKey() {}

	float get_time()
	{
		return m_time;
	}
// 	CVrpProperty& get_property(const tstring& name)
// 	{
// 		PropertyMap::iterator it = m_properties.find( name );
// 		if (it != m_properties.end())
// 		{
// 			return it->second;
// 		}
// 
// 		static CVrpProperty notfind;
// 		return notfind;
// 	}
// 
// 	template<class T>
// 	void add_property(const tstring& name, T& data)
// 	{
// 		m_properties.insert( PropertyMap::value_type( name, CVrpProperty(name.c_str(), data) ) );
// 	}

	float m_time;
	//PropertyMap m_properties;
};

struct IMovieTrack;
typedef std::vector<IMovieTrack*> TrackList;
struct IMovieTrack
{
	virtual ~IMovieTrack() {}

	/**
	 @brief 在time处查找或打上一个关键帧
	 @return 返回改出的关键帧
	 @param float time
	 @remark 在操作内部建立关键帧
	*/
	virtual bool set_key( IKey* key ) =0;

	/**
	 @brief 移除在time处的关键帧
	 @return 
	 @param float time
	 @remark 
	*/
	virtual void remove_key( float time ) =0;

	/**
	 @brief 取得关键帧列表
	 @return 
	 @remark 
	*/
	virtual KeyArray& get_keylist() =0;

	/**
	 @brief 从time处取得插值后的VEC3
	 @return 
	 @param float time
	 @param VEC3 & out
	 @remark 
	*/
	virtual void get_value( float time, Vec3& out ) {}


	virtual void get_key_info( float time, const TCHAR*& strings, float& time_duration  ) {}
	/**
	 @brief 查找在time之前的一个关键帧
	 @return 
	 @param float time
	 @remark 
	*/
	//virtual IKey* search_prev_key(float time) =0;


};

struct IMovieActorNode;
typedef std::list<IMovieActorNode*> AnimNodeList;
struct IMovieActorNode
{
	virtual ~IMovieActorNode() {}

	virtual void create_default_track() =0;
	/**
	 @brief 节点更新
	 @return 
	 @param const SAnimateContext & context
	 @remark 
	*/
	virtual void update( const SMovieContext& context ) {}

	virtual void end() {}
	/**
	 @brief 手动添加一条轨道
	 @return 
	 @param ITrack * track
	 @remark 
	*/
	virtual IMovieTrack* add_track( EAnimTrackType type ) =0;

	/**
	 @brief 手动移除一条轨道
	 @return 
	 @param ITrack * track
	 @remark 
	*/
	virtual void remove_track( IMovieTrack* track ) =0;

	/**
	 @brief 取得所有轨道
	 @return TrackList
	 @remark 
	*/
	virtual TrackList& get_tracklist() =0;


	//////////////////////////////////////////////////////////////////////////
	// 对于空间节点的快速轨道存取接口，不一定存在

	virtual IMovieTrack* get_track(EAnimTrackType type) =0;
};

	/**
	 @brief TrackBus内的一个镜头
	 @remark 
	*/
struct IMovieShot
{
		/**
	 @brief 镜头更新
	 @return 
	 @param float elpasedTime 帧时间
	 @remark 
	*/
	virtual void Update( float elpasedTime ) =0;

	/**
	 @brief 添加节点
	 @return 
	 @param IAnimNode * node
	 @remark 
	*/
	virtual void AddAnimNode( IMovieActorNode* node ) =0;

	/**
	 @brief 移除节点
	 @return 
	 @param IAnimNode * node
	 @remark 
	*/
	virtual void RemoveAnimNode( IMovieActorNode* node ) =0;

	/**
	 @brief 取得所有节点
	 @return 
	 @remark 
	*/
	virtual AnimNodeList& GetAnimNodes() =0;

	/**
	 @brief 利用startparam的定义，强制的播放镜头
	 @return 
	 @param const SSequenceStartParam & param
	 @remark 
	*/
	virtual void Play(const SSequenceStartParam& param) =0;

	/**
	 @brief 暂停镜头（镜头逻辑依然存在，只是时间流逝为0），要完全终止，请使用active
	 @return 
	 @remark 
	*/
	virtual void Pause() =0;

	/**
	 @brief 继续镜头
	 @return 
	 @remark 
	*/
	virtual void Resume() =0;

	/**
	 @brief 激活/反激活镜头，完全终止镜头逻辑
	 @return 
	 @param bool active
	 @remark 
	*/
	virtual void Active( bool active ) =0;


	/**
	 @brief 取得实时播放参数SAnimateContext
	 @return SAnimateContext
	 @remark 
	*/
	//virtual const SAnimateContext& get_realtime_param() const =0;

	/**
	 @brief 取得镜头参数
	 @return 
	 @remark 
	*/
	virtual const SSequenceParam& GetParam() const=0;

	/**
	 @brief 设置镜头参数
	 @return 
	 @param const SSequenceParam & param
	 @remark 
	*/
	virtual void SetParam( const SSequenceParam& param ) =0;



// 	virtual void TimeTo(SMovieContext& context) =0;
// 
// 	virtual void AddActorNode(IMovieActorNode* node) =0;
// 
// 	virtual void RemoveActorNode(IMovieActorNode* node) =0;
};



typedef std::map<gkStdString, IMovieShot*> MovieShotMap;
	/**
	 @brief TrackBus的模块接口
	 @remark 
	*/
struct ITrackBus
{
	/**
	 @brief 
	 @return 
	 @param float frameTime
	 @remark 
	*/
	virtual void Update(float frameTime) =0;

	virtual IMovieShot* CreateMovieShot(const gkStdString& name) =0;

	virtual IMovieShot* GetMovieShot( const gkStdString& name ) =0;

	virtual MovieShotMap& GetMovieShots() =0;

	virtual IMovieActorNode* CreateActor(EAnimNodeType type, void* param1 = 0, void* param2 = 0) =0;

};



struct ISoundKey : public IKey
{
	ISoundKey(float time):IKey(time)
	{
	}
	gkStdString m_filename;
};

struct ISplineVec3Key : public IKey
{
	ISplineVec3Key(float time):IKey(time)
	{

	}
	Vec3 m_value;
	Vec3 m_dd;
	Vec3 m_ds;
};

struct ISplineFloatKey : public IKey
{
	ISplineFloatKey(float time):IKey(time)
	{

	}
	float m_value;
	float m_dd;
	float m_ds;
};

struct IDirectorKey : public IKey
{
	IDirectorKey( float time) :IKey(time)
	{
		m_target_cam_name = _T("none");
	}

	gkStdString m_target_cam_name;
};

struct IVideoKey : public IKey
{
	IVideoKey( float time):IKey(time)
	{

	}
	gkStdString m_filename;
	Vec4 m_screen_coord;
};

struct ICharacterKey : public IKey
{
	ICharacterKey( float time):IKey(time)
	{

	}
	gkStdString m_animname;
	float m_speedScale;
	float m_duration;
};


#endif
