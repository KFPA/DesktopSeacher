#pragma once

#include <string/tstring.h>

namespace SOUI
{
	/**
    * @struct     IPropBag
    * @brief      ��Ⱦ�������԰�
    * 
    * Describe    
    */
	struct IPropBag : IObjRef
	{
		virtual void SetKeyValue(LPCTSTR pszKey, LPCTSTR pszValue) = 0;
		virtual LPCTSTR GetValue(LPCTSTR pszKey) const = 0;
		virtual void RemoveKey(LPCTSTR pszKey) = 0;
		virtual void RemoveAll() = 0;
		virtual SStringT ToXml() const = 0;
	};

}