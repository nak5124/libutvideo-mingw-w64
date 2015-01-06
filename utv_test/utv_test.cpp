/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id: utv_test.cpp 807 2011-11-26 17:36:37Z umezawa $ */

// utv_test.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <utvideo.h>
#include "utv_core.h"

int main(int argc, char **argv)
{
#ifdef _DEBUG
	UnitTest_core();
#endif

	return 0;
}
