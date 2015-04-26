/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: MFTCodec.h 1289 2015-04-18 14:34:30Z umezawa $ */

// MFTCodec.h : CMFTCodec �̐錾

#pragma once
#include "resource.h"       // ���C�� �V���{��

#include "utvideo.h"
#include "utv_mft.h"
//#include <vfw.h>
#include "Codec.h"
#include "ClsID.h"
#include <Format.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "DCOM �̊��S�T�|�[�g���܂�ł��Ȃ� Windows Mobile �v���b�g�t�H�[���̂悤�� Windows CE �v���b�g�t�H�[���ł́A�P��X���b�h COM �I�u�W�F�N�g�͐������T�|�[�g����Ă��܂���BATL ���P��X���b�h COM �I�u�W�F�N�g�̍쐬���T�|�[�g���邱�ƁA����т��̒P��X���b�h COM �I�u�W�F�N�g�̎����̎g�p�������邱�Ƃ���������ɂ́A_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ���`���Ă��������B���g�p�� rgs �t�@�C���̃X���b�h ���f���� 'Free' �ɐݒ肳��Ă���ADCOM Windows CE �ȊO�̃v���b�g�t�H�[���ŃT�|�[�g�����B��̃X���b�h ���f���Ɛݒ肳��Ă��܂����B"
#endif

void FormatInfoToRegisterTypeInfo(const utvf_t *putvf, UINT32 *pcTypes, MFT_REGISTER_TYPE_INFO **ppTypes);

class LockAttr
{
private:
	IMFAttributes *m_p;
public:
	LockAttr(IMFAttributes *p) : m_p(p)
	{
		m_p->LockStore();
	}
	~LockAttr()
	{
		m_p->UnlockStore();
	}
};

// CMFTCodec

template<class T>
class ATL_NO_VTABLE CMFTCodec :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IMFTransform
{
protected:
	class LockIt
	{
	private:
		T *const m_p;
	public:
		LockIt(T *p) : m_p(p)
		{
			m_p->Lock();
		}
		~LockIt()
		{
			m_p->Unlock();
		}
	};

protected:
	const DWORD m_fcc;
	const CLSID m_clsid;
	CCodec *m_pCodec;
	IMFSample *m_pInputSample;
	IMFMediaType *m_pInputMediaType;
	IMFMediaType *m_pOutputMediaType;
	utvf_t m_infmt, m_outfmt;
	UINT8 *m_pInputUserData;
	UINT8 *m_pOutputUserData;
	UINT32 m_cbInputUserData;
	UINT32 m_cbOutputUserData;
	UINT32 m_nFrameWidth, m_nFrameHeight;
	bool m_bStreamBegin;

public:
	CMFTCodec(DWORD fcc, REFCLSID clsid) :
		m_fcc(fcc), m_clsid(clsid)
	{
		if (IsLogWriterInitializedOrDebugBuild())
		{
			OLECHAR szClsId[48];
			StringFromGUID2(clsid, szClsId, _countof(szClsId));
			LOGPRINTF("%p CMFTCodec::CMFTCodec(fcc=%08X, clsid=%S)", this, fcc, szClsId);
		}

		utvf_t utvf;

		VCMFormatToUtVideoFormat(&utvf, fcc, 0);
		m_pCodec = CCodec::CreateInstance(utvf, "MFT");
		m_pInputSample = NULL;
		m_pInputMediaType = NULL;
		m_pOutputMediaType = NULL;
		m_pInputUserData = NULL;
		m_pOutputUserData = NULL;
		m_bStreamBegin = false;

		LOGPRINTF("%p  m_pCodec=%p, TinyName=\"%s\"", this, m_pCodec, m_pCodec->GetTinyName());
	}

	virtual ~CMFTCodec()
	{
		LOGPRINTF("%p CMFTCodec::~CMFTCodec()", this);

		EndStream();

		if (m_pInputSample != NULL)
			m_pInputSample->Release();
		if (m_pInputMediaType != NULL)
			m_pInputMediaType->Release();
		if (m_pOutputMediaType != NULL)
			m_pOutputMediaType->Release();
		if (m_pInputUserData != NULL)
			CoTaskMemFree(m_pInputUserData);
		if (m_pOutputUserData != NULL)
			CoTaskMemFree(m_pOutputUserData);
		CCodec::DeleteInstance(m_pCodec);
	}

	static HRESULT WINAPI UpdateRegistry(DWORD fcc, REFCLSID clsid, BOOL bRegister)
	{
		if (IsLogWriterInitializedOrDebugBuild())
		{
			OLECHAR szClsId[48];
			StringFromGUID2(clsid, szClsId, _countof(szClsId));
			LOGPRINTF("CMFTCodec::UpdateRegistry(fcc=%08X, clsid=%S, bRegister=%s)", fcc, szClsId, bRegister ? "true" : "false");
		}

		HRESULT hr;
		OLECHAR szFcc[5];
		OLECHAR szClsID[64];
		_ATL_REGMAP_ENTRY regmap[3] = {
			{ L"FCC",   szFcc   },
			{ L"CLSID", szClsID },
			{ NULL,     NULL    }
		};
		WCHAR szCodecName[128];

#define FCC4PRINTF(fcc) \
	(uint8_t)(fcc), \
	(uint8_t)(fcc >> 8), \
	(uint8_t)(fcc >> 16), \
	(uint8_t)(fcc >> 24)

		wsprintfW(szFcc, L"%C%C%C%C", FCC4PRINTF(fcc));
		StringFromGUID2(clsid, szClsID, _countof(szClsID));
		hr = ATL::_pAtlModule->UpdateRegistryFromResource(T::IDR, bRegister, regmap);
		if (FAILED(hr))
			return hr;

		if (bRegister)
		{
			CCodec *pCodec;
			MFT_REGISTER_TYPE_INFO *pInTypes;
			MFT_REGISTER_TYPE_INFO *pOutTypes;
			UINT32 cInTypes, cOutTypes;
			utvf_t utvf;

			VCMFormatToUtVideoFormat(&utvf, fcc, 0);
			pCodec = CCodec::CreateInstance(utvf, "MFT");
			FormatInfoToRegisterTypeInfo(T::GetInputFormatInfo(pCodec), &cInTypes, &pInTypes);
			FormatInfoToRegisterTypeInfo(T::GetOutputFormatInfo(pCodec), &cOutTypes, &pOutTypes);
			T::GetName(pCodec, szCodecName, _countof(szCodecName));
			hr = MFTRegister(clsid, T::MFTCATEGORY, szCodecName, 0, cInTypes, pInTypes, cOutTypes, pOutTypes, NULL);
			delete pInTypes;
			delete pOutTypes;
			CCodec::DeleteInstance(pCodec);
			return hr;
		}
		else
		{
			// http://social.msdn.microsoft.com/Forums/zh/mediafoundationdevelopment/thread/7d3dc70f-8eae-4ad0-ad90-6c596cf78c80
			// �ɂ��ƁAMFTUnregister ���������ł� 0x80070002 ��Ԃ����Ƃ�����̂� "known bug" �炵���c
			hr = MFTUnregister(clsid);
			if (hr == 0x80070002) // 0x80070002 is HRESULT-style ERROR_FILE_NOT_FOUND (2)
				hr = S_OK;
			return hr;
		}
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	// IMFTransform

	HRESULT STDMETHODCALLTYPE GetStreamLimits(DWORD *pdwInputMinimum, DWORD *pdwInputMaximum, DWORD *pdwOutputMinimum, DWORD *pdwOutputMaximum)
	{
		*pdwInputMinimum = 1;
		*pdwInputMaximum = 1;
		*pdwOutputMinimum = 1;
		*pdwOutputMaximum = 1;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetStreamCount(DWORD *pcInputStreams, DWORD *pcOutputStreams)
	{
		*pcInputStreams = 1;
		*pcOutputStreams = 1;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetStreamIDs(DWORD dwInputIDArraySize, DWORD *pdwInputIDs, DWORD dwOutputIDArraySize, DWORD *pdwOutputIDs)
	{
		return E_NOTIMPL; // default ID assignment
	}

	HRESULT STDMETHODCALLTYPE GetInputStreamInfo(DWORD dwInputStreamID, MFT_INPUT_STREAM_INFO *pStreamInfo)
	{
		if (dwInputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		pStreamInfo->hnsMaxLatency = 0;
		pStreamInfo->dwFlags = MFT_INPUT_STREAM_WHOLE_SAMPLES | MFT_INPUT_STREAM_SINGLE_SAMPLE_PER_BUFFER;
		pStreamInfo->cbSize = 0;
		pStreamInfo->cbMaxLookahead = 0;
		pStreamInfo->cbAlignment = 4;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetOutputStreamInfo(DWORD dwOutputStreamID, MFT_OUTPUT_STREAM_INFO *pStreamInfo)
	{
		if (dwOutputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		pStreamInfo->dwFlags = MFT_OUTPUT_STREAM_WHOLE_SAMPLES | MFT_OUTPUT_STREAM_SINGLE_SAMPLE_PER_BUFFER | MFT_OUTPUT_STREAM_PROVIDES_SAMPLES;
		pStreamInfo->cbSize = 0;
		pStreamInfo->cbAlignment = 4;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetAttributes(IMFAttributes **pAttributes)
	{
		return E_NOTIMPL; // XXX?
	}

	HRESULT STDMETHODCALLTYPE GetInputStreamAttributes(DWORD dwInputStreamID, IMFAttributes **pAttributes)
	{
		return E_NOTIMPL; // XXX?
	}

	HRESULT STDMETHODCALLTYPE GetOutputStreamAttributes(DWORD dwOutputStreamID, IMFAttributes **pAttributes)
	{
		return E_NOTIMPL; // XXX?
	}

	HRESULT STDMETHODCALLTYPE DeleteInputStream(DWORD dwStreamID)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE AddInputStreams(DWORD cStreams, DWORD *adwStreamIDs)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE GetInputAvailableType(DWORD dwInputStreamID, DWORD dwTypeIndex, IMFMediaType **ppType)
	{
		LOGPRINTF("%p CMFTCodec::GetInputAvailableType(dwInputStreamID=%d, dwTypeIndex=%d, ppType=%p)", this, dwInputStreamID, dwTypeIndex, ppType);

		LockIt lck(static_cast<T *>(this));

		HRESULT hr;

		if (dwInputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		const utvf_t *putvf = T::GetInputFormatInfo(m_pCodec);
		GUID subtype;

		for (;; putvf++)
		{
			if (!*putvf)
				return MF_E_NO_MORE_TYPES;
			if (UtVideoFormatToMediaFoundationFormat(&subtype, *putvf) != 0)
				continue;
			if (dwTypeIndex == 0)
				break;
			dwTypeIndex--;
		}

		hr = MFCreateMediaType(ppType);
		if (FAILED(hr))
			return hr;

		(*ppType)->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
		(*ppType)->SetGUID(MF_MT_SUBTYPE, subtype);
		(*ppType)->SetUINT32(MF_MT_COMPRESSED, !T::bEncoding);
		(*ppType)->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, T::bEncoding || !m_pCodec->IsTemporalCompressionSupported());

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetOutputAvailableType(DWORD dwOutputStreamID, DWORD dwTypeIndex, IMFMediaType **ppType)
	{
		LOGPRINTF("%p CMFTCodec::GetOutputAvailableType(dwOutputStreamID=%d, dwTypeIndex=%d, ppType=%p)", this, dwOutputStreamID, dwTypeIndex, ppType);

		LockIt lck(static_cast<T *>(this));

		HRESULT hr;

		if (dwOutputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		const utvf_t *putvf = T::GetOutputFormatInfo(m_pCodec);
		GUID subtype;

		for (;; putvf++)
		{
			if (!*putvf)
				return MF_E_NO_MORE_TYPES;
			if (UtVideoFormatToMediaFoundationFormat(&subtype, *putvf) != 0)
				continue;
			if (dwTypeIndex == 0)
				break;
			dwTypeIndex--;
		}

		hr = MFCreateMediaType(ppType);
		if (FAILED(hr))
			return hr;

		(*ppType)->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
		(*ppType)->SetGUID(MF_MT_SUBTYPE, subtype);
		(*ppType)->SetUINT32(MF_MT_COMPRESSED, T::bEncoding);
		(*ppType)->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, !T::bEncoding || !m_pCodec->IsTemporalCompressionSupported());

		if (m_pInputMediaType != NULL)
		{
			void *buf;
			size_t cbExtraData;
			UINT64 u64FrameRate;
			utvf_t outfmt = *putvf;

			MFSetAttributeSize((*ppType), MF_MT_FRAME_SIZE, m_nFrameWidth, m_nFrameHeight);
			if (SUCCEEDED(m_pInputMediaType->GetUINT64(MF_MT_FRAME_RATE, &u64FrameRate)))
				(*ppType)->SetUINT64(MF_MT_FRAME_RATE, u64FrameRate);
			// �Œ���t���[���T�C�Y�i�ƃt���[�����[�g�j�͐ݒ肵�Ă����K�v������悤��

			cbExtraData = ((T *)this)->GetExtraDataSize();
			buf = malloc(max(cbExtraData, 1)); // cbExtraData �� 0 �̎��� malloc() �� 0 ��n���� NULL ���Ԃ��Ă���̂�h��
			((T *)this)->GetExtraData(buf, cbExtraData, outfmt, m_infmt, m_nFrameWidth, m_nFrameHeight);
			(*ppType)->SetBlob(MF_MT_USER_DATA, (UINT8 *)buf, (UINT32)cbExtraData);
			free(buf);
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetInputType(DWORD dwInputStreamID, IMFMediaType *pType, DWORD dwFlags)
	{
		LOGPRINTF("%p CMFTCodec::SetInputType(dwInputStreamID=%d, pType=%p, dwFlags=0x%08X)", this, dwInputStreamID, pType, dwFlags);

		LockIt lck(static_cast<T *>(this));

		HRESULT hr;
		int ret;
		IMFMediaType *pNewType;
		UINT8 *pNewUserData;
		UINT32 cbNewUserData;
		GUID guidNewMajorType, guidNewSubtype;
		UINT32 nFrameWidth, nFrameHeight;
		utvf_t infmt;

		if (dwInputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		if (pType == NULL)
		{
			m_pInputMediaType->Release();
			m_pInputMediaType = NULL;
			CoTaskMemFree(m_pInputUserData);
			m_pInputUserData = NULL;
			return S_OK;
		}

		LockAttr lockattr(pType);

		// check type

		if (FAILED(pType->GetGUID(MF_MT_MAJOR_TYPE, &guidNewMajorType)))
			return MF_E_INVALIDTYPE;
		if (FAILED(pType->GetGUID(MF_MT_SUBTYPE, &guidNewSubtype)))
			return MF_E_INVALIDTYPE;
		if (FAILED(MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &nFrameWidth, &nFrameHeight)))
			return MF_E_INVALIDTYPE;

		if (!IsEqualGUID(guidNewMajorType, MFMediaType_Video))
			return MF_E_INVALIDMEDIATYPE;
		if (MediaFoundationFormatToUtVideoFormat(&infmt, guidNewSubtype, (INT32)MFGetAttributeUINT32(pType, MF_MT_DEFAULT_STRIDE, 1) < 0) != 0)
			return MF_E_INVALIDMEDIATYPE;

		if (FAILED(pType->GetAllocatedBlob(MF_MT_USER_DATA, &pNewUserData, &cbNewUserData)))
		{
			pNewUserData = (UINT8 *)CoTaskMemAlloc(0);
			cbNewUserData = 0;
		}

		ret = ((T *)this)->Query(UTVF_INVALID, infmt, nFrameWidth, nFrameHeight, NULL, 0, pNewUserData, cbNewUserData);
		if (ret != 0)
		{
			CoTaskMemFree(pNewUserData);
			return MF_E_INVALIDMEDIATYPE;
		}

		// set type

		if (dwFlags & MFT_SET_TYPE_TEST_ONLY)
		{
			CoTaskMemFree(pNewUserData);
			return S_OK;
		}

		hr = MFCreateMediaType(&pNewType);
		if (FAILED(hr))
		{
			CoTaskMemFree(pNewUserData);
			return hr;
		}
		hr = pType->CopyAllItems(pNewType);
		if (FAILED(hr))
		{
			CoTaskMemFree(pNewUserData);
			pNewType->Release();
			return hr;
		}
		if (m_pInputMediaType != NULL)
			m_pInputMediaType->Release();
		if (m_pInputUserData != NULL)
			CoTaskMemFree(m_pInputUserData);
		m_pInputMediaType = pNewType;
		m_pInputUserData = pNewUserData;
		m_cbInputUserData = cbNewUserData;
		m_nFrameWidth = nFrameWidth;
		m_nFrameHeight = nFrameHeight;
		m_infmt = infmt;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetOutputType(DWORD dwOutputStreamID, IMFMediaType *pType, DWORD dwFlags)
	{
		LOGPRINTF("%p CMFTCodec::SetOutputType(dwOutputStreamID=%d, pType=%p, dwFlags=0x%08X)", this, dwOutputStreamID, pType, dwFlags);

		LockIt lck(static_cast<T *>(this));

		HRESULT hr;
		int ret;
		IMFMediaType *pNewType;
		UINT8 *pNewUserData;
		UINT32 cbNewUserData;
		GUID guidNewMajorType, guidNewSubtype;
		UINT32 nFrameWidth, nFrameHeight;
		utvf_t outfmt;

		if (dwOutputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		if (pType == NULL)
		{
			m_pOutputMediaType->Release();
			m_pOutputMediaType = NULL;
			CoTaskMemFree(m_pOutputUserData);
			m_pOutputUserData = NULL;
			return S_OK;
		}

		if (m_pInputMediaType == NULL)
			return MF_E_TRANSFORM_TYPE_NOT_SET;

		LockAttr lockattr(pType);

		// check type

		if (FAILED(pType->GetGUID(MF_MT_MAJOR_TYPE, &guidNewMajorType)))
			return MF_E_INVALIDTYPE;
		if (FAILED(pType->GetGUID(MF_MT_SUBTYPE, &guidNewSubtype)))
			return MF_E_INVALIDTYPE;
		if (FAILED(MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &nFrameWidth, &nFrameHeight)))
			return MF_E_INVALIDTYPE;

		if (!IsEqualGUID(guidNewMajorType, MFMediaType_Video))
			return MF_E_INVALIDMEDIATYPE;
		if (MediaFoundationFormatToUtVideoFormat(&outfmt, guidNewSubtype, (INT32)MFGetAttributeUINT32(pType, MF_MT_DEFAULT_STRIDE, 1) < 0) != 0)
			return MF_E_INVALIDMEDIATYPE;
		if (nFrameWidth != m_nFrameWidth || nFrameHeight != m_nFrameHeight)
			return MF_E_INVALIDMEDIATYPE;

		if (FAILED(pType->GetAllocatedBlob(MF_MT_USER_DATA, &pNewUserData, &cbNewUserData)))
		{
			pNewUserData = (UINT8 *)CoTaskMemAlloc(0);
			cbNewUserData = 0;
		}

		ret = ((T *)this)->Query(outfmt, m_infmt, m_nFrameWidth, m_nFrameHeight, pNewUserData, cbNewUserData, m_pInputUserData, m_cbInputUserData);
		if (ret != 0)
		{
			CoTaskMemFree(pNewUserData);
			return MF_E_INVALIDMEDIATYPE;
		}

		// set type

		if (dwFlags & MFT_SET_TYPE_TEST_ONLY)
		{
			CoTaskMemFree(pNewUserData);
			return S_OK;
		}

		hr = MFCreateMediaType(&pNewType);
		if (FAILED(hr))
		{
			CoTaskMemFree(pNewUserData);
			return hr;
		}
		hr = pType->CopyAllItems(pNewType);
		if (FAILED(hr))
		{
			CoTaskMemFree(pNewUserData);
			pNewType->Release();
			return hr;
		}
		if (m_pOutputMediaType != NULL)
			m_pOutputMediaType->Release();
		if (m_pOutputUserData != NULL)
			CoTaskMemFree(m_pOutputUserData);
		m_pOutputMediaType = pNewType;
		m_pOutputUserData = pNewUserData;
		m_cbOutputUserData = cbNewUserData;
		m_outfmt = outfmt;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetInputCurrentType(DWORD dwInputStreamID, IMFMediaType **ppType)
	{
		LockIt lck(static_cast<T *>(this));

		HRESULT hr;

		if (dwInputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		if (m_pInputMediaType == NULL)
			return MF_E_TRANSFORM_TYPE_NOT_SET;

		hr = MFCreateMediaType(ppType);
		if (FAILED(hr))
			return hr;
		hr = m_pInputMediaType->CopyAllItems(*ppType);
		if (FAILED(hr))
		{
			(*ppType)->Release();
			return hr;
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetOutputCurrentType(DWORD dwOutputStreamID, IMFMediaType **ppType)
	{
		LockIt lck(static_cast<T *>(this));

		HRESULT hr;

		if (dwOutputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		if (m_pOutputMediaType == NULL)
			return MF_E_TRANSFORM_TYPE_NOT_SET;

		hr = MFCreateMediaType(ppType);
		if (FAILED(hr))
			return hr;
		hr = m_pOutputMediaType->CopyAllItems(*ppType);
		if (FAILED(hr))
		{
			(*ppType)->Release();
			return hr;
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetInputStatus(DWORD dwInputStreamID, DWORD *pdwFlags)
	{
		LockIt lck(static_cast<T *>(this));

		if (dwInputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		*pdwFlags = m_pInputSample == NULL ? MFT_INPUT_STATUS_ACCEPT_DATA : 0;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetOutputStatus(DWORD *pdwFlags)
	{
		LockIt lck(static_cast<T *>(this));

		*pdwFlags = m_pInputSample == NULL ? 0 : MFT_OUTPUT_STATUS_SAMPLE_READY;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetOutputBounds(LONGLONG hnsLowerBound, LONGLONG hnsUpperBound)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE ProcessEvent(DWORD dwInputStreamID, IMFMediaEvent *pEvent)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE ProcessMessage(MFT_MESSAGE_TYPE eMessage, ULONG_PTR ulParam)
	{
		switch (eMessage)
		{
		case MFT_MESSAGE_COMMAND_FLUSH:
			return Flush();
		case MFT_MESSAGE_NOTIFY_BEGIN_STREAMING:
			return BeginStream();
		case MFT_MESSAGE_NOTIFY_END_STREAMING:
			return EndStream();
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE ProcessInput(DWORD dwInputStreamID, IMFSample *pSample, DWORD dwFlags)
	{
		LockIt lck(static_cast<T *>(this));

		BeginStream();

		if (dwInputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		if (m_pInputSample != NULL)
			return MF_E_NOTACCEPTING;

		m_pInputSample = pSample;
		m_pInputSample->AddRef();

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE ProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, MFT_OUTPUT_DATA_BUFFER *pOutputSamples, DWORD *pdwStatus)
	{
		LockIt lck(static_cast<T *>(this));

		HRESULT hr;
		IMFMediaBuffer *pInputBuffer;
		IMFMediaBuffer *pOutputBuffer;
		IMFSample *pOutputSample;
		LONGLONG ll;

		if (m_pInputSample == NULL)
			return MF_E_TRANSFORM_NEED_MORE_INPUT;

		hr = m_pInputSample->ConvertToContiguousBuffer(&pInputBuffer);
		if (FAILED(hr))
			return hr;
		hr = MFCreateSample(&pOutputSample);
		if (FAILED(hr))
		{
			pInputBuffer->Release();
			return hr;
		}

		MFCreateAlignedMemoryBuffer((DWORD)((T *)this)->GetSize(m_outfmt, m_infmt, m_nFrameWidth, m_nFrameHeight), 4, &pOutputBuffer);
		pOutputSample->AddBuffer(pOutputBuffer);

		((T *)this)->InternalProcessOutput(pOutputSample, m_pInputSample);

		if (SUCCEEDED(m_pInputSample->GetSampleTime(&ll)))
			pOutputSample->SetSampleTime(ll);
		if (SUCCEEDED(m_pInputSample->GetSampleDuration(&ll)))
			pOutputSample->SetSampleDuration(ll);

		pInputBuffer->Release();
		pOutputBuffer->Release();

		pOutputSamples->pSample = pOutputSample;

		m_pInputSample->Release();
		m_pInputSample = NULL;
		return S_OK;
	}


	HRESULT BeginStream(void)
	{
		// ���̂��Đ����� BeginStream() ���Ă΂ꑱ����̂Ń��O���o���Ȃ��B

		HRESULT hr;

		if (m_bStreamBegin)
			return S_OK;

		hr = ((T *)this)->InternalBeginStream();
		if (SUCCEEDED(hr))
			m_bStreamBegin = true;

		return hr;
	}

	HRESULT EndStream(void)
	{
		HRESULT hr;

		if (!m_bStreamBegin)
			return S_OK;

		Flush();

		hr = ((T *)this)->InternalEndStream();
		if (SUCCEEDED(hr))
			m_bStreamBegin = false;

		return S_OK;
	}

	HRESULT Flush(void)
	{
		LOGPRINTF("%p CMFTCodec::Flush()", this);

		if (m_pInputSample != NULL)
		{
			m_pInputSample->Release();
			m_pInputSample = NULL;
		}

		return S_OK;
	}
};
