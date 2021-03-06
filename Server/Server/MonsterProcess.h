#pragma once
#include "Process.h"

class CMonsterProcess : 
	public CProcess
{
public:
	explicit CMonsterProcess();
	virtual ~CMonsterProcess();

private:
	function<void(USHORT, USHORT)> m_fpMonsterUpdate[AUT_END];
	function<void(USHORT)>	m_fpMonsterByPlayerUpdate;

public:
	void BindMonsterUpdate() {
		m_fpMonsterUpdate[AUT_ATTACK] = [&](USHORT AnimalId, USHORT uiTarget) {
			AttackEvent(AnimalId, uiTarget); };
		m_fpMonsterUpdate[AUT_FOLLOW] = [&](USHORT AnimalId, USHORT uiTarget) {
			FollowEvent(AnimalId, uiTarget); };
		m_fpMonsterUpdate[AUT_EVASION] = [&](USHORT AnimalId, USHORT uiTarget) {
			EvastionEvent(AnimalId, uiTarget); };
		m_fpMonsterUpdate[AUT_IDLE] = [&](USHORT AnimalId, USHORT uiTarget) {
			IdleEvent(AnimalId); };
		m_fpMonsterUpdate[AUT_DIE] = [&](USHORT AnimalId, USHORT uiTarget) {
			DieEvent(AnimalId); };
		m_fpMonsterUpdate[AUT_REMOVE] = [&](USHORT AnimalId, USHORT usTarget) {
			RemoveEvent(AnimalId);};
		m_fpMonsterUpdate[AUT_RESPAWN] = [&](USHORT AnimalId, USHORT usTarget) {
			RespawnEvent(AnimalId);};
	}

public:
	void UpdateMonster(char udpateType, USHORT AnimalId, USHORT TargetId) {
		m_fpMonsterUpdate[udpateType](AnimalId, TargetId);
	}

public:
	void AttackEvent(USHORT AnimalId, USHORT uiTarget);
	void FollowEvent(USHORT AnimalId, USHORT uiTarget);
	void EvastionEvent(USHORT AnimalId, USHORT uiTarget);
	void IdleEvent(USHORT AnimalId);
	void DieEvent(USHORT AnimalId);
	void RemoveEvent(USHORT AnimalId);
	void RespawnEvent(USHORT AnimalId);


public:
	void InRangePlayer(concurrent_unordered_set<USHORT>& cusLogin, concurrent_unordered_set<USHORT>& cusList, USHORT uiMonster);
	USHORT FindTarget(USHORT Animal_Id);
};

