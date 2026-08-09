// Auto-generated header stub from DWARF info
// Original path: inc/shift.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_SHIFT_H_H_
#define SECSVR_INC_SHIFT_H_H_

// sizeof = 8
struct CShift { // line 7
private:
static const unsigned char FUCKING_SHIFTBBIT[];
static const unsigned char FUCKING_SHIFTBB[];
static const unsigned char FUCKING_SHIFTIP[];
void *m_pSys;
void *m_pLocal;
public:
CShift(); // line 9
~CShift(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Shift(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void shiftCrypt(unsigned char *arg0, unsigned char *arg1, bool arg2); // line 17
void SetshiftKey(unsigned char *arg0, unsigned char *arg1); // line 18
};
extern const unsigned char FUCKING_SHIFTBBIT[]; // line 20
extern const unsigned char FUCKING_SHIFTBB[]; // line 21
extern const unsigned char FUCKING_SHIFTIP[]; // line 22

#endif // SECSVR_INC_SHIFT_H_H_
