// Auto-generated header stub from DWARF info
// Original path: inc/subtracter.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_SUBTRACTER_H_H_
#define SECSVR_INC_SUBTRACTER_H_H_

// sizeof = 8
struct CSubtracter { // line 7
private:
static const unsigned char FUCKING_SUBBIT[];
static const unsigned char FUCKING_SUBIP[];
void *m_pSys;
void *m_pLocal;
public:
CSubtracter(); // line 9
~CSubtracter(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Subtracter(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void SetSubKey(unsigned char *arg0, unsigned char *arg1); // line 17
};
extern const unsigned char FUCKING_SUBBIT[]; // line 19
extern const unsigned char FUCKING_SUBIP[]; // line 20

#endif // SECSVR_INC_SUBTRACTER_H_H_
