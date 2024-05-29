#pragma once

#include "fstdlib/pointers.h"

namespace fe {

template<typename T>
class Singleton {
public:
	static void CreateInstance() {
		if (!s_pInstance) {
			s_pInstance.reset(new T());
		}
	}

	static void DeleteInstance() {
		if (s_pInstance) {
			s_pInstance.reset();
		}
	}

	static T& Instance() {
		return *s_pInstance;
	}

private:
	static ScopedPtr<T> s_pInstance;
};

template<typename T>
ScopedPtr<T> Singleton<T>::s_pInstance = nullptr;

}