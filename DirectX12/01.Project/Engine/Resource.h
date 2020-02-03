#pragma once

#include "Entity.h"

class CResource : 
	public CEntity
{
private:
	template<typename T>
	friend class Ptr;

public:
	CResource( RES_TYPE  eType );
	CResource( const CResource& res );
	virtual ~CResource();

private:
	wstring		m_strPath;	// �����
	RES_TYPE	m_eType;

	int			m_iRefCount;
	UINT		m_iCloneCount;		// ���ҽ� ���� Ƚ��

public:
	RES_TYPE GetType();
	UINT	GetCloneCount();

	void SetPath( const wstring& strPath );
	const wstring& GetPath();

public:
	void AddRef();
	void SubRef();

public:
	virtual void Load( const wstring& strFullPath );
	virtual void Save( const wstring& strFullPath );
	void SaveToScene( FILE* pFile );

public:
	virtual CResource* Clone();

	template<typename T>
	bool LoadFromScene( FILE* pFile );
};

template<typename T>
inline bool CResource::LoadFromScene( FILE * pFile )
{
	// Key
	wstring strName = GET_SINGLE( CPathManager )->LoadWString( pFile );

	// Path
	m_strPath = GET_SINGLE( CPathManager )->LoadWString( pFile );

	// ���ҽ��� �̹� �ε��Ǿ��ִ��� üũ
	Ptr<T> pRes = GET_SINGLE( CResourceManager )->FineRes<T>( strName );
	if ( NULL != pRes )
		return false;

	SetName( strName );

	wstring strPath = GET_SINGLE( CPathManager )->GetResPath();
	strPath += m_strPath;
	Load( strPath );
}
