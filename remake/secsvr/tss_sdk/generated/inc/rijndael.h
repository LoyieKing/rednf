// Auto-generated header stub from DWARF info
// Original path: inc/rijndael.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_RIJNDAEL_H_H_
#define SECSVR_INC_RIJNDAEL_H_H_

// sizeof = 528
struct CRijndael { // line 22
    // sizeof = 488
    struct tagRIJNDAELContext {
    public:
    unsigned int keys[];
    unsigned int ikeys[];
    int nrounds;
    int mode;
    };
private:
CRijndael::tagRIJNDAELContext m_Context;
unsigned int *dtbl;
unsigned int *itbl;
unsigned char *sbox;
unsigned char *isbox;
unsigned char *Logtable;
unsigned char *Alogtable;
unsigned int *idx[];
unsigned int *iidx[];
void *m_pSys;
void *m_pLocal;
public:
CRijndael(); // line 25
~CRijndael(); // line 26
void Init(void *arg0, void *arg1); // line 27
void UnInit(); // line 28
void RijnDael(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 29
private:
void rijndael_setup(CRijndael::tagRIJNDAELContext *arg0, unsigned int arg1, const unsigned char *arg2); // line 42
void rijndael_encrypt(CRijndael::tagRIJNDAELContext *arg0, const unsigned char *arg1, unsigned char *arg2); // line 44
void rijndael_decrypt(CRijndael::tagRIJNDAELContext *arg0, const unsigned char *arg1, unsigned char *arg2); // line 46
void block_encrypt(CRijndael::tagRIJNDAELContext *arg0, unsigned char *arg1, int arg2, unsigned char *arg3, unsigned char *arg4); // line 48
void block_decrypt(CRijndael::tagRIJNDAELContext *arg0, unsigned char *arg1, int arg2, unsigned char *arg3, unsigned char *arg4); // line 50
void SetKey(unsigned int arg0, const unsigned char *arg1); // line 52
void Encrypt(const unsigned char *arg0, unsigned char *arg1); // line 56
void Decrypt(const unsigned char *arg0, unsigned char *arg1); // line 60
void BlockEncrypt(unsigned char *arg0, int arg1, unsigned char *arg2, unsigned char *arg3); // line 65
void BlockDecrypt(unsigned char *arg0, int arg1, unsigned char *arg2, unsigned char *arg3); // line 69
void Initialize(int arg0, int arg1); // line 74
unsigned char xtime(unsigned char arg0); // line 76
unsigned char mul(unsigned char arg0, unsigned char arg1); // line 78
void inv_mix_column(unsigned int *arg0, unsigned int *arg1); // line 80
void key_addition_8to32(const unsigned char *arg0, unsigned int *arg1, unsigned int *arg2); // line 82
void key_addition32(const unsigned int *arg0, unsigned int *arg1, unsigned int *arg2); // line 84
void key_addition32to8(const unsigned int *arg0, unsigned int *arg1, unsigned char *arg2); // line 86
};
namespace CRijndael {
    // sizeof = 488
    struct tagRIJNDAELContext { // line 33
    public:
    unsigned int keys[];
    unsigned int ikeys[];
    int nrounds;
    int mode;
    };
} // namespace CRijndael

#endif // SECSVR_INC_RIJNDAEL_H_H_
