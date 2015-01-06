/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: VCMCodec.h 710 2011-08-19 13:25:46Z umezawa $ */

#pragma once
#include "Codec.h"

class CVCMCodec
{
private:
	const DWORD m_fcc;
	CCodec *m_pCodec;

private:
	CVCMCodec(DWORD fccHandler);
public:
	~CVCMCodec(void);

public:
	static CVCMCodec *Open(ICOPEN *icopen);

	LRESULT QueryAbout(void);
	LRESULT About(HWND hwnd);
	LRESULT GetInfo(ICINFO *icinfo, SIZE_T cb);

	LRESULT QueryConfigure(void);
	LRESULT Configure(HWND hwnd);
	LRESULT GetStateSize(void);
	LRESULT GetState(void *pState, SIZE_T cb);
	LRESULT SetState(const void *pState, SIZE_T cb);
	LRESULT Compress(const ICCOMPRESS *icc, SIZE_T cb);
	LRESULT CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	LRESULT CompressEnd(void);
	LRESULT CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	LRESULT CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	LRESULT CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);

	LRESULT Decompress(const ICDECOMPRESS *icd, SIZE_T cb);
	LRESULT DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	LRESULT DecompressEnd(void);
	LRESULT DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	LRESULT DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
};
