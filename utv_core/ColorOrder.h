/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: ColorOrder.h 1019 2013-05-21 13:53:00Z umezawa $ */

#pragma once

class CBGRColorOrder
{
public:
	static const int A = INT_MIN;
	static const int R = 2;
	static const int G = 1;
	static const int B = 0;
	static const int BYPP = 3;
};

class CBGRAColorOrder
{
public:
	static const int A = 3;
	static const int R = 2;
	static const int G = 1;
	static const int B = 0;
	static const int BYPP = 4;
};

class CRGBColorOrder
{
public:
	static const int A = INT_MIN;
	static const int R = 0;
	static const int G = 1;
	static const int B = 2;
	static const int BYPP = 3;
};

class CARGBColorOrder
{
public:
	static const int A = 0;
	static const int R = 1;
	static const int G = 2;
	static const int B = 3;
	static const int BYPP = 4;
};

class CYUYVColorOrder
{
public:
	static const int Y0 = 0;
	static const int Y1 = 2;
	static const int U  = 1;
	static const int V  = 3;
};

class CUYVYColorOrder
{
public:
	static const int Y0 = 1;
	static const int Y1 = 3;
	static const int U  = 0;
	static const int V  = 2;
};
