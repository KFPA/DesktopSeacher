/**
* Copyright (C) 2014-2050
* All rights reserved.
*
* @file       SSingleton.h
* @brief
* @version    v1.0
* @author     SOUI group
* @date       2014/08/02
*
* Describe    SOUI系统中使用的单件模块
*/

#pragma once

#include <assert.h>

namespace SOUI
{

	/**
	* @class      SSingleton
	* @brief      单件模板
	*
	* Describe
	*/
	template <typename T>
	class CSingleton
	{
	protected:
		static    T* ms_Singleton;

	public:
		CSingleton(void)
		{
			assert(!ms_Singleton);
			ms_Singleton = static_cast<T*>(this);
		}
		virtual ~CSingleton(void)
		{
			assert(ms_Singleton);
			ms_Singleton = 0;
		}
		static T& getSingleton(void)
		{
			assert(ms_Singleton);
			return (*ms_Singleton);
		}
		static T* getSingletonPtr(void)
		{
			return (ms_Singleton);
		}

	private:
		CSingleton& operator=(const CSingleton&)
		{
			return *this;
		}
		CSingleton(const CSingleton&) {}
	};

}//namespace SOUI