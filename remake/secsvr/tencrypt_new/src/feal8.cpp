/* feal8.cpp -- FEAL-8 块密码（NTT 1989 实现，与二进制逐字节核对）
   Rot2 查表（FUCKING_ROT2VAL 从 .rodata 0x081ccc80 提取 = rotl(x,2)）。
   结构：SetKey 用 FK 生成 16 轮子钥 + K89/K1011/K1213/K1415；
   8 轮 Feistel + 首尾密钥混合。 */

#include "inc/feal8.h"
#include "include/TenCrypt.h"

#include <string.h>

typedef unsigned char ByteType;
typedef unsigned long HalfWord;
typedef unsigned int QuarterWord;

const unsigned char CFeal8::FUCKING_ROT2VAL[256] = {
#include "feal8_rot2.inc"
};

// mangled: _ZN6CFeal84Rot2Eh
unsigned char CFeal8::Rot2(unsigned char X) {
    int First;
    unsigned char RetVal[256];
    int i;
    int High;
    int Low;

    (void)First; (void)RetVal; (void)i; (void)High; (void)Low;
    return FUCKING_ROT2VAL[X];
}

// mangled: _ZN6CFeal86MakeH1EPh
unsigned int CFeal8::MakeH1(unsigned char *B) {
    union {
        unsigned long All;
        ByteType Byte[4];
    } RetVal;

    RetVal.Byte[0] = *B++;
    RetVal.Byte[1] = *B++;
    RetVal.Byte[2] = *B++;
    RetVal.Byte[3] = *B;
    return RetVal.All;
}

// mangled: _ZN6CFeal81fEjj
unsigned int CFeal8::f(unsigned int AA, unsigned int BB) {
    ByteType f1;
    ByteType f2;
    union {
        unsigned long All;
        ByteType Byte[4];
    } RetVal, A;
    union {
        unsigned int All;
        ByteType Byte[2];
    } B;

    A.All = AA;
    B.All = BB;
    f1 = A.Byte[1] ^ B.Byte[0] ^ A.Byte[0];
    f2 = A.Byte[2] ^ B.Byte[1] ^ A.Byte[3];
    f1 = (ByteType)(Rot2((ByteType)((f1 + f2 + 1) & 0xff)));  /* S1(f1,f2) */
    f2 = (ByteType)(Rot2((ByteType)((f2 + f1) & 0xff)));      /* S0(f2,f1) */
    RetVal.Byte[1] = f1;
    RetVal.Byte[2] = f2;
    RetVal.Byte[0] = (ByteType)(Rot2((ByteType)((A.Byte[0] + f1) & 0xff))); /* S0 */
    RetVal.Byte[3] = (ByteType)(Rot2((ByteType)((A.Byte[3] + f2 + 1) & 0xff))); /* S1 */
    return RetVal.All;
}

// mangled: _ZN6CFeal82FKEjj
unsigned int CFeal8::FK(unsigned int AA, unsigned int BB) {
    ByteType FK1;
    ByteType FK2;
    union {
        unsigned long All;
        ByteType Byte[4];
    } RetVal, A, B;

    A.All = AA;
    B.All = BB;
    FK1 = (ByteType)(A.Byte[1] ^ A.Byte[0]);
    FK2 = (ByteType)(A.Byte[2] ^ A.Byte[3]);
    FK1 = (ByteType)(Rot2((ByteType)((FK1 + (FK2 ^ B.Byte[0]) + 1) & 0xff))); /* S1 */
    FK2 = (ByteType)(Rot2((ByteType)((FK2 + (FK1 ^ B.Byte[1])) & 0xff)));    /* S0 */
    RetVal.Byte[1] = FK1;
    RetVal.Byte[2] = FK2;
    RetVal.Byte[0] = (ByteType)(Rot2((ByteType)((A.Byte[0] + (FK1 ^ B.Byte[2])) & 0xff))); /* S0 */
    RetVal.Byte[3] = (ByteType)(Rot2((ByteType)((A.Byte[3] + (FK2 ^ B.Byte[3]) + 1) & 0xff))); /* S1 */
    return RetVal.All;
}

// mangled: _ZN6CFeal86DissQ1EjPh
void CFeal8::DissQ1(unsigned int Q, unsigned char *B) {
    union {
        QuarterWord All;
        ByteType Byte[2];
    } QQ;

    QQ.All = Q;
    *B++ = QQ.Byte[0];
    *B = QQ.Byte[1];
}

// mangled: _ZN6CFeal86MakeH2EPj
unsigned int CFeal8::MakeH2(unsigned int *Q) {
    unsigned char B[4];

    DissQ1(*Q++, B);
    DissQ1(*Q, B + 2);
    return MakeH1(B);
}

// mangled: _ZN6CFeal86DissH1EjPh
void CFeal8::DissH1(unsigned int H, unsigned char *D) {
    union {
        HalfWord All;
        ByteType Byte[4];
    } T;

    T.All = H;
    *D++ = T.Byte[0];
    *D++ = T.Byte[1];
    *D++ = T.Byte[2];
    *D = T.Byte[3];
}

// mangled: _ZN6CFeal86SetKeyEPh
void CFeal8::SetKey(unsigned char *KP) {
    union {
        HalfWord All;
        ByteType Byte[4];
    } A, B, D, NewB;
    union {
        QuarterWord All;
        ByteType Byte[2];
    } Q;
    int i;
    unsigned int *Out;

    A.Byte[0] = *KP++;
    A.Byte[1] = *KP++;
    A.Byte[2] = *KP++;
    A.Byte[3] = *KP++;
    B.Byte[0] = *KP++;
    B.Byte[1] = *KP++;
    B.Byte[2] = *KP++;
    B.Byte[3] = *KP;
    D.All = 0;

    for (i = 1, Out = K; i <= 8; i++) {
        NewB.All = FK(A.All, B.All ^ D.All);
        D = A;
        A = B;
        B = NewB;
        Q.Byte[0] = B.Byte[0];
        Q.Byte[1] = B.Byte[1];
        *Out++ = (unsigned int)Q.Byte[0] | ((unsigned int)Q.Byte[1] << 8);
        Q.Byte[0] = B.Byte[2];
        Q.Byte[1] = B.Byte[3];
        *Out++ = (unsigned int)Q.Byte[0] | ((unsigned int)Q.Byte[1] << 8);
    }
    K89 = MakeH2(K + 8);
    K1011 = MakeH2(K + 10);
    K1213 = MakeH2(K + 12);
    K1415 = MakeH2(K + 14);
}

// mangled: _ZN6CFeal87EncryptEPhS0_
void CFeal8::Encrypt(unsigned char *Plain, unsigned char *Cipher) {
    unsigned int L;
    unsigned int R;
    unsigned int NewR;
    int r;

    L = MakeH1(Plain);
    R = MakeH1(Plain + 4);
    L ^= K89;
    R ^= K1011;
    R ^= L;
    for (r = 0; r < 8; r++) {
        NewR = L ^ f(R, K[r]);
        L = R;
        R = NewR;
    }
    L ^= R;
    R ^= K1213;
    L ^= K1415;
    DissH1(R, Cipher);
    DissH1(L, Cipher + 4);
}

// mangled: _ZN6CFeal87DecryptEPhS0_
void CFeal8::Decrypt(unsigned char *Cipher, unsigned char *Plain) {
    unsigned int L;
    unsigned int R;
    unsigned int NewL;
    int r;

    R = MakeH1(Cipher);
    L = MakeH1(Cipher + 4);
    R ^= K1213;
    L ^= K1415;
    L ^= R;
    for (r = 7; r >= 0; r--) {
        NewL = R ^ f(L, K[r]);
        R = L;
        L = NewL;
    }
    R ^= L;
    R ^= K1011;
    L ^= K89;
    DissH1(L, Plain);
    DissH1(R, Plain + 4);
}

// mangled: _ZN6CFeal85Feal8EPhbS0_j
void CFeal8::Feal8(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (buflen != 0) {
        SetKey(key);
        nLen = buflen / 8;
        for (i = 0; i < nLen; i++) {
            if (benc)
                Encrypt(buf + i * 8, buf + i * 8);
            else
                Decrypt(buf + i * 8, buf + i * 8);
        }
        pLocal = CreateTsLocal();
        pLocal->Seattos(key, benc, buf + nLen * 8, buflen - nLen * 8);
    }
}

// mangled: _ZN6CFeal84InitEPvS0_
void CFeal8::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
}

// mangled: _ZN6CFeal86UnInitEv
void CFeal8::UnInit() {
}

// mangled: _ZN6CFeal8C1Ev / _ZN6CFeal8C2Ev
CFeal8::CFeal8() {
    Init(0, 0);
}

// mangled: _ZN6CFeal8D1Ev / _ZN6CFeal8D2Ev
CFeal8::~CFeal8() {
    UnInit();
}

// mangled: _Z8TenFeal8PhbS_j
void TenFeal8(unsigned char *key, bool benc, unsigned char *buf,
              unsigned int buflen) {
    CFeal8 feal8;

    feal8.Feal8(key, benc, buf, buflen);
}
