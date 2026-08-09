// Auto-generated header stub from DWARF info
// Original path: inc/desnew.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_DESNEW_H_H_
#define SECSVR_INC_DESNEW_H_H_

// sizeof = 8
struct CDesNew { // line 7
private:
static const unsigned char FUCKING_NF[];
void *m_pSys;
void *m_pLocal;
public:
CDesNew(); // line 9
~CDesNew(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Des_New(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void decrypt(unsigned char *arg0, unsigned char *arg1); // line 18
void encrypt(unsigned char *arg0, unsigned char *arg1); // line 19
};
extern const unsigned char FUCKING_NF[]; // line 21

#endif // SECSVR_INC_DESNEW_H_H_
