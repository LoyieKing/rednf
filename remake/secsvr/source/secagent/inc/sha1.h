// Auto-generated header stub from DWARF info
// Original path: inc/sha1.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_SHA1_H_H_
#define SECSVR_INC_SHA1_H_H_

// sizeof = 92
struct _SHA1_CTX { // line 7
public:
unsigned int state[];
unsigned int count[];
unsigned char buffer[];
};
typedef _SHA1_CTX SHA1_CTX;
// sizeof = 1
struct CSha1 { // line 14
public:
CSha1(); // line 17
~CSha1(); // line 18
void SHA1_Init(SHA1_CTX *arg0); // line 20
void SHA1_Update(SHA1_CTX *arg0, unsigned char *arg1, unsigned int arg2); // line 21
void SHA1_Final(unsigned char *arg0, SHA1_CTX *arg1); // line 22
private:
void SHA1Transform(unsigned int *arg0, unsigned char *arg1); // line 26
};

#endif // SECSVR_INC_SHA1_H_H_
