/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: MFTDecoder.cpp 1289 2015-04-18 14:34:30Z umezawa $ */

// MFTDecoder.cpp : CMFTDecoder �̎���

#include "stdafx.h"
#include "MFTDecoder.h"
#include <Format.h>

// CMFTDecoder

const GUID &CMFTDecoder::MFTCATEGORY = MFT_CATEGORY_VIDEO_DECODER;

HRESULT CMFTDecoder::InternalBeginStream(void)
{
	LOGPRINTF("%p CMFTDecoder::InternalBeginStream()", this);

	if (m_pCodec->DecodeBegin(m_outfmt, m_nFrameWidth, m_nFrameHeight, CBGROSSWIDTH_WINDOWS, m_pInputUserData, m_cbInputUserData) == 0)
		return S_OK;
	else
		return E_FAIL;
}

HRESULT CMFTDecoder::InternalEndStream(void)
{
	LOGPRINTF("%p CMFTDecoder::InternalEndStream()", this);

	m_pCodec->DecodeEnd();

	return S_OK;
}

HRESULT CMFTDecoder::InternalProcessOutput(IMFSample *pOutputSample, IMFSample *pInputSample)
{
	size_t cbOutput;
	UINT32 bKeyFrame;
	IMFMediaBuffer *pInputBuffer;
	IMFMediaBuffer *pOutputBuffer;
	BYTE *pInputByteBuffer;
	BYTE *pOutputByteBuffer;

	if (FAILED(m_pInputSample->GetUINT32(MFSampleExtension_CleanPoint, &bKeyFrame)))
		bKeyFrame = FALSE;
	pInputSample->GetBufferByIndex(0, &pInputBuffer);
	pOutputSample->GetBufferByIndex(0, &pOutputBuffer);
	pInputBuffer->Lock(&pInputByteBuffer, NULL, NULL);
	pOutputBuffer->Lock(&pOutputByteBuffer, NULL, NULL);
	cbOutput = m_pCodec->DecodeFrame(pOutputByteBuffer, pInputByteBuffer);
	pInputBuffer->Unlock();
	pOutputBuffer->Unlock();
	pOutputBuffer->SetCurrentLength((DWORD)cbOutput);
	pOutputSample->SetUINT32(MFSampleExtension_CleanPoint, TRUE);
	pInputBuffer->Release();
	pOutputBuffer->Release();

	return S_OK;
}
