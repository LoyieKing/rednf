// Auto-generated header stub from DWARF info
// Original path: include/TenCrypt.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INCLUDE_TENCRYPT_H_H_
#define SECSVR_INCLUDE_TENCRYPT_H_H_

// sizeof = 4
struct ITsLocal { // line 6
public:
int (***_vptr.ITsLocal)(...);
ITsLocal();
ITsLocal(const ITsLocal &arg0);
virtual void ProcessLastBytes(unsigned char *arg0, bool arg1, unsigned int arg2, unsigned int arg3, unsigned char *arg4); // line 8
virtual void * LocalGetModuleHandle(); // line 9
virtual void * LocalGetProcAddress(); // line 10
virtual void Seattos(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 12
virtual ~ITsLocal(); // line 13
};


extern ITsLocal * CreateTsLocal(); // line 1006 (inferred)
extern void FreeTsLocal(); // line 1015 (inferred)
extern void TenAdder(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 699 (inferred)
extern void TenAmoeba(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 869 (inferred)
extern void TenBlowFish16(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 566 (inferred)
extern void TenBlowFish24(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 582 (inferred)
extern void TenBlowFish32(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 598 (inferred)
extern void TenBlowFish56(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 614 (inferred)
extern void TenBlowFish8(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 550 (inferred)
extern void TenCast(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 76 (inferred)
extern void TenD3des161(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 119 (inferred)
extern void TenD3des162(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 135 (inferred)
extern void TenD3des24(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 103 (inferred)
extern void TenDes(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 58 (inferred)
extern void TenDes2P(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 473 (inferred)
extern void TenDesBig(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 490 (inferred)
extern void TenDesDea(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 7 (inferred)
extern void TenDesMo(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 649 (inferred)
extern void TenDesNew(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 456 (inferred)
extern void TenDesPC(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 524 (inferred)
extern void TenDesToo(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 507 (inferred)
extern void TenDiamond(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 852 (inferred)
extern void TenFastDes(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 422 (inferred)
extern void TenFeal8(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 632 (inferred)
extern void TenFrog(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 439 (inferred)
extern void TenGene(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 666 (inferred)
extern void TenGeneNew(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 683 (inferred)
extern void TenGost(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 337 (inferred)
extern void TenGramary(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 716 (inferred)
extern void TenHpc(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 244 (inferred)
extern void TenIdea(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 262 (inferred)
extern void TenLoki89(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 903 (inferred)
extern void TenLoki91(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 920 (inferred)
extern void TenLoki97(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 886 (inferred)
extern void TenMagic16(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 304 (inferred)
extern void TenMagic32(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 319 (inferred)
extern void TenMagic8(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 288 (inferred)
extern void TenMars(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 354 (inferred)
extern void TenMbc(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 371 (inferred)
extern void TenMmb(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 388 (inferred)
extern void TenNsea(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 835 (inferred)
extern void TenRc2(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 208 (inferred)
extern void TenRc4(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 226 (inferred)
extern void TenRc5(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 190 (inferred)
extern void TenRc6(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 172 (inferred)
extern void TenRijnDael(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 40 (inferred)
extern void TenRing(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 818 (inferred)
extern void TenSafer(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 733 (inferred)
extern void TenSeattos(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 22 (inferred)
extern void TenShift(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 767 (inferred)
extern void TenSquare(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 784 (inferred)
extern void TenSubtracter(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 801 (inferred)
extern void TenTean(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 154 (inferred)
extern void TenThreeWay(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 937 (inferred)
extern void TenTwofish16(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 963 (inferred)
extern void TenTwofish24(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 979 (inferred)
extern void TenTwofish32(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 995 (inferred)

#endif // SECSVR_INCLUDE_TENCRYPT_H_H_
