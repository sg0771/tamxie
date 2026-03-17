/*
C++ Thread Class base C++11
by Tam.Xie 2017.09.22
*/

#include "WXBase.h"
#include <thread>

void WXThread::ThreadFunction() {
	this->ThreadRun();
}

bool WXThread::ThreadStart(){
	if (!m_bThreadStop) return false;
	m_bThreadStop = false;
	std::thread *thread = new std::thread(&WXThread::ThreadFunction,this);
	m_impl = (void*)thread;
	return true;
}

void WXThread::ThreadStop() {
	m_bThreadStop = true;
	if (m_impl) {
		std::thread *thread = (std::thread *)m_impl;
		thread->join();
		delete thread;
		m_impl = nullptr;
	}
}