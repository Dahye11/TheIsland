#pragma once

/*_____________________________________________
			[통신을 위한 구조체]
_______________________________________________*/
typedef struct Over_ex
{
	WSAOVERLAPPED	m_Overlapped;
	WSABUF			m_DataBuffer;
	char			m_MessageBuffer[MAX_BUF];
	EVENT_TYPE		m_Event;
}OVER_EX, POVER_EX;

typedef struct Position2d {
	short x;
	short y;

	Position2d(short _x, short _y) : x(_x), y(_y) {}
}POS2D, PPOS2D;

typedef struct Position3d {
	short x;
	short y;
	short z;

	Position3d(short _x, short _y, short _z) : x(_x), y(_y), z(_z) {}
}POS3D, PPOS3D;

typedef struct User_info {
	SOCKET				socket;
	OVER_EX				over;
	unsigned short		id;
	POS3D				pos;
	set<unsigned int>	near_id;
	mutex				near_lock;
};

/*_____________________________________________
			[Timer Event]
_______________________________________________*/
struct Object_Event {
	std::chrono::high_resolution_clock::time_point wakeup_time;
	EVENT_TYPE	m_EventType;
	EVENT_TYPE	m_OverType;
	unsigned short m_usID;
	unsigned short m_usOtherID;

	constexpr bool operator <(const Object_Event& _left) const
	{
		return wakeup_time > _left.wakeup_time;
	}
};

struct T_EVENT {
	int do_object;
	EVENT_TYPE event_type;
	high_resolution_clock::time_point	start_time;
	int from_object;

	constexpr bool operator <(const T_EVENT& _Left) const {
		return (start_time > _Left.start_time);
	}
};

/*_____________________________________________
			[DataBase Event]
_______________________________________________*/
typedef struct DataBase_Event {
	char	state;
	unsigned short client_num;
	wchar_t	id[MAX_STR_LEN];
	wchar_t	password[MAX_STR_LEN];
};

struct User_Data {
	std::string m_sID;
	unsigned short m_usID;
	int m_iPlayerNum;

	User_Data(std::string sID, unsigned short usNum, int iPlayerNum) {
		m_sID = sID;
		m_usID = usNum;
		m_iPlayerNum = iPlayerNum;
	}
};