// Auto-generated header stub from DWARF info
// Original path: inc/hpc.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_HPC_H_H_
#define SECSVR_INC_HPC_H_H_

// sizeof = 28
struct CHpc { // line 7
private:
unsigned int *l_key[];
unsigned int *spice[];
unsigned int *p119;
unsigned int *e19;
unsigned int *r220;
void *m_pSys;
void *m_pLocal;
public:
CHpc(); // line 9
~CHpc(); // line 10
void Init(void *arg0, void *arg1); // line 14
void UnInit(); // line 15
void Hpc(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 17
private:
void decrypt(unsigned int *arg0); // line 19
void encrypt(unsigned int *arg0); // line 20
void setKey(const unsigned int *arg0, unsigned int arg1); // line 21
void mult_64(unsigned int *arg0, const unsigned int *arg1, const unsigned int *arg2); // line 22
};

#endif // SECSVR_INC_HPC_H_H_
