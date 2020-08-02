#include "stdafx.h"
#include "Layer.h"

#include "SceneMgr.h"
#include "Scene.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
	Safe_Delete_Vector(m_vecParentObj);
}

void CLayer::AddGameObject(CGameObject * _pObject, bool _bMoveAll)
{	
	if (!_pObject->GetParent())
	{
		// �ֻ��� ������Ʈ
		m_vecParentObj.push_back(_pObject);

		// �ٸ� ���̾��� �־�����
		if (-1 != _pObject->GetLayerIdx())
		{
			CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
			pCurScene->GetLayer(_pObject->GetLayerIdx())->RemoveParentObj(_pObject);
		}
	}

	_pObject->SetLayerIdx(m_iLayerIdx);
	
	// �ʺ�켱 Ž��
	list<CGameObject*> queue;

	const vector<CGameObject*>& vecChild = _pObject->GetChild();
	queue.insert(queue.end(), vecChild.begin(), vecChild.end());

	while (!queue.empty())
	{
		CGameObject* pChild = queue.front();
		queue.pop_front();

		const vector<CGameObject*>& vecChild = pChild->GetChild();
		queue.insert(queue.end(), vecChild.begin(), vecChild.end());

		// Layer Index ���� ����
		if (_bMoveAll)
		{
			pChild->SetLayerIdx(m_iLayerIdx);
		}
		else
		{
			if (pChild->GetLayerIdx() == -1)
			{
				pChild->SetLayerIdx(m_iLayerIdx);
			}
		}
	}
}

void CLayer::CheckParentObj()
{
	vector<CGameObject*>::iterator iter = m_vecParentObj.begin();

	for (; iter != m_vecParentObj.end();)
	{
		if ((*iter)->GetParent())
		{
			iter = m_vecParentObj.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CLayer::RemoveParentObj(CGameObject * _pObject)
{
	vector<CGameObject*>::iterator iter = m_vecParentObj.begin();
	for (; iter != m_vecParentObj.end(); ++iter)
	{
		if ((*iter) == _pObject)
		{
			m_vecParentObj.erase(iter);
			return;
		}
	}

	assert(nullptr);
}

void CLayer::RemoveAll()
{
	m_vecLayerObj.clear();
	m_vecParentObj.clear();
}

CGameObject * CLayer::GetMainCamera()
{
	if ( m_iLayerIdx != 0 )
		return nullptr;

	vector<CGameObject*>::iterator iter = m_vecParentObj.begin();
	for ( ; iter != m_vecParentObj.end(); ++iter )
	{
		if ( ( *iter )->GetName() == L"MainCam" )
		{
			return *iter;
		}
	}

	return nullptr;
}

void CLayer::Awake()
{
	for (UINT i = 0; i < m_vecParentObj.size(); ++i)
	{
		if(m_vecParentObj[i]->IsActive())
			m_vecParentObj[i]->Awake();
	}
}

void CLayer::Start()
{
	for (UINT i = 0; i < m_vecParentObj.size(); ++i)
	{
		if (m_vecParentObj[i]->IsActive())
			m_vecParentObj[i]->Start();
	}
}

void CLayer::Update()
{
	for (UINT i = 0; i < m_vecParentObj.size(); ++i)
	{
		if (m_vecParentObj[i]->IsActive())
			m_vecParentObj[i]->Update();
	}	
}

void CLayer::LateUpdate()
{
	for (UINT i = 0; i < m_vecParentObj.size(); ++i)
	{
		if (m_vecParentObj[i]->IsActive())
			m_vecParentObj[i]->LateUpdate();
	}
}

void CLayer::FinalUpdate()
{
	m_vecLayerObj.clear();

	vector<CGameObject*>::iterator iter = m_vecParentObj.begin();

	for (; iter != m_vecParentObj.end();)
	{
		if ((*iter)->IsActive())
		{
			// ��ü��Ͽ� ��� (���� ���� ������Ʈ ���������� �浹ó�� ������ �ϱ� ���ؼ�)
			(*iter)->FinalUpdate();
		}

		(*iter)->RegisterToLayer();

		if ((*iter)->IsDead())
			iter = m_vecParentObj.erase(iter);		
		else		
			++iter;
	}
}

void CLayer::Render()
{
	//for (UINT i = 0; i < m_vecLayerObj.size(); ++i)
	//{
	//	m_vecLayerObj[i]->Render();
	//}
}
