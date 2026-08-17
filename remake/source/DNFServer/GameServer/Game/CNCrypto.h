#ifndef GAME_CNCRYPTO_H_
#define GAME_CNCRYPTO_H_

// df_game_r CNCrypto* 统一分组密码封装（G7，2026-08-16 还原）。
// 14 类统一接口：Initialize/SetMode/Encrypt(2)/Decrypt(2)/Encrypt(4)/Decrypt(4)/
// GetBlockSize/GetKeySize，由 CreateCryptoGraph 工厂构造进 Cipher。
// 底层算法实现见 CNCryptoAlgo.cpp（LibTomCrypt 移植，算法正确即可）。

#include "Cipher.h"

class CNCryptoShift : public ICryptoGraph
{
public:
    CNCryptoShift();
    virtual ~CNCryptoShift();
    CNCryptoShift(const CNCryptoShift& other);
    CNCryptoShift& operator=(const CNCryptoShift& other);
    virtual int SetMode(int mode);
    virtual int Initialize(const unsigned char* key, int keyLen);
    virtual int Encrypt(unsigned char* buf, int len);
    virtual int Decrypt(unsigned char* buf, int len);
    virtual int Encrypt(const unsigned char* in, int inLen,
                        unsigned char* out, int outCap);
    virtual int Decrypt(const unsigned char* in, int inLen,
                        unsigned char* out, int outCap);
    virtual int GetKeySize() const;
    virtual int GetBlockSize() const;
private:
    unsigned int m_shift;   // +0x08
    unsigned int m_key;     // +0x0c
};

class CNCryptoRijndael : public ICryptoGraph
{
public:
    CNCryptoRijndael();
    virtual ~CNCryptoRijndael();
    CNCryptoRijndael(const CNCryptoRijndael& other);
    CNCryptoRijndael& operator=(const CNCryptoRijndael& other);
    virtual int SetMode(int mode);
    virtual int Initialize(const unsigned char* key, int keyLen);
    virtual int Encrypt(unsigned char* buf, int len);
    virtual int Decrypt(unsigned char* buf, int len);
    virtual int Encrypt(const unsigned char* in, int inLen,
                        unsigned char* out, int outCap);
    virtual int Decrypt(const unsigned char* in, int inLen,
                        unsigned char* out, int outCap);
    virtual int GetKeySize() const;
    virtual int GetBlockSize() const;
private:
    void* m_pCrypto;   // +0x08 CRijndael*
};

class CNCryptoTea : public ICryptoGraph
{
public:
    CNCryptoTea();
    virtual ~CNCryptoTea();
    CNCryptoTea(const CNCryptoTea& other);
    CNCryptoTea& operator=(const CNCryptoTea& other);
    virtual int SetMode(int mode);
    virtual int Initialize(const unsigned char* key, int keyLen);
    virtual int Encrypt(unsigned char* buf, int len);
    virtual int Decrypt(unsigned char* buf, int len);
    virtual int Encrypt(const unsigned char* in, int inLen,
                        unsigned char* out, int outCap);
    virtual int Decrypt(const unsigned char* in, int inLen,
                        unsigned char* out, int outCap);
    virtual int GetKeySize() const;
    virtual int GetBlockSize() const;
private:
    void* m_pCrypto;   // +0x08 CTEA*
};

class CNCryptoBlowFish : public ICryptoGraph
{
public:
    CNCryptoBlowFish();
    virtual ~CNCryptoBlowFish();
    CNCryptoBlowFish(const CNCryptoBlowFish& other);
    CNCryptoBlowFish& operator=(const CNCryptoBlowFish& other);
    virtual int SetMode(int mode);
    virtual int Initialize(const unsigned char* key, int keyLen);
    virtual int Encrypt(unsigned char* buf, int len);
    virtual int Decrypt(unsigned char* buf, int len);
    virtual int Encrypt(const unsigned char* in, int inLen,
                        unsigned char* out, int outCap);
    virtual int Decrypt(const unsigned char* in, int inLen,
                        unsigned char* out, int outCap);
    virtual int GetKeySize() const;
    virtual int GetBlockSize() const;
private:
    void* m_pCrypto;     // +0x08 symmetric_key*
    unsigned int m_mode; // +0x0c
};

#define CNCrypto_DECL(Name, TypeId)                                        \
class Name : public ICryptoGraph                                           \
{                                                                          \
public:                                                                    \
    Name();                                                                \
    virtual ~Name();                                                       \
    Name(const Name& other);                                               \
    Name& operator=(const Name& other);                                    \
    virtual int SetMode(int mode);                                         \
    virtual int Initialize(const unsigned char* key, int keyLen);          \
    virtual int Encrypt(unsigned char* buf, int len);                      \
    virtual int Decrypt(unsigned char* buf, int len);                      \
    virtual int Encrypt(const unsigned char* in, int inLen,                \
                        unsigned char* out, int outCap);                   \
    virtual int Decrypt(const unsigned char* in, int inLen,                \
                        unsigned char* out, int outCap);                   \
    virtual int GetKeySize() const;                                        \
    virtual int GetBlockSize() const;                                      \
private:                                                                   \
    void* m_pCrypto;   /* +0x08 symmetric_key* */                          \
};

CNCrypto_DECL(CNCryptoRc6, CRYPTO_GRAPH_RC6)
CNCrypto_DECL(CNCryptoTwoFish, CRYPTO_GRAPH_TWOFISH)
CNCrypto_DECL(CNCryptoKasumi, CRYPTO_GRAPH_KASUMI)
CNCrypto_DECL(CNCryptoXTea, CRYPTO_GRAPH_XTEA)
CNCrypto_DECL(CNCryptoNoekeon, CRYPTO_GRAPH_NOEKEON)
CNCrypto_DECL(CNCryptoKhazad, CRYPTO_GRAPH_KHAZAD)
CNCrypto_DECL(CNCryptoCast5, CRYPTO_GRAPH_CAST5)
CNCrypto_DECL(CNCryptoSkipjack, CRYPTO_GRAPH_SKIPJACK)
CNCrypto_DECL(CNCryptoMulti2, CRYPTO_GRAPH_MULTI2)
CNCrypto_DECL(CNCryptoAnubis, CRYPTO_GRAPH_ANUBIS)

#undef CNCrypto_DECL

#endif  // GAME_CNCRYPTO_H_
