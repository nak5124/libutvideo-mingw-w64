/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: MFTEncoder.h 1143 2014-04-06 13:35:12Z umezawa $ */

// MFTEncoder.h : CMFTEncoder �̐錾

#pragma once
#include "resource.h"       // ���C�� �V���{��

#include "utvideo.h"
#include "utv_mft.h"
#include <vfw.h>
#include "Codec.h"
#include "ClsID.h"

#include "MFTCodec.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "DCOM �̊��S�T�|�[�g���܂�ł��Ȃ� Windows Mobile �v���b�g�t�H�[���̂悤�� Windows CE �v���b�g�t�H�[���ł́A�P��X���b�h COM �I�u�W�F�N�g�͐������T�|�[�g����Ă��܂���BATL ���P��X���b�h COM �I�u�W�F�N�g�̍쐬���T�|�[�g���邱�ƁA����т��̒P��X���b�h COM �I�u�W�F�N�g�̎����̎g�p�������邱�Ƃ���������ɂ́A_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ���`���Ă��������B���g�p�� rgs �t�@�C���̃X���b�h ���f���� 'Free' �ɐݒ肳��Ă���ADCOM Windows CE �ȊO�̃v���b�g�t�H�[���ŃT�|�[�g�����B��̃X���b�h ���f���Ɛݒ肳��Ă��܂����B"
#endif



// CMFTEncoder
// <FCC>-E992-460D-840B-C1C6497457EF

class ATL_NO_VTABLE CMFTEncoder :
	public CMFTCodec<CMFTEncoder>
{
public:
	CMFTEncoder(DWORD fcc, REFCLSID clsid) : CMFTCodec(fcc, clsid) {}
	virtual ~CMFTEncoder() {}

	BEGIN_COM_MAP(CMFTEncoder)
		COM_INTERFACE_ENTRY(IMFTransform)
	END_COM_MAP()

public:
	// CMFTCodec
	static const GUID &MFTCATEGORY /* = MFT_CATEGORY_VIDEO_ENCODER */;
	static const UINT IDR = IDR_MFTENCODER;
	static const bool bEncoding = true;

	static const utvf_t *GetInputFormatInfo(CCodec *pCodec) { return pCodec->GetEncoderInputFormat(); }
	static const utvf_t *GetOutputFormatInfo(CCodec *pCodec) { return pCodec->GetCompressedFormat(); }
	static const void GetName(CCodec *pCodec, WCHAR *szCodecName, size_t cchCodecName) { pCodec->GetLongFriendlyName(szCodecName, cchCodecName); }

	size_t GetExtraDataSize(void)
	{
		return m_pCodec->EncodeGetExtraDataSize();
	}

	int GetExtraData(void *pExtraData, size_t cbExtraData, utvf_t outfmt, utvf_t infmt, unsigned int width, unsigned int height)
	{
		return m_pCodec->EncodeGetExtraData(pExtraData, cbExtraData, infmt, width, height, CBGROSSWIDTH_WINDOWS);
	}

	size_t GetSize(utvf_t outfmt, utvf_t infmt, unsigned int width, unsigned int height)
	{
		return m_pCodec->EncodeGetOutputSize(infmt, width, height, CBGROSSWIDTH_WINDOWS);
	}

	int Query(utvf_t outfmt, utvf_t infmt, unsigned int width, unsigned int height, const void *pOutExtraData, size_t cbOutExtraData, const void *pInExtraData, size_t cbInExtraData)
	{
		return m_pCodec->EncodeQuery(infmt, width, height, CBGROSSWIDTH_WINDOWS);
	}

public:
	HRESULT InternalBeginStream(void);
	HRESULT InternalEndStream(void);
	HRESULT InternalProcessOutput(IMFSample *pOutputSample, IMFSample *pInputSample);
};


template<DWORD fcc, const CLSID *pclsid>
class ATL_NO_VTABLE CSpecializedMFTEncoder :
	public CMFTEncoder,
	public CComCoClass<CSpecializedMFTEncoder<fcc, pclsid>, pclsid>
{
public:
	CSpecializedMFTEncoder() : CMFTEncoder(fcc, *pclsid) {}

	static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
	{
		return CMFTEncoder::UpdateRegistry(fcc, *pclsid, bRegister);
	}
};

#define MFTENCODER_ENTRY_AUTO(fcc, fccname) \
	typedef CSpecializedMFTEncoder<fcc, &CLSID_##fccname##MFTEncoder> C##fccname##MFTEncoder; \
	OBJECT_ENTRY_AUTO(CLSID_##fccname##MFTEncoder, C##fccname##MFTEncoder)

MFTENCODER_ENTRY_AUTO(FCC('ULRA'), ULRA);
MFTENCODER_ENTRY_AUTO(FCC('ULRG'), ULRG);
MFTENCODER_ENTRY_AUTO(FCC('ULY0'), ULY0);
MFTENCODER_ENTRY_AUTO(FCC('ULY2'), ULY2);
MFTENCODER_ENTRY_AUTO(FCC('ULH0'), ULH0);
MFTENCODER_ENTRY_AUTO(FCC('ULH2'), ULH2);
MFTENCODER_ENTRY_AUTO(FCC('UQY2'), UQY2);
