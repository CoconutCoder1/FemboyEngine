#pragma once

#include <memory>

namespace fe {

template<typename T>
using ScopedPtr = std::unique_ptr<T>;

template<typename T>
using RefPtr = std::shared_ptr<T>;

/*
struct RefCounter {
	size_t strongRefs;
	size_t weakRefs;
};

template<typename T>
class ScopedPtr {
public:
	ScopedPtr()
		: m_Ptr(nullptr) {}

	explicit ScopedPtr(T* ptr)
		: m_Ptr(ptr) {}

	ScopedPtr(ScopedPtr<T>&& other) noexcept {
		T* oldPointer = other.m_Ptr;
		other.m_Ptr = nullptr;

		m_Ptr = oldPointer;
	}

	~ScopedPtr() {
		if (m_Ptr) {
			m_Ptr->~T();
			delete m_Ptr;
			m_Ptr = nullptr;
		}
	}

	ScopedPtr(const ScopedPtr<T>& other) = delete;

	void operator=(ScopedPtr<T>&& rhs) noexcept {
		ScopedPtr(static_cast<ScopedPtr<T>&&>(rhs));
	}

	/ *void operator=(T* ptr) {
		if (m_Ptr) {
			this->~ScopedPtr();
		}

		ScopedPtr(rhs);
	}* /

	T& operator*() {
		return *m_Ptr;
	}

	const T& operator*() const {
		return *m_Ptr;
	}

	T* operator->() const {
		return Get();
	}

	T* Get() const {
		return m_Ptr;
	}

private:
	T* m_Ptr;
};

template<typename T>
class RefPtr {
public:
	RefPtr()
		: m_Ptr(nullptr), m_RefCount(nullptr) {}

	explicit RefPtr(T* ptr)
		: m_Ptr(ptr)
	{
		CreateRefCount();
	}

	RefPtr(const RefPtr<T>& other) {
		m_Ptr = other.m_Ptr;
		m_RefCount = other.m_RefCount;

		AddStrongRef();
	}

	RefPtr(RefPtr<T>&& other) noexcept {
		T* pOld = other.m_Ptr;
		RefCounter* pRefCounter = other.m_RefCount;

		other.m_Ptr = nullptr;
		other.m_RefCount = nullptr;

		m_Ptr = pOld;
		m_RefCount = pRefCounter;
	}

	~RefPtr() {
		ReleaseStrong();
	}

	void operator=(const RefPtr<T>& rhs) {
		if (m_Ptr) {
			ReleaseStrong();
			m_Ptr = nullptr;
			m_RefCount = nullptr;
		}

		m_Ptr = rhs.m_Ptr;
		m_RefCount = rhs.m_RefCount;

		AddStrongRef();
	}

	T& operator*() {
		return *m_Ptr;
	}

	const T& operator*() const {
		return *m_Ptr;
	}

	T* operator->() const {
		return Get();
	}

	T* Get() const {
		return m_Ptr;
	}

private:
	void CreateRefCount() {
		m_RefCount = new RefCounter();
		m_RefCount->weakRefs = 0;
		m_RefCount->strongRefs = 1;
	}

	void SanitizeRefCounter() {
		if (m_Ptr && !m_RefCount) {
			CreateRefCount();
		}
	}

	void AddStrongRef() {
		SanitizeRefCounter();

		if (m_RefCount) {
			m_RefCount->strongRefs++;
		}
	}

	void AddWeakRef() {
		SanitizeRefCounter();

		if (m_RefCount) {
			m_RefCount->weakRefs++;
		}
	}

	void ReleaseStrong() {
		if (m_RefCount) {
			m_RefCount->strongRefs--;

			if (m_RefCount->strongRefs == 0) {
				delete m_Ptr;
				delete m_RefCount;

				m_Ptr = nullptr;
				m_RefCount = nullptr;
			}
		}
	}

	void ReleaseWeak() {
		if (m_RefCount && m_RefCount->weakRefs > 0) {
			m_RefCount->weakRefs--;
		}
	}

private:
	T* m_Ptr;
	RefCounter* m_RefCount;
};*/

}