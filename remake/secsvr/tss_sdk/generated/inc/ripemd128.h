// Auto-generated header stub from DWARF info
// Original path: inc/ripemd128.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_RIPEMD128_H_H_
#define SECSVR_INC_RIPEMD128_H_H_

// sizeof = 92
struct CRipeMD128 { // line 8
private:
unsigned int m_auiBuf[];
unsigned int m_auiBits[];
unsigned char m_aucIn[];
bool m_bAddData;
public:
CRipeMD128(); // line 10
~CRipeMD128(); // line 11
void AddData(unsigned char *arg0, unsigned int arg1); // line 13
void FinalDigest(unsigned char *arg0); // line 14
void Reset(); // line 15
private:
unsigned int ROL(unsigned int arg0, unsigned int arg1); // line 18
unsigned int F(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 21
unsigned int G(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 23
unsigned int H(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 25
unsigned int I(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 27
void FF128(unsigned int &arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5); // line 32
void GG128(unsigned int &arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5); // line 35
void HH128(unsigned int &arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5); // line 38
void II128(unsigned int &arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5); // line 41
void FFF128(unsigned int &arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5); // line 44
void GGG128(unsigned int &arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5); // line 47
void HHH128(unsigned int &arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5); // line 50
void III128(unsigned int &arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5); // line 53
void Transform(); // line 55
};

#endif // SECSVR_INC_RIPEMD128_H_H_
