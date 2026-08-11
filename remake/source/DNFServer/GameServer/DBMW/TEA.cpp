// df_dbmw_r - TEA (ORIG TEA.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

// ---- CSHA：SHA-256 ----
const char CTEA::sm_chain0[8] = {0, 0, 0, 0, 0, 0, 0, 0};

CTEA::CTEA()
{
    m_blockSize = 8;
    m_keyLength = 0x10;
    memset(m_key, 0, 0x10);
    memset(m_chain, 0, 8);
    memset(m_chain2, 0, 8);
    memset(m_iv, 0, 0x10);
}
CTEA::~CTEA() {}
int CTEA::Initialize(char const* key, int keyLen, char const* iv, int ivLen, int mode)
{
    if (!key)
        return 0x70000001;
    if (keyLen <= 0)
        return 0x70000002;
    if (mode < 0 || mode > 2)
        return 0x70000003;
    if (ivLen < 0 || ivLen > 2)
        return 0x70000004;
    m_mode = mode;
    m_padding = ivLen;
    unsigned char tmpKey[0x10] = {0};
    for (int i = 0; i < m_keyLength; i++)
        tmpKey[i] = key[i % keyLen];
    char ivSame = 0;
    char keySame = 0;
    if (m_initialized == 1)
    {
        if (memcmp(m_chain, iv, m_blockSize) == 0)
            ivSame = 1;
        if (memcmp(m_key, tmpKey, m_keyLength) == 0)
            keySame = 1;
        if (ivSame)
            memcpy(m_chain, m_chain2, m_blockSize);
        else
        {
            memcpy(m_chain, iv, m_blockSize);
            memcpy(m_chain2, iv, m_blockSize);
        }
        if (keySame)
            return 0x6fffffff;
    }
    memcpy(m_key, tmpKey, m_keyLength);
    BytesToWord(tmpKey, *(unsigned int*)&m_iv[0]);
    BytesToWord(tmpKey + 4, *(unsigned int*)&m_iv[4]);
    BytesToWord(tmpKey + 8, *(unsigned int*)&m_iv[8]);
    BytesToWord(tmpKey + 0xc, *(unsigned int*)&m_iv[0xc]);
    m_initialized = 1;
    return 0x6fffffff;
}
int CTEA::ResetChain()
{
    if (m_initialized == 1)
        return 0x70000005;
    memcpy(m_chain, m_chain2, m_blockSize);
    return 0x6fffffff;
}
int CTEA::EncryptBlock(unsigned char const* src, unsigned char* dst)
{
    unsigned int v0, v1;
    BytesToWord(src, v0);
    BytesToWord(src + 4, v1);
    unsigned int sum = 0;
    const unsigned int delta = 0x9e3779b9;
    unsigned int* key = (unsigned int*)m_iv;
    for (int i = 0; i < 0x20; i++)
    {
        v0 += ((((v1 << 4) ^ (v1 >> 5)) + v1) ^ (key[sum & 3] + sum));
        sum += delta;
        v1 += ((((v0 << 4) ^ (v0 >> 5)) + v0) ^ (key[(sum >> 11) & 3] + sum));
    }
    WordToBytes(v0, dst);
    WordToBytes(v1, dst + 4);
    return 0x6fffffff;
}
int CTEA::DecryptBlock(unsigned char const* src, unsigned char* dst)
{
    unsigned int v0, v1;
    BytesToWord(src, v0);
    BytesToWord(src + 4, v1);
    unsigned int sum = 0xc6ef3720;
    const unsigned int delta = 0x9e3779b9;
    unsigned int* key = (unsigned int*)m_iv;
    for (int i = 0; i < 0x20; i++)
    {
        v1 -= ((((v0 << 4) ^ (v0 >> 5)) + v0) ^ (key[(sum >> 11) & 3] + sum));
        sum -= delta;
        v0 -= ((((v1 << 4) ^ (v1 >> 5)) + v1) ^ (key[sum & 3] + sum));
    }
    WordToBytes(v0, dst);
    WordToBytes(v1, dst + 4);
    return 0x6fffffff;
}
int CTEA::Encrypt(char const* src, char* dst, unsigned int len)
{
    if (m_initialized != 1)
        return 0x70000005;
    if (len == 0 || len % (unsigned int)m_blockSize != 0)
        return 0x7000000a;
    if (m_mode == 1)
    {
        for (int i = 0; i < (int)(len / (unsigned int)m_blockSize); i++)
        {
            Xor((char*)src, (char const*)m_chain2);
            EncryptBlock((unsigned char const*)src, (unsigned char*)dst);
            memcpy(m_chain2, dst, m_blockSize);
            src += m_blockSize;
            dst += m_blockSize;
        }
    }
    else if (m_mode == 2)
    {
        for (int i = 0; i < (int)(len / (unsigned int)m_blockSize); i++)
        {
            EncryptBlock(m_chain2, (unsigned char*)dst);
            Xor((char*)src, (char const*)dst);
            memcpy(m_chain2, dst, m_blockSize);
            src += m_blockSize;
            dst += m_blockSize;
        }
    }
    else
    {
        for (int i = 0; i < (int)(len / (unsigned int)m_blockSize); i++)
        {
            EncryptBlock((unsigned char const*)src, (unsigned char*)dst);
            src += m_blockSize;
            dst += m_blockSize;
        }
    }
    return 0x6fffffff;
}
int CTEA::Decrypt(char const* src, char* dst, unsigned int len)
{
    if (m_initialized != 1)
        return 0x70000005;
    if (len == 0 || len % (unsigned int)m_blockSize != 0)
        return 0x7000000a;
    if (m_mode == 1)
    {
        for (int i = 0; i < (int)(len / (unsigned int)m_blockSize); i++)
        {
            DecryptBlock((unsigned char const*)src, (unsigned char*)dst);
            Xor((char*)m_chain2, (char const*)dst);
            memcpy(m_chain2, src, m_blockSize);
            src += m_blockSize;
            dst += m_blockSize;
        }
    }
    else if (m_mode == 2)
    {
        for (int i = 0; i < (int)(len / (unsigned int)m_blockSize); i++)
        {
            EncryptBlock(m_chain2, (unsigned char*)dst);
            Xor((char*)src, (char const*)dst);
            memcpy(m_chain2, src, m_blockSize);
            src += m_blockSize;
            dst += m_blockSize;
        }
    }
    else
    {
        for (int i = 0; i < (int)(len / (unsigned int)m_blockSize); i++)
        {
            DecryptBlock((unsigned char const*)src, (unsigned char*)dst);
            src += m_blockSize;
            dst += m_blockSize;
        }
    }
    return 0x6fffffff;
}
int CTEA::Signature(char* digest)
{
    char buffer[0x18] = {0};
    memcpy(m_key, "TEA", strlen("TEA"));
    sprintf(buffer, "%d%d%d", m_keyLength, m_mode, m_padding);
    CSHA sha;
    sha.AddData(buffer, strlen(buffer));
    sha.FinalDigest(digest);
    return 0x6fffffff;
}
