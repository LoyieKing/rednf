// Auto-generated header stub from DWARF info
// Original path: inc/adder.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_ADDER_H_H_
#define SECSVR_INC_ADDER_H_H_

// sizeof = 8
struct CAdder { // line 7
private:
static const unsigned char FUCKING_M_ADDERBBIT[];
static const unsigned char FUCKING_M_ADDERIP[];
void *m_pSys;
void *m_pLocal;
public:
CAdder(); // line 9
~CAdder(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Adder(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void SetAdderKey(unsigned char *arg0, unsigned char *arg1); // line 18
};
extern const unsigned char FUCKING_M_ADDERBBIT[]; // line 22
extern const unsigned char FUCKING_M_ADDERIP[]; // line 23

#endif // SECSVR_INC_ADDER_H_H_
