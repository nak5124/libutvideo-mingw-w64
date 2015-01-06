/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: DMOEncoder.cpp 914 2012-09-17 10:05:59Z umezawa $ */

// DMOEncoder.cpp : CDMOEncoder �̎���

#include "stdafx.h"
#include "DMOEncoder.h"
#include <Format.h>

// CDMOEncoder

const GUID &CDMOEncoder::DMOCATEGORY = DMOCATEGORY_VIDEO_ENCODER;

HRESULT CDMOEncoder::InternalAllocateStreamingResources()
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalAllocateStreamingResources()\n");

	const DMO_MEDIA_TYPE *pmtIn  = InputType(0);
	const VIDEOINFOHEADER *pvihIn  = (const VIDEOINFOHEADER *)pmtIn->pbFormat;
	utvf_t infmt;

	if (DirectShowFormatToUtVideoFormat(&infmt, pvihIn->bmiHeader.biCompression, pvihIn->bmiHeader.biBitCount, pmtIn->subtype) != 0)
		return DMO_E_INVALIDTYPE;

	if (m_pCodec->EncodeBegin(infmt, pvihIn->bmiHeader.biWidth, pvihIn->bmiHeader.biHeight, CBGROSSWIDTH_WINDOWS) == 0)
		return S_OK;
	else
		return E_FAIL;
}

HRESULT CDMOEncoder::InternalFreeStreamingResources()
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalFreeStreamingResources()\n");

	m_pCodec->EncodeEnd();

	return S_OK;
}

HRESULT CDMOEncoder::InternalProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, DMO_OUTPUT_DATA_BUFFER *pOutputBuffers, DWORD *pdwStatus)
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalProcessOutput()\n");

	BYTE *pInput;
	BYTE *pOutput;
	bool bKeyFrame;
	size_t cbOutput;

	if (m_pInputBuffer == NULL)
		return S_FALSE;

	pOutputBuffers->pBuffer->GetBufferAndLength(&pOutput, NULL);
	m_pInputBuffer->GetBufferAndLength(&pInput, NULL);

	cbOutput = m_pCodec->EncodeFrame(pOutput, &bKeyFrame, pInput);

	pOutputBuffers->dwStatus = 0;
	if (bKeyFrame)
		pOutputBuffers->dwStatus |= DMO_OUTPUT_DATA_BUFFERF_SYNCPOINT;
	if (m_bInputTimestampValid)
	{
		pOutputBuffers->dwStatus |= DMO_OUTPUT_DATA_BUFFERF_TIME;
		pOutputBuffers->rtTimestamp = m_rtInputTimestamp;
	}
	if (m_bInputTimelengthValid)
	{
		pOutputBuffers->dwStatus |= DMO_OUTPUT_DATA_BUFFERF_TIMELENGTH;
		pOutputBuffers->rtTimelength = m_rtInputTimelength;
	}
	pOutputBuffers->pBuffer->SetLength((DWORD)cbOutput);
	_RPT1(_CRT_WARN, "Size = %u\n", (unsigned int)cbOutput);

	m_pInputBuffer->Release();
	m_pInputBuffer = NULL;

	return S_OK;	
}


// IPersist

HRESULT STDMETHODCALLTYPE CDMOEncoder::GetClassID(CLSID *pClassID)
{
	*pClassID = m_clsid;

	return S_OK;
}


// IPersistStream

HRESULT STDMETHODCALLTYPE CDMOEncoder::IsDirty(void)
{
	return S_OK; /* XXX ��� dirty */
}

HRESULT STDMETHODCALLTYPE CDMOEncoder::Load(IStream *pStm)
{
	ULONG cbState;
	ULONG cbRead;
	void *pState;
	HRESULT hr;

	LockIt lck(this);

	cbState = (ULONG)m_pCodec->GetStateSize();
	pState = malloc(cbState);
	hr = pStm->Read(pState, cbState, &cbRead);
	if (FAILED(hr))
	{
		free(pState);
		return hr;
	}
	m_pCodec->SetState(pState, cbRead);
	free(pState);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDMOEncoder::Save(IStream *pStm, BOOL fClearDirty)
{
	ULONG cbState;
	ULONG cbWritten;
	void *pState;
	HRESULT hr;

	LockIt lck(this);

	cbState = (ULONG)m_pCodec->GetStateSize();
	pState = malloc(cbState);
	m_pCodec->GetState(pState, cbState);
	hr = pStm->Write(pState, cbState, &cbWritten);
	free(pState);
	if (FAILED(hr))
		return hr;
	if (cbWritten != cbState)
		return STG_E_MEDIUMFULL;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDMOEncoder::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	LockIt lck(this);

	pcbSize->QuadPart = m_pCodec->GetStateSize();

	return S_OK;
}


// IAMVfwCompressDialogs

HRESULT STDMETHODCALLTYPE CDMOEncoder::ShowDialog(int iDialog, HWND hwnd)
{
	LockIt lck(this);

	switch (iDialog)
	{
	case VfwCompressDialog_Config:
		m_pCodec->Configure(hwnd);
		return S_OK;
	case VfwCompressDialog_About:
		m_pCodec->About(hwnd);
		return S_OK;
	case VfwCompressDialog_QueryConfig:
	case VfwCompressDialog_QueryAbout:
		return S_OK;
	default:
		return E_INVALIDARG;
	}
}

HRESULT STDMETHODCALLTYPE CDMOEncoder::GetState(LPVOID pState, int *pcbState)
{
	LockIt lck(this);

	*pcbState = m_pCodec->GetState(pState, *pcbState);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDMOEncoder::SetState(LPVOID pState, int cbState)
{
	LockIt lck(this);

	m_pCodec->SetState(pState, cbState);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDMOEncoder::SendDriverMessage(int uMsg, long dw1, long dw2)
{
	return E_NOTIMPL;
}
