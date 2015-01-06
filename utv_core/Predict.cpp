/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: Predict.cpp 1191 2014-06-11 14:44:13Z umezawa $ */

#include "stdafx.h"
#include "utvideo.h"
#include "Predict.h"
#include "TunedFunc.h"

template<class T>
inline T median(T a, T b, T c)
{
	return max(min(max(a,b),c),min(a,b));
}

void cpp_PredictWrongMedianAndCount(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, uint32_t *pCountTable)
{
	const uint8_t *p = pSrcBegin;
	uint8_t *q = pDst;

	// �ŏ��̃��C���̍ŏ��̃s�N�Z���� 0x80 ��\�����Ă����B
	*q = *p - 0x80;
	pCountTable[*q]++;
	p++;
	q++;

	// �ŏ��̃��C���̎c��̃s�N�Z���� predict left �Ɠ����B
	for (; p < pSrcBegin + dwStride; p++, q++)
	{
		*q = *p - *(p - 1);
		pCountTable[*q]++;
	}

	// 1 ���C�������Ȃ��ꍇ�͂����ŏI���B
	if (p == pSrcEnd)
		return;

	// ���̃��C���̍ŏ��̃s�N�Z���� predict above�B
	// �������Ă����ƃA�Z���u�����������ɏ������኱�ȒP�ɂȂ�B
	*q = *p - *(p - dwStride);
	pCountTable[*q]++;
	p++;
	q++;

	// �c��̃s�N�Z���� predict median �̖{��
	for (; p < pSrcEnd; p++, q++)
	{
		*q = *p - median<uint8_t>(*(p - dwStride), *(p - 1), *(p - dwStride) + *(p - 1) - *(p - 1 - dwStride));
		pCountTable[*q]++;
	}
}

void cpp_PredictLeftAndCount(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, uint32_t *pCountTable)
{
	const uint8_t *p = pSrcBegin;
	uint8_t *q = pDst;

	// �ŏ��̃��C���̍ŏ��̃s�N�Z���� 0x80 ��\�����Ă����B
	*q = *p - 0x80;
	pCountTable[*q]++;
	p++;
	q++;

	// �c��̃s�N�Z���� predict left �̖{��
	for (; p < pSrcEnd; p++, q++)
	{
		*q = *p - *(p-1);
		pCountTable[*q]++;
	}
}

void cpp_RestoreWrongMedian(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride)
{
	const uint8_t *p = pSrcBegin;
	uint8_t *q = pDst;

	*q++ = *p++ + 0x80;

	for (; p < pSrcBegin + dwStride; p++, q++)
	{
		*q = *p + *(q - 1);
	}

	if (p == pSrcEnd)
		return;

	*q = *p + *(q - dwStride);
	p++;
	q++;

	for (; p < pSrcEnd; p++, q++)
	{
		*q = *p + median<uint8_t>(*(q - dwStride), *(q - 1), *(q - dwStride) + *(q - 1) - *(q - 1 - dwStride));
	}
}

void cpp_RestoreWrongMedianBlock4(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	const uint8_t *pSrcStripeBegin;
	uint8_t *pDstStripeBegin;
	const uint8_t *p;
	uint8_t *q;
	uint8_t left[4] = { 0, 0, 0, 0 };
	uint8_t topleft[4] = { 0, 0, 0, 0 };

	pDst[0] = pSrcBegin[0] + 0x80;
	pDst[1] = pSrcBegin[1] + 0x80;
	pDst[2] = pSrcBegin[2] + 0x80;
	pDst[3] = pSrcBegin[3] + 0x80;

	for (p = pSrcBegin + 4, q = pDst + 4; p < pSrcBegin + cbWidth; p += 4, q += 4)
	{
		q[0] = p[0] + q[-4];
		q[1] = p[1] + q[-3];
		q[2] = p[2] + q[-2];
		q[3] = p[3] + q[-1];
	}

	for (pSrcStripeBegin = pSrcBegin + scbStride, pDstStripeBegin = pDst + scbStride; pSrcStripeBegin != pSrcEnd; pSrcStripeBegin += scbStride, pDstStripeBegin += scbStride)
	{
		for (p = pSrcStripeBegin, q = pDstStripeBegin; p < pSrcStripeBegin + cbWidth; p += 4, q += 4)
		{
			q[0] = p[0] + median<uint8_t>(q[0-scbStride], left[0], q[0-scbStride] + left[0] - topleft[0]); left[0] = q[0]; topleft[0] = q[0-scbStride];
			q[1] = p[1] + median<uint8_t>(q[1-scbStride], left[1], q[1-scbStride] + left[1] - topleft[1]); left[1] = q[1]; topleft[1] = q[1-scbStride];
			q[2] = p[2] + median<uint8_t>(q[2-scbStride], left[2], q[2-scbStride] + left[2] - topleft[2]); left[2] = q[2]; topleft[2] = q[2-scbStride];
			q[3] = p[3] + median<uint8_t>(q[3-scbStride], left[3], q[3-scbStride] + left[3] - topleft[3]); left[3] = q[3]; topleft[3] = q[3-scbStride];
		}
	}
}


template<int B>
void cpp_PredictCylindricalLeftAndCount(typename CSymbolBits<B>::symbol_t *pDst, const typename CSymbolBits<B>::symbol_t *pSrcBegin, const typename CSymbolBits<B>::symbol_t *pSrcEnd, typename CSymbolBits<B>::symbol_t initial, uint32_t *pCountTable)
{
	const typename CSymbolBits<B>::symbol_t *p = pSrcBegin;
	typename CSymbolBits<B>::symbol_t *q = pDst;

	*q = (*p - initial) & CSymbolBits<B>::maskval;
	pCountTable[*q]++;
	p++;
	q++;

	// �c��̃s�N�Z���� predict left �̖{��
	for (; p < pSrcEnd; p++, q++)
	{
		*q = (*p - *(p-1)) & CSymbolBits<B>::maskval;
		pCountTable[*q]++;
	}
}

template void cpp_PredictCylindricalLeftAndCount<10>(CSymbolBits<10>::symbol_t *pDst, const CSymbolBits<10>::symbol_t *pSrcBegin, const CSymbolBits<10>::symbol_t *pSrcEnd, CSymbolBits<10>::symbol_t initial, uint32_t *pCountTable);
