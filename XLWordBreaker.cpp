// XLWordBreaker.cpp: XLWordBreaker クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "XLWordBreaker.h"
#include "XLNeetIUnknown.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
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

	//初期化
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
	//形態素解析中に何度かコールバックされる関数を定義するためのインナークラス
	class pfnFillTextBufferTmp
	{
	public:
		//何か知らんけど、これを返すようにするらしい。
		static long __stdcall pfnFillTextBuffer(TEXT_SOURCE * pTextSource)
		{
			// return WBREAK_E_END_OF_TEXT
			return 0x80041780;
		}
	};
	TEXT_SOURCE textsource;
	//形態素解析中に何度かコールバックされる関数!
	textsource.pfnFillTextBuffer =  pfnFillTextBufferTmp::pfnFillTextBuffer;
	//変換する文字列 もちろん WCHAR* で
	textsource.awcBuffer = _bstr_t(inString.c_str());
	//形態素解析を開始する地点(WCHARなので1 だと2バイト進む)
    textsource.iCur = 0;
	//形態素解析を終了する地点(WCHARなので1 だと2バイト進む)
    textsource.iEnd = lstrlenW(textsource.awcBuffer);	//WなワイドAPIでやるよ


	//何かコールバックされるらしい。だからオーバーライドする。
	//何でコールバックにしたのか。IRegExpみたいに結果を配列に入れて返せばいいのに。
	//実装者はアホだなw
	class pfnWordSink : public XLNeetIUnknown<IWordSink>
	{
	public:
		//形態素解析されるたびに飛んでいます。
		void pushWord(ULONG cwc, WCHAR const *pwcInBuf, ULONG cwcSrcLen, ULONG cwcSrcPos)
		{
			USES_CONVERSION;
			const char * p ;

			//何かまれに変換漏れが起きるような。。。
			if (this->lastPos != cwcSrcPos)
			{
				//変換漏れを起こしたワードがあったらしい。
				//変換漏れが発生した場合、　。と、は区切られないので強引に区切る。
				std::wstring wstrFull = std::wstring(poolawcBuffer , this->lastPos , cwcSrcPos - this->lastPos);
				std::wstring wstrFullMinusOne = std::wstring(poolawcBuffer , this->lastPos , cwcSrcPos - this->lastPos - 1);
				std::wstring wstrLast = wstrFull.substr(wstrFullMinusOne.length() , 1);
				if ( wstrFullMinusOne.length() >= 2 && (wstrLast[0] == 0x3001 || wstrLast[0] == 0x3002) )
				{
					//。と、は区切る
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

			//ここまで変換した。
			this->lastPos = cwcSrcPos + cwcSrcLen;
		}

		virtual HRESULT STDMETHODCALLTYPE PutWord(ULONG cwc, WCHAR const *pwcInBuf, ULONG cwcSrcLen, ULONG cwcSrcPos)
		{
			this->pushWord(cwc,pwcInBuf,cwcSrcLen,cwcSrcPos);
			return 0;
		}
		virtual HRESULT STDMETHODCALLTYPE PutAltWord(ULONG cwc, WCHAR const *pwcInBuf, ULONG cwcSrcLen, ULONG cwcSrcPos)
		{
			//これの意味がよく分からない。
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

		//インナーだし全部publicで。

		//たまに形態素解析の結果がやってこないワードがあるみたいなので補正する.
		//変換する文字列
		WCHAR const *  poolawcBuffer;
		//戦闘から何個目(バイトにあらず)まで変換したか。
		ULONG          lastPos;

		//これに形態素解析の結果を代入する.
		XLStringList * poolStringList;
	};


	//IWordSinkを継承して作った独自クラスを実体化、、つーかスタック上に確保。
	pfnWordSink		wordSink;
	//これに結果を入れてもらう.
	wordSink.poolStringList = outStringList;
	wordSink.poolawcBuffer = textsource.awcBuffer;
	wordSink.lastPos = 0;

	//形態素解析開始
	HRESULT hr;
	hr = this->WordBreaker->BreakText(&textsource,&wordSink,NULL);
	if (FAILED(hr))
	{
		_com_issue_error(hr);
	}
	//よく最後が忘れ去られるのでつけてあげる。
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
		//漢字の範囲を間違っていたので安全 が 安 | 全 となってしまっていたので修正
		{
			wb.BreakText("全てクライアントサイドで解析を行うため、セキュリティの観点から見ても安全です。分かち書きの単位はMeCab + ipadicと互換性があります。", &slist);
			XLStringList::iterator it = slist.begin();
			ASSERT(slist.size() == 19);
			ASSERT((r = *it) == "全て");	it++;
			ASSERT((r = *it) == "クライアントサイドで");	it++;
			ASSERT((r = *it) == "解析を");	it++;
			ASSERT((r = *it) == "行うため");	it++;
			ASSERT((r = *it) == "、");	it++;
			ASSERT((r = *it) == "セキュリティの");	it++;
			ASSERT((r = *it) == "観点から");	it++;
			ASSERT((r = *it) == "見ても");	it++;
			ASSERT((r = *it) == "安全です");	it++;
			ASSERT((r = *it) == "。");	it++;
			ASSERT((r = *it) == "分かち書きの");	it++;
			ASSERT((r = *it) == "単位は");	it++;
			ASSERT((r = *it) == "MeCab");	it++;
			ASSERT((r = *it) == " + ");	it++;
			ASSERT((r = *it) == "ipadicと");	it++;
			ASSERT((r = *it) == "互換");	it++;
			ASSERT((r = *it) == "性が");	it++;
			ASSERT((r = *it) == "あります");	it++;
			ASSERT((r = *it) == "。");	it++;
		}
	}
	{
		//パーステスト
		XLWordBreaker wb;
		XLStringList slist;
		wb.BreakText("科学の力ではどうしようもできない、魑魅魍魎などの奇怪な輩に立ち向かう胡散臭い男。", &slist);
		XLStringList::iterator it = slist.begin();
		std::string r;

		ASSERT(slist.size() == 13);
		ASSERT((r = *it) == "科学の");	it++;
		ASSERT((r = *it) == "力では");	it++;
		ASSERT((r = *it) == "どう");	it++;
		ASSERT((r = *it) == "しようもできない");	it++;
		ASSERT((r = *it) == "、");	it++;
		ASSERT((r = *it) == "魑魅");	it++;
		ASSERT((r = *it) == "魍魎などの");	it++;
		ASSERT((r = *it) == "奇怪な");	it++;
		ASSERT((r = *it) == "輩に");	it++;
		ASSERT((r = *it) == "立ち向かう");	it++;
		ASSERT((r = *it) == "胡散臭い");	it++;
		ASSERT((r = *it) == "男");	it++;
		ASSERT((r = *it) == "。");	it++;
	}
}
