#include "XLWordBreaker.h"

void main()
{
	//COMの初期化
	CoInitialize(NULL);

	{
		//テスト
		XLWordBreaker::test();
	}

	{

		XLWordBreaker wb;
		XLStringList slist;
		wb.BreakText("科学の力ではどうしようもできない、魑魅魍魎などの奇怪な輩に立ち向かう胡散臭い男。",&slist);

		for(XLStringList::iterator it = slist.begin() ; it != slist.end() ; ++it)
		{
			printf("%s\r\n" , it->c_str() );
		}
	}

	//COMの開放
	CoUninitialize();
}