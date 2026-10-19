// Auto-generated header stub from DWARF info
// Original path: inc/magic.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_MAGIC_H_H_
#define SECSVR_INC_MAGIC_H_H_

// sizeof = 8
struct CMagic8 { // line 7
private:
void *m_pSys;
void *m_pLocal;
public:
CMagic8(); // line 9
~CMagic8(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Magic8_8(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 16
void Magic8_16(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 20
void Magic8_32(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 24
private:
void MagicDc8(unsigned char *arg0, unsigned int arg1, unsigned char *arg2, unsigned char *arg3); // line 28
void MagicEc8(unsigned char *arg0, unsigned int arg1, unsigned char *arg2, unsigned char *arg3); // line 29
};

#endif // SECSVR_INC_MAGIC_H_H_
