#pragma once

//�����d�������Ȃ� IUnknown.
template <class _T> class XLNeetIUnknown : public _T
{
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(const struct _GUID &,void **)
	{
		//�N���X�쐬�H �}���h�N�Z�[
		return E_FAIL;
	}
	virtual ULONG STDMETHODCALLTYPE AddRef( void)
	{
		//�ǉ��H �Ƃ肠���� 1�Ԃ��Ƃ��΂�����ł���B
		//���Ă΂ꂽ��{�C�o���B
		return 1;
	}

	virtual ULONG STDMETHODCALLTYPE Release( void)
	{
		//�J���Ȃ�Ă��Ȃ��񂾂�o�o�@
		return 0;
	}
};
