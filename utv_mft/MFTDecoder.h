/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: MFTDecoder.h 1143 2014-04-06 13:35:12Z umezawa $ */

// MFTDecoder.h : CMFTDecoder �̐錾

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



// CMFTDecoder
// <FCC>-D992-460D-840B-C1C6497457EF

class ATL_NO_VTABLE CMFTDecoder :
	public CMFTCodec<CMFTDecoder>
{
public:
	CMFTDecoder(DWORD fcc, REFCLSID clsid) : CMFTCodec(fcc, clsid) {}
	virtual ~CMFTDecoder() {}

	BEGIN_COM_MAP(CMFTDecoder)
		COM_INTERFACE_ENTRY(IMFTransform)
	END_COM_MAP()

public:
	// CMFTCodec
	static const GUID &MFTCATEGORY /* = MFT_CATEGORY_VIDEO_DECODER */;
	static const UINT IDR = IDR_MFTDECODER;
	static const bool bEncoding = false;

	static const utvf_t *GetInputFormatInfo(CCodec *pCodec) { return pCodec->GetCompressedFormat(); }
	static const utvf_t *GetOutputFormatInfo(CCodec *pCodec) { return pCodec->GetDecoderOutputFormat(); }
	static const void GetName(CCodec *pCodec, WCHAR *szCodecName, size_t cchCodecName) { wsprintfW(szCodecName, L"%S Decoder MFT", pCodec->GetTinyName()); }

	size_t GetExtraDataSize(void)
	{
		return 0;
	}

	int GetExtraData(void *pExtraData, size_t cbExtraData, utvf_t outfmt, utvf_t infmt, unsigned int width, unsigned int height)
	{
		return 0;
	}

	size_t GetSize(utvf_t outfmt, utvf_t infmt, unsigned int width, unsigned int height)
	{
		return m_pCodec->DecodeGetOutputSize(outfmt, width, height, CBGROSSWIDTH_WINDOWS);
	}

	int Query(utvf_t outfmt, utvf_t infmt, unsigned int width, unsigned int height, const void *pOutExtraData, size_t cbOutExtraData, const void *pInExtraData, size_t cbInExtraData)
	{
		return m_pCodec->DecodeQuery(outfmt, width, height, CBGROSSWIDTH_WINDOWS, pInExtraData, cbInExtraData);
	}

public:
	HRESULT InternalBeginStream(void);
	HRESULT InternalEndStream(void);
	HRESULT InternalProcessOutput(IMFSample *pOutputSample, IMFSample *pInputSample);
};


template<DWORD fcc, const CLSID *pclsid>
class ATL_NO_VTABLE CSpecializedMFTDecoder :
	public CMFTDecoder,
	public CComCoClass<CSpecializedMFTDecoder<fcc, pclsid>, pclsid>
{
public:
	CSpecializedMFTDecoder() : CMFTDecoder(fcc, *pclsid) {}

	static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
	{
		return CMFTDecoder::UpdateRegistry(fcc, *pclsid, bRegister);
	}
};

#define MFTDECODER_ENTRY_AUTO(fcc, fccname) \
	typedef CSpecializedMFTDecoder<fcc, &CLSID_##fccname##MFTDecoder> C##fccname##MFTDecoder; \
	OBJECT_ENTRY_AUTO(CLSID_##fccname##MFTDecoder, C##fccname##MFTDecoder)

MFTDECODER_ENTRY_AUTO(FCC('ULRA'), ULRA);
MFTDECODER_ENTRY_AUTO(FCC('ULRG'), ULRG);
MFTDECODER_ENTRY_AUTO(FCC('ULY0'), ULY0);
MFTDECODER_ENTRY_AUTO(FCC('ULY2'), ULY2);
MFTDECODER_ENTRY_AUTO(FCC('ULH0'), ULH0);
MFTDECODER_ENTRY_AUTO(FCC('ULH2'), ULH2);
MFTDECODER_ENTRY_AUTO(FCC('UQY2'), UQY2);
