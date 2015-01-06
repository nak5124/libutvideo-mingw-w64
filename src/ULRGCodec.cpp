/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: ULRGCodec.cpp 1129 2014-02-25 13:00:19Z umezawa $ */

#include "stdafx.h"
#include "utvideo.h"
#include "ULRGCodec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"

const utvf_t CULRGCodec::m_utvfEncoderInput[] = {
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

const utvf_t CULRGCodec::m_utvfDecoderOutput[] = {
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

const utvf_t CULRGCodec::m_utvfCompressed[] = {
	UTVF_ULRG,
	UTVF_INVALID,
};

CULRGCodec::CULRGCodec(const char *pszInterfaceName) : CUL00Codec("ULRG", pszInterfaceName)
{
}

void CULRGCodec::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_cbPlaneSize[0]          = width * height;
	m_cbPlaneSize[1]          = width * height;
	m_cbPlaneSize[2]          = width * height;

	m_cbPlaneWidth[0]         = width;
	m_cbPlaneWidth[1]         = width;
	m_cbPlaneWidth[2]         = width;

	m_cbPlaneStripeSize[0]    = width;
	m_cbPlaneStripeSize[1]    = width;
	m_cbPlaneStripeSize[2]    = width;

	m_cbPlanePredictStride[0] = width;
	m_cbPlanePredictStride[1] = width;
	m_cbPlanePredictStride[2] = width;
}

void CULRGCodec::ConvertToPlanar(uint32_t nBandIndex)
{
	uint8_t *g, *b, *r;
	const uint8_t *pSrcBegin, *pSrcEnd;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd   = ((uint8_t *)m_pInput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGR_BU:
		ConvertBGRToULRG(g, b, r, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertBGRXToULRG(g, b, r, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertBGRToULRG(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertBGRXToULRG(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertRGBToULRG(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertXRGBToULRG(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	}
}

void CULRGCodec::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *g, *b, *r;
	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pDstEnd   = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGR_BU:
		ConvertULRGToBGR(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertULRGToBGRX(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertULRGToBGR(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertULRGToBGRX(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertULRGToRGB(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULRGToXRGB(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	}
}

int CULRGCodec::DecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData)
{
	int ret;

	ret = CUL00Codec::DecodeBegin(outfmt, width, height, cbGrossWidth, pExtraData, cbExtraData);
	if (ret != 0)
		return ret;

	m_pRawDecoded = new CFrameBuffer();
	m_pRawDecoded->AddPlane(m_cbRawSize, 0);

	return 0;
}

int CULRGCodec::DecodeEnd(void)
{
	delete m_pRawDecoded;

	return CUL00Codec::DecodeEnd();
}

bool CULRGCodec::DecodeDirect(uint32_t nBandIndex)
{
	uint8_t *pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	uint8_t *pDstEnd   = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;

	uint8_t *pDecBegin = ((uint8_t *)m_pRawDecoded->GetBuffer()) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	uint8_t *pDecEnd   = ((uint8_t *)m_pRawDecoded->GetBuffer()) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;

	switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
			HuffmanDecodeAndAccumStep3          (pDstEnd-m_cbRawGrossWidth+1, pDstBegin-m_cbRawGrossWidth+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep3ForBGRBlue(pDstEnd-m_cbRawGrossWidth+0, pDstBegin-m_cbRawGrossWidth+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep3ForBGRRed (pDstEnd-m_cbRawGrossWidth+2, pDstBegin-m_cbRawGrossWidth+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_BU:
			HuffmanDecodeAndAccumStep4                       (pDstEnd-m_cbRawGrossWidth+1, pDstBegin-m_cbRawGrossWidth+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForBGRXBlue            (pDstEnd-m_cbRawGrossWidth+0, pDstBegin-m_cbRawGrossWidth+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha(pDstEnd-m_cbRawGrossWidth+2, pDstBegin-m_cbRawGrossWidth+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGR_TD:
			HuffmanDecodeAndAccumStep3          (pDstBegin+1, pDstEnd+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep3ForBGRBlue(pDstBegin+0, pDstEnd+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep3ForBGRRed (pDstBegin+2, pDstEnd+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_TD:
			HuffmanDecodeAndAccumStep4                       (pDstBegin+1, pDstEnd+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForBGRXBlue            (pDstBegin+0, pDstEnd+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha(pDstBegin+2, pDstEnd+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_RGB_TD:
			HuffmanDecodeAndAccumStep3          (pDstBegin+1, pDstEnd+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep3ForRGBBlue(pDstBegin+2, pDstEnd+2, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep3ForRGBRed (pDstBegin+0, pDstEnd+0, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_ARGB_TD:
			HuffmanDecodeAndAccumStep4                       (pDstBegin+2, pDstEnd+2, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForXRGBBlue            (pDstBegin+3, pDstEnd+3, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha(pDstBegin+1, pDstEnd+1, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGRX_BU:
			HuffmanDecodeStep4(pDecEnd-m_cbRawGrossWidth+1, pDecBegin-m_cbRawGrossWidth+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			HuffmanDecodeStep4(pDecEnd-m_cbRawGrossWidth+0, pDecBegin-m_cbRawGrossWidth+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			HuffmanDecodeStep4(pDecEnd-m_cbRawGrossWidth+2, pDecBegin-m_cbRawGrossWidth+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			RestoreWrongMedianBlock4(pDstEnd-m_cbRawGrossWidth, pDecEnd-m_cbRawGrossWidth, pDecBegin-m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			EncorrelateInplaceBGRX(pDstEnd-m_cbRawGrossWidth, pDstBegin-m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_TD:
			HuffmanDecodeStep4(pDecBegin+1, pDecEnd+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeStep4(pDecBegin+0, pDecEnd+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeStep4(pDecBegin+2, pDecEnd+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_cbRawNetWidth, m_cbRawGrossWidth);
			RestoreWrongMedianBlock4(pDstBegin, pDecBegin, pDecEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
			EncorrelateInplaceBGRX(pDstBegin, pDstEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_ARGB_TD:
			HuffmanDecodeStep4(pDecBegin+2, pDecEnd+2, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeStep4(pDecBegin+3, pDecEnd+3, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeStep4(pDecBegin+1, pDecEnd+1, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_cbRawNetWidth, m_cbRawGrossWidth);
			RestoreWrongMedianBlock4(pDstBegin, pDecBegin, pDecEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
			EncorrelateInplaceXRGB(pDstBegin, pDstEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		}
	}

	return false;
}
