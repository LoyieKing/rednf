// Auto-generated header stub from DWARF info
// Original path: inc/des.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_DES_H_H_
#define SECSVR_INC_DES_H_H_

// sizeof = 40
struct CDES { // line 5
private:
void *m_pSys;
void *m_pLocal;
unsigned int *SP1;
unsigned int *SP2;
unsigned int *SP3;
unsigned int *SP4;
unsigned int *SP5;
unsigned int *SP6;
unsigned int *SP7;
unsigned int *SP8;
public:
CDES(); // line 7
~CDES(); // line 8
void Init(void *arg0, void *arg1); // line 10
void UnInit(); // line 11
void Des(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 13
private:
void DesKey(unsigned char *arg0, bool arg1, unsigned int *arg2); // line 15
void DesBase(unsigned int *arg0, unsigned char *arg1, unsigned char *arg2); // line 16
};

#endif // SECSVR_INC_DES_H_H_
