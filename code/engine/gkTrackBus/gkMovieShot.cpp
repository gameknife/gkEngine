#include "StableHeader.h"

#include "gkMovieShot.h"


gkMovieShot::gkMovieShot(void)
{
}


gkMovieShot::~gkMovieShot(void)
{
}

void gkMovieShot::Update( float elpasedTime )
{
	if ( m_active)
	{
		// 更新每一个node
		AnimNodeList::iterator it = m_nodes.begin();
		for (; it != m_nodes.end(); ++it)
		{
			if (*it)
			{
				(*it)->update( m_context );
			}
		}

		// 使用elapsedTime来更新curr_time
		
		m_context.m_curr_time += elpasedTime * m_context.m_speed_scale * m_secondary_time_scale;

		// 结束判断
		if (m_context.m_curr_time > m_param.m_length)
		{
			m_context.m_curr_time = 0;

			AnimNodeList::iterator it = m_nodes.begin();
			for (; it != m_nodes.end(); ++it)
			{
				if (*it)
				{
					(*it)->end();
				}
			}

			if (!m_param.m_loop)
			{
				m_active = false;
			}
		}
	}
}

void gkMovieShot::AddAnimNode( IMovieActorNode* node )
{
	if (node)
	{
		m_nodes.push_back(node);
	}
}

void gkMovieShot::RemoveAnimNode( IMovieActorNode* node )
{
	if (node)
	{
		AnimNodeList::iterator it = m_nodes.begin();
		for (; it != m_nodes.end(); ++it)
		{
			if (*it == node)
			{
				m_nodes.erase( it );
				break;
			}
		}
	}
}

AnimNodeList& gkMovieShot::GetAnimNodes()
{
	return m_nodes;
}

void gkMovieShot::Play( const SSequenceStartParam& param )
{
	m_context.m_curr_time = param.m_start_time;
	m_context.m_speed_scale = param.m_speed_scale;
	Resume();
}

void gkMovieShot::Pause()
{
	m_secondary_time_scale = 0.f;
}

void gkMovieShot::Resume()
{
	m_secondary_time_scale = 1.f;
}

void gkMovieShot::Active( bool active )
{
	m_active = active;
}

const SSequenceParam& gkMovieShot::GetParam() const
{
	return m_param;
}

void gkMovieShot::SetParam( const SSequenceParam& param )
{
	m_param = param;
}
