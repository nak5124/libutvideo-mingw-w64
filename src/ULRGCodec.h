/* $Id: ULRGCodec.h 1271 2015-04-04 11:42:09Z umezawa $ */

#pragma once
#include "Codec.h"
#include "UL00Codec.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

class CULRGCodec :
    public CUL00Codec
{
public:
    static const utvf_t m_utvfCodec = UTVF_ULRG;

private:
    static const utvf_t m_utvfEncoderInput[];
    static const utvf_t m_utvfDecoderOutput[];
    static const utvf_t m_utvfCompressed[];

protected:
    CFrameBuffer *m_pRawDecoded;

public:
    CULRGCodec(const char *pszInterfaceName);
    virtual ~CULRGCodec(void) {}
    static CCodec *CreateInstance(const char *pszInterfaceName) { return new CULRGCodec(pszInterfaceName); }

    virtual int InternalDecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData);
    virtual int InternalDecodeEnd(void);

public:
    virtual const utvf_t *GetEncoderInputFormat(void) { return m_utvfEncoderInput; }
    virtual const utvf_t *GetDecoderOutputFormat(void) { return m_utvfDecoderOutput; }
    virtual const utvf_t *GetCompressedFormat(void) { return m_utvfCompressed; }

protected:
    virtual const char *GetColorFormatName(void) { return "RGB"; }
    virtual int GetRealBitCount(void) { return 24; }
    virtual int GetNumPlanes(void) { return 3; }
    virtual int GetMacroPixelWidth(void) { return 1; }
    virtual int GetMacroPixelHeight(void) { return 1; }

    virtual void CalcPlaneSizes(unsigned int width, unsigned int height);
    virtual void ConvertToPlanar(uint32_t nBandIndex);
    virtual void ConvertFromPlanar(uint32_t nBandIndex);
    virtual bool DecodeDirect(uint32_t nBandIndex);
};
