// Auto-generated header stub from DWARF info
// Original path: inc/frog.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_FROG_H_H_
#define SECSVR_INC_FROG_H_H_

// sizeof = 4372
struct CFrog { // line 7
    // sizeof = 288
    struct ._31 {
    public:
    unsigned char k_xbu[];
    unsigned char k_spu[];
    unsigned char k_bpu[];
    };
    // sizeof = 4352
    struct ._32 {
    public:
    CFrog::._31 f_key[];
    unsigned char i_key[][];
    };
private:
unsigned char *seed;
unsigned int ik_len;
CFrog::._32 sim_key;
CFrog::._32 *lkp;
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
void MakeKey(CFrog::._32 *arg0); // line 38
};
namespace CFrog {
    // sizeof = 288
    struct ._31 { // line 19
    public:
    unsigned char k_xbu[];
    unsigned char k_spu[];
    unsigned char k_bpu[];
    };
    // sizeof = 4352
    struct ._32 { // line 26
    public:
    CFrog::._31 f_key[];
    unsigned char i_key[][];
    };
    // sizeof = 288
    struct ._30 { // line 19
    public:
    unsigned char k_xbu[];
    unsigned char k_spu[];
    unsigned char k_bpu[];
    };
} // namespace CFrog

#endif // SECSVR_INC_FROG_H_H_
