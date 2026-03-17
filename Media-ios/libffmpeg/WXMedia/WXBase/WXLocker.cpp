
/*
C++ Locker By C++11
by Tam.Xie 2017.08.01
edit 2018.4.4
*/

#include "WXBase.h"
#include <mutex>

WXLocker::WXLocker() {
	m_impl = (void*)(new std::recursive_mutex);
}

WXLocker::~WXLocker() {
	if (m_impl) {
		std::recursive_mutex *mutex = (std::recursive_mutex*)m_impl;
		delete mutex;
		m_impl = nullptr;
	}
}

void WXLocker::Lock() {
	if (m_impl) {
		std::recursive_mutex *mutex = (std::recursive_mutex*)m_impl;
		mutex->lock();
	}
}

void WXLocker::Unlock() {
	if (m_impl) {
		std::recursive_mutex *mutex = (std::recursive_mutex*)m_impl;
		mutex->unlock();
	}
}

WXAutoLock::WXAutoLock(WXLocker * lock){
	m_lock = lock;
	m_lock->Lock();
}

WXAutoLock::~WXAutoLock(){
	m_lock->Unlock();
}