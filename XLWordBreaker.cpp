// XLWordBreaker.cpp: XLWordBreaker �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "XLWordBreaker.h"
#include "XLNeetIUnknown.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
/*
#pragma pack(push, 8)

struct __declspec(uuid("80A3E9B0-A246-11D3-BB8C-0090272FA362"))   CLSID_WordBreaker_Win7;
struct __declspec(uuid("E1E8F15E-8BEC-45DF-83BF-50FF84D0CAB5"))   CLSID_IWordBreaker_WinVista;
struct __declspec(uuid("BE41F4E6-9EAD-498f-A473-F3CA66F9BE8B"))   CLSID_IWordBreaker_WinXP;

#pragma pack(pop)

*/

XLWordBreaker::XLWordBreaker()
{
	HRESULT hr;

	CLSID clsidWordBreaker;
	hr = CLSIDFromString(L"NLG.Japanese Wordbreaker",&clsidWordBreaker);
	if (FAILED(hr))
	{
		_com_issue_error(hr);
	}
//	hr = this->WordBreaker.CoCreateInstance(__uuidof(CLSID_IWordBreaker_WinXP));
	hr = this->WordBreaker.CoCreateInstance(clsidWordBreaker);
	if (FAILED(hr))
	{
		_com_issue_error(hr);
	}

	//������
	BOOL pfLicense = TRUE;
	hr = WordBreaker->Init(true, 2000, &pfLicense);
	if (FAILED(hr))
	{
		_com_issue_error(hr);
	}
}

XLWordBreaker::~XLWordBreaker()
{
}


bool XLWordBreaker::BreakText(const std::string & inString , XLStringList * outStringList)
{
	//�`�ԑf��͒��ɉ��x���R�[���o�b�N�����֐����`���邽�߂̃C���i�[�N���X
	class pfnFillTextBufferTmp
	{
	public:
		//�����m��񂯂ǁA�����Ԃ��悤�ɂ���炵���B
		static long __stdcall pfnFillTextBuffer(TEXT_SOURCE * pTextSource)
		{
			// return WBREAK_E_END_OF_TEXT
			return 0x80041780;
		}
	};
	TEXT_SOURCE textsource;
	//�`�ԑf��͒��ɉ��x���R�[���o�b�N�����֐�!
	textsource.pfnFillTextBuffer =  pfnFillTextBufferTmp::pfnFillTextBuffer;
	//�ϊ����镶���� ������� WCHAR* ��
	textsource.awcBuffer = _bstr_t(inString.c_str());
	//�`�ԑf��͂��J�n����n�_(WCHAR�Ȃ̂�1 ����2�o�C�g�i��)
    textsource.iCur = 0;
	//�`�ԑf��͂��I������n�_(WCHAR�Ȃ̂�1 ����2�o�C�g�i��)
    textsource.iEnd = lstrlenW(textsource.awcBuffer);	//W�ȃ��C�hAPI�ł���


	//�����R�[���o�b�N�����炵���B������I�[�o�[���C�h����B
	//���ŃR�[���o�b�N�ɂ����̂��BIRegExp�݂����Ɍ��ʂ�z��ɓ���ĕԂ��΂����̂ɁB
	//�����҂̓A�z����w
	class pfnWordSink : public XLNeetIUnknown<IWordSink>
	{
	public:
		//�`�ԑf��͂���邽�тɔ��ł��܂��B
		void pushWord(ULONG cwc, WCHAR const *pwcInBuf, ULONG cwcSrcLen, ULONG cwcSrcPos)
		{
			USES_CONVERSION;
			const char * p ;

			//�����܂�ɕϊ��R�ꂪ�N����悤�ȁB�B�B
			if (this->lastPos != cwcSrcPos)
			{
				//�ϊ��R����N���������[�h���������炵���B
				//�ϊ��R�ꂪ���������ꍇ�A�@�B�ƁA�͋�؂��Ȃ��̂ŋ����ɋ�؂�B
				std::wstring wstrFull = std::wstring(poolawcBuffer , this->lastPos , cwcSrcPos - this->lastPos);
				std::wstring wstrFullMinusOne = std::wstring(poolawcBuffer , this->lastPos , cwcSrcPos - this->lastPos - 1);
				std::wstring wstrLast = wstrFull.substr(wstrFullMinusOne.length() , 1);
				if ( wstrFullMinusOne.length() >= 2 && (wstrLast[0] == 0x3001 || wstrLast[0] == 0x3002) )
				{
					//�B�ƁA�͋�؂�
					p = W2A(wstrFullMinusOne.c_str());
					poolStringList->push_back( p );

					p = W2A(wstrLast.c_str());
					poolStringList->push_back( p );
				}
				else
				{
					p = W2A(wstrFull.c_str());
					poolStringList->push_back( p );
				}
				this->lastPos = cwcSrcPos;
			}

			std::wstring wstrFull = std::wstring(poolawcBuffer , cwcSrcPos , cwcSrcLen);
			p = W2A(wstrFull.c_str());
			poolStringList->push_back( p );

			//�����܂ŕϊ������B
			this->lastPos = cwcSrcPos + cwcSrcLen;
		}

		virtual HRESULT STDMETHODCALLTYPE PutWord(ULONG cwc, WCHAR const *pwcInBuf, ULONG cwcSrcLen, ULONG cwcSrcPos)
		{
			this->pushWord(cwc,pwcInBuf,cwcSrcLen,cwcSrcPos);
			return 0;
		}
		virtual HRESULT STDMETHODCALLTYPE PutAltWord(ULONG cwc, WCHAR const *pwcInBuf, ULONG cwcSrcLen, ULONG cwcSrcPos)
		{
			//����̈Ӗ����悭������Ȃ��B
			return 0;
		}
		virtual HRESULT STDMETHODCALLTYPE StartAltPhrase( void) 
		{
			return 0;
		}

		virtual HRESULT STDMETHODCALLTYPE EndAltPhrase( void)
		{
			return 0;
		}

		virtual HRESULT STDMETHODCALLTYPE PutBreak( WORDREP_BREAK_TYPE )
		{
		   return 0;
		}

		//�C���i�[�����S��public�ŁB

		//���܂Ɍ`�ԑf��͂̌��ʂ�����Ă��Ȃ����[�h������݂����Ȃ̂ŕ␳����.
		//�ϊ����镶����
		WCHAR const *  poolawcBuffer;
		//�퓬���牽��(�o�C�g�ɂ��炸)�܂ŕϊ��������B
		ULONG          lastPos;

		//����Ɍ`�ԑf��͂̌��ʂ�������.
		XLStringList * poolStringList;
	};


	//IWordSink���p�����č�����Ǝ��N���X�����̉��A�A�[���X�^�b�N��Ɋm�ہB
	pfnWordSink		wordSink;
	//����Ɍ��ʂ����Ă��炤.
	wordSink.poolStringList = outStringList;
	wordSink.poolawcBuffer = textsource.awcBuffer;
	wordSink.lastPos = 0;

	//�`�ԑf��͊J�n
	HRESULT hr;
	hr = this->WordBreaker->BreakText(&textsource,&wordSink,NULL);
	if (FAILED(hr))
	{
		_com_issue_error(hr);
	}
	//�悭�Ōオ�Y�ꋎ����̂ł��Ă�����B
	if (wordSink.lastPos < textsource.iEnd)
	{
		USES_CONVERSION;
		std::wstring wstrFull = std::wstring(wordSink.poolawcBuffer , wordSink.lastPos , textsource.iEnd - wordSink.lastPos);
		const char * p = W2A(wstrFull.c_str());
		outStringList->push_back( p );
	}

	return true;
}

void XLWordBreaker::test()
{
	{
		XLWordBreaker wb;
		XLStringList slist;
		std::string r;
		//�����͈̔͂��Ԉ���Ă����̂ň��S �� �� | �S �ƂȂ��Ă��܂��Ă����̂ŏC��
		{
			wb.BreakText("�S�ăN���C�A���g�T�C�h�ŉ�͂��s�����߁A�Z�L�����e�B�̊ϓ_���猩�Ă����S�ł��B�����������̒P�ʂ�MeCab + ipadic�ƌ݊���������܂��B", &slist);
			XLStringList::iterator it = slist.begin();
			ASSERT(slist.size() == 19);
			ASSERT((r = *it) == "�S��");	it++;
			ASSERT((r = *it) == "�N���C�A���g�T�C�h��");	it++;
			ASSERT((r = *it) == "��͂�");	it++;
			ASSERT((r = *it) == "�s������");	it++;
			ASSERT((r = *it) == "�A");	it++;
			ASSERT((r = *it) == "�Z�L�����e�B��");	it++;
			ASSERT((r = *it) == "�ϓ_����");	it++;
			ASSERT((r = *it) == "���Ă�");	it++;
			ASSERT((r = *it) == "���S�ł�");	it++;
			ASSERT((r = *it) == "�B");	it++;
			ASSERT((r = *it) == "������������");	it++;
			ASSERT((r = *it) == "�P�ʂ�");	it++;
			ASSERT((r = *it) == "MeCab");	it++;
			ASSERT((r = *it) == " + ");	it++;
			ASSERT((r = *it) == "ipadic��");	it++;
			ASSERT((r = *it) == "�݊�");	it++;
			ASSERT((r = *it) == "����");	it++;
			ASSERT((r = *it) == "����܂�");	it++;
			ASSERT((r = *it) == "�B");	it++;
		}
	}
	{
		//�p�[�X�e�X�g
		XLWordBreaker wb;
		XLStringList slist;
		wb.BreakText("�Ȋw�̗͂ł͂ǂ����悤���ł��Ȃ��A鳖��鲂Ȃǂ̊���Ȕy�ɗ����������ӎU�L���j�B", &slist);
		XLStringList::iterator it = slist.begin();
		std::string r;

		ASSERT(slist.size() == 13);
		ASSERT((r = *it) == "�Ȋw��");	it++;
		ASSERT((r = *it) == "�͂ł�");	it++;
		ASSERT((r = *it) == "�ǂ�");	it++;
		ASSERT((r = *it) == "���悤���ł��Ȃ�");	it++;
		ASSERT((r = *it) == "�A");	it++;
		ASSERT((r = *it) == "鳖�");	it++;
		ASSERT((r = *it) == "�鲂Ȃǂ�");	it++;
		ASSERT((r = *it) == "�����");	it++;
		ASSERT((r = *it) == "�y��");	it++;
		ASSERT((r = *it) == "����������");	it++;
		ASSERT((r = *it) == "�ӎU�L��");	it++;
		ASSERT((r = *it) == "�j");	it++;
		ASSERT((r = *it) == "�B");	it++;
	}
}
