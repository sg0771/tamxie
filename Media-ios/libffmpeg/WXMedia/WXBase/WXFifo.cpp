/*
C++  FIFO Buffer
edit 2018.4.4
*/
#include "WXBase.h"
#include  <assert.h>
#include  <queue>
#undef min
#include  <algorithm>

class WXFifoImpl :public WXLocker {
	std::queue<DataFrame*>m_queue;
	int		  m_iSize = 0;
public:
	WXFifoImpl() {}

	~WXFifoImpl() {Flush();}

	inline int GetSize() {return m_iSize;}
public:
	int WriteData(uint8_t *buf, int size) {
		WXAutoLock al(this);
		DataFrame *frame = new DataFrame(buf, size);
		m_queue.push(frame);
		m_iSize += size;
		return size;
	}

	int ReadData(uint8_t *buf, int size) {//buf=NULL Skip
		WXAutoLock al(this);
		if (m_iSize >= size) {
			DataFrame *m = m_queue.front();
			int sz = 0;
			while (sz < size) {
                int cplen = std::min((int)(m->m_iBufSize - m->m_iPos), (size - sz));
				if(buf)
					memcpy(buf + sz, m->m_pBuf + m->m_iPos, cplen);
				sz += cplen; 
				m->m_iPos += cplen;
				if (m->m_iPos == m->m_iBufSize) {
					m_queue.pop();
					delete m;
					if (sz == size)break;
					m = m_queue.front();
				}
			}
			m_iSize -= size;
			return size;
		}
		return 0;
	}

	void Flush() {
		WXAutoLock al(this);
		while (!m_queue.empty()) {
			DataFrame *frame = m_queue.front();
			m_queue.pop();
			delete frame;
		}
	}
};

int WXFifo::Size() {
	return m_impl->GetSize();
}

WXFifo::WXFifo(){
	m_impl = new WXFifoImpl;
}

WXFifo::~WXFifo(){
	SAFE_DELETE(m_impl);
}

int WXFifo::Write(uint8_t *buf, int size) {
	return m_impl->WriteData(buf, size);
}

int WXFifo::Read(uint8_t *buf, int size) {
	return m_impl->ReadData(buf, size);
}
