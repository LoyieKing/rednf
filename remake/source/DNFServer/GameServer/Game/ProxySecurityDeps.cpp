// ProxySecurityDeps.cpp —— checkDenyIP / cutIPdclass / getSha256UsingSalt 真实定义。
// 全部按 ORIG objdump 逐指令转写（报告缺失，地址见 ProxySecurityDeps.h）。

#include "ProxySecurityDeps.h"
#include "ChecksumAlgo.h"        // CreateCryptoChecksum/DestroyCryptoChecksum/IChecksum
#include <string.h>              // strncmp/strcpy/strcat/memset

// ORIG T _Z11cutIPdclassPKc @0x081f440a sz 0xb1（sret）：
//   std::string s(ip); s.erase(s.rfind('.', npos), npos); return s;
// [b28d-N7] fresh 复验 58=58：ORIG `mov %ebx,%esi`（sret 提前入 callee-saved）
// 跨 allocator ctor 保活、4.4 恒在 call 后 `mov %ebx,%eax` 重载 ⇒ 寄存器角色
// 互换（P52/N7 族；探针形不可择）。语义逐条全等。
std::string cutIPdclass(const char* ip)
{
    std::string s(ip);                            // ORIG _ZNSsC1EPKcRKSaIcE
    s.erase(s.rfind('.'), std::string::npos);     // rfind '.'@0x2e pos=npos; erase(pos,-1)
    return s;
}

// ORIG T _Z11checkDenyIPPKcS0_ @0x081f44bb sz 0x1d0：
//   null/空串 → false；strncmp(a,b,0x10)==0 → false；
//   cutIPdclass 相等 → false；a=="0" → false；返回 (b == "0")。
// [b28d-N86] fresh 复验 132/117：差值全在 std::string 成员函数外联调用序列
// （4.1 保留 find/erase/operator= 中间结果寄存器链，4.4 多 15 条 sret/临时槽）
// 与多分支早返块布局（P47/N104）⇒ 跨版本 libstdc++ 内联差，语义逐分支全等。
bool checkDenyIP(const char* innerIp, const char* peerIp)
{
    if (innerIp == 0 || peerIp == 0)
        return false;
    if (innerIp[0] == '\0' || peerIp[0] == '\0')
        return false;
    if (strncmp(innerIp, peerIp, 0x10) == 0)
        return false;
    std::string a = cutIPdclass(innerIp);
    std::string b = cutIPdclass(peerIp);
    if (a == b)                                   // ORIG 0x081f4556 eq(string,string)
        return false;
    // ORIG 0x081f457f..0x081f45e6：a/b 各再做 find('.',0)+erase(pos,npos)
    // （取到第一个 '.'，即仅保留首段）。
    a.erase(a.find('.'), std::string::npos);
    b.erase(b.find('.'), std::string::npos);
    if (a == "0")                                 // ORIG 0x081f45f9 eq(string,"0")
        return false;
    return b == "0";
}

// ORIG T _Z18getSha256UsingSaltPKhiPc @0x0808cc6a sz 0x18c：
//   null→false；首字节 '\0'→true（不处理）；
//   buf=new uchar[len+4]; memset(0); strcpy(src); strcat("nhs");
//   digest[0x20]={0}; CreateCryptoChecksum(CRYPTO_CHECKSUM_TYPE(1))；
//   虚槽 +0xc（仓库 ChecksumAlgo.h 槽序为 MakeChecksumFromMemory，直呼同方法——
//   槽位偏移差异属共享头排列豁免，行为一致）：
//     MakeChecksumFromMemory(buf, len+4, digest, 0x20)；==0x6fffffff → ok=true；
//   for i in 0..31: sprintf(out+i*2, "%02X", digest[i])；
//   DestroyCryptoChecksum(&ck); delete[] buf; return ok。
// [b28d-N82] fresh 复验 113/99：ORIG 帧 0x60（含 push %esi）vs 4.4 0x64；
// salt[4] 逐字节初始化 ORIG 从 -0x21 递减（n/h/s/0 高到低）、4.4 反向；
// 全帧槽号平移 + x87/EH 段 4.4 折叠 ⇒ 4.1 帧策略差（P112/N82），语义等价。
bool getSha256UsingSalt(const unsigned char* src, int len, char* out)
{
    if (src == 0)
        return false;
    if (src[0] == '\0')
        return true;

    bool ok = false;                              // ORIG -0x1d
    unsigned char salt[4];                        // ORIG -0x21..-0x1e 逐字节置值
    salt[0] = 'n';
    salt[1] = 'h';
    salt[2] = 's';
    salt[3] = '\0';

    unsigned char* buf = new unsigned char[len + 4];      // ORIG _Znaj
    memset(buf, 0, len + 4);
    strcpy(reinterpret_cast<char*>(buf),
           reinterpret_cast<const char*>(src));
    strcat(reinterpret_cast<char*>(buf),
           reinterpret_cast<const char*>(salt));

    unsigned char digest[0x20];                   // ORIG -0x48 零初始化展开
    memset(digest, 0, sizeof(digest));

    IChecksum* checksum = CreateCryptoChecksum((CRYPTO_CHECKSUM_TYPE)1);
    if (checksum->MakeChecksumFromMemory(buf, len + 4, digest, 0x20) ==
        static_cast<int>(0x6fffffff))
        ok = true;

    for (int i = 0; i <= 0x1f; ++i)
        sprintf(out + i * 2, "%02X", digest[i]);

    DestroyCryptoChecksum(checksum);
    delete[] buf;                                 // ORIG _ZdaPv
    return ok;
}
