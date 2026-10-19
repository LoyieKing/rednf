// Auto-generated header stub from DWARF info
// Original path: inc/seattos.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_SEATTOS_H_H_
#define SECSVR_INC_SEATTOS_H_H_

// sizeof = 16
struct CSeattos { // line 7
private:
unsigned char *seattosbbit;
unsigned char *seattosIP;
void *m_pSys;
void *m_pLocal;
public:
CSeattos(); // line 9
~CSeattos(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Seattos(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void SetseattosKey(unsigned char *arg0, unsigned char *arg1); // line 17
};

#endif // SECSVR_INC_SEATTOS_H_H_
