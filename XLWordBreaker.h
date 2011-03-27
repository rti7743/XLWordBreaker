// XLWordBreaker.h: XLWordBreaker クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLWORDBREAKER_H__2FA97595_1CAE_4251_9992_3A7DAF6871DB__INCLUDED_)
#define AFX_XLWORDBREAKER_H__2FA97595_1CAE_4251_9992_3A7DAF6871DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comm.h"
#include <atlbase.h>
#include <comdef.h>
#include <indexsrv.h>

class XLWordBreaker  
{
public:
	XLWordBreaker();
	virtual ~XLWordBreaker();

	bool Init();
	bool BreakText(const std::string & inString, XLStringList * outStringList);
	static void test();
private:

	CComPtr<IWordBreaker>	WordBreaker;
};

#endif // !defined(AFX_XLWORDBREAKER_H__2FA97595_1CAE_4251_9992_3A7DAF6871DB__INCLUDED_)
