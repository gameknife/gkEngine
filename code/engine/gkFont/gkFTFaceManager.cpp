#include "gkFTFaceManager.h"
#include "ISystem.h"
#include "IResFile.h"


gkFTFaceManager::gkFTFaceManager(void)
{
}


gkFTFaceManager::~gkFTFaceManager(void)
{
}

gkFTFace* gkFTFaceManager::OpenFace( const TCHAR* filename )
{
	IResFile* pFile = gEnv->pFileSystem->loadResFile( filename );

	int error = 0;

	if (!pFile)
	{
		return NULL;
	}

	gkFTFaceMap::iterator it = m_facemap.find( pFile );
	if(it != m_facemap.end())
	{
		return &(it->second);
	}

	gkFTFace face;
	face.m_memFile = pFile;

	// load font file
	error = FT_New_Memory_Face( getFontMain()->getFTLib(), (FT_Byte*)(pFile->DataPtr()), pFile->Size(), 0, &(face.m_face) );
	error = FT_Select_Charmap(face.m_face,FT_ENCODING_UNICODE);

	// build pixel info
	if (1)
	{
		//float lean=0.35f;
		FT_Matrix matrix;
		matrix.xx=0x10000L;
		{
			matrix.xy=0;
		}

		matrix.yx = 0;
		matrix.yy = 0x10000L;
		FT_Set_Transform(face.m_face,&matrix,0);
	}

	m_facemap.insert( gkFTFaceMap::value_type(pFile, face) );

	return OpenFace(filename);
}
