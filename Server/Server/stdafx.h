#pragma once
#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <Windows.h>

#include <sqlext.h>  

#include <iostream>
#include <thread>

#include <cstdlib>
#include <mutex>

#include <vector>
#include <set>
#include <map>

using namespace std;
using namespace chrono;

// SingleTon
#include "SingleTon.h"

// enum ��
#include "types.h"

// Ŭ���̾�Ʈ�� ������ ����ϱ� ���� �⺻���� ��� ��Ģ(���� �� Ŭ���̾�Ʈ ������, Ŭ���̾�Ʈ�� ����ȭ�� ������ ��ģ��.)
#include "protocol.h"

// struct ����ü ��
#include "struct.h"

#include "extern.h"

// boost asio �������� ���̺귯�� (iocp�� boost �� �ϳ��� ���� �ȴ�.)
// #include "boost_asio.h"