/* $Id: TunedFunc.cpp 1163 2014-05-10 13:42:32Z umezawa $ */

#include "stdafx.h"
#include "utvideo.h"
#include "TunedFunc.h"
#include "Predict.h"
#include "HuffmanCode.h"
#include "Convert.h"
#include "ColorOrder.h"
#include "Coefficient.h"

const TUNEDFUNC_PREDICT tfnPredictCPP = {
	NULL,
	{ 0 },
	cpp_PredictWrongMedianAndCount,
	cpp_PredictLeftAndCount,
	cpp_RestoreWrongMedian,
	cpp_RestoreWrongMedianBlock4,
};

const TUNEDFUNC_HUFFMAN_ENCODE tfnHuffmanEncodeCPP = {
	NULL,
	{ 0 },
	cpp_HuffmanEncode<8>,
};

const TUNEDFUNC_HUFFMAN_DECODE tfnHuffmanDecodeCPP = {
	NULL,
	{ 0 },
	cpp_HuffmanDecode,
	cpp_HuffmanDecodeStep4,
	cpp_HuffmanDecodeAndAccum,
	cpp_HuffmanDecodeAndAccumStep2,
	cpp_HuffmanDecodeAndAccumStep3,
	cpp_HuffmanDecodeAndAccumStep4,
	cpp_HuffmanDecodeAndAccumStep3ForBGRBlue,
	cpp_HuffmanDecodeAndAccumStep3ForBGRRed,
	cpp_HuffmanDecodeAndAccumStep4ForBGRXBlue,
	cpp_HuffmanDecodeAndAccumStep4ForBGRXRed,
	cpp_HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha,
	cpp_HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha,
};

const TUNEDFUNC_CONVERT_YUVRGB tfnConvertYUVRGBCPP = {
	NULL,
	{ 0 },
	{
		cpp_ConvertULY2ToRGB<CBT601Coefficient, CBGRColorOrder>,
		cpp_ConvertULY2ToRGB<CBT601Coefficient, CBGRAColorOrder>,
		cpp_ConvertULY2ToRGB<CBT601Coefficient, CRGBColorOrder>,
		cpp_ConvertULY2ToRGB<CBT601Coefficient, CARGBColorOrder>,
		cpp_ConvertRGBToULY2<CBT601Coefficient, CBGRColorOrder>,
		cpp_ConvertRGBToULY2<CBT601Coefficient, CBGRAColorOrder>,
		cpp_ConvertRGBToULY2<CBT601Coefficient, CRGBColorOrder>,
		cpp_ConvertRGBToULY2<CBT601Coefficient, CARGBColorOrder>,
	},
	{
		cpp_ConvertULY2ToRGB<CBT709Coefficient, CBGRColorOrder>,
		cpp_ConvertULY2ToRGB<CBT709Coefficient, CBGRAColorOrder>,
		cpp_ConvertULY2ToRGB<CBT709Coefficient, CRGBColorOrder>,
		cpp_ConvertULY2ToRGB<CBT709Coefficient, CARGBColorOrder>,
		cpp_ConvertRGBToULY2<CBT709Coefficient, CBGRColorOrder>,
		cpp_ConvertRGBToULY2<CBT709Coefficient, CBGRAColorOrder>,
		cpp_ConvertRGBToULY2<CBT709Coefficient, CRGBColorOrder>,
		cpp_ConvertRGBToULY2<CBT709Coefficient, CARGBColorOrder>,
	},
};

const TUNEDFUNC_CONVERT_SHUFFLE tfnConvertShuffleCPP = {
	NULL,
	{ 0 },
	cpp_ConvertRGBToULRG<CBGRColorOrder>,
	cpp_ConvertRGBToULRG<CBGRAColorOrder>,
	cpp_ConvertRGBToULRG<CARGBColorOrder>,
	cpp_ConvertARGBToULRA<CBGRAColorOrder>,
	cpp_ConvertARGBToULRA<CARGBColorOrder>,
	cpp_ConvertYUV422ToULY2<CYUYVColorOrder>,
	cpp_ConvertYUV422ToULY2<CUYVYColorOrder>,
	cpp_ConvertULRGToRGB<CBGRColorOrder>,
	cpp_ConvertULRGToRGB<CBGRAColorOrder>,
	cpp_ConvertULRGToRGB<CARGBColorOrder>,
	cpp_ConvertULRAToARGB<CBGRAColorOrder>,
	cpp_ConvertULRAToARGB<CARGBColorOrder>,
	cpp_ConvertULY2ToYUV422<CYUYVColorOrder>,
	cpp_ConvertULY2ToYUV422<CUYVYColorOrder>,
};

const TUNEDFUNC_CORRELATE tfnCorrelateCPP = {
	NULL,
	{ 0 },
	cpp_EncorrelateInplaceXRGB<CBGRAColorOrder, false>,
	cpp_EncorrelateInplaceXRGB<CBGRAColorOrder, true>,
	cpp_EncorrelateInplaceXRGB<CARGBColorOrder, false>,
	cpp_EncorrelateInplaceXRGB<CARGBColorOrder, true>,
};

TUNEDFUNC tfn = {
	&tfnPredictCPP,
	&tfnHuffmanEncodeCPP,
	&tfnHuffmanDecodeCPP,
	&tfnConvertYUVRGBCPP,
	&tfnConvertShuffleCPP,
	&tfnCorrelateCPP,
};


void ResolveTunedFunc(const TUNEDFUNC *ptfnRoot, const uint32_t *pdwSupportedFeatures)
{
	const TUNEDFUNC_FRAGMENT **pSrc, *pTest;
	const TUNEDFUNC_FRAGMENT **pDst;

	pSrc = (const TUNEDFUNC_FRAGMENT **)ptfnRoot;
	pDst = (const TUNEDFUNC_FRAGMENT **)&tfn;

	for (int i = 0; i < (sizeof(TUNEDFUNC) / sizeof(void *)); i++)
	{
		for (pTest = pSrc[i]; pTest != NULL; pTest = (const TUNEDFUNC_FRAGMENT *)pTest->pNext)
		{
			int j;
			for (j = 0; j < _countof(pTest->dwRequiredFeatures); j++)
			{
				if ((pTest->dwRequiredFeatures[j] & pdwSupportedFeatures[j]) != pTest->dwRequiredFeatures[j])
					break;
			}
			if (j == _countof(pTest->dwRequiredFeatures))
				break;
		}
		if (pTest != NULL)
			pDst[i] = pTest;
	}
}
