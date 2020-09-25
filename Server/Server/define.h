#pragma once

#define	SINGLE(type)\
		private:\
			type();\
			~type();\
		public:\
			static	type*	GetInst()\
			{\
					static type mgr;\
					return &mgr;\
			}\

//#define		DT	CTimeMgr::GetInst()->GetDeltaTime()
#define		SAFE_DELETE(p)	if(nullptr != p) p->Release()
#define		SAFE_RELEASE(p)	if(nullptr != p) delete p; p = nullptr;

typedef DirectX::SimpleMath::Vector2 Vec2;
typedef DirectX::SimpleMath::Vector3 Vec3;
typedef DirectX::SimpleMath::Vector4 Vec4;

//#define DB_ON
//#define ANIMAL_TEST

