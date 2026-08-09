// Auto-generated header stub from DWARF info
// Original path: inc/ring.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_RING_H_H_
#define SECSVR_INC_RING_H_H_

// sizeof = 16
struct CRing { // line 7
private:
unsigned char *Ringbbit;
unsigned char *RingIP;
void *m_pSys;
void *m_pLocal;
public:
CRing(); // line 9
~CRing(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Ring(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void RingDecrypt(unsigned char *arg0, unsigned char *arg1); // line 17
void RingEncrypt(unsigned char *arg0, unsigned char *arg1); // line 18
void SetRingKey(unsigned char *arg0, unsigned char *arg1, unsigned char *arg2); // line 19
};

#endif // SECSVR_INC_RING_H_H_
