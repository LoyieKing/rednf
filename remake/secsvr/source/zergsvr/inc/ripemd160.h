// Auto-generated header stub from DWARF info
// Original path: inc/ripemd160.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_RIPEMD160_H_H_
#define SECSVR_INC_RIPEMD160_H_H_

// sizeof = 96
struct CRipeMD160 { // line 9
private:
bool m_bAddData;
unsigned int m_auiBuf[];
unsigned int m_auiBits[];
unsigned char m_aucIn[];
public:
CRipeMD160(); // line 11
~CRipeMD160(); // line 12
void AddData(unsigned char *arg0, unsigned int arg1); // line 14
void FinalDigest(unsigned char *arg0); // line 15
void Reset(); // line 16
private:
unsigned int ROL(unsigned int arg0, unsigned int arg1); // line 19
unsigned int F(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 22
unsigned int G(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 24
unsigned int H(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 26
unsigned int I(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 28
unsigned int J(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 30
void FF160(unsigned int &arg0, unsigned int arg1, unsigned int &arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6); // line 34
void GG160(unsigned int &arg0, unsigned int arg1, unsigned int &arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6); // line 37
void HH160(unsigned int &arg0, unsigned int arg1, unsigned int &arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6); // line 40
void II160(unsigned int &arg0, unsigned int arg1, unsigned int &arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6); // line 43
void JJ160(unsigned int &arg0, unsigned int arg1, unsigned int &arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6); // line 46
void FFF160(unsigned int &arg0, unsigned int arg1, unsigned int &arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6); // line 49
void GGG160(unsigned int &arg0, unsigned int arg1, unsigned int &arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6); // line 52
void HHH160(unsigned int &arg0, unsigned int arg1, unsigned int &arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6); // line 55
void III160(unsigned int &arg0, unsigned int arg1, unsigned int &arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6); // line 58
void JJJ160(unsigned int &arg0, unsigned int arg1, unsigned int &arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6); // line 61
void Transform(); // line 63
};

#endif // SECSVR_INC_RIPEMD160_H_H_
