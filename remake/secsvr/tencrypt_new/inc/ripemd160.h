/* ripemd160.h -- RIPEMD-160（Bosselaers 参考实现）的 tencrypt_new 变体
   由 DWARF 重建：CRipeMD160（sizeof 96 = bool+pad+20+8+64）。
   符号核对：_ZN10CRipeMD160... 系列。 */

#ifndef TENCRYPT_INC_RIPEMD160_H_
#define TENCRYPT_INC_RIPEMD160_H_

class CRipeMD160 {
public:
    CRipeMD160();
    ~CRipeMD160();
    void AddData(unsigned char *pcData, unsigned int iDataLength);
    void FinalDigest(unsigned char *pcDigest);
    void Reset();
private:
    bool m_bAddData;
    unsigned int m_auiBuf[5];    /* MDbuf 状态，20 字节 */
    unsigned int m_auiBits[2];   /* 位长计数，8 字节 */
    unsigned char m_aucIn[64];   /* 输入缓冲，64 字节 */

    unsigned int ROL(unsigned int x, unsigned int n);
    unsigned int F(unsigned int x, unsigned int y, unsigned int z);
    unsigned int G(unsigned int x, unsigned int y, unsigned int z);
    unsigned int H(unsigned int x, unsigned int y, unsigned int z);
    unsigned int I(unsigned int x, unsigned int y, unsigned int z);
    unsigned int J(unsigned int x, unsigned int y, unsigned int z);
    void FF160(unsigned int &ra, unsigned int b, unsigned int &rc,
               unsigned int d, unsigned int e, unsigned int x, unsigned int s);
    void GG160(unsigned int &ra, unsigned int b, unsigned int &rc,
               unsigned int d, unsigned int e, unsigned int x, unsigned int s);
    void HH160(unsigned int &ra, unsigned int b, unsigned int &rc,
               unsigned int d, unsigned int e, unsigned int x, unsigned int s);
    void II160(unsigned int &ra, unsigned int b, unsigned int &rc,
               unsigned int d, unsigned int e, unsigned int x, unsigned int s);
    void JJ160(unsigned int &ra, unsigned int b, unsigned int &rc,
               unsigned int d, unsigned int e, unsigned int x, unsigned int s);
    void FFF160(unsigned int &ra, unsigned int b, unsigned int &rc,
                unsigned int d, unsigned int e, unsigned int x, unsigned int s);
    void GGG160(unsigned int &ra, unsigned int b, unsigned int &rc,
                unsigned int d, unsigned int e, unsigned int x, unsigned int s);
    void HHH160(unsigned int &ra, unsigned int b, unsigned int &rc,
                unsigned int d, unsigned int e, unsigned int x, unsigned int s);
    void III160(unsigned int &ra, unsigned int b, unsigned int &rc,
                unsigned int d, unsigned int e, unsigned int x, unsigned int s);
    void JJJ160(unsigned int &ra, unsigned int b, unsigned int &rc,
                unsigned int d, unsigned int e, unsigned int x, unsigned int s);
    void Transform();
};

#endif /* TENCRYPT_INC_RIPEMD160_H_ */
