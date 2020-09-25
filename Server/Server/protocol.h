#pragma once

/* ----------------------------------------
[NETWORK]
-----------------------------------------*/
constexpr	int	SERVER_PORT = 9000;
constexpr	int	MAX_USER = 100;
constexpr	int	NO_TARGET = 9999;

constexpr	int	BEGIN_ANIMAL = MAX_USER;
constexpr	int	ANIMAL_BEAR = 10;
constexpr	int	ANIMAL_BOAR = 5;
constexpr	int	ANIMAL_DEER = 10;
constexpr	int	ANIMAL_WOLF = 5;
constexpr	int	MAX_ANIMAL = ANIMAL_BEAR + ANIMAL_BOAR + ANIMAL_DEER + ANIMAL_WOLF;
constexpr	int	END_ANIMAL = BEGIN_ANIMAL + MAX_ANIMAL;

constexpr	int	BEGIN_NATURAL = END_ANIMAL;
constexpr	int	MAX_NATURAL = 306;
constexpr	int	END_NATURAL = BEGIN_NATURAL + MAX_NATURAL;

constexpr	int	BEGIN_HOUSE = END_NATURAL;

constexpr	int	MAX_BUF = 255;
constexpr	int	MAX_STR_LEN = 15;
constexpr	int MAX_MSG_LEN = 50;
constexpr	int MAX_QUICK_SLOT = 5;

constexpr	float	Warlike_Spawn_radius = 2000.f;
constexpr	float	Passive_Spawn_radius = 2000.f;
constexpr	float	Evasion_Spawn_radius = 8000.f;

//char SERVERIP[20] = "127.0.0.1";

// Client -> Server Packet Protocol
enum CS_PACKET_TYPE {
	CS_LOGIN,
	CS_MOVE,
	CS_CHAT,
	CS_LOGOUT,
	CS_POS,
	CS_ROT,
	CS_COLLISION,
	CS_HOUSING_INSTALL,
	CS_HOUSING_REMOVE,
	CS_HOUSING_UPGRADE,
	CS_ATTACK,
	CS_ANIMATION,
	CS_GET_ITEM,
	CS_REMOVE_ITEM,
	CS_USE_ITEM,
	CS_EQUIP_ARMOR,
	CS_DESTROY_ARMOR,
	CS_END
};

// [ Server -> Client Packet Protocol ]
enum SC_PACKET_TYPE {
	// - Common
	SC_POS,
	SC_ROT,
	SC_REMOVE,
	SC_ANIMATION,
	// - Login
	SC_LOGIN_OK,
	SC_LOGIN_FAIL,
	SC_FULL_SERVER,
	SC_DISCONNECT_SERVER,
	// - Player
	SC_STATUS_PLAYER,
	SC_CHAT,
	SC_DEATH_PLAYER,
	// - Natural
	SC_PUT_NATURAL,
	SC_DESTROY_NATURAL,
	// - Housing
	SC_INSTALL_HOUSE,
	SC_REMOVE_HOUSE,
	SC_CHECK_HOUSE,
	SC_UPGRADE_HOUSE,
	// - Item
	SC_ADD_ITEM,
	SC_REMOVE_ITEM,
	// - Etc
	SC_WEATHER,
	SC_TIME,

	SC_END
};
// About Player
constexpr float PLAYER_VIEW_RANGE = 4000.f;
constexpr float ANIMAL_VIEW_RANGE = 2000.f;

// enum class
enum class PLAYER_ANIMATION_TYPE
{
	WALK, RUN, IDLE1, IDLE2, DIE, TAKE_WEAPON, ATTACK1, ATTACK2, ATTACK3, HIT1, HIT2, JUMP, END
};

enum class ANIMAL_ANIMATION_TYPE { WALK, RUN, IDLE, EAT, DIE, ATTACK };
#pragma	pack(push, 1)
// ___________________________________________________________________
//						[ Sever -> Client ]
// ___________________________________________________________________
// [ COMMON ]
struct sc_pos_packet {
	char	size;
	char	type;
	USHORT	usId;

	Vec3	vPos;
	Vec3	vRot;
};

struct sc_rot_packet {
	char	size;
	char	type;
	USHORT	usId;

	Vec3	vRot;
};

struct sc_remove_packet {
	char	size;
	char	type;
	USHORT	usId;
};

struct sc_animation_packet {
	char	size;
	char	type;
	USHORT	usId;	// Object Id
	UINT	uiType; // Animation Type
};
// Player
struct sc_login_ok_packet {
	char	size;
	char	type;
	USHORT	id;

	float	fHealth;
	float	fHungry;
	float	fThirst;

	Vec3	vPos;
};

struct sc_login_fail_packet {
	char			size;
	char			type;
};

struct sc_logout_packet{
	char	size;
	char	type;
	USHORT id;
};

struct sc_death_player_packet {
	char size;
	char type;
	USHORT id;

	float	fHealth;
	float	fStamina;
	float	fHungry;

	Vec3	vLocalPos;
};

struct sc_status_player_packet{
	char size;
	char type;


	float	fHealth;
	float	fThrist;
	float	fHungry;
};

struct sc_chat_packet {
	char size;
	char type;
	char ID[MAX_STR_LEN];
	char meesage[MAX_STR_LEN];
};

// [ Housing ]
struct sc_install_housing_packet
{
	char	size;
	char	type;
	USHORT	house_id;

	UINT	house_grade;

	UINT	housing_type;

	Vec3	vLocalPos;
	Vec3	vLocalRot;
	Vec3	vLocalScale;

	Vec3	m_vOffsetPos;
	Vec3	m_vOffsetScale;
};

struct sc_remove_housing_packet
{
	char	size;
	char	type;
	USHORT	house_id;
};

struct sc_check_housing_packet
{
	char	size;
	char	type;
	USHORT	house_id;
	bool	bCheck;
	char	eItemType;
	int		iCount;
};

struct sc_upgrade_housing_packet
{
	char	size;
	char	type;
	USHORT	house_id;
	UINT	house_grade;
	char	house_type;
};
// [ Natural ]
struct sc_put_natural_packet
{
	char size;
	char type;
	USHORT natural_id;
	float	fHealth;
	bool	bDestroy;
	Vec3	vLocalPos;
	Vec3	vLocalScale;
	Vec3	vOffsetPos;
	Vec3	vOffsetScale;
};

struct sc_destroy_natural_packet
{
	char size;
	char type;
	USHORT natural_id;
	Vec3	vOriginRot;
};

// [ Item ]
struct sc_add_item_packet
{
	char size;
	char type;
	char eItemType;
	int	iAmount;
};

// [ Etc ]
struct sc_weather_packet
{
	char size;
	char type;
	bool bRain;
};

struct sc_time_packet
{
	char size;
	char type;
	//float fTime;
	int	imin;
	int	ihour;
	int iday;
};

struct sc_full_server_packet {
	char	size;
	char	type;
};

struct sc_disconnect_server_packet {
	char	size;
	char	tpye;
};

// ___________________________________________________________________
//						[ Client -> Server ]
// ___________________________________________________________________

struct cs_login_packet {
	char			size;
	char			type;
	char			player_id[MAX_STR_LEN];	
};

struct cs_logout_packet {
	char	size;
	char	type;
};

struct cs_move_packet {
	char size;
	char type;

	bool bRun;
	Vec3 vLocalPos;
	Vec3 vWorldDir;
	float fHeight;
};

struct cs_pos_packet {
	char size;
	char type;
	Vec3 vLocalPos;
};

struct cs_rot_packet {
	char size;
	char type;
	Vec3	vRot;
	Vec3	vDir[3];
};

struct cs_chat_packet {
	char size;
	char type;
	USHORT id;
	char meesage[MAX_STR_LEN];
};

struct cs_attack_packet {
	char size;
	char type;
	UINT attack_uiType;
	USHORT attack_id;
	char	eType;
};

// [ Housing ] 
struct cs_install_housing_packet
{
	char	size;
	char	type;
	USHORT	house_id;

	UINT	housing_type;
	
	Vec3	vLocalPos;
	Vec3	vLocalRot;
	Vec3	vLocalScale;

	Vec3	vOffsetPos;
	Vec3	vOffsetScale;
};

struct cs_remove_housing_packet
{
	char	size;
	char	type;
	USHORT	house_id;
};

struct cs_upgrade_housing_packet
{
	char	size;
	char	type;
	USHORT	house_id;

	UINT	house_type;
};

struct cs_natural_attack_packet
{
	char	size;
	char	type;
	USHORT	natural_id;
};

struct cs_item_get_packet
{
	char	size;
	char	type;
	UINT	uiType;
	UINT	uiInvenNum;
};

struct cs_item_remove_packet
{
	char	size;
	char	type;
	UINT	uiType;
	UINT	uiInvenNum;
};

struct cs_animation_packet {
	char	size;
	char	type;
	UINT	uiType;
};

struct cs_use_item_packet {
	char	size;
	char	type;
	char	eType;
};

struct cs_equip_armor_packet {
	char	size;
	char	type;
	char	eType;
};

struct cs_destroy_armor_packet {
	char	size;
	char	type;
};

struct cs_dead_animal_packet {
	char	size;
	char	type;
	USHORT	index;
};

#pragma pack (pop)