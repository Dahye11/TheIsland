#pragma once
#include <Engine/Script.h>
#define INVINCIBLE_TIME 3.f

class CStatusScript :
	public CScript
{
	float m_fHealth;
	float m_fArmor;
	float m_fHungry;
	float m_fThirst;

	bool m_bGameOver;

	bool m_bInvincible = false;

	CGameObject* m_pArmor;

	CGameObject* m_pScreenDamage;
public:
	CStatusScript();
	virtual ~CStatusScript();

public:
	virtual void Update();

public:
	CLONE(CStatusScript);

public:
	bool Damage(float fDamage, bool bTrueDamage = false);

	void SetIncreaseHealth(float fAmount);
	void SetIncreaseHungry(float fAmount);
	void SetIncreasefThirst(float fAmount);

	void SetHealth(float fHealth);
	void SetHungry(float fHungry);
	void SetThirst(float fThirst);

	void SetScreenDamage(CGameObject* pObject);

	bool GetGameOver();

	void EquipArmor(CGameObject* pArmor, float fArmor);

	void Invincible(bool bOn = true);
};

