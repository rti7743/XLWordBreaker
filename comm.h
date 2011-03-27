#ifndef __COMM_INCLUDE__
#define __COMM_INCLUDE__

//STL
	#pragma warning (disable: 4786) 
	#pragma warning (disable: 4503) 
	#include <deque>
	#include <vector>
	#include <map>
	#include <string>
	#include <set>
	#include <algorithm>
	#include <functional>
	#include <utility>
	#include <new>
	#include <memory>
	using namespace std;

//WINDOWS
	#include <windows.h>
//	#include <winsock.h>

//C
	#include <stdlib.h>
	#include <stdio.h>
	#include <time.h>

//WINCE
#ifdef _WIN32_WCE
	#include <stdarg.h>
	#include <aygshell.h>
	#include <sipapi.h>
#endif


//x‰‡
#ifdef _WIN32_WCE
	inline static string num2str(int num)
	{
		char buf[20];
		return _itoa(num , buf , 10);
	}
#else
	inline static string num2str(int num)
	{
		char buf[20];
		return itoa(num , buf , 10);
	}
	inline static string num2str(unsigned int num)
	{
		char buf[20];
		return ultoa(num , buf , 10);
	}
#endif

static string format(const char* Format,...)
{
	char buffer[1024];
	_vsnprintf(buffer,1024,Format,(char*)(&Format+1));

	return buffer;
}

#ifdef _DEBUG
	static void TRACE(const char* Format,...)
	{
		char buffer[1024];
		_vsnprintf(buffer,1024,Format,(char*)(&Format+1));
		buffer[1023] = 0;
		OutputDebugString( buffer );
	}
	static void TRACE(const string & inStr)
	{
		TRACE(inStr.c_str());
	}

	#define ASSERT(f) \
		if (!(f) ) \
		{ \
			MSG msg;	\
			BOOL bQuit = PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);	\
			if (bQuit)	PostQuitMessage(msg.wParam);	\
			__asm { int 3 }; \
		} 
#else
	#define ASSERT(f) 
	#define TRACE	
#endif



//•Ö—˜‚ÈŒ^
typedef vector<char>		XLBuffer;
typedef map<string,string>	XLStringMap;
typedef map<string,int>		XLIntMap;
typedef deque<string>		XLStringList;
typedef vector<int>			XLArray;

//2ŸŒ³ƒe[ƒuƒ‹‚Ì”‰ñ”‚Ìæ“¾
#define TABLE_COUNT_OF(table)	(sizeof((table)) / sizeof((table)[0]))

//TRUE ‚ğ true ‚É
#define Btob(x)	((x) != FALSE)
//true ‚ğ TRUE ‚É
#define btoB(x)	((DWORD) x)

#define ETRACE	(format("throws::%s:%d " , __FILE__ , __LINE__) )



#endif //__COMM_INCLUDE__