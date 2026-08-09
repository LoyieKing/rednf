// Auto-generated header stub from DWARF info
// Original path: inc/cast.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_CAST_H_H_
#define SECSVR_INC_CAST_H_H_

// sizeof = 132（DWARF 匿名结构实为 cast_key：xkey[32] + rounds）
struct cast_key { // line 7
unsigned int xkey[32];
int rounds;
};
// sizeof = 8
struct CCast { // line 13
private:
static const unsigned int FUCKING_CAST_SBOX[8][256];
void *m_pSys;
void *m_pLocal;
public:
CCast(); // line 15
~CCast(); // line 16
void Init(void *arg0, void *arg1); // line 18
void UnInit(); // line 19
void Cast(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 21
private:
void cast_setkey(cast_key *arg0, unsigned char *arg1, int arg2); // line 24
void cast_decrypt(cast_key *arg0, unsigned char *arg1, unsigned char *arg2); // line 25
void cast_encrypt(cast_key *arg0, unsigned char *arg1, unsigned char *arg2); // line 26
};
extern const unsigned int FUCKING_CAST_SBOX[8][256]; // line 28

#endif // SECSVR_INC_CAST_H_H_
