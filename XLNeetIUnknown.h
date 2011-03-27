#pragma once

//何も仕事をしない IUnknown.
template <class _T> class XLNeetIUnknown : public _T
{
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(const struct _GUID &,void **)
	{
		//クラス作成？ マンドクセー
		return E_FAIL;
	}
	virtual ULONG STDMETHODCALLTYPE AddRef( void)
	{
		//追加？ とりあえず 1返しとけばいいんでしょ。
		//次呼ばれたら本気出す。
		return 1;
	}

	virtual ULONG STDMETHODCALLTYPE Release( void)
	{
		//開放なんてしないんだよババァ
		return 0;
	}
};
