/* $Id: UQ00Codec.cpp 1187 2014-06-08 11:53:29Z umezawa $ */

#include "stdafx.h"
#include "utvideo.h"
#include "utv_core.h"
#include "UQ00Codec.h"
#include "Predict.h"
#include "TunedFunc.h"
#include "ByteOrder.h"
#include "resource.h"

CUQ00Codec::CUQ00Codec(const char *pszTinyName, const char *pszInterfaceName) : CCodecBase(pszTinyName, pszInterfaceName)
{
	memset(&m_ec, 0, sizeof(ENCODERCONF));

	LoadConfig();
}

void CUQ00Codec::GetLongFriendlyName(char *pszName, size_t cchName)
{
	char buf[128];

	sprintf(buf, "UtVideo Pro %s 10bit %s",
		GetColorFormatName(),
		m_pszInterfaceName);
	strncpy(pszName, buf, cchName);
	pszName[cchName - 1] = '\0';
}

#ifdef _WIN32
INT_PTR CUQ00Codec::Configure(HWND hwnd)
{
	DialogBoxParam(hModule, MAKEINTRESOURCE(IDD_UQ00_CONFIG), hwnd, DialogProc, (LPARAM)this);
	return 0;
}

INT_PTR CALLBACK CUQ00Codec::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CUQ00Codec *pThis = (CUQ00Codec *)GetWindowLongPtr(hwnd, DWLP_USER);
	char buf[256];
	int	n;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		pThis = (CUQ00Codec *)lParam;
		pThis->GetLongFriendlyName(buf, _countof(buf));
		SetWindowText(hwnd, buf);
		wsprintf(buf, "%d", pThis->m_ec.ecDivideCountMinusOne + 1);
		SetDlgItemText(hwnd, IDC_DIVIDE_COUNT_EDIT, buf);
#if 0
		switch (pThis->m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK)
		{
		default:
			_ASSERT(false);
			/* FALLTHROUGH */
		case EC_FLAGS0_INTRAFRAME_PREDICT_LEFT:
			CheckDlgButton(hwnd, IDC_INTRAFRAME_PREDICT_LEFT_RADIO, BST_CHECKED);
			break;
		case EC_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
			CheckDlgButton(hwnd, IDC_INTRAFRAME_PREDICT_WRONG_MEDIAN_RADIO, BST_CHECKED);
			break;
		}
#endif
		if (pThis->m_ec.ecFlags & EC_FLAGS_DIVIDE_COUNT_IS_NUM_PROCESSORS)
		{
			CheckDlgButton(hwnd, IDC_DIVIDE_COUNT_IS_NUM_PROCESSORS, BST_CHECKED);
			EnableDlgItem(hwnd, IDC_DIVIDE_COUNT_EDIT, FALSE);
		}
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			memset(&pThis->m_ec, 0, sizeof(ENCODERCONF));
			if (IsDlgButtonChecked(hwnd, IDC_DIVIDE_COUNT_IS_NUM_PROCESSORS))
			{
				pThis->m_ec.ecFlags |= EC_FLAGS_DIVIDE_COUNT_IS_NUM_PROCESSORS;
				pThis->m_ec.ecDivideCountMinusOne = CThreadManager::GetNumProcessors() - 1;
			}
			else
			{
				GetDlgItemText(hwnd, IDC_DIVIDE_COUNT_EDIT, buf, sizeof(buf));
				n = atoi(buf);
				if (n < 1 || n > 256)
				{
					MessageBox(hwnd, "1 <= DIVIDE_COUNT <= 256", "ERR", MB_ICONERROR);
					return TRUE;
				}
				pThis->m_ec.ecDivideCountMinusOne = n - 1;
			}
#if 0
			if (IsDlgButtonChecked(hwnd, IDC_INTRAFRAME_PREDICT_LEFT_RADIO))
				pThis->m_ec.dwFlags0 |= EC_FLAGS0_INTRAFRAME_PREDICT_LEFT;
			else if (IsDlgButtonChecked(hwnd, IDC_INTRAFRAME_PREDICT_WRONG_MEDIAN_RADIO))
				pThis->m_ec.dwFlags0 |= EC_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN;
#endif
			pThis->SaveConfig();
			/* FALLTHROUGH */
		case IDCANCEL:
			EndDialog(hwnd, 0);
			return TRUE;
		case IDC_DIVIDE_COUNT_IS_NUM_PROCESSORS:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				EnableDlgItem(hwnd, IDC_DIVIDE_COUNT_EDIT, !IsDlgButtonChecked(hwnd, IDC_DIVIDE_COUNT_IS_NUM_PROCESSORS));
				if (IsDlgButtonChecked(hwnd, IDC_DIVIDE_COUNT_IS_NUM_PROCESSORS))
				{
					wsprintf(buf, "%d", CThreadManager::GetNumProcessors());
					SetDlgItemText(hwnd, IDC_DIVIDE_COUNT_EDIT, buf);
				}
			}
			break;
		}
		break;
	}

	return FALSE;
}
#endif

size_t CUQ00Codec::GetStateSize(void)
{
	return sizeof(ENCODERCONF);
}

int CUQ00Codec::GetState(void *pState, size_t cb)
{
	if (cb < sizeof(ENCODERCONF))
		return -1;

	memcpy(pState, &m_ec, sizeof(ENCODERCONF));
	return 0;
}

int CUQ00Codec::InternalSetState(const void *pState, size_t cb)
{
	ENCODERCONF ec;

	memset(&ec, 0, sizeof(ENCODERCONF));
	memcpy(&ec, pState, min(sizeof(ENCODERCONF), cb));

	if (ec.ecPreferredEncodingMode != 0)
		return -1;
	if (ec.ecReserved != 0)
		return -1;
	/* ecDivideCountMinusOne �͑S�Ă̒l���L���Ȃ̂Ń`�F�b�N���Ȃ� */
	if ((ec.ecFlags & EC_FLAGS_RESERVED) != 0)
		return -1;

	memcpy(&m_ec, &ec, sizeof(ENCODERCONF));

	if (m_ec.ecFlags & EC_FLAGS_DIVIDE_COUNT_IS_NUM_PROCESSORS)
	{
		m_ec.ecDivideCountMinusOne = CThreadManager::GetNumProcessors() - 1;
	}

	return 0;
}

size_t CUQ00Codec::EncodeFrame(void *pOutput, bool *pbKeyFrame, const void *pInput)
{
	union
	{
		FRAMEINFO fi;
		FRAMEINFO_MODE0 fi0;
	};

	uint8_t *p;
	uint32_t count[1024];
	HUFFMAN_CODELEN_TABLE<10> CodeLengthTable;

	m_pInput = pInput;
	m_pOutput = pOutput;

	memset(&fi, 0, sizeof(FRAMEINFO));

	for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		m_ptm->SubmitJob(new CThreadJob(this, &CUQ00Codec::PredictProc, nBandIndex), nBandIndex);
	m_ptm->WaitForJobCompletion();

	fi.fiEncodingMode = 0;
	fi0.fiPredictionType = PREDICT_CYLINDRICAL_LEFT;
	fi0.fiDivideCountMinusOne = m_ec.ecDivideCountMinusOne;

	p = (uint8_t *)pOutput;

	memcpy(p, &fi, sizeof(FRAMEINFO_MODE0));
	p += sizeof(FRAMEINFO_MODE0);

	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		uint32_t dwCurrentOffset;
		for (int i = 0; i < 1024; i++)
			count[i] = 0;
		for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
			for (int i = 0; i < 1024; i++)
				count[i] += m_counts[nBandIndex].dwCount[nPlaneIndex][i];
		GenerateHuffmanCodeLengthTable<10>(&CodeLengthTable, count);
		GenerateHuffmanEncodeTable<10>(&m_het[nPlaneIndex], &CodeLengthTable);
		m_pdwOffsetTable[nPlaneIndex] = (uint32_t *)p;
		dwCurrentOffset = 0;
		for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		{
			uint32_t dwBits;
			dwBits = 0;
			for (int i = 0; i < 1024; i++)
				dwBits += CodeLengthTable.codelen[i] * m_counts[nBandIndex].dwCount[nPlaneIndex][i];
			dwCurrentOffset += ROUNDUP(dwBits, 32) / 8;
			*(uint32_t *)p = htol32(dwCurrentOffset);
			p += 4;
		}
		m_pEncodedBits[nPlaneIndex] = p;
		p += dwCurrentOffset;
		m_pCodeLengthTable[nPlaneIndex] = (HUFFMAN_CODELEN_TABLE<10> *)p;
		memcpy(m_pCodeLengthTable[nPlaneIndex], &CodeLengthTable, 1024);
		p += 1024;
	}

	for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		m_ptm->SubmitJob(new CThreadJob(this, &CUQ00Codec::EncodeProc, nBandIndex), nBandIndex);
	m_ptm->WaitForJobCompletion();

	*pbKeyFrame = true;

	return p - ((uint8_t *)pOutput);
}

int CUQ00Codec::CalcFrameMetric(utvf_t rawfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData)
{
	const STREAMINFO *p = (const STREAMINFO *)pExtraData;

	CalcRawFrameMetric(rawfmt, width, height, cbGrossWidth);
	CalcPlaneSizes(width, height);

	m_dwNumStripes = height / GetMacroPixelHeight();
	m_cbRawStripeSize = m_cbRawGrossWidth * GetMacroPixelHeight();

	return 0;
}

void CUQ00Codec::CalcStripeMetric(void)
{
	for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
	{
		m_dwStripeBegin[nBandIndex] = m_dwNumStripes *  nBandIndex      / m_dwDivideCount;
		m_dwStripeEnd[nBandIndex]   = m_dwNumStripes * (nBandIndex + 1) / m_dwDivideCount;
	}
}

int CUQ00Codec::EncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth)
{
	int ret;
	STREAMINFO si;

	ret = EncodeQuery(infmt, width, height, cbGrossWidth);
	if (ret != 0)
		return ret;

	m_utvfRaw = infmt;
	m_nWidth = width;
	m_nHeight = height;

	EncodeGetExtraData(&si, sizeof(si), infmt, width, height, cbGrossWidth);
	ret = CalcFrameMetric(infmt, width, height, cbGrossWidth, &si, sizeof(si));
	if (ret != 0)
		return ret;
	m_dwDivideCount = m_ec.ecDivideCountMinusOne + 1;
	CalcStripeMetric();

	m_pCurFrame = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pCurFrame->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);

	m_pMedianPredicted = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pMedianPredicted->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);

#ifdef _WIN32
	m_counts = (COUNTS *)VirtualAlloc(NULL, sizeof(COUNTS) * m_dwDivideCount, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#endif
#if defined(__APPLE__) || defined(__unix__)
	m_counts = (COUNTS *)mmap(NULL, sizeof(COUNTS) * m_dwDivideCount, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
#endif

	m_ptm = new CThreadManager();

	return 0;
}

int CUQ00Codec::EncodeEnd(void)
{
	delete m_pCurFrame;
	delete m_pMedianPredicted;

#ifdef _WIN32
	VirtualFree(m_counts, 0, MEM_RELEASE);
#endif
#if defined(__APPLE__) || defined(__unix__)
	munmap(m_counts, sizeof(COUNTS) * m_dwDivideCount);
#endif

	delete m_ptm;

	return 0;
}

size_t CUQ00Codec::EncodeGetExtraDataSize(void)
{
	return sizeof(STREAMINFO);
}

int CUQ00Codec::EncodeGetExtraData(void *pExtraData, size_t cb, utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth)
{
	STREAMINFO *p = (STREAMINFO *)pExtraData;

	if (cb < sizeof(STREAMINFO))
		return -1;

	memset(p, 0, cb);

	p->siEncoderVersionAndImplementation = htol32(UTVIDEO_VERSION_AND_IMPLEMENTATION);
	p->siOriginalFormat                  = UNFCC(infmt);

	return 0;
}

size_t CUQ00Codec::EncodeGetOutputSize(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth)
{
	return ROUNDUP(width, 4) * ROUNDUP(height, 2) * GetRealBitCount() / 8 + 4096; // +4096 �͂ǂ�Ԃ芨��B
}

int CUQ00Codec::EncodeQuery(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth)
{
	if (width % GetMacroPixelWidth() != 0 || height % GetMacroPixelHeight() != 0)
		return -1;

	for (const utvf_t *utvf = GetEncoderInputFormat(); *utvf; utvf++)
	{
		if (infmt == *utvf)
			return 0;
	}

	return -1;
}

void CUQ00Codec::PredictProc(uint32_t nBandIndex)
{
	ConvertToPlanar(nBandIndex);

	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		size_t cbPlaneBegin = m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[nPlaneIndex];
		size_t cbPlaneEnd   = m_dwStripeEnd[nBandIndex]   * m_cbPlaneStripeSize[nPlaneIndex];

		for (int i = 0; i < 1024; i++)
			m_counts[nBandIndex].dwCount[nPlaneIndex][i] = 0;

		PredictCylindricalLeftAndCount10((uint16_t *)(m_pMedianPredicted->GetPlane(nPlaneIndex) + cbPlaneBegin), (uint16_t *)(m_pCurFrame->GetPlane(nPlaneIndex) + cbPlaneBegin), (uint16_t *)(m_pCurFrame->GetPlane(nPlaneIndex) + cbPlaneEnd), m_syInitialPredict[nPlaneIndex], m_counts[nBandIndex].dwCount[nPlaneIndex]);
	}
}

void CUQ00Codec::EncodeProc(uint32_t nBandIndex)
{
	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		size_t cbPlaneBegin = m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[nPlaneIndex];
		size_t cbPlaneEnd   = m_dwStripeEnd[nBandIndex]   * m_cbPlaneStripeSize[nPlaneIndex];

		uint32_t dwDstOffset;
#ifdef _DEBUG
		uint32_t dwDstEnd;
		uint32_t dwEncodedSize;
#endif

		if (nBandIndex == 0)
			dwDstOffset = 0;
		else
			dwDstOffset = m_pdwOffsetTable[nPlaneIndex][nBandIndex - 1];
#ifdef _DEBUG
		dwDstEnd = m_pdwOffsetTable[nPlaneIndex][nBandIndex];
		dwEncodedSize =
#endif
		HuffmanEncode10(m_pEncodedBits[nPlaneIndex] + dwDstOffset, (uint16_t *)(m_pMedianPredicted->GetPlane(nPlaneIndex) + cbPlaneBegin), (uint16_t *)(m_pMedianPredicted->GetPlane(nPlaneIndex) + cbPlaneEnd), &m_het[nPlaneIndex]);
		_ASSERT(dwEncodedSize == dwDstEnd - dwDstOffset);
	}
}

size_t CUQ00Codec::DecodeFrame(void *pOutput, const void *pInput, bool bKeyFrame)
{
	/* const */ uint8_t *p;
	union
	{
		FRAMEINFO *fi;
		FRAMEINFO_MODE0 *fi0;
	};

	m_pInput = pInput;
	m_pOutput = pOutput;

	p = (uint8_t *)pInput;
	fi = (FRAMEINFO *)p;

	if (fi->fiEncodingMode == 0)
		p += sizeof(FRAMEINFO_MODE0);
	else
		return -1;

	if (fi0->fiPredictionType != PREDICT_CYLINDRICAL_LEFT)
		return -1;

	m_dwDivideCount = fi0->fiDivideCountMinusOne + 1;
	CalcStripeMetric();

	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		m_pdwOffsetTable[nPlaneIndex] = (uint32_t *)p;
		p += sizeof(uint32_t) * m_dwDivideCount;

		m_pEncodedBits[nPlaneIndex] = p;
		p += m_pdwOffsetTable[nPlaneIndex][m_dwDivideCount - 1];

		m_pCodeLengthTable[nPlaneIndex] = (HUFFMAN_CODELEN_TABLE<10> *)p;
		GenerateHuffmanDecodeTable<10>(&m_hdt[nPlaneIndex], m_pCodeLengthTable[nPlaneIndex]);
		p += 1024;
	}

	for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		m_ptm->SubmitJob(new CThreadJob(this, &CUQ00Codec::DecodeProc, nBandIndex), nBandIndex);
	m_ptm->WaitForJobCompletion();

	return m_cbRawSize;
}

int CUQ00Codec::DecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData)
{
	int ret;

	ret = DecodeQuery(outfmt, width, height, cbGrossWidth, pExtraData, cbExtraData);
	if (ret != 0)
		return ret;

	ret = CalcFrameMetric(outfmt, width, height, cbGrossWidth, pExtraData, cbExtraData);
	if (ret != 0)
		return ret;

	m_utvfRaw = outfmt;
	m_nWidth = width;
	m_nHeight = height;

	m_pRestoredFrame = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pRestoredFrame->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);

	m_pDecodedFrame = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pDecodedFrame->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);

	m_ptm = new CThreadManager();

	return 0;
}

int CUQ00Codec::DecodeEnd(void)
{
	delete m_pRestoredFrame;
	delete m_pDecodedFrame;

	delete m_ptm;

	return 0;
}

size_t CUQ00Codec::DecodeGetOutputSize(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth)
{
	int ret;

	ret = CalcRawFrameMetric(outfmt, width, height, cbGrossWidth);
	if (ret != 0)
		return 0;

	return m_cbRawSize;
}

int CUQ00Codec::DecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData)
{
	if (width % GetMacroPixelWidth() != 0 || height % GetMacroPixelHeight() != 0)
		return -1;

	if (!outfmt)
		return 0;

	for (const utvf_t *utvf = GetDecoderOutputFormat(); *utvf; utvf++)
	{
		if (outfmt == *utvf)
			return 0;
	}

	return -1;
}

void CUQ00Codec::DecodeProc(uint32_t nBandIndex)
{
	if (DecodeDirect(nBandIndex))
		return;

	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		size_t cbPlaneBegin = m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[nPlaneIndex];
		size_t cbPlaneEnd   = m_dwStripeEnd[nBandIndex]   * m_cbPlaneStripeSize[nPlaneIndex];

		uint32_t dwOffset;

		if (nBandIndex == 0)
			dwOffset = 0;
		else
			dwOffset = m_pdwOffsetTable[nPlaneIndex][nBandIndex - 1];

		cpp_HuffmanDecodeAndAccum10((uint16_t *)(m_pDecodedFrame->GetPlane(nPlaneIndex) + cbPlaneBegin), (uint16_t *)(m_pDecodedFrame->GetPlane(nPlaneIndex) + cbPlaneEnd), m_pEncodedBits[nPlaneIndex] + dwOffset, &m_hdt[nPlaneIndex], cbPlaneEnd - cbPlaneBegin, cbPlaneEnd - cbPlaneBegin, m_syInitialPredict[nPlaneIndex]);
		m_pCurFrame = m_pDecodedFrame;
	}

	ConvertFromPlanar(nBandIndex);
}

bool CUQ00Codec::DecodeDirect(uint32_t nBandIndex)
{
	return false;
}
