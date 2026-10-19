// Auto-generated header stub from DWARF info
// Original path: inc/mbc.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_MBC_H_H_
#define SECSVR_INC_MBC_H_H_

// sizeof = 16
struct CMbc { // line 7
private:
unsigned int *iv_tbl;
unsigned int *key;
void *m_pSys;
void *m_pLocal;
public:
CMbc(); // line 9
~CMbc(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Mbc(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void SetKey(unsigned int *arg0); // line 17
void Encrypt(unsigned int *arg0); // line 18
void Decrypt(unsigned int *arg0); // line 19
};

#endif // SECSVR_INC_MBC_H_H_
