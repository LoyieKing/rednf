/* ripemd128.h -- RIPEMD-128（Bosselaers 参考实现）的 tencrypt_new 变体
   由 DWARF 重建：CRipeMD128（sizeof 92 = 16+8+64+1+3）。
   符号核对：_ZN10CRipeMD128... 系列。 */

#ifndef TENCRYPT_INC_RIPEMD128_H_
#define TENCRYPT_INC_RIPEMD128_H_

class CRipeMD128 {
public:
    CRipeMD128();
    ~CRipeMD128();
    void AddData(unsigned char *pcData, unsigned int iDataLength);
    void FinalDigest(unsigned char *pcDigest);
    void Reset();
private:
    unsigned int m_auiBuf[4];    /* MDbuf 状态，16 字节 */
    unsigned int m_auiBits[2];   /* 位长计数，8 字节 */
    unsigned char m_aucIn[64];   /* 输入缓冲，64 字节 */
    bool m_bAddData;

    unsigned int ROL(unsigned int x, unsigned int n);
    unsigned int F(unsigned int x, unsigned int y, unsigned int z);
    unsigned int G(unsigned int x, unsigned int y, unsigned int z);
    unsigned int H(unsigned int x, unsigned int y, unsigned int z);
    unsigned int I(unsigned int x, unsigned int y, unsigned int z);
    void FF128(unsigned int &ra, unsigned int b, unsigned int c,
               unsigned int d, unsigned int x, unsigned int s);
    void GG128(unsigned int &ra, unsigned int b, unsigned int c,
               unsigned int d, unsigned int x, unsigned int s);
    void HH128(unsigned int &ra, unsigned int b, unsigned int c,
               unsigned int d, unsigned int x, unsigned int s);
    void II128(unsigned int &ra, unsigned int b, unsigned int c,
               unsigned int d, unsigned int x, unsigned int s);
    void FFF128(unsigned int &ra, unsigned int b, unsigned int c,
                unsigned int d, unsigned int x, unsigned int s);
    void GGG128(unsigned int &ra, unsigned int b, unsigned int c,
                unsigned int d, unsigned int x, unsigned int s);
    void HHH128(unsigned int &ra, unsigned int b, unsigned int c,
                unsigned int d, unsigned int x, unsigned int s);
    void III128(unsigned int &ra, unsigned int b, unsigned int c,
                unsigned int d, unsigned int x, unsigned int s);
    void Transform();
};

#endif /* TENCRYPT_INC_RIPEMD128_H_ */
