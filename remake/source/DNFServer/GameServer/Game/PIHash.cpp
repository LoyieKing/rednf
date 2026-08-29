// U3 PI 反作弊 hash 链 TU（U3ShopDb 批次，2026-08-23）。
// 逐符号对照 docs/class_func_reports（_lt_global_gt_/PISenderManager/CUser）与
// objdump 反汇编：
//   DoSuperFastHash(char*, uint, uint&)                     0x0808ca7a
//   MakeHash(uchar*, uint, uchar)                           0x0808c311 (W)
//   PISenderManager::MakeHashForSending(uchar*, uint, bool) 0x0808be6e
//   _NS_PI_2ND_GetPacketIndex(char*)                        0x082772a9 (W)
//   _NS_PI_2ND_ReconstructCheckSumRandomkey(uint&, uint)    0x0808ca60
//   _NS_PI_2ND_SendHash_SC(char*, ushort, CUser*)           0x082751ee
//   SetUserManagerToolIP(CUser*, SIG_UPDATE_LOGOUT*)        0x08655327

#include "CUser.h"
#include "SigTypes.h"
#include <cstring>
#include <cstdlib>

// ---- DoSuperFastHash ORIG @ 0x0808ca7a ----
int DoSuperFastHash(char* data, unsigned int len, unsigned int& hash)
{
    hash = 0;
    unsigned int rem = len;
    if (len == 0 || data == 0)
        return 0;

    unsigned int h = len;
    unsigned int tmp = len & 3;
    for (unsigned int i = len >> 2; i != 0; --i)
    {
        h = (h + *reinterpret_cast<unsigned short*>(data)) * 0x10000 ^
            (static_cast<unsigned int>(*reinterpret_cast<unsigned short*>(data + 2)) << 11) ^
            (h + *reinterpret_cast<unsigned short*>(data));
        data += 4;
        h = h + (h >> 11);
    }
    if (tmp == 2)
    {
        h = h + *reinterpret_cast<unsigned short*>(data) ^
            (h + *reinterpret_cast<unsigned short*>(data)) * 0x800;
        h = h + (h >> 17);
    }
    else if (tmp == 3)
    {
        h = h + *reinterpret_cast<unsigned short*>(data) ^
            (h + *reinterpret_cast<unsigned short*>(data)) * 0x10000 ^
            (static_cast<unsigned int>(static_cast<signed char>(data[2])) << 18);
        h = h + (h >> 11);
    }
    else if (tmp == 1)
    {
        // ORIG movsx（signed char）——与 rem==3 的 data[2] 一致。
        h = h + static_cast<int>(static_cast<signed char>(*data)) ^
            (h + static_cast<int>(static_cast<signed char>(*data))) * 0x400;
        h = h + (h >> 1);
    }
    h = h ^ (h << 3);
    h = h + (h >> 5);
    h = h ^ (h * 0x10);
    h = h + (h >> 17);
    h = h ^ (h * 0x2000000);
    hash = h + (h >> 6);
    return 1;
}

// ORIG W 0x0808c174：unsigned char* 重载（与 char* 版同实现）。
int DoSuperFastHash(unsigned char* data, unsigned int len, unsigned int& hash)
{
    return DoSuperFastHash(reinterpret_cast<char*>(data), len, hash);
}

// ---- MakeHash ORIG @ 0x0808c311 ----
// SuperFastHash 后取 4 字节摘要逐字节 xor 再 xor 种子。
unsigned char MakeHash(unsigned char* data, unsigned int len, unsigned char seed)
{
    unsigned int hash = 0;
    DoSuperFastHash(reinterpret_cast<char*>(data), len, hash);
    unsigned char* digest = reinterpret_cast<unsigned char*>(&hash);
    return static_cast<unsigned char>(digest[0] ^ digest[1] ^ digest[2] ^
                                      digest[3] ^ seed);
}

// ---- _NS_PI_2ND_GetPacketIndex ORIG @ 0x082772a9 ----
short _NS_PI_2ND_GetPacketIndex(char* packet)
{
    return *reinterpret_cast<short*>(packet + 1);
}

// ---- PISenderManager::MakeHashForSending ORIG @ 0x0808be6e ----
// this+0x10 为当前 key 字节；bNewKey 时先写固定种子 0x27，再以 rand() 刷新。
unsigned int PISenderManager::MakeHashForSending(unsigned char* data,
                                                 unsigned int len,
                                                 bool bNewKey)
{
    if (bNewKey)
        m_field10 = 0x27;
    unsigned char key = m_field10;
    unsigned int rnd = static_cast<unsigned int>(rand());
    m_field10 = static_cast<unsigned char>(rnd);

    unsigned char h;
    if (len == 0)
        h = static_cast<unsigned char>(rand());
    else
        h = MakeHash(data, len, key);

    unsigned int r = static_cast<unsigned int>(rand());
    return (r & 0xff) |
           ((((key >> 6) & 0xff) | ((h & 3) << 6) | ((h & 0xc) << 12) |
             ((h & 0x30) >> 2) | ((h & 0xc0) << 4) | ((rnd & 3) << 12) |
             ((rnd & 0xc) << 2) | ((rnd & 0x30) << 4))
            << 8);
}

// ---- _NS_PI_2ND_ReconstructCheckSumRandomkey ORIG @ 0x0808ca60 ----
void _NS_PI_2ND_ReconstructCheckSumRandomkey(unsigned int& checksum,
                                             unsigned int randomKey)
{
    checksum = (checksum & 0xff) | (randomKey << 8);
}

// ---- _NS_PI_2ND_SendHash_SC ORIG @ 0x082751ee ----
void _NS_PI_2ND_SendHash_SC(char* data, unsigned short len, CUser* user)
{
    char first = data[0];
    short index = _NS_PI_2ND_GetPacketIndex(data);
    bool bNonClient = (first == '\0' && index == 1);

    PISenderManager* sender =
        reinterpret_cast<PISenderManager*>(user->GetPISenderManager());
    unsigned int hash =
        sender->MakeHashForSending(
            reinterpret_cast<unsigned char*>(data + 0xf),
            static_cast<unsigned int>(len - 0xf), bNonClient);
    _NS_PI_2ND_ReconstructCheckSumRandomkey(
        *reinterpret_cast<unsigned int*>(data + 0xb), hash);
}

// ---- SetUserManagerToolIP ORIG @ 0x08655327 ----
// SIG_UPDATE_LOGOUT 布局（本函数实证）：+0x22 premium 管理工具标志(byte)、
// +0x23 ip 相关 dword、+0x27 ip[0x10]、+0x37 前段、+0x43 后段（'.' 拆分）。
void SetUserManagerToolIP(CUser* user, SIG_UPDATE_LOGOUT* sig)
{
    sig->m_pcbangFlag = user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) ? 1 : 0;

    char* ip = sig->m_ipBuf;
    user->m_network.GetPeerIP2(ip, 0x10);

    // peer IP 为空时回退用户 m_mcap.m_pcRoomIp（+0x8e418）记录的 IP。
    if (*ip == '\0')
    {
        const char* savedIp = user->m_mcap.m_pcRoomIp;
        if (*savedIp != '\0')
        {
            std::memset(ip, 0, 0x10);
            std::strncpy(ip, savedIp, 0x10);
        }
    }

    if (std::strlen(ip) > 0x10)
        return;

    // 以最后一个 '.' 把 a.b.c.d 拆为前缀(+0x37)/后缀(+0x43)。
    char* dot = std::strrchr(ip, '.');
    if (dot == 0)
        return;

    int prefixLen = static_cast<int>(dot - ip);
    if (static_cast<unsigned int>(prefixLen) > 0xc)
        return;

    int suffixLen = static_cast<int>(std::strlen(dot + 1));
    if (static_cast<unsigned int>(suffixLen) > 4)
        return;
    std::strncpy(sig->m_ipPrefix, ip, prefixLen);
    std::strncpy(sig->m_ipSuffix, dot + 1, suffixLen);
    sig->m_ipDword = static_cast<unsigned int>(user->m_mcap.m_field24);  // +0x8e414
}


// ============================================================================
// OnlyOrigBatch11 落地：_NS_PI_2ND_MakeHash / GetRandomHashKey /
// ReconstructCheckSum（报告 _lt_global_gt_ 三份，objdump 逐指令转写）
// ============================================================================

// ORIG T 0x0808c9a0：DoSuperFastHash(data,len,&h) → 4 字节异或 ^ seed ^ 0x53
// → (v & 0xf) ^ (v >> 4)（返回低半字节折叠）。
unsigned char _NS_PI_2ND_MakeHash(char* data, unsigned int len,
                                  unsigned char seed)
{
    unsigned int h = 0;
    DoSuperFastHash(data, len, h);
    unsigned char* p = reinterpret_cast<unsigned char*>(&h);
    unsigned char v = static_cast<unsigned char>(
        p[0] ^ p[1] ^ p[2] ^ p[3] ^ seed ^ 0x53);
    return static_cast<unsigned char>((v & 0xf) ^ (v >> 4));
}

// ORIG T 0x0808ca1c：return rand() & 0xf。
unsigned char _NS_PI_2ND_GetRandomHashKey()
{
    return static_cast<unsigned char>(rand() & 0xf);
}

// ORIG T 0x0808ca2c：checksum = (cksum & 0xff) | ((k1<<8) & 0xffff00) |
// ((k2<<24) & 0xf000000) | (k3 << 28)。
void _NS_PI_2ND_ReconstructCheckSum(unsigned int& checksum, unsigned int k1,
                                    unsigned int k2, unsigned int k3)
{
    unsigned int v = checksum & 0xff;
    v |= (k1 << 8) & 0xffff00;
    v |= (k2 << 24) & 0xf000000;
    v |= k3 << 28;
    checksum = v;
}
