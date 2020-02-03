#pragma once

// simd ��� ����
#define _XM_NO_INTRINSICS_

#include <d3d12.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include "SimpleMath.h"


#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;


#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>

using namespace std;

#include "Macro.h" // ��ũ��
#include "Flag.h"
#include "Types.h"
#include "d3dx12.h"
#include "Ptr.h"

