#pragma once

typedef struct tResolution
{
	float fWidth;
	float fHeight;
}RESOLUTION;

// ���� ����ü
struct VTX
{
	Vec3 vPos;
	Vec4 vColor;
};

// ������� �޸�
struct tTransform
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;
};