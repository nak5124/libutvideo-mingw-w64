/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: FrameBuffer.cpp 782 2011-10-19 12:12:25Z umezawa $ */

#include "stdafx.h"
#include "utvideo.h"
#include "FrameBuffer.h"

CFrameBuffer::CFrameBuffer(void)
{
	for (int i = 0; i < MAX_PLANE; i++)
	{
		m_pAllocatedAddr[i] = NULL;
		m_pBufferAddr[i] = NULL;
	}
	m_nPlanes = 0;
}

CFrameBuffer::~CFrameBuffer(void)
{
	for (int i = 0; i < MAX_PLANE; i++)
	{
		if (m_pAllocatedAddr[i] != NULL)
		{
#ifdef _WIN32
			VirtualFree(m_pAllocatedAddr[i], 0, MEM_RELEASE);
#endif
#if defined(__APPLE__) || defined(__unix__)
			munmap(m_pAllocatedAddr[i], m_cbAllocated[i]);
#endif
		}
	}
}

void CFrameBuffer::AddPlane(size_t cbBuffer, size_t cbMargin)
{
#ifdef _WIN32
	SYSTEM_INFO si;
#endif
	size_t cbAllocateUnit;
	size_t cbAllocated;
	uint8_t *pAllocatedAddr;

#ifdef _WIN32
	GetSystemInfo(&si);
	cbAllocateUnit = si.dwPageSize;
#endif
#if defined(__APPLE__) || defined(__unix__)
	cbAllocateUnit = getpagesize();
#endif
	cbMargin = ROUNDUP(cbMargin, cbAllocateUnit);
	cbAllocated = cbMargin + ROUNDUP(cbBuffer + cbMargin, cbAllocateUnit);

#ifdef _WIN32
	pAllocatedAddr = (uint8_t *)VirtualAlloc(NULL, cbAllocated, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pAllocatedAddr == NULL)
		return;
#endif
#if defined(__APPLE__) || defined(__unix__)
	pAllocatedAddr = (uint8_t *)mmap(NULL, cbAllocated, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (pAllocatedAddr == MAP_FAILED)
		return;
#endif

	m_pAllocatedAddr[m_nPlanes] = pAllocatedAddr;
	m_cbAllocated[m_nPlanes] = cbAllocated;
	/*
	 * �v���[�����ƂɊJ�n�A�h���X�����������炵�A�L���b�V���̃X���b�V���O���������B
	 * 256 �̓}�W�b�N�i���o�[�ł��邪�A
	 *   - �L���b�V�����C���T�C�Y�i�ŋ߂̑����̃v���Z�b�T�ł� 64 �o�C�g�j�̐����{
	 *   - �L���b�V���T�C�Y���\��������
	 * �𖞂����K�v������B
	 */
	m_pBufferAddr[m_nPlanes] = pAllocatedAddr + cbMargin + m_nPlanes * 256;

	m_nPlanes++;

	return;
}
