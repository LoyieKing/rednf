// Auto-generated header stub from DWARF info
// Original path: inc/md4.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_MD4_H_H_
#define SECSVR_INC_MD4_H_H_

// sizeof = 88
struct ._33 { // line 9
public:
unsigned int state[];
unsigned int count[];
unsigned char buffer[];
};
// sizeof = 4
struct CMD4 { // line 16
private:
unsigned char *m_pszPadding;
public:
CMD4(); // line 19
~CMD4(); // line 20
void Init(); // line 22
void UnInit(); // line 23
void MD4Init(._33 *arg0); // line 25
void MD4Update(._33 *arg0, unsigned char *arg1, unsigned int arg2); // line 26
void MD4Final(unsigned char *arg0, ._33 *arg1); // line 27
private:
void MD4Transform(unsigned int *arg0, unsigned char *arg1); // line 31
void Encode(unsigned char *arg0, unsigned int *arg1, unsigned int arg2); // line 32
void Decode(unsigned int *arg0, unsigned char *arg1, unsigned int arg2); // line 33
};

#endif // SECSVR_INC_MD4_H_H_
