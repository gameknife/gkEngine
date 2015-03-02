#include "StableHeader.h"
#include "gkHavokLoader.h"
#include "IResFile.h"


gkHavokLoader::gkHavokLoader(void)
{
	m_hkLoader = new hkLoader();
}


gkHavokLoader::~gkHavokLoader(void)
{
	delete m_hkLoader;
}

hkRootLevelContainer* gkHavokLoader::load( const char* filename )
{
#ifdef UNICODE
	TCHAR buffer[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, filename, -1, buffer, MAX_PATH );
	gkBindResReader  reader(buffer);
#else
	gkBindResReader  reader(filename);
#endif

	
	return m_hkLoader->load( &reader );
}

hkRootLevelContainer* gkHavokLoader::load( hkStreamReader* reader )
{
	return m_hkLoader->load( reader );
}

gkBindResReader::gkBindResReader( const TCHAR* name )
{
	curpos = 0;
	resfile = gEnv->pFileSystem->loadResFile( name );
}

gkBindResReader::~gkBindResReader()
{
	gEnv->pFileSystem->closeResFile( resfile );
}

hkBool gkBindResReader::isOk() const
{
	if (resfile && resfile->IsOpen())
	{
		return hkBool(true);
	}
	else
	{
		return hkBool(false);
	}	
}

int gkBindResReader::read( void* buf, int nbytes )
{
	if (resfile && resfile->IsOpen())
	{
		if ( curpos + nbytes > resfile->Size() )
		{
			nbytes = resfile->Size() - curpos;
		}
		memcpy( buf, (char*)(resfile->DataPtr()) + curpos, nbytes);
		curpos += nbytes;
	}

	return nbytes;
}

hkResult gkBindResReader::seek( int offset, SeekWhence whence )
{
	//throw std::exception("The method or operation is not implemented.");
    return HK_SUCCESS;
}

int gkBindResReader::tell() const
{
	return curpos;
}

int gkBindResReader::skip( int nbytes )
{
	//throw std::exception("The method or operation is not implemented.");
    return 0;
}

hkBool gkBindResReader::markSupported() const
{
	//throw std::exception("The method or operation is not implemented.");
    return true;
}

hkResult gkBindResReader::setMark( int markLimit )
{
	markpos = curpos;
	return HK_SUCCESS;
}

hkResult gkBindResReader::rewindToMark()
{
	curpos = markpos;
	return HK_SUCCESS;
}

int gkBindResReader::peek(void* buf, int nbytes)
{
	if (resfile && resfile->IsOpen())
	{
		if ( curpos + nbytes > resfile->Size() )
		{
			nbytes = resfile->Size() - curpos;
		}
		memcpy( buf, (char*)(resfile->DataPtr()) + curpos, nbytes);
		//curpos += nbytes;
	}

	return nbytes;
}
