/**************************************************************************
 * Copyright (C) 2019 Rendease Co., Ltd.
 * All rights reserved.
 *
 * This program is commercial software: you must not redistribute it
 * and/or modify it without written permission from Rendease Co., Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * End User License Agreement for more details.
 *
 * You should have received a copy of the End User License Agreement along
 * with this program.  If not, see <http://www.rendease.com/licensing/>
 *************************************************************************/
#pragma once
#include <memory>

template <typename T>
class Singleton
{
public:
	Singleton() {}

	static T *GetInstance()
	{
		if (m_pSingleton == NULL)
			m_pSingleton = new T();

		return m_pSingleton;
	}
	static T *m_pSingleton;
private:
	// GC 机制
	class GC
	{
	public:
		~GC()
		{
			if (m_pSingleton != NULL) {
				delete m_pSingleton;
				m_pSingleton = NULL;
			}
		}
		static GC gc;  // 用于释放单例
	};
};
