// Auto-generated header stub from DWARF info
// Original path: inc/square.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_SQUARE_H_H_
#define SECSVR_INC_SQUARE_H_H_

// sizeof = 56
struct CSquare { // line 10
private:
unsigned char *Se;
unsigned char *Sd;
unsigned int *phi;
unsigned int *offset;
unsigned int *Te0;
unsigned int *Te1;
unsigned int *Te2;
unsigned int *Te3;
unsigned int *Td0;
unsigned int *Td1;
unsigned int *Td2;
unsigned int *Td3;
void *m_pSys;
void *m_pLocal;
public:
CSquare(); // line 12
~CSquare(); // line 13
void Init(void *arg0, void *arg1); // line 18
void UnInit(); // line 19
void Square(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 21
private:
void SquareTransform(unsigned int *arg0); // line 24
void GenerateRoundKeys(unsigned char *arg0, unsigned int (*arg1)[4], unsigned int (*arg2)[4]); // line 25
void ExpandKey(unsigned char *arg0, unsigned int (*arg1)[4]); // line 26
void Encrypt(unsigned int *arg0, unsigned int (*arg1)[4]); // line 28
void Decrypt(unsigned int *arg0, unsigned int (*arg1)[4]); // line 29
};

#endif // SECSVR_INC_SQUARE_H_H_
