// Auto-generated header stub from DWARF info
// Original path: inc/rc4.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_RC4_H_H_
#define SECSVR_INC_RC4_H_H_

// sizeof = 258
struct rc4_key { // line 7
public:
unsigned char state[];
unsigned char x;
unsigned char y;
};
typedef rc4_key rc4_key;
// sizeof = 12
struct CRc4 { // line 14
private:
rc4_key *key;
void *m_pSys;
void *m_pLocal;
public:
CRc4(); // line 16
~CRc4(); // line 17
void Init(void *arg0, void *arg1); // line 19
void UnInit(); // line 20
void Rc4(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 22
private:
void crypt(unsigned char *arg0, unsigned int arg1); // line 24
void set_key(unsigned char *arg0, unsigned int arg1); // line 25
};

#endif // SECSVR_INC_RC4_H_H_
