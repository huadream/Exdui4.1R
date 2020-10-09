#include "Layout_ex.h"

bool _layout_register(int nType, void* lpfnLayoutProc)
{
	bool ret = false;
	if (!HashTable_IsExit(g_Li.hTableLayout, nType))
	{
		ret = HashTable_Set(g_Li.hTableLayout, nType, (size_t)lpfnLayoutProc);
	}
	return ret;
}

bool _layout_unregister(int nType)
{
	bool ret = false;
	if (!HashTable_IsExit(g_Li.hTableLayout, nType))
	{
		ret = HashTable_Remove(g_Li.hTableLayout, nType);
	}
	return ret;
}

void _layout_free_info(void* hArr, int nIndex, void* pvItem, int nType)
{
	void* pLayout =(void*) Array_GetExtra(hArr);
	((LayoutTwoPROC)((layout_s*)pLayout)->lpfnProc_)(ELN_UNINITCHILDPROPS, __get(pvItem, 0), (size_t)pvItem);
	�ͷ��ڴ�((void*)((size_t)pvItem - 16));
}

size_t _layout_create(int nType, size_t hObjBind)
{
	size_t hLayout = 0;
	int nError = 1;
	if (nType > 0 && hObjBind != 0)
	{
		size_t lpfnProc = 0;
		HashTable_Get(g_Li.hTableLayout, nType, &lpfnProc);
		if (lpfnProc != 0)
		{
			void* pLayout = �����ڴ�(sizeof(layout_s));
			void* pObj = nullptr;
			if (pLayout != 0)
			{
				if (_handle_validate(hObjBind, HT_OBJECT, &pObj, &nError))
				{
					((layout_s*)pLayout)->nBindType_ = HT_OBJECT;
				}
				else if (_handle_validate(hObjBind, HT_DUI, &pObj, &nError))
				{
					((layout_s*)pLayout)->nBindType_ = HT_DUI;
				}
				else {
					nError = ERROR_EX_INVALID_OBJECT;
				}
				if (nError == 0)
				{
					((layout_s*)pLayout)->nType_ = nType;
					((layout_s*)pLayout)->fUpdateable_ = true;
					((layout_s*)pLayout)->lpfnProc_ = (void*)lpfnProc;
					((layout_s*)pLayout)->hBind_ = hObjBind;
					int nCount=((LayoutThreePROC)lpfnProc)(NULL, ELN_GETPROPSCOUNT, NULL, NULL);
					void* pInfo = (void*)((size_t)�����ڴ�((nCount + (size_t)4) * (size_t)4) + (size_t)16);
					((layout_s*)pLayout)->lpLayoutInfo_ = pInfo;
					((LayoutThreePROC)lpfnProc)(NULL, ELN_INITPROPS, NULL, (size_t)pInfo);
				    nCount = ((LayoutThreePROC)lpfnProc)(NULL, ELN_GETCHILDPROPCOUNT, NULL, NULL);
					((layout_s*)pLayout)->cbInfoLen_ = (nCount + 5) * 4;
					void* hArr = Array_Create(0);
					Array_BindEvent(hArr, �����¼�_ɾ����Ա, &_layout_free_info);
					Array_SetExtra(hArr, (size_t)pLayout);
					((layout_s*)pLayout)->hArrChildrenInfo_ = hArr;
					hLayout = _handle_create(HT_LAYOUT, pLayout, &nError);
					((layout_s*)pLayout)->hLayout_ = hLayout;
					((wnd_s*)pObj)->hLayout_ = hLayout;
				}
				else {
					�ͷ��ڴ�(pLayout);
				}
			}
			else {
				nError = ERROR_EX_MEMORY_ALLOC;
			}
		}
		else {
			nError = ERROR_EX_LAYOUT_INVALID;
		}
	}
	else {
		nError = ERROR_EX_LAYOUT_INVALID;
	}
	Ex_SetLastError(nError);
	return hLayout;
}

size_t _layout_get_parent_layout(size_t hObj)
{
	int nError = 1;
	void* pObj = nullptr;
	size_t hLayoutParent = 0;
	if (_handle_validate(hObj, HT_OBJECT, &pObj, &nError))
	{
		size_t hObj=((obj_s*)pObj)->objParent_;
		if (hObj == 0)
		{
			auto pWnd = ((obj_s*)pObj)->pwnd_;
			hObj = ((wnd_s*)pWnd)->hexdui_;
		}
		hLayoutParent = Ex_ObjLayoutGet(hObj);
	}
	return hLayoutParent;
}

bool _layout_destory(size_t hLayout)
{
	int nError = 1;
	void* pLayout = nullptr;
	if (_handle_validate(hLayout, HT_LAYOUT, &pLayout, &nError))
	{
		void* lpfnProc = ((layout_s*)pLayout)->lpfnProc_;
		((LayoutTwoPROC)lpfnProc)(ELN_UNINITPROPS, 0, (size_t)((layout_s*)pLayout)->lpLayoutInfo_);
		Array_Destroy(((layout_s*)pLayout)->hArrChildrenInfo_);
		�ͷ��ڴ�((void*)((size_t)((layout_s*)pLayout)->lpLayoutInfo_ - 16));
		�ͷ��ڴ�(pLayout);
		_handle_destroy(hLayout, &nError);
	}
	return nError == 0;
}

bool _layout_enum_find_obj(void* hArr, int nIndex, void* pvItem, int nType, void* pvParam)
{
	return ((void*)__get(pvItem, 0) == pvParam);
}

void* _layout_get_child(void* pLayout, size_t hObj)
{
	auto hObjP = ((layout_s*)pLayout)->hBind_;
	size_t hExDUI = 0;
	void* pInfo = nullptr;
	if(Ex_ObjGetParentEx(hObj,&hExDUI)==hObjP)
	{
		if (hExDUI == hObjP)
		{
			void* hArr = ((layout_s*)pLayout)->hArrChildrenInfo_;
			int nIndex=Array_Emum(hArr, &_layout_enum_find_obj, hObj);
			if (nIndex > 0)
			{
				pInfo=(void*)Array_GetMember(hArr, nIndex);
			}
		}
	}
	return pInfo;
}

bool _layout_update(size_t hLayout)
{
	int nError = 1;
	void* pLayout = nullptr;
	if (_handle_validate(hLayout, HT_LAYOUT, &pLayout, &nError))
	{
		if (((layout_s*)pLayout)->fUpdateable_)
		{
			void* lpfnProc = ((layout_s*)pLayout)->lpfnProc_;
			((LayoutThreePROC)lpfnProc)(pLayout, ELN_UPDATE, ((layout_s*)pLayout)->hBind_, 0);
		}
	}
	Ex_SetLastError(nError);
	return nError==0;
}

int _layout_gettype(size_t hLayout)
{
	int nError = 1;
	void* pLayout = nullptr;
	int nType = 0;
	if (_handle_validate(hLayout, HT_LAYOUT, &pLayout, &nError))
	{
		nType = ((layout_s*)pLayout)->nType_;
	}
	Ex_SetLastError(nError);
	return nType;
}

bool _layout_enableupdate(size_t hLayout,bool fUpdateable)
{
	int nError = 1;
	void* pLayout = nullptr;
	int nType = 0;
	if (_handle_validate(hLayout, HT_LAYOUT, &pLayout, &nError))
	{
		((layout_s*)pLayout)->fUpdateable_= fUpdateable;
	}
	Ex_SetLastError(nError);
	return nError==0;
}

size_t _layout_notify(size_t hLayout,int nEvent,void* wParam,void* lParam)
{
	int nError = 1;
	void* pLayout = nullptr;
	int ret = 1;
	if (_handle_validate(hLayout, HT_LAYOUT, &pLayout, &nError))
	{
		if (((layout_s*)pLayout)->fUpdateable_)
		{
			void* lpfnProc = ((layout_s*)pLayout)->lpfnProc_;
			ret=((LayoutThreePROC)lpfnProc)(pLayout, nEvent, (size_t)wParam, (size_t)lParam);
		}
	}
	Ex_SetLastError(nError);
	return ret;
}

bool _layout_table_setinfo(size_t hLayout, void* aRowHeight, int cRows, void* aCellWidth, int cCells)
{
	int nError = 1;
	void* pLayout = nullptr;
	int ret = 1;
	if (_handle_validate(hLayout, HT_LAYOUT, &pLayout, &nError))
	{
		if (((layout_s*)pLayout)->nType_ == ELT_TABLE)
		{
			void* pInfo = ((layout_s*)pLayout)->lpLayoutInfo_;
			void* hArr=(void*)__get(pInfo, (ELP_TABLE_ARRAY_ROW - 1) * sizeof(void*));
			Array_Redefine(hArr, cRows, false);
			for (int i=0;i<cRows;i++)
			{
				Array_SetMember(hArr, i + 1, __get(aRowHeight, i * sizeof(void*)));
			}
			hArr = (void*)__get(pInfo, (ELP_TABLE_ARRAY_CELL - 1) * sizeof(void*));
			Array_Redefine(hArr, cCells, false);
			for (int i = 0; i < cRows; i++)
			{
				Array_SetMember(hArr, i + (size_t)1, __get(aCellWidth, i * sizeof(void*)));
			}
		}
		else {
			nError = ERROR_EX_LAYOUT_INVALID;
		}
	}
	Ex_SetLastError(nError);
	return nError == 0;
}
