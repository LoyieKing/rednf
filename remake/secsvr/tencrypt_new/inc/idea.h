// Auto-generated header stub from DWARF info
// Original path: inc/idea.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_IDEA_H_H_
#define SECSVR_INC_IDEA_H_H_

// sizeof = 8
struct CIdea { // line 12
private:
void *m_pSys;
void *m_pLocal;
public:
CIdea(); // line 14
~CIdea(); // line 15
void Init(void *arg0, void *arg1); // line 17
void UnInit(); // line 18
void Idea(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 20
private:
unsigned int decipher_file(unsigned char *arg0, unsigned int arg1, short unsigned int *arg2); // line 22
unsigned int cipher_file(unsigned char *arg0, unsigned int arg1, short unsigned int *arg2); // line 23
void cipher_idea(short unsigned int *arg0, short unsigned int *arg1, short unsigned int *arg2); // line 24
short unsigned int mul(short unsigned int arg0, short unsigned int arg1); // line 25
void de_key_idea(short unsigned int *arg0, short unsigned int *arg1); // line 26
void en_key_idea(short unsigned int *arg0, short unsigned int *arg1); // line 27
short unsigned int inv(short unsigned int arg0); // line 28
};

#endif // SECSVR_INC_IDEA_H_H_
