// df_game_r CRijndael（G7 AES/Rijndael 底层）还原（2026-08-16）。
// 算法正确即可：完整 Rijndael（Nb=4/6/8，Nk=4/6/8），ECB/CBC/CFB 三种模式，
// 接口与 docs/class_func_reports/CRijndael.md 一致。

#include "CRijndael.h"

#include <cstdio>
#include <cstring>

#include "SHA.h"   // nsl::CSHA（SHA-256，ServerLib 已还原，Signature 用）

// ============================================================================
// IMethod 扩展方法（G7 分组密码模式基类；ORIG @0x80b42d2 区）
// ============================================================================

void IMethod::Xor(char* buff, const char* chain)
{
    for (int i = 0; i < m_blockSize; ++i)
    {
        buff[i] = (char)((unsigned char)buff[i] ^ (unsigned char)chain[i]);
    }
}

void IMethod::SetMode(int iMode)
{
    if (iMode >= 0 && iMode < 3)
    {
        m_mode = iMode;
    }
}

// ============================================================================
// Rijndael GF(2^8) 基础运算
// ============================================================================

namespace
{

// 标准 AES S 盒（ORIG 0x8b1fca0 同值）
const unsigned char kSBox[256] = {
    0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
    0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
    0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
    0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
    0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
    0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
    0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
    0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
    0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
    0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
    0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
    0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
    0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
    0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
    0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
    0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

// 逆 S 盒（ORIG 0x8b1fda0）
const unsigned char kSBoxInv[256] = {
    0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
    0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
    0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
    0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
    0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
    0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
    0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
    0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
    0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
    0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
    0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
    0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
    0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
    0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
    0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
    0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

const unsigned char kRcon[30] = {
    0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36,0x6c,0xd8,0xab,0x4d,0x9a,
    0x2f,0x5e,0xbc,0x63,0xc6,0x97,0x35,0x6a,0xd4,0xb3,0x7d,0xfa,0xef,0xc5,0x91
};

// sm_shifts（ORIG 0x8b22ec0）：[Nb 行][8] = {enc1,enc2,enc3,dec1,dec2,dec3}
const int kShifts[3][6] = {
    {1, 2, 3, 3, 2, 1},   // Nb=4
    {1, 2, 3, 5, 4, 3},   // Nb=6
    {1, 3, 4, 7, 5, 4}    // Nb=8
};

inline unsigned char GfMul(unsigned char a, unsigned char b)
{
    unsigned char r = 0;
    while (b)
    {
        if (b & 1)
            r = (unsigned char)(r ^ a);
        unsigned char hi = (unsigned char)(a & 0x80);
        a = (unsigned char)(a << 1);
        if (hi)
            a = (unsigned char)(a ^ 0x1b);
        b = (unsigned char)(b >> 1);
    }
    return r;
}

inline unsigned int RotWord(unsigned int w)
{
    return ((w << 8) & 0xffffff00U) | ((w >> 24) & 0xffU);
}

inline unsigned int SubWord(unsigned int w)
{
    return (unsigned int)kSBox[(w >> 24) & 0xff] << 24 |
           (unsigned int)kSBox[(w >> 16) & 0xff] << 16 |
           (unsigned int)kSBox[(w >> 8) & 0xff] << 8 |
           (unsigned int)kSBox[w & 0xff];
}

}  // namespace

// ============================================================================
// CRijndael
// ============================================================================

const char CRijndael::sm_chain0[0x10] = {0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0};

CRijndael::CRijndael()
{
    m_bInit = false;
    m_blockSize = 0x10;
    m_keyLength = 0;
    m_mode = 0;
    m_padding = 0;
    std::memset(m_key, 0, sizeof(m_key));
    std::memset(m_w, 0, sizeof(m_w));
    std::memset(m_wd, 0, sizeof(m_wd));
    std::memset(m_block, 0, sizeof(m_block));
    std::memset(m_chain, 0, sizeof(m_chain));
    std::memset(m_temp, 0, sizeof(m_temp));
    std::memset(m_temp2, 0, sizeof(m_temp2));
    std::memset(m_temp3, 0, sizeof(m_temp3));
    m_rounds = 0;
}

CRijndael::~CRijndael()
{
}

int CRijndael::Initialize(const char* key, int keyLen, const char* chain,
                          int chainLen, int blockSize, int mode, int padding)
{
    if (key == 0)
        return 0x70000001;
    if (keyLen < 1)
        return 0x70000002;
    if (chainLen != 0x10 && chainLen != 0x18 && chainLen != 0x20)
        return 0x70000009;
    if (blockSize != 0x10 && blockSize != 0x18 && blockSize != 0x20)
        return 0x70000006;
    if (mode < 0 || mode > 2)
        return 0x70000003;
    if (padding < 0 || padding > 2)
        return 0x70000004;

    m_mode = mode;
    m_padding = padding;

    // key 循环填充到 chainLen 字节（ORIG local_58）
    unsigned char cycled[0x20];
    int src = 0;
    for (int i = 0; i < chainLen; ++i)
    {
        cycled[i] = (unsigned char)key[src];
        src = (src + 1) % keyLen;
    }

    m_blockSize = blockSize;
    std::memcpy(m_block, chain, blockSize);
    std::memcpy(m_chain, m_block, blockSize);
    m_keyLength = chainLen;
    std::memcpy(m_key, cycled, m_keyLength);

    int nk = m_keyLength / 4;
    int nb = m_blockSize / 4;
    if (nk == 4)
        m_rounds = (nb == 4) ? 10 : (nb == 6) ? 12 : 14;
    else if (nk == 6)
        m_rounds = (nb == 8) ? 14 : 12;
    else
        m_rounds = 14;

    KeyExpansion();
    m_bInit = true;
    return 0x6fffffff;
}

void CRijndael::KeyExpansion()
{
    int nk = m_keyLength / 4;
    int nb = m_blockSize / 4;
    int nr = (int)m_rounds;
    int total = nb * (nr + 1);

    for (int i = 0; i < nk; ++i)
    {
        m_w[i] = ((unsigned int)m_key[i * 4] << 24) |
                 ((unsigned int)m_key[i * 4 + 1] << 16) |
                 ((unsigned int)m_key[i * 4 + 2] << 8) |
                 (unsigned int)m_key[i * 4 + 3];
    }

    for (int i = nk; i < total; ++i)
    {
        unsigned int temp = m_w[i - 1];
        if (i % nk == 0)
        {
            temp = SubWord(RotWord(temp)) ^ (unsigned int)kRcon[i / nk - 1] << 24;
        }
        else if (nk > 6 && i % nk == 4)
        {
            temp = SubWord(temp);
        }
        m_w[i] = m_w[i - nk] ^ temp;
    }
}

void CRijndael::AddRoundKey(unsigned char state[4][8], int round)
{
    int nb = m_blockSize / 4;
    for (int c = 0; c < nb; ++c)
    {
        unsigned int w = m_w[round * nb + c];
        state[0][c] = (unsigned char)(state[0][c] ^ ((w >> 24) & 0xff));
        state[1][c] = (unsigned char)(state[1][c] ^ ((w >> 16) & 0xff));
        state[2][c] = (unsigned char)(state[2][c] ^ ((w >> 8) & 0xff));
        state[3][c] = (unsigned char)(state[3][c] ^ (w & 0xff));
    }
}

void CRijndael::InvAddRoundKey(unsigned char state[4][8], int round)
{
    AddRoundKey(state, round);
}

void CRijndael::EncryptCore(const unsigned char* in, unsigned char* out)
{
    int nb = m_blockSize / 4;
    int nr = (int)m_rounds;
    const int* sh = kShifts[(nb - 4) / 2];
    unsigned char state[4][8];

    for (int c = 0; c < nb; ++c)
        for (int r = 0; r < 4; ++r)
            state[r][c] = in[r + c * 4];

    AddRoundKey(state, 0);

    for (int round = 1; round < nr; ++round)
    {
        // SubBytes
        for (int c = 0; c < nb; ++c)
            for (int r = 0; r < 4; ++r)
                state[r][c] = kSBox[state[r][c]];

        // ShiftRows（行 r 左移 sh[r-1]）
        for (int r = 1; r < 4; ++r)
        {
            unsigned char tmp[8];
            int s = sh[r - 1];
            for (int c = 0; c < nb; ++c)
                tmp[c] = state[r][(c + s) % nb];
            for (int c = 0; c < nb; ++c)
                state[r][c] = tmp[c];
        }

        // MixColumns
        for (int c = 0; c < nb; ++c)
        {
            unsigned char a0 = state[0][c], a1 = state[1][c];
            unsigned char a2 = state[2][c], a3 = state[3][c];
            state[0][c] = (unsigned char)(GfMul(a0, 2) ^ GfMul(a1, 3) ^ a2 ^ a3);
            state[1][c] = (unsigned char)(a0 ^ GfMul(a1, 2) ^ GfMul(a2, 3) ^ a3);
            state[2][c] = (unsigned char)(a0 ^ a1 ^ GfMul(a2, 2) ^ GfMul(a3, 3));
            state[3][c] = (unsigned char)(GfMul(a0, 3) ^ a1 ^ a2 ^ GfMul(a3, 2));
        }

        AddRoundKey(state, round);
    }

    // 最后一轮
    for (int c = 0; c < nb; ++c)
        for (int r = 0; r < 4; ++r)
            state[r][c] = kSBox[state[r][c]];
    for (int r = 1; r < 4; ++r)
    {
        unsigned char tmp[8];
        int s = sh[r - 1];
        for (int c = 0; c < nb; ++c)
            tmp[c] = state[r][(c + s) % nb];
        for (int c = 0; c < nb; ++c)
            state[r][c] = tmp[c];
    }
    AddRoundKey(state, nr);

    for (int c = 0; c < nb; ++c)
        for (int r = 0; r < 4; ++r)
            out[r + c * 4] = state[r][c];
}

void CRijndael::DecryptCore(const unsigned char* in, unsigned char* out)
{
    int nb = m_blockSize / 4;
    int nr = (int)m_rounds;
    const int* sh = kShifts[(nb - 4) / 2];
    unsigned char state[4][8];

    for (int c = 0; c < nb; ++c)
        for (int r = 0; r < 4; ++r)
            state[r][c] = in[r + c * 4];

    InvAddRoundKey(state, nr);

    for (int round = nr - 1; round >= 1; --round)
    {
        // InvShiftRows（行 r 右移 sh[r-1]）
        for (int r = 1; r < 4; ++r)
        {
            unsigned char tmp[8];
            int s = sh[r - 1];
            for (int c = 0; c < nb; ++c)
                tmp[c] = state[r][(c - s + nb) % nb];
            for (int c = 0; c < nb; ++c)
                state[r][c] = tmp[c];
        }

        // InvSubBytes
        for (int c = 0; c < nb; ++c)
            for (int r = 0; r < 4; ++r)
                state[r][c] = kSBoxInv[state[r][c]];

        InvAddRoundKey(state, round);

        // InvMixColumns
        for (int c = 0; c < nb; ++c)
        {
            unsigned char a0 = state[0][c], a1 = state[1][c];
            unsigned char a2 = state[2][c], a3 = state[3][c];
            state[0][c] = (unsigned char)(GfMul(a0, 14) ^ GfMul(a1, 11) ^ GfMul(a2, 13) ^ GfMul(a3, 9));
            state[1][c] = (unsigned char)(GfMul(a0, 9) ^ GfMul(a1, 14) ^ GfMul(a2, 11) ^ GfMul(a3, 13));
            state[2][c] = (unsigned char)(GfMul(a0, 13) ^ GfMul(a1, 9) ^ GfMul(a2, 14) ^ GfMul(a3, 11));
            state[3][c] = (unsigned char)(GfMul(a0, 11) ^ GfMul(a1, 13) ^ GfMul(a2, 9) ^ GfMul(a3, 14));
        }
    }

    // 最后一轮
    for (int r = 1; r < 4; ++r)
    {
        unsigned char tmp[8];
        int s = sh[r - 1];
        for (int c = 0; c < nb; ++c)
            tmp[c] = state[r][(c - s + nb) % nb];
        for (int c = 0; c < nb; ++c)
            state[r][c] = tmp[c];
    }
    for (int c = 0; c < nb; ++c)
        for (int r = 0; r < 4; ++r)
            state[r][c] = kSBoxInv[state[r][c]];
    InvAddRoundKey(state, 0);

    for (int c = 0; c < nb; ++c)
        for (int r = 0; r < 4; ++r)
            out[r + c * 4] = state[r][c];
}

int CRijndael::EncryptBlock(const char* in, char* out)
{
    if (!m_bInit)
        return 0x70000005;
    if (m_blockSize == 0x10)
        return DefEncryptBlock(in, out);
    EncryptCore((const unsigned char*)in, (unsigned char*)out);
    return 0x6fffffff;
}

int CRijndael::DecryptBlock(const char* in, char* out)
{
    if (!m_bInit)
        return 0x70000005;
    if (m_blockSize == 0x10)
        return DefDecryptBlock(in, out);
    DecryptCore((const unsigned char*)in, (unsigned char*)out);
    return 0x6fffffff;
}

int CRijndael::DefEncryptBlock(const char* in, char* out)
{
    if (!m_bInit)
        return 0x70000005;
    EncryptCore((const unsigned char*)in, (unsigned char*)out);
    return 0x6fffffff;
}

int CRijndael::DefDecryptBlock(const char* in, char* out)
{
    if (!m_bInit)
        return 0x70000005;
    DecryptCore((const unsigned char*)in, (unsigned char*)out);
    return 0x6fffffff;
}

int CRijndael::Encrypt(const char* szIn, char* szOut, size_t iLength)
{
    if (!m_bInit)
        return 0x70000005;
    if (iLength == 0 || iLength % (size_t)m_blockSize != 0)
        return 0x7000000a;

    int blocks = (int)(iLength / m_blockSize);
    if (m_mode == 1)   // CBC
    {
        for (int i = 0; i < blocks; ++i)
        {
            IMethod::Xor((char*)m_chain, szIn);
            EncryptBlock((const char*)m_chain, szOut);
            std::memcpy(m_chain, szOut, m_blockSize);
            szIn += m_blockSize;
            szOut += m_blockSize;
        }
    }
    else if (m_mode == 2)   // CFB
    {
        for (int i = 0; i < blocks; ++i)
        {
            EncryptBlock((const char*)m_chain, szOut);
            IMethod::Xor(szOut, szIn);
            std::memcpy(m_chain, szOut, m_blockSize);
            szIn += m_blockSize;
            szOut += m_blockSize;
        }
    }
    else   // ECB
    {
        for (int i = 0; i < blocks; ++i)
        {
            EncryptBlock(szIn, szOut);
            szIn += m_blockSize;
            szOut += m_blockSize;
        }
    }
    return 0x6fffffff;
}

int CRijndael::Decrypt(const char* szIn, char* szOut, size_t iLength)
{
    if (!m_bInit)
        return 0x70000005;
    if (iLength == 0 || iLength % (size_t)m_blockSize != 0)
        return 0x7000000a;

    int blocks = (int)(iLength / m_blockSize);
    if (m_mode == 1)   // CBC
    {
        for (int i = 0; i < blocks; ++i)
        {
            DecryptBlock(szIn, szOut);
            IMethod::Xor(szOut, (const char*)m_chain);
            std::memcpy(m_chain, szIn, m_blockSize);
            szIn += m_blockSize;
            szOut += m_blockSize;
        }
    }
    else if (m_mode == 2)   // CFB
    {
        for (int i = 0; i < blocks; ++i)
        {
            EncryptBlock((const char*)m_chain, szOut);
            IMethod::Xor(szOut, szIn);
            std::memcpy(m_chain, szIn, m_blockSize);
            szIn += m_blockSize;
            szOut += m_blockSize;
        }
    }
    else   // ECB
    {
        for (int i = 0; i < blocks; ++i)
        {
            DecryptBlock(szIn, szOut);
            szIn += m_blockSize;
            szOut += m_blockSize;
        }
    }
    return 0x6fffffff;
}

void CRijndael::ResetChain()
{
    if (m_bInit)
        std::memcpy(m_chain, m_block, m_blockSize);
}

void CRijndael::Signature(char* szSignature)
{
    // ORIG：SHA-256("RIJDAEL" + key + sprintf("%d%d%d%d", bs, kl, mode, pad))
    char buf[0x40];
    std::memset(buf, 0, sizeof(buf));
    std::strcat(buf, "RIJDAEL");
    int len = (int)std::strlen(buf);
    std::memcpy(buf + len, m_key, m_keyLength);
    std::sprintf(buf + len + m_keyLength, "%d%d%d%d",
                 m_blockSize, m_keyLength, m_mode, m_padding);

    nsl::CSHA sha;
    sha.AddData(buf, (int)std::strlen(buf));
    sha.FinalDigest(szSignature);
}
