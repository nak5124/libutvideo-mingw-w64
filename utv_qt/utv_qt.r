/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: utv_qt.r 1213 2015-01-03 08:48:31Z umezawa $ */

#define thng_RezTemplateVersion 2
#define cfrg_RezTemplateVersion 1

#if (Environ_OS_Mac)
#include <QuickTime/QuickTimeComponents.r>
#include <QuickTime/ImageCodec.r>
#define platformTarget platformIA32NativeEntryPoint
#elif defined(Environ_OS_Win32)
#include "QuickTimeComponents.r"
#include "ImageCodec.r"
#define platformTarget platformWin32
#else
#error
#endif

#define k32ARGBPixelFormat 32
#define k24RGBPixelFormat  24

resource 'cdci' (256) {
	"Ut Video Codec Suite",				// Name
	1,									// Version
	1,									// Revision level
	'Umzw',								// Manufacturer
	codecInfoDoes32,					// Decompression Flags
	0,									// Compression Flags
	codecInfoDepth32,					// Format Flags
	128,								// Compression Accuracy
	128,								// Decomression Accuracy
	200,								// Compression Speed
	200,								// Decompression Speed
	128,								// Compression Level
	0,									// Reserved
	1,									// Minimum Height
	1,									// Minimum Width
	0,									// Decompression Pipeline Latency
	0,									// Compression Pipeline Latency
	0									// Private Data
};

resource 'dlle' (256) {
	"QTDecoderComponentDispatch"
};

resource 'dlle' (257) {
	"QTEncoderComponentDispatch"
};

#define SUBTYPE      ULRA
#define SUBTYPE_VAL  'ULRA'
#define SUBTYPE_STR  "ULRA"
#define SUBTYPE_RID  256
#define SUBTYPE_INFO "RGBA"
#define SUBTYPE_ENC  { k32ARGBPixelFormat }
#define SUBTYPE_DEC  { k32ARGBPixelFormat }
#include "utv_qt.in.r"

#define SUBTYPE      ULRG
#define SUBTYPE_VAL  'ULRG'
#define SUBTYPE_STR  "ULRG"
#define SUBTYPE_RID  257
#define SUBTYPE_INFO "RGB"
#define SUBTYPE_ENC  { k32ARGBPixelFormat, k24RGBPixelFormat }
#define SUBTYPE_DEC  { k32ARGBPixelFormat, k24RGBPixelFormat }
#include "utv_qt.in.r"

#define SUBTYPE      ULY0
#define SUBTYPE_VAL  'ULY0'
#define SUBTYPE_STR  "ULY0"
#define SUBTYPE_RID  258
#define SUBTYPE_INFO "YUV420 BT.601"
#define SUBTYPE_ENC  { 'yuvs', '2vuy', k32ARGBPixelFormat, k24RGBPixelFormat }
#define SUBTYPE_DEC  { 'yuvs', '2vuy', k32ARGBPixelFormat, k24RGBPixelFormat }
#include "utv_qt.in.r"

#define SUBTYPE      ULY2
#define SUBTYPE_VAL  'ULY2'
#define SUBTYPE_STR  "ULY2"
#define SUBTYPE_RID  259
#define SUBTYPE_INFO "YUV422 BT.601"
#define SUBTYPE_ENC  { 'yuvs', '2vuy', k32ARGBPixelFormat, k24RGBPixelFormat }
#define SUBTYPE_DEC  { 'yuvs', '2vuy', k32ARGBPixelFormat, k24RGBPixelFormat }
#include "utv_qt.in.r"

#define SUBTYPE      ULH0
#define SUBTYPE_VAL  'ULH0'
#define SUBTYPE_STR  "ULH0"
#define SUBTYPE_RID  260
#define SUBTYPE_INFO "YUV420 BT.709"
#define SUBTYPE_ENC  { /*'YV12',*/ 'HDYC', 'yuvs', '2vuy', k32ARGBPixelFormat, k24RGBPixelFormat }
#define SUBTYPE_DEC  { /*'YV12',*/ 'HDYC', 'yuvs', '2vuy', k32ARGBPixelFormat, k24RGBPixelFormat }
#include "utv_qt.in.r"

#define SUBTYPE      ULH2
#define SUBTYPE_VAL  'ULH2'
#define SUBTYPE_STR  "ULH2"
#define SUBTYPE_RID  261
#define SUBTYPE_INFO "YUV422 BT.709"
#define SUBTYPE_ENC  { 'HDYC', 'yuvs', '2vuy', k32ARGBPixelFormat, k24RGBPixelFormat }
#define SUBTYPE_DEC  { 'HDYC', 'yuvs', '2vuy', k32ARGBPixelFormat, k24RGBPixelFormat }
#include "utv_qt.in.r"

#define SUBTYPE      UQY2
#define SUBTYPE_VAL  'UQY2'
#define SUBTYPE_STR  "UQY2"
#define SUBTYPE_RID  262
#define SUBTYPE_INFO "Pro YUV422 10bit"
#define SUBTYPE_ENC  { 'v210' }
#define SUBTYPE_DEC  { 'v210' }
#include "utv_qt.in.r"
