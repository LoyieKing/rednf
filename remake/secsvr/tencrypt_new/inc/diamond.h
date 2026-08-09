// Auto-generated header stub from DWARF info
// Original path: inc/diamond.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_DIAMOND_H_H_
#define SECSVR_INC_DIAMOND_H_H_

// sizeof = 48
struct CDiamond { // line 7
private:
unsigned int *Ccitt32Table;
unsigned char *s;
unsigned char *si;
unsigned char *key;
unsigned int accum;
unsigned int keysize;
unsigned int keyindex;
unsigned int roundsize;
unsigned int numrounds;
unsigned int blocksize;
void *m_pSys;
void *m_pLocal;
public:
CDiamond(); // line 9
~CDiamond(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Diamond(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void diamond_decrypt_block(unsigned char *arg0, unsigned char *arg1); // line 18
void ipermute(unsigned char *arg0, unsigned char *arg1); // line 19
void isubst(unsigned int arg0, unsigned char *arg1, unsigned char *arg2); // line 20
void diamond_encrypt_block(unsigned char *arg0, unsigned char *arg1); // line 21
void permute(unsigned char *arg0, unsigned char *arg1); // line 22
void substitute(unsigned int arg0, unsigned char *arg1, unsigned char *arg2); // line 23
int set_diamond_key(unsigned char *arg0, unsigned int arg1, unsigned int arg2, bool arg3, unsigned int arg4); // line 25
unsigned int keyrand(unsigned int arg0); // line 27
void makeonebox(unsigned int arg0, unsigned int arg1); // line 29
void lite_encrypt_block(unsigned char *arg0, unsigned char *arg1); // line 31
void lite_decrypt_block(unsigned char *arg0, unsigned char *arg1); // line 33
void BuildCRCTable(); // line 35
};


extern void ipermute_lite(unsigned char *b, unsigned char *a); // line 302 (inferred)
extern void permute_lite(unsigned char *a, unsigned char *b); // line 278 (inferred)

#endif // SECSVR_INC_DIAMOND_H_H_
