// Auto-generated header stub from DWARF info
// Original path: inc/tean.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_TEAN_H_H_
#define SECSVR_INC_TEAN_H_H_

// sizeof = 8
struct CTean { // line 12
private:
void *m_pSys;
void *m_pLocal;
public:
CTean(); // line 14
~CTean(); // line 15
void Init(void *arg0, void *arg1); // line 17
void UnInit(); // line 18
void Tean(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 20
private:
void tean(unsigned int *arg0, unsigned int *arg1, int arg2); // line 22
};

#endif // SECSVR_INC_TEAN_H_H_
