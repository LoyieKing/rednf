/* TenCrypt.h -- TenCrypt 公共接口（由 DWARF 重建 + 可编译修正）
   ITsLocal vtable 布局（二进制验证，vptr->vtable[0]）：
   [0] ProcessLastBytes [1] LocalGetModuleHandle [2] LocalGetProcAddress
   [3] Seattos [4] ~ITsLocal D1 [5] ~ITsLocal D0
   （虚析构按声明顺序排在最后）。 */
#ifndef TENCRYPT_INCLUDE_TENCRYPT_H_
#define TENCRYPT_INCLUDE_TENCRYPT_H_

// sizeof = 4
struct ITsLocal {
public:
    ITsLocal() {}
    virtual void ProcessLastBytes(unsigned char *pbyKey, bool bEncrypt,
                                  unsigned int dwBufLen, unsigned int dwKeySize,
                                  unsigned char *pbyBuf) = 0; // line 8
    virtual void * LocalGetModuleHandle() = 0;                // line 9
    virtual void * LocalGetProcAddress() = 0;                 // line 10
    virtual void Seattos(unsigned char *key, bool benc,
                         unsigned char *buf, unsigned int buflen) = 0; // line 12
    virtual ~ITsLocal() {}                                     // line 13
};

extern ITsLocal * CreateTsLocal();
extern void FreeTsLocal();

extern void TenAdder(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenAmoeba(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenBlowFish16(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenBlowFish24(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenBlowFish32(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenBlowFish56(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenBlowFish8(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenCast(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenD3des161(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenD3des162(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenD3des24(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenDes(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenDes2P(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenDesBig(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenDesDea(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenDesMo(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenDesNew(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenDesPC(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenDesToo(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenDiamond(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenFastDes(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenFeal8(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenFrog(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenGene(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenGeneNew(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void GeneNew(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenGost(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenGramary(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenHpc(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenIdea(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenLoki89(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenLoki91(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenLoki97(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenMagic16(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenMagic32(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenMagic8(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenMars(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenMbc(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenMmb(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenNsea(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenRc2(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenRc4(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenRc5(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenRc6(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenRijnDael(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenRing(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenSafer(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenSeattos(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenShift(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenSquare(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenSubtracter(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenTean(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenThreeWay(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenTwofish16(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenTwofish24(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);
extern void TenTwofish32(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen);

#endif /* TENCRYPT_INCLUDE_TENCRYPT_H_ */
