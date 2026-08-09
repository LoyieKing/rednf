// CBlowFish -- 标准 Blowfish（CBC/ECB）
// 布局：m_oChain0(0)/m_oChain(8)/m_pdwAuiP[18](0x10)/m_pdwAuiS[4][256](0x58)
// m_pSys(0x1058)/m_pLocal(0x105c)，sizeof 0x1060。
#ifndef SECSVR_INC_BLOWFISH_H_H_
#define SECSVR_INC_BLOWFISH_H_H_

// sizeof = 4192
struct CBlowFish { // line 7
    // sizeof = 8
    struct SBlock {
    public:
    unsigned int m_uil;
    unsigned int m_uir;
    SBlock(unsigned int arg0, unsigned int arg1) : m_uil(arg0), m_uir(arg1) {} // line 12
    SBlock(const CBlowFish::SBlock &arg0) : m_uil(arg0.m_uil), m_uir(arg0.m_uir) {} // line 13
    CBlowFish::SBlock & operator^=(CBlowFish::SBlock &arg0) {
        m_uil ^= arg0.m_uil;
        m_uir ^= arg0.m_uir;
        return *this;
    } // line 14
    };
private:
static const unsigned int FUCKING_M_PDWINITP[];
static const unsigned int FUCKING_M_PDWINITS[4][256];
CBlowFish::SBlock m_oChain0;
CBlowFish::SBlock m_oChain;
unsigned int m_pdwAuiP[18];
unsigned int m_pdwAuiS[4][256];
void *m_pSys;
void *m_pLocal;
public:
CBlowFish(); // line 19
~CBlowFish(); // line 20
void Init(void *arg0, void *arg1); // line 22
void UnInit(); // line 23
void BlowFish8(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 27
void BlowFish16(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 31
void BlowFish24(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 35
void BlowFish32(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 39
void BlowFish56(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 43
private:
void BlowFish(unsigned char *arg0, unsigned int arg1); // line 47
void Encrypt(CBlowFish::SBlock &arg0); // line 49
void Decrypt(CBlowFish::SBlock &arg0); // line 50
void Encrypt(unsigned char *arg0, unsigned char *arg1, unsigned int arg2, int arg3); // line 52
void Decrypt(unsigned char *arg0, unsigned char *arg1, unsigned int arg2, int arg3); // line 53
unsigned int F(unsigned int arg0); // line 71
void BytesToBlock(unsigned char *arg0, CBlowFish::SBlock &arg1); // line 72
void BlockToBytes(CBlowFish::SBlock &arg0, unsigned char *arg1); // line 73
};
extern const unsigned int FUCKING_M_PDWINITP[]; // line 58
extern const unsigned int FUCKING_M_PDWINITS[4][256]; // line 60

#endif // SECSVR_INC_BLOWFISH_H_H_
