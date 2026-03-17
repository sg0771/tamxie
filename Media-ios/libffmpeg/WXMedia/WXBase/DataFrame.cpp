#include "WXBase.h"
#include <string.h>

void DataFrame::Init(uint8_t *buf, int size) {
	if (m_pBuf)delete[]m_pBuf;
	m_pBuf = new uint8_t[size];
	if (buf != nullptr)
		memcpy(m_pBuf, buf, size);
	m_iBufSize = size;
}

DataFrame::DataFrame() {}

DataFrame::DataFrame(uint8_t *buf, int size) {
	Init(buf, size);
}

DataFrame::~DataFrame() {
	if (m_pBuf) {
		delete[]m_pBuf;
		m_pBuf = nullptr;
		m_iBufSize = 0;
	}
}
