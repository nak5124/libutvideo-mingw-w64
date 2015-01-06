/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: CodecBase.h 1182 2014-05-29 12:49:01Z umezawa $ */

#pragma once
#include "Codec.h"

class CCodecBase :
	public CCodec
{
protected:
	const char *const m_pszTinyName;
	const char *const m_pszInterfaceName;

	bool m_bBottomUpFrame;
	size_t m_cbRawSize;
	size_t m_cbRawGrossWidth;
	size_t m_cbRawNetWidth;

protected:
	CCodecBase(const char *pszTinyName, const char *pszInterfaceName);
	virtual ~CCodecBase(void);

public:
	virtual const char *GetTinyName(void);
	virtual void GetShortFriendlyName(char *pszName, size_t cchName);
	virtual void GetShortFriendlyName(wchar_t *pszName, size_t cchName);
	virtual void GetLongFriendlyName(char *pszName, size_t cchName) = 0;
	virtual void GetLongFriendlyName(wchar_t *pszName, size_t cchName);
	virtual int SetState(const void *pState, size_t cb);

protected:
	int LoadConfig(void);
	int SaveConfig(void);
	virtual int InternalSetState(const void *pState, size_t cb) = 0;
	int CalcRawFrameMetric(utvf_t rawfmt, unsigned int width, unsigned int height, size_t cbGrossWidth);
};
