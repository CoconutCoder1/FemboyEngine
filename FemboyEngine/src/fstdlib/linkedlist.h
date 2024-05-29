#pragma once

namespace fe {

template<typename T>
class LinkedList {
public:
	struct Iterator_t {
		T* pData;
		Iterator_t* pPrevious;
		Iterator_t* pNext;
	};

	LinkedList() {
		m_pHead = nullptr;
		m_pTail = nullptr;
	}

	Iterator_t* GetHead() const {
		return m_pHead;
	}

	Iterator_t* GetTail() const {
		return m_pTail;
	}

	~LinkedList() {
		Clear();
	}

	void Clear() {
		while (m_pHead) {
			Iterator_t* pNextIter = m_pHead->pNext;

			delete m_pHead;
			m_pHead = pNextIter;
		}

		m_pTail = nullptr;
		m_pHead = nullptr;
	}

	void ClearAndDeleteElements() {
		while (m_pHead) {
			Iterator_t* pNextIter = m_pHead->pNext;

			if (m_pHead->pData) {
				delete m_pHead->pData;

				// Remove was not called by the destructor of iterated element.
				if (m_pHead->pNext) {
					delete m_pHead;
				}
			}

			m_pHead = pNextIter;
		}

		m_pHead = nullptr;
		m_pTail = nullptr;
	}

	Iterator_t* AddToHead(T* pData) {
		Iterator_t* pPreviousHead = m_pHead;

		m_pHead = new Iterator_t();
		m_pHead->pData = pData;
		m_pHead->pNext = nullptr;
		m_pHead->pPrevious = nullptr;

		if (!m_pTail) {
			m_pTail = m_pHead;
		}

		if (pPreviousHead) {
			pPreviousHead->pPrevious = m_pHead;
			m_pHead->pNext = pPreviousHead;
		}

		return m_pHead;
	}

	Iterator_t* AddToTail(T* pData) {
		Iterator_t* pPreviousTail = m_pTail;

		m_pTail = new Iterator_t();
		m_pTail->pData = pData;
		m_pTail->pNext = nullptr;
		m_pTail->pPrevious = nullptr;

		if (!m_pHead) {
			m_pHead = m_pTail;
		}

		if (pPreviousTail) {
			pPreviousTail->pNext = m_pTail;
			m_pTail->pPrevious = pPreviousTail;
		}

		return m_pTail;
	}

	// Calling this on an iterator that was not
	//	created by this list will cause unidentified behavior.
	void Remove(Iterator_t* pIterator) {
		Iterator_t* pNext = pIterator->pNext;
		Iterator_t* pPrevious = pIterator->pPrevious;

		pIterator->pNext = nullptr;
		pIterator->pPrevious = nullptr;

		if (pNext) {
			pNext->pPrevious = pPrevious;
		}
		if (pPrevious) {
			pPrevious->pNext = pNext;
		}

		if (pIterator == m_pHead) {
			m_pHead = pNext;
		}
		if (pIterator == m_pTail) {
			m_pTail = pPrevious;
		}

		delete pIterator;
	}

private:
	Iterator_t* m_pHead;
	Iterator_t* m_pTail;
};

}