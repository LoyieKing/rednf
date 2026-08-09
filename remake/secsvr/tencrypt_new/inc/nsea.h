// Auto-generated header stub from DWARF info
// Original path: inc/nsea.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_NSEA_H_H_
#define SECSVR_INC_NSEA_H_H_

// sizeof = 56
struct CNsea { // line 7
private:
unsigned char *tempKey;
unsigned int *sBoxes1;
unsigned int *sBoxes2;
unsigned int rotTemp;
unsigned int ivLLeft1;
unsigned int ivRLeft1;
unsigned int ivLRight1;
unsigned int ivRRight1;
unsigned int ivLLeft2;
unsigned int ivRLeft2;
unsigned int ivLRight2;
unsigned int ivRRight2;
void *m_pSys;
void *m_pLocal;
public:
CNsea(); // line 9
~CNsea(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Nsea(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void decrypt(unsigned char *arg0, unsigned char *arg1); // line 17
void initSBoxes(unsigned char *arg0, int arg1, unsigned int arg2); // line 18
void initTempKey(unsigned char *arg0, int arg1, short unsigned int arg2, unsigned char *arg3); // line 19
void initIV(unsigned int arg0); // line 20
void encrypt(unsigned char *arg0, unsigned char *arg1); // line 21
void permuteSBox(unsigned char *arg0, int arg1); // line 22
void setInitialSBox(unsigned char *arg0); // line 23
};

#endif // SECSVR_INC_NSEA_H_H_
