/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: FrameBuffer.h 728 2011-08-30 13:09:11Z umezawa $ */

#pragma once

class CFrameBuffer
{
private:
	static const int MAX_PLANE = 4;

private:
	int m_nPlanes;
	uint8_t *m_pAllocatedAddr[MAX_PLANE];
	uint8_t *m_pBufferAddr[MAX_PLANE];
	size_t m_cbAllocated[MAX_PLANE];

public:
	CFrameBuffer(void);
	~CFrameBuffer(void);

public:
	void AddPlane(size_t cbBuffer, size_t cbMargin);

public:
	inline uint8_t *GetPlane(int n)
	{
		return m_pBufferAddr[n];
	}

	inline uint8_t *GetBuffer(void)
	{
		return GetPlane(0);
	}
};
