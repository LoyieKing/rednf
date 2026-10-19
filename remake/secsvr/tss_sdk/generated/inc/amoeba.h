// Auto-generated header stub from DWARF info
// Original path: inc/amoeba.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_AMOEBA_H_H_
#define SECSVR_INC_AMOEBA_H_H_

// sizeof = 20
struct CAmoeba { // line 7
private:
unsigned char *amobbit;
unsigned char *amobb;
unsigned char *amoIP;
void *m_pSys;
void *m_pLocal;
public:
CAmoeba(); // line 9
~CAmoeba(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Amoeba(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void amoDeCrypt(unsigned char *arg0, unsigned char *arg1); // line 17
void amoEnCrypt(unsigned char *arg0, unsigned char *arg1); // line 18
void amoKey(unsigned char *arg0, unsigned char *arg1); // line 19
};

#endif // SECSVR_INC_AMOEBA_H_H_
