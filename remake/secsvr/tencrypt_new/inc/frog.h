// CFrog -- FROG 块密码（Michael Paul Johnson 公版）
// 布局按 DWARF：seed* / ik_len / sim_key(key_str) / lkp* / m_pSys / m_pLocal
// MakeKey 的形参 mangled 为 CFrog::key_str（PNS_7key_strE），内部 f_key 为匿名结构。
#ifndef SECSVR_INC_FROG_H_H_
#define SECSVR_INC_FROG_H_H_

// sizeof = 4372
struct CFrog { // line 7
    // sizeof = 288
    struct key_str {
        // f_key 元素（匿名结构，DWARF 名 CFrog::._31）
        struct {
        public:
        unsigned char k_xbu[16];
        unsigned char k_spu[256];
        unsigned char k_bpu[16];
        } f_key[8];
        unsigned char i_key[8][256];
    };
private:
unsigned char *seed;
unsigned int ik_len;
key_str sim_key;
key_str *lkp;
void *m_pSys;
void *m_pLocal;
public:
CFrog(); // line 9
~CFrog(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Frog(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void Decrypt(unsigned int *arg0, unsigned int *arg1); // line 32
void Encrypt(unsigned int *arg0, unsigned int *arg1); // line 33
void SetKey(unsigned int *arg0, unsigned int arg1); // line 35
void MakePerm(unsigned char *arg0, unsigned int arg1); // line 37
void MakeKey(key_str *arg0); // line 38
};

#endif // SECSVR_INC_FROG_H_H_
