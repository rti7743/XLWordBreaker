#include "XLWordBreaker.h"

void main()
{
	//COM�̏�����
	CoInitialize(NULL);

	{
		//�e�X�g
		XLWordBreaker::test();
	}

	{

		XLWordBreaker wb;
		XLStringList slist;
		wb.BreakText("�Ȋw�̗͂ł͂ǂ����悤���ł��Ȃ��A鳖��鲂Ȃǂ̊���Ȕy�ɗ����������ӎU�L���j�B",&slist);

		for(XLStringList::iterator it = slist.begin() ; it != slist.end() ; ++it)
		{
			printf("%s\r\n" , it->c_str() );
		}
	}

	//COM�̊J��
	CoUninitialize();
}