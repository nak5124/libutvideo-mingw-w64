/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: DMOCodec.cpp 917 2012-09-23 14:17:31Z umezawa $ */

// DMOCodec.cpp : CDMOCodec �̎���

#include "stdafx.h"
#include "DMOCodec.h"
#include <Format.h>

void FormatInfoToPartialMediaType(const utvf_t *putvf, DWORD *pcTypes, DMO_PARTIAL_MEDIATYPE **ppTypes)
{
	const utvf_t *p;
	DMO_PARTIAL_MEDIATYPE *ppmt;
	DWORD n;

	p = putvf;
	n = 0;
	while (*p)
	{
		n++;
		p++;
	}

	*pcTypes = 0;
	*ppTypes = new DMO_PARTIAL_MEDIATYPE[n];

	ppmt = *ppTypes;
	for (p = putvf; *p; p++)
	{
		if (UtVideoFormatToDirectShowFormat(&ppmt->subtype, *p) == 0)
		{
			ppmt->type = MEDIATYPE_Video;
			ppmt++;
			(*pcTypes)++;
		}
	}
}
