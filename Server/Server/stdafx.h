#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <Windows.h>
#include <functional>

#include <sql.h>
#include <sqlext.h>

#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <wchar.h>
#include <ctime>
#include <math.h>
#include <algorithm>

#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <shared_mutex>

#include <concurrent_queue.h>
#include <concurrent_unordered_set.h>
#include <concurrent_unordered_map.h>
#include <concurrent_priority_queue.h>

#include <vector>
#include <set>
#include <map>

/*///////////////////////////////
	�ٷ� ���� �ʿ� ��� ����.
*////////////////////////////////
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <DirectXMathVector.inl>

#include <sdkddkver.h>
#include <DirectXMath.h>
#include "SimpleMath.h"

using namespace std;
using namespace chrono;
using namespace concurrency;

// define
#include "define.h"

// enum ��
#include "types.h"

// Ŭ���̾�Ʈ�� ������ ����ϱ� ���� �⺻���� ��� ��Ģ(���� �� Ŭ���̾�Ʈ ������, Ŭ���̾�Ʈ�� ����ȭ�� ������ ��ģ��.)
#include "protocol.h"

// struct ����ü ��
#include "struct.h"
