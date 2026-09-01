// OnlyOrigBatch14.cpp —— Batch14 自由函数/杂散函数落地（ORIG objdump 逐符号转写）。
// 单 TU 验证：source/toolchain/check_tu_game_orig.sh DNFServer/GameServer/Game/OnlyOrigBatch14.cpp
// 符号来源：/tmp/df_batches/OnlyOrigBatch14.txt（65 个仅 ORIG 符号）。
// 本 TU 承载：crypto 族（ROR/ROL/pi*/decrypt/encrypt/SHA384/SHA512/compareSortedKeyArray）、
// Scan*/export*/import* 脚本族、PetScript、杂散小函数。
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <string>
#include <vector>
#include <map>
#include <utility>

#include "DNFLexWrapper.h"        // g_dnfLex / ScanType / ScanInt / ScanStr / ScanFloat
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "LogManager.h"
#include "CEnvironment.h"         // SpinLock
#include "NpcScript.h"            // getCharacterJob / STScriptFileList
#include "CMailBoxHelper.h"       // SIG_LETTER_INFO（PrintLetter 形参）
#include "CMap.h"                 // STAICharacterMapArrangeData
#include "CDataManager.h"         // G_CDataManager()（getNeedMaterial(int)）
#include "Quest.h"                // RewardItemInfo
#include "STCharacterScript.h"    // STRDCharacterStatus / CharacterEtcStats
#include "STTileScript.h"
#include "STBreakAwayMemberData.h"
#include "NeedMaterialDataManager.h"
#include "RestrictCategory.h"
#include "CItem.h"
#include "CMission.h"             // CMission（MissionLessThan::operator()）
#include "MissionLessThan.h"
#include "STStackableScript.h"    // STStackablePackageData
#include "SyncScriptDeps.h"       // toTString
#include "CreatureScript.h"       // getActiveStatusType / getDrawLayer
#include "PetScript.h"
#include "DNFFunctionLib.h"       // timeGetTime
#include "EncryptTool.h"          // EncryptTool（G_EncryptTool）
#include "CUser.h"                // CUser（GiveItemReturnUser / DecreaseTradeLimitCountByUser）
#include "CUserCharacInfo.h"      // getCurCharacNo / getCurCharacLastPlayDungeonIndex /
                                  //   getCurCharacR / getCurCharacInvenW
#include "CInventory.h"           // CInventory::GetInvenRef
#include "CItem.h"                // CItem::make_item（已在上面 include，重复无害）
#include "Inven_Item.h"           // Inven_Item / DecreaseTradeLimitCount
#include "CDungeon.h"             // CDungeon::GetDungeonName
#include "GameEnums.h"            // ENUM_PACKETCLASS / ENUM_NOTIPACKET / ENUM_CMDPACKET
#include "W8LT_PassiveObjectScript.h"      // STPassiveObjectScript 及子对象
#include "STPassiveObjectAdditionalParticleInfo.h"
#include "CDeathTowerPlayDataStatistic.h"
#include "CacheCharacterType.h"   // ENUM_INTERNALPACKET（全局枚举，权威定义）

// ---- 跨 TU 依赖声明（定义见各宿主）----
int ScanStrOrInt(bool& isStr, std::string& s, bool& isInt, bool consume);  // DNFLexWrapperHelpers.cpp
std::string getObjectCategoryString(ENUM_CATEGORY cat);  // ScriptDepsA.cpp（ORIG T 0x08a3ca81）
extern char g_petScriptBaseDirectory[0x100];   // OnlyOrigBatch12.cpp（ORIG B 0x094ff3c0）
extern STScriptFileList g_petScriptFileList;   // OnlyOrigBatch12.cpp（ORIG B 0x094ff4c0）
extern std::map<std::string, int> g_mapKeyNameIndexTable;  // STEquipmentScript.cpp（ORIG B 0x0949b348）
extern char g_szPassiveObjectScriptBaseDirectory[0x100];   // InitScriptProducers_4.cpp（ORIG B 0x94ff2a0）
extern std::map<int, std::string> g_stPassiveObjectScriptFileList;  // InitScriptProducers_4.cpp（ORIG B 0x94ff3a0）
extern int getObjectCategory(std::string name);            // ScriptDepsA.cpp（ORIG T 0x08a3ca1b）
extern bool isValidExtension(const std::string& str, const std::string& ext);  // OnlyOrigBatch12.cpp（ORIG T 0x08a3c67d）

// ---- 本批新增小结构（仅本文件使用）----
struct STUltimateSkillCutScene  // ORIG 无导出成员符号，仅 importUltimateSkillCutScene 空体引用
{
    int m_field0;
};

// ORIG importActiveStatus 0x088c1e68 逐偏移：+0 int、+4 float、+8 int、+0xc byte、
// +0x10 int、+0x14 vector<int>。
struct STActiveStatus
{
    int m_type;             // +0x00
    float m_value4;         // +0x04
    int m_value8;           // +0x08
    bool m_flagC;           // +0x0c
    int m_value10;          // +0x10
    std::vector<int> m_list14;  // +0x14
};

// ============================================================================
// 1) crypto 族（ORIG 全部为 _ZL 局部符号；静态函数自动匹配 _ZL 名）
// ============================================================================

// ORIG _ZL3ROLji @0x80c3dc0 / _ZL3RORji @0x809c42d：rol/ror %cl。
static unsigned int ROL(unsigned int value, int count)
{
    return (value << count) | (value >> (32 - count));
}

static unsigned int ROR(unsigned int value, int count)
{
    return (value >> count) | (value << (32 - count));
}

// ORIG _ZL3pi1Pm @0x80b46c9：buf[1] ^= buf[0]。
static void pi1(unsigned long* buf)
{
    buf[1] ^= buf[0];
}

// ORIG _ZL3pi2PmS_ @0x80b46e5：
// t = buf[1]+key[0]; t = ROL(t,1)+t-1; t = ROL(t,4)^t; buf[0] ^= t。
static void pi2(unsigned long* buf, unsigned long* key)
{
    unsigned int t = (unsigned int)buf[1] + (unsigned int)key[0];
    t = ROL(t, 1) + t - 1;
    t = ROL(t, 4) ^ t;
    buf[0] = (unsigned long)((unsigned int)buf[0] ^ t);
}

// ORIG _ZL3pi3PmS_ @0x80b4741：
// t = buf[0]+key[1]; t = ROL(t,2)+t+1; t = ROL(t,8)^t; t += key[2];
// t = ROL(t,1); t = ROL(t,16) ^ (buf[0] | t); buf[1] ^= t。
static void pi3(unsigned long* buf, unsigned long* key)
{
    unsigned int t = (unsigned int)buf[0] + (unsigned int)key[1];
    t = ROL(t, 2) + t + 1;
    t = ROL(t, 8) ^ t;
    t = t + (unsigned int)key[2];
    t = ROL(t, 1);
    t = ROL(t, 16) ^ ((unsigned int)buf[0] | t);
    buf[1] = (unsigned long)((unsigned int)buf[1] ^ t);
}

// ORIG _ZL3pi4PmS_ @0x80b47e5：
// t = buf[1]+key[3]; t = ROL(t,2)+t+1; buf[0] ^= t。
static void pi4(unsigned long* buf, unsigned long* key)
{
    unsigned int t = (unsigned int)buf[1] + (unsigned int)key[3];
    t = ROL(t, 2) + t + 1;
    buf[0] = (unsigned long)((unsigned int)buf[0] ^ t);
}

// ORIG _ZL7encryptPmiS_ @0x80b4986：i=j=0；pi1/pi2/pi3/pi4 依次执行，
// 每步后 j==n 即返回；一轮后 i^=4。
static void encrypt(unsigned long* buf, int n, unsigned long* key)
{
    int i = 0;
    int j = 0;
    for (;;)
    {
        pi1(buf);
        ++j;
        if (j == n)
            return;
        pi2(buf, key + i);
        ++j;
        if (j == n)
            return;
        pi3(buf, key + i);
        ++j;
        if (j == n)
            return;
        pi4(buf, key + i);
        ++j;
        if (j == n)
            return;
        i ^= 4;
    }
}

// ORIG _ZL7decryptPmiS_ @0x80b4a49：i=(n&1)?0:4；while(j>3){ pi4;pi3;pi2;pi1; i^=4 }。
static void decrypt(unsigned long* buf, int n, unsigned long* key)
{
    int i = (n & 1) ? 0 : 4;
    int j = n;
    while (j > 3)
    {
        pi4(buf, key + i);
        --j;
        pi3(buf, key + i);
        --j;
        pi2(buf, key + i);
        --j;
        pi1(buf);
        --j;
        i ^= 4;
    }
}

// ORIG _ZL21compareSortedKeyArrayPmS_i @0x8504cc5：逐 4 字节比较。
static int compareSortedKeyArray(unsigned long* a, unsigned long* b, int n)
{
    for (int i = 0; i < n; ++i)
    {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

// ---- SHA-512 压缩（ORIG _ZL16SHA384_TransformPmS_ @0x80be32c /
//      _ZL16SHA512_TransformPmS_ @0x80c077c；标准 FIPS 180-4，64 位字大端装载）----

static const unsigned long long g_sha512K[80] = {
    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL,
    0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
    0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL,
    0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
    0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
    0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
    0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL, 0x2de92c6f592b0275ULL,
    0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
    0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL,
    0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
    0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL,
    0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
    0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL,
    0x92722c851482353bULL, 0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
    0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
    0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
    0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL,
    0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
    0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL,
    0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL,
    0xc67178f2e372532bULL, 0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
    0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL,
    0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
    0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
    0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
    0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};

#define SHA512_ROTR64(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

static unsigned long long sha512_bswap64(unsigned long long v)
{
    return ((v & 0x00000000000000ffULL) << 56) |
           ((v & 0x000000000000ff00ULL) << 40) |
           ((v & 0x0000000000ff0000ULL) << 24) |
           ((v & 0x00000000ff000000ULL) << 8) |
           ((v & 0x000000ff00000000ULL) >> 8) |
           ((v & 0x0000ff0000000000ULL) >> 24) |
           ((v & 0x00ff000000000000ULL) >> 40) |
           ((v & 0xff00000000000000ULL) >> 56);
}

// ORIG _ZL16SHA512_TransformPmS_ @0x80c077c：压缩函数（与 SHA384 同体）。
static void SHA512_Transform(unsigned long* state, unsigned long* data)
{
    const unsigned long long* m =
        reinterpret_cast<const unsigned long long*>(data);
    unsigned long long* s = reinterpret_cast<unsigned long long*>(state);
    unsigned long long W[80];
    unsigned long long a, b, c, d, e, f, g, h;

    for (int i = 0; i < 16; ++i)
    {
        W[i] = sha512_bswap64(m[i]);
    }
    for (int i = 16; i < 80; ++i)
    {
        unsigned long long s0 = SHA512_ROTR64(W[i - 15], 1) ^
                                SHA512_ROTR64(W[i - 15], 8) ^
                                (W[i - 15] >> 7);
        unsigned long long s1 = SHA512_ROTR64(W[i - 2], 19) ^
                                SHA512_ROTR64(W[i - 2], 61) ^
                                (W[i - 2] >> 6);
        W[i] = W[i - 16] + s0 + W[i - 7] + s1;
    }

    a = s[0];
    b = s[1];
    c = s[2];
    d = s[3];
    e = s[4];
    f = s[5];
    g = s[6];
    h = s[7];

    for (int i = 0; i < 80; ++i)
    {
        unsigned long long S1 = SHA512_ROTR64(e, 14) ^
                                SHA512_ROTR64(e, 18) ^
                                SHA512_ROTR64(e, 41);
        unsigned long long ch = (e & f) ^ (~e & g);
        unsigned long long t1 = h + S1 + ch + g_sha512K[i] + W[i];
        unsigned long long S0 = SHA512_ROTR64(a, 28) ^
                                SHA512_ROTR64(a, 34) ^
                                SHA512_ROTR64(a, 39);
        unsigned long long maj = (a & b) ^ (a & c) ^ (b & c);
        unsigned long long t2 = S0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    s[0] += a;
    s[1] += b;
    s[2] += c;
    s[3] += d;
    s[4] += e;
    s[5] += f;
    s[6] += g;
    s[7] += h;
}

// ORIG _ZL16SHA384_TransformPmS_ @0x80be32c：同 SHA512 压缩。
static void SHA384_Transform(unsigned long* state, unsigned long* data)
{
    const unsigned long long* m =
        reinterpret_cast<const unsigned long long*>(data);
    unsigned long long* s = reinterpret_cast<unsigned long long*>(state);
    unsigned long long W[80];
    unsigned long long a, b, c, d, e, f, g, h;

    for (int i = 0; i < 16; ++i)
    {
        W[i] = sha512_bswap64(m[i]);
    }
    for (int i = 16; i < 80; ++i)
    {
        unsigned long long s0 = SHA512_ROTR64(W[i - 15], 1) ^
                                SHA512_ROTR64(W[i - 15], 8) ^
                                (W[i - 15] >> 7);
        unsigned long long s1 = SHA512_ROTR64(W[i - 2], 19) ^
                                SHA512_ROTR64(W[i - 2], 61) ^
                                (W[i - 2] >> 6);
        W[i] = W[i - 16] + s0 + W[i - 7] + s1;
    }

    a = s[0];
    b = s[1];
    c = s[2];
    d = s[3];
    e = s[4];
    f = s[5];
    g = s[6];
    h = s[7];

    for (int i = 0; i < 80; ++i)
    {
        unsigned long long S1 = SHA512_ROTR64(e, 14) ^
                                SHA512_ROTR64(e, 18) ^
                                SHA512_ROTR64(e, 41);
        unsigned long long ch = (e & f) ^ (~e & g);
        unsigned long long t1 = h + S1 + ch + g_sha512K[i] + W[i];
        unsigned long long S0 = SHA512_ROTR64(a, 28) ^
                                SHA512_ROTR64(a, 34) ^
                                SHA512_ROTR64(a, 39);
        unsigned long long maj = (a & b) ^ (a & c) ^ (b & c);
        unsigned long long t2 = S0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    s[0] += a;
    s[1] += b;
    s[2] += c;
    s[3] += d;
    s[4] += e;
    s[5] += f;
    s[6] += g;
    s[7] += h;
}

// ============================================================================
// 2) 杂散小函数
// ============================================================================

// ORIG T 0x0841d2ea _Z11PrintLetter15SIG_LETTER_INFO：空体。
void PrintLetter(SIG_LETTER_INFO)
{
}

// ORIG T 0x08104884 _Z11SetSockNonbim：ioctl(fd, FIONBIO=0x5421, &arg) == 0。
bool SetSockNonb(int fd, unsigned long arg)
{
    unsigned long on = arg;
    return ioctl(fd, 0x5421, &on) == 0;
}

// ORIG T 0x089bd03b _Z20IsDamageActiveStatusi：
// status<=0xb && ((1<<status) & 0xac4)。
bool IsDamageActiveStatus(int status)
{
    if (status > 0xb)
        return false;
    return ((1 << status) & 0xac4) != 0;
}

// ORIG T 0x0808c74b _Z13PI_GetType_SCcs：type==1 查表、type==2 if 链、缺省 0。
int PI_GetType_SC(char type, short value)
{
    if (type == 1)
    {
        switch (value)
        {
        case 0x13: return 26;
        case 0x1d: return 11;
        case 0x1f: return 9;
        case 0x20: return 25;
        case 0x2e: return 18;
        case 0x2f: return 12;
        case 0x43: return 27;
        case 0x48: return 22;
        case 0x49: return 23;
        case 0x4a: return 24;
        case 0x53: return 44;
        default:   return 0;
        }
    }
    if (type == 2)
    {
        if (value == 0x28) return 4;
        if (value == 0x8e) return 5;
        if (value == 0x8f) return 6;
        if (value == 0x1c) return 7;
        if (value == 0x76) return 13;
        if (value == 0x25) return 16;
        if (value == 0x27) return 17;
        if (value == 0x73) return 19;
        if (value == 0x22) return 20;
        if (value == 0x23) return 21;
        if (value == 0x85) return 39;
        return 0;
    }
    return 0;
}

// ORIG T 0x0808bc7e _Z6Rand_rPj：LCG rand_r（NULL 时用全局种子）。
unsigned int Rand_r(unsigned int* seed)
{
    static unsigned int staticSeed = 0;  // ORIG _ZZ6Rand_rPjE10staticSeed 0x0936e480
    if (seed == 0)
    {
        staticSeed += timeGetTime();
        return Rand_r(&staticSeed);
    }
    unsigned int next = *seed;
    next = next * 0x41c64e6d + 0x3039;
    unsigned int result = (next >> 16) & 0x7ff;
    next = next * 0x41c64e6d + 0x3039;
    result = (result << 10) ^ ((next >> 16) & 0x3ff);
    next = next * 0x41c64e6d + 0x3039;
    result = (result << 10) ^ ((next >> 16) & 0x3ff);
    *seed = next;
    return result;
}

// ============================================================================
// 3) 脚本 import/export 族
// ============================================================================

// ORIG T 0x088bdb56 _Z11ScanAnyDataRiRSsS_b：任意类型值扫描。
bool ScanAnyData(int& type, std::string& str, int& value, bool ignoreUnknown)
{
    type = 3;
    bool got = false;
    for (;;)
    {
        int token = g_dnfLex.getToken();
        // ORIG if 链布局（非 switch 表）；case 9 成功时 ORIG 将 token 置 7，
        // 使后续循环 break 检查（2/3/4/7/5/0）命中，避免继续循环。
        if (token == 2)   // decimal
        {
            value = g_dnfLex.getDecimal();
            type = 1;
            got = true;
        }
        else if (token == 3)   // hex
        {
            value = g_dnfLex.getHexDecimal();
            type = 1;
            got = true;
        }
        else if (token == 4)   // float
        {
            value = (int)g_dnfLex.getFloat();
            type = 1;
            got = true;
        }
        else if (token == 7)   // string
        {
            str = toTString(g_dnfLex.getString());
            got = true;
            type = 0;
        }
        else if (token == 5)   // type
        {
            str = toTString(g_dnfLex.getType());
            got = true;
            type = 2;
        }
        else if (token == 9)   // pair int
        {
            int key = g_dnfLex.getPairInt();
            if (g_dnfLex.getToken() == 0xa)
            {
                str = g_scriptStringManager_.findString(key,
                                                        g_dnfLex.getPairStr(),
                                                        0);
                type = 0;
                got = true;
                token = 7;   // ORIG 0x88bdcb8 movl $0x7,-0x14(%ebp)
            }
        }
        else
        {
            got = false;
            type = 3;
        }
        if (token == 2 || token == 3 || token == 4 || token == 7 ||
            token == 5 || token == 0)
        {
            break;
        }
        if (!ignoreUnknown)
        {
            LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x746,
                                  "ERROR! %s - TYPE mismatch. %s:%d, token = '%s'",
                                  g_dnfLex.getStreamName().c_str(),
                                  "ScanAnyData",
                                  g_dnfLex.getLastLineNo(),
                                  g_dnfLex.getLastTokenDataAsString().c_str());
        }
    }
    return got;
}

// ORIG T 0x089e6dc4 _Z20ScanChampionFeaturesiR27STAICharacterMapArrangeData：
// count 行 × 0x25 特征名表匹配 → 命中 push 表项原值。
static const unsigned int g_championFeatureValue[0x25] = {
    0x1ff00, 0xff01, 0x2ff02, 0x2ff03, 0x30004, 0x30005, 0x30006, 0x30007,
    0x40008, 0x3fe09, 0x3fe0a, 0x3fe0b, 0x3fe0c, 0x2000d, 0x3000e, 0x3000f,
    0x20010, 0x40011, 0x10012, 0x20013, 0x2fd14, 0x2fd15, 0x2fd16, 0x2fd17,
    0x2fd18, 0x2fd19, 0x2fd1a, 0x3001b, 0x4fd1c, 0x4fd1d, 0x4fd1e, 0x4fd1f,
    0x4fd20, 0x4fd21, 0x4fd22, 0x4fd23, 0x4fd24
};
static const char* const g_championFeatureName[0x25] = {
    "[Immune-Magic]", "[Immune-Physic]", "[Enhance-Physicaldefense]",
    "[Enhance-Magicaldefense]", "[Immune-Fire]", "[Immune-Water]",
    "[Immune-Light]", "[Immune-Dark]", "[Immune-ActiveStatus]",
    "[Enchant-Fire]", "[Enchant-Water]", "[Enchant-Light]", "[Enchant-Dark]",
    "[Speedup-Move]", "[Speedup-Attack]", "[Speedup-Hitrecovery]",
    "[Speedup-Wakeup]", "[Speedup-Hpregen]", "[Etc-Neverfly]",
    "[Etc-Reserrection]", "[onAttack-stun]", "[onAttack-bleeding]",
    "[onAttack-confuse]", "[onAttack-slow]", "[onAttack-lightning]",
    "[onAttack-poison]", "[onAttack-blind]", "[high-activestatus]",
    "[Etc-divide]", "[Etc-invisible]", "[Etc-healed]", "[Etc-infected]",
    "[Etc-counted]", "[Etc-berserk]", "[Etc-sleep]", "[Etc-revive]",
    "[Etc-teleport]"
};
void ScanChampionFeatures(int count, STAICharacterMapArrangeData& data)
{
    std::string s;
    for (int line = 0; line < count; ++line)
    {
        ScanStr(&s);
        for (int i = 0; i <= 0x24; ++i)
        {
            if (s == g_championFeatureName[i])
            {
                data.m_championFeatures.push_back(
                    (int)g_championFeatureValue[i]);
                break;
            }
        }
    }
}

// ORIG T 0x08a65da5 _Z15ScanRewardItemsRSt6vectorI14RewardItemInfoSaIS0_EESs。
bool ScanRewardItems(std::vector<RewardItemInfo>& out, std::string token)
{
    bool ok1 = false;
    bool ok2 = false;
    RewardItemInfo info;
    std::string s;
    for (;;)
    {
        int v = ScanTypeOrInt(ok1, s, ok2, 1);
        if (ok1)
        {
            if (s == token)
                return true;
            return false;
        }
        if (!ok2)
            return false;
        info.m_itemIdx = v;
        info.m_job = 0xb;
        v = ScanStrOrInt(ok2, s, ok1, 1);
        if (!ok2)
            return false;
        if (ok1)
        {
            if (s == "[job]")
            {
                if (!ScanInt(&info.m_job))
                    return false;
                if (!ScanInt(&info.m_growType))
                    return false;
            }
            else
            {
                if (!ScanInt(&info.m_count))
                    return false;
            }
        }
        else
        {
            info.m_count = v;
        }
        out.push_back(info);
    }
}

// ORIG T 0x08a0fff6 _Z30exportCharacterGiveSkillScriptP8_IO_FILEPSt6vectorISt4pairIiiESaIS3_EE。
void exportCharacterGiveSkillScript(FILE* fp, std::vector<std::pair<int, int> >* vec)
{
    if (!vec->empty())
    {
        fwrite("[skill]\n", 1, 8, fp);
        for (std::vector<std::pair<int, int> >::iterator it = vec->begin();
             it != vec->end(); ++it)
        {
            fprintf(fp, "%d %d\n", it->first, it->second);
        }
        fwrite("[/skill]\n", 1, 9, fp);
    }
}

// ORIG T 0x08a100f3 _Z29exportCharacterEtcStatsScriptP8_IO_FILEP17CharacterEtcStats。
void exportCharacterEtcStatsScript(FILE* fp, CharacterEtcStats* stats)
{
    if (!stats->m_category.empty())
    {
        fwrite("[category]\n", 1, 0xb, fp);
        for (unsigned int i = 0; i < stats->m_category.size(); ++i)
        {
            std::string s = getObjectCategoryString((ENUM_CATEGORY)stats->m_category[i]);
            fprintf(fp, "`%s`\n", s.c_str());
        }
        fwrite("[/category]\n", 1, 0xc, fp);
    }
    int i = 0;
    for (i = 0; i <= 3; ++i)
    {
        if (stats->m_moduleDamageRate[i] != 0.0f)
            break;
    }
    if (i != 4)
    {
        fwrite("[module damage rate]", 1, 0x14, fp);
        for (int j = 0; j <= 3; ++j)
        {
            fprintf(fp, " %.3f", (double)stats->m_moduleDamageRate[j]);
        }
        fputc(0xa, fp);
    }
}

// ORIG T 0x08a0fb38 _Z27exportCharacterStatusScriptP8_IO_FILEP19STRDCharacterStatus：
// 非零字段输出（值 /10.0f）。
static const char* const g_activeResistName[17] = {
    "[slow resistance]", "[freeze resistance]", "[poison resistance]",
    "[stun resistance]", "[curse resistance]", "[blind resistance]",
    "[lightning resistance]", "[stone resistance]", "[sleep resistance]",
    "[burn resistance]", "[weapon break resistance]", "[bleeding resistance]",
    "[piercing resistance]", "[stuck resistance]", "[confuse resistance]",
    "[hold resistance]", "[armor break resistance]"
};
void exportCharacterStatusScript(FILE* fp, STRDCharacterStatus* st)
{
    if (st->m_hpMax != 0)
        fprintf(fp, "[HP MAX] %.1f\n", (double)st->m_hpMax / 10.0f);
    if (st->m_mpMax != 0)
        fprintf(fp, "[MP MAX] %.1f\n", (double)st->m_mpMax / 10.0f);
    if (st->m_physAttack != 0)
        fprintf(fp, "[physical attack] %.1f\n", (double)st->m_physAttack / 10.0f);
    if (st->m_physDefense != 0)
        fprintf(fp, "[physical defense] %.1f\n", (double)st->m_physDefense / 10.0f);
    if (st->m_magAttack != 0)
        fprintf(fp, "[magical attack] %.1f\n", (double)st->m_magAttack / 10.0f);
    if (st->m_magDefense != 0)
        fprintf(fp, "[magical defense] %.1f\n", (double)st->m_magDefense / 10.0f);
    if (st->m_elemResist[0] != 0)
        fprintf(fp, "[fire resistance] %.1f\n", (double)st->m_elemResist[0] / 10.0f);
    if (st->m_elemResist[1] != 0)
        fprintf(fp, "[water resistance] %.1f\n", (double)st->m_elemResist[1] / 10.0f);
    if (st->m_elemResist[2] != 0)
        fprintf(fp, "[dark resistance] %.1f\n", (double)st->m_elemResist[2] / 10.0f);
    if (st->m_elemResist[3] != 0)
        fprintf(fp, "[light resistance] %.1f\n", (double)st->m_elemResist[3] / 10.0f);
    for (int i = 0; i <= 0x10; ++i)
    {
        if (st->m_activeResist[i] != 0)
        {
            fprintf(fp, "%s %.1f\n", g_activeResistName[i],
                    (double)st->m_activeResist[i] / 10.0f);
        }
    }
    if (st->m_inventoryLimit != 0)
        fprintf(fp, "[inventory limit] %.1f\n", (double)st->m_inventoryLimit / 10.0f);
    if (st->m_hpRegen != 0)
        fprintf(fp, "[HP regen speed] %.1f\n", (double)st->m_hpRegen / 10.0f);
    if (st->m_mpRegen != 0)
        fprintf(fp, "[MP regen speed] %.1f\n", (double)st->m_mpRegen / 10.0f);
    if (st->m_moveSpeed != 0)
        fprintf(fp, "[move speed] %.1f\n", (double)st->m_moveSpeed / 10.0f);
    if (st->m_attackSpeed != 0)
        fprintf(fp, "[attack speed] %.1f\n", (double)st->m_attackSpeed / 10.0f);
    if (st->m_castSpeed != 0)
        fprintf(fp, "[cast speed] %.1f\n", (double)st->m_castSpeed / 10.0f);
    if (st->m_hitRecovery != 0)
        fprintf(fp, "[hit recovery] %.1f\n", (double)st->m_hitRecovery / 10.0f);
    if (st->m_jumpPower != 0)
        fprintf(fp, "[jump power] %.1f\n", (double)st->m_jumpPower / 10.0f);
    if (st->m_weight != 0)
        fprintf(fp, "[weight] %.1f\n", (double)st->m_weight / 10.0f);
}

// ORIG T 0x08919127 _Z22importQuestPieceScriptPKcRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE。
bool importQuestPieceScript(const char* path, std::map<int, int>& out)
{
    if (!loadRDARScriptFile(path, ""))
        return false;
    std::string token;
    for (;;)
    {
        if (!ScanType(token, true))
            return false;
        if (token == "[piece quest list]")
        {
            bool ok = false;
            for (;;)
            {
                int left = ScanInt(&ok);
                if (!ok)
                    break;
                int right = ScanInt(&ok);
                if (!ok)
                    break;
                out.insert(std::make_pair(left, right));
            }
        }
    }
}

// ORIG T 0x088ffaa9 _Z15importSlangListPcPSt6vectorISsSaISsEE。
bool importSlangList(char* path, std::vector<std::string>* out)
{
    if (!loadRDARScriptFile(path, ""))
        return false;
    std::string token;
    std::string a;
    std::string b;
    for (;;)
    {
        if (!ScanType(token, true))
            return true;
        if (token == "[SLANG]")
        {
            for (;;)
            {
                if (!ScanStr(&a))
                    break;
                if (!ScanStr(&b))
                    break;
                out->push_back(a);
                out->push_back(b);
            }
        }
    }
}

// ORIG T 0x08917848 _Z23importHiddenNpcListInfoPKcRSt6vectorIiSaIiEE。
bool importHiddenNpcListInfo(const char* path, std::vector<int>& out)
{
    if (!loadRDARScriptFile(path, ""))
        return false;
    std::string token;
    bool done = false;
    bool isType = false;
    bool isInt = false;
    out.clear();
    while (!done)
    {
        if (!ScanType(token, true))
            return false;
        if (token != "[hidden npc]")
            return false;
        for (;;)
        {
            std::string s;
            int v = ScanTypeOrInt(isType, s, isInt, 1);
            if (!isInt)
                return false;
            if (isType)
            {
                if (s == "[/hidden npc]")
                {
                    done = true;
                    return true;
                }
                return false;
            }
            out.push_back(v);
        }
    }
    return true;
}

// ORIG T 0x088ffe75 _Z29importUniqueRewardForAllUsersPcPSt3mapIiSt6vectorIS1_I22STStackablePackageDataSaIS2_EESaIS4_EESt4lessIiESaISt4pairIKiS6_EEE。
bool importUniqueRewardForAllUsers(
    char* path,
    std::map<int, std::vector<std::vector<STStackablePackageData> > >* out)
{
    if (out == 0)
        return false;
    if (!loadRDARScriptFile(path, ""))
        return false;
    std::string token;
    std::string name;
    for (;;)
    {
        if (!ScanType(token, true))
            return true;
        if (token == "[group]")
        {
            if (!ScanStr(&name))
                return false;
            int job = getCharacterJob(name.c_str());
            std::vector<std::vector<STStackablePackageData> > jobs;
            ScanType(token, true);
            while (token == "[package selection]")
            {
                std::vector<STStackablePackageData> items;
                for (int itemIdx = 0; ScanInt(&itemIdx); )
                {
                    STStackablePackageData p;
                    p.m_field0 = itemIdx;
                    p.m_field4 = ScanInt((bool*)0);
                    items.push_back(p);
                }
                jobs.push_back(items);
                ScanType(token, true);
            }
            (*out)[job] = jobs;
        }
    }
}

// ORIG T 0x08a358d7 _Z15importPetScriptP9PetScriptPKc。
bool importPetScript(PetScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_petScriptBaseDirectory, path))
        return false;
    script->clear();
    getDirectoryPath(script->m_directoryPath, "%s%s",
                     g_petScriptBaseDirectory, path);
    std::string token;
    std::string tmp;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            script->m_idx0 = g_petScriptFileList.FindIndexByFullScan(path);
            return true;
        }
        if (token == "[name]")
        {
            ScanStr(&script->m_name);
        }
        else if (token == "[width]")
        {
            script->m_width1 = ScanInt((bool*)0);
            script->m_width2 = ScanInt((bool*)0);
        }
        else if (token == "[floating height]")
        {
            script->m_floatingHeight = ScanInt((bool*)0);
        }
        else if (token == "[layer]")
        {
            ScanStr(&tmp);
            script->m_layer = getDrawLayer(tmp.c_str());
        }
        else if (token == "[recovery time]")
        {
            script->m_recoveryTime = ScanInt((bool*)0);
        }
        else if (token == "[direction correct]")
        {
            script->m_directionCorrect = ScanInt((bool*)0) != 0;
        }
        else if (token == "[move speed]")
        {
            script->m_moveSpeed[0] = ScanInt((bool*)0);
            script->m_moveSpeed[1] = ScanInt((bool*)0);
            script->m_moveSpeed[2] = ScanInt((bool*)0);
            script->m_moveSpeed[3] = ScanInt((bool*)0);
            script->m_moveSpeed[4] = ScanInt((bool*)0);
        }
        else if (token == "[gravity]")
        {
            script->m_gravity = ScanInt((bool*)0);
        }
        else if (token == "[basic motion]")
        {
            ScanStr(&script->m_basicMotion);
        }
        else if (token == "[etc motion]")
        {
            script->m_etcMotion.clear();
            while (ScanStr(&tmp))
            {
                script->m_etcMotion.push_back(tmp);
            }
        }
        else if (token == "[attack info]")
        {
            script->m_attackInfo.clear();
            while (ScanStr(&tmp))
            {
                script->m_attackInfo.push_back(tmp);
            }
        }
        else if (token == "[int data]")
        {
            script->m_intData.clear();
            bool ok = false;
            for (int v = ScanInt(&ok); ok; v = ScanInt(&ok))
            {
                script->m_intData.push_back(v);
            }
        }
        else if (token == "[string data]")
        {
            script->m_stringData.clear();
            while (ScanStr(&tmp))
            {
                script->m_stringData.push_back(tmp);
            }
        }
    }
}

// ORIG T 0x08a3dbaa _ZN9PetScript5clearEv。
void PetScript::clear()
{
    m_idx0 = 0;
    m_name = "";
    m_width1 = 0;
    m_width2 = 0;
    m_floatingHeight = 0;
    m_layer = 1;
    m_recoveryTime = 0;
    m_directionCorrect = true;
    for (int i = 0; i <= 1; ++i)
    {
        m_moveSpeed[i * 2] = 0;
        m_moveSpeed[i * 2 + 1] = 0;
    }
    m_gravity = 0;
    m_basicMotion = "";
    m_etcMotion.clear();
    m_attackInfo.clear();
    m_intData.clear();
    m_stringData.clear();
    m_directoryPath = "";
}

// ORIG T 0x0890a334 _Z27importUltimateSkillCutScenePKcRSt6vectorI23STUltimateSkillCutSceneSaIS2_EERi：
// 空体（mov $0,%eax; ret）。
int importUltimateSkillCutScene(const char*,
                                std::vector<STUltimateSkillCutScene>&, int&)
{
    return 0;
}

// ORIG T 0x088c1e68 _Z18importActiveStatusR14STActiveStatusb。
void importActiveStatus(STActiveStatus& st, bool flag)
{
    std::string s;
    ScanStr(&s);
    int type = getActiveStatusType(s);
    st.m_type = type;
    st.m_value4 = ScanFloat((bool*)0);
    st.m_value8 = ScanInt((bool*)0);
    if (flag)
    {
        st.m_flagC = ScanInt((bool*)0) != 0;
    }
    st.m_value10 = ScanInt((bool*)0);
    // ORIG if 链布局（非 switch 位掩码）：5/0/0xc 各 2 值，
    // 7/0xf/0x11/0xa/2/6/0xb 各 1 值，9/0xd/4 各循环 4 值。
    if (type == 5 || type == 0 || type == 0xc)
    {
        st.m_list14.push_back(ScanInt((bool*)0));
        st.m_list14.push_back(ScanInt((bool*)0));
    }
    else if (type == 7 || type == 0xf || type == 0x11 ||
             type == 0xa || type == 2 || type == 6 || type == 0xb)
    {
        st.m_list14.push_back(ScanInt((bool*)0));
    }
    else if (type == 9 || type == 0xd || type == 4)
    {
        for (int i = 0; i <= 3; ++i)
        {
            st.m_list14.push_back(ScanInt((bool*)0));
        }
    }
}

// ============================================================================
// 4) 成员/类函数
// ============================================================================

// ORIG T 0x089e82f6 _ZN12STTileScript5ClearEv。
void STTileScript::Clear()
{
    m_actions.clear();
    m_flagC = 0;
    m_collisions.clear();
    m_field1c = 0;
}

// ORIG W 0x0844d584 _ZN21STBreakAwayMemberData5clearEv。
void STBreakAwayMemberData::clear()
{
    m_field00 = 0;
    memset(m_name, 0, 0x1f);
    m_field24 = 0;
    m_field28 = 0;
    m_field2c = 0;
    m_field30 = 0;
    m_field34 = 0;
    m_field38 = 0;
    m_field3c = 0;
    m_field40 = 0;
}

// ORIG T 0x085e709e _ZN15MissionLessThanclEPK8CMissionS2_：
// 先比 MissionKind 再比 MissionIndex。
bool MissionLessThan::operator()(const CMission* a, const CMission* b)
{
    if (a->GetMissionKind() != b->GetMissionKind())
    {
        return a->GetMissionKind() < b->GetMissionKind();
    }
    if (a->GetMissionIndex() != b->GetMissionIndex())
    {
        return a->GetMissionIndex() < b->GetMissionIndex();
    }
    return false;
}

// ORIG T 0x081b5bf2 _ZNK23NeedMaterialDataManager15getNeedMaterialEi：
// getNeedMaterial(G_CDataManager()->find_item(i))。
CItem* NeedMaterialDataManager::getNeedMaterial(int index) const
{
    return getNeedMaterial(G_CDataManager()->find_item(index));
}

// ORIG T 0x0816d454 _ZN16RestrictCategory12enumToStringENS_4EnumE。
const char* RestrictCategory::enumToString(Enum e)
{
    static const char* const enumStringRestrict[4] = {
        "NullPolicy",
        "AuthMobilePolicy",
        "CreateCharacterPolicy",
        "SecurityPolicy"
    };
    return enumStringRestrict[e];
}

// ORIG T 0x088c1558 _ZL23importKeyNameIndexTablev（static）。
static bool importKeyNameIndexTable()
{
    if (!loadRDARScriptFile("", "Skill/RDARKeyIndex.dat"))
        return false;
    int value = 0;
    std::string s;
    for (;;)
    {
        int token = g_dnfLex.getToken();
        if (token == 0)
            break;
        if (token == 2)
        {
            value = g_dnfLex.getDecimal();
        }
        else if (token == 6)
        {
            s = toTString(g_dnfLex.getCustomData());
            g_mapKeyNameIndexTable[s] = value;
        }
    }
    return true;
}

// ORIG T 0x0808aa37 _Z16WriteCodeSectionPKc：读取 PE 头后写回 .code.bin。
void WriteCodeSection(const char* path)
{
    int e_lfanew = 0;
    int sizeOfCode = 0;
    int baseOfCode = 0;
    int sizeOfHeaders = 0;
    FILE* f = fopen(path, "rb");
    if (f != 0)
    {
        fseek(f, 0x3c, 0);
        fread(&e_lfanew, 4, 1, f);
        fseek(f, e_lfanew + 0x2c, 0);
        fread(&sizeOfCode, 4, 1, f);
        fseek(f, e_lfanew + 0x34, 0);
        fread(&baseOfCode, 4, 1, f);
        fseek(f, e_lfanew + 0x1c, 0);
        fread(&sizeOfHeaders, 4, 1, f);
        fclose(f);
    }
    char buf[0x100];
    strncpy(buf, path, 0x100);
    memcpy(buf + strlen(buf), ".code.bin", 0xa);
    FILE* out = fopen(buf, "wb");
    if (out != 0)
    {
        fwrite((const void*)(baseOfCode + sizeOfCode), 1, sizeOfHeaders, out);
        fclose(out);
    }
}

// ORIG T 0x089bbe1e _Z32getActivePropertyTypeToEnumValueRSs：
// 函数局部 static map + SpinLock 惰性填充；键 "slow resistance" 等 16 项。
int getActivePropertyTypeToEnumValue(std::string& s)
{
    // ORIG 用 less_TCHAR（strcmp 内容比较）：查找键为 s.c_str() 动态指针，
    // 默认 std::less<const char*>（指针比较）必然 miss → 恒返回 17（错误）。
    // ORIG 16 处登记各只有两次 call：先就地构造 map::value_type
    // （_ZNSt4pairIKPKciEC1IRA<N>_S0_iEEOT_OT0_ = pair<const char* const,int>
    //  的 pair<char const(&)[N], int> 模板构造），再 insert(value_type const&)。
    // 用 make_pair 会多出 pair<const char*,int> → value_type 的转换临时。
    typedef std::map<const char*, int, less_TCHAR> LookupMap;
    static LookupMap __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;
    for (;;)
    {
        int v;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it = __lookup.find(s.c_str());
            v = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;
            }
            v = 0;
        }
        switch (v)
        {
        case 0x2c4b:
            if (!__inited) { __lookup.insert(LookupMap::value_type("slow resistance", 0x2c4b)); }
            else return 0;
            break;
        case 0x2c4d:
            if (!__inited) { __lookup.insert(LookupMap::value_type("freeze resistance", 0x2c4d)); }
            else return 1;
            break;
        case 0x2c4f:
            if (!__inited) { __lookup.insert(LookupMap::value_type("poison resistance", 0x2c4f)); }
            else return 2;
            break;
        case 0x2c51:
            if (!__inited) { __lookup.insert(LookupMap::value_type("stun resistance", 0x2c51)); }
            else return 3;
            break;
        case 0x2c53:
            if (!__inited) { __lookup.insert(LookupMap::value_type("curse resistance", 0x2c53)); }
            else return 4;
            break;
        case 0x2c55:
            if (!__inited) { __lookup.insert(LookupMap::value_type("blind resistance", 0x2c55)); }
            else return 5;
            break;
        case 0x2c57:
            if (!__inited) { __lookup.insert(LookupMap::value_type("lightning resistance", 0x2c57)); }
            else return 6;
            break;
        case 0x2c59:
            if (!__inited) { __lookup.insert(LookupMap::value_type("stone resistance", 0x2c59)); }
            else return 7;
            break;
        case 0x2c5b:
            if (!__inited) { __lookup.insert(LookupMap::value_type("sleep resistance", 0x2c5b)); }
            else return 8;
            break;
        case 0x2c5d:
            if (!__inited) { __lookup.insert(LookupMap::value_type("burn resistance", 0x2c5d)); }
            else return 9;
            break;
        case 0x2c5f:
            if (!__inited) { __lookup.insert(LookupMap::value_type("weapon break resistance", 0x2c5f)); }
            else return 10;
            break;
        case 0x2c61:
            if (!__inited) { __lookup.insert(LookupMap::value_type("bleeding resistance", 0x2c61)); }
            else return 11;
            break;
        case 0x2c63:
            if (!__inited) { __lookup.insert(LookupMap::value_type("piercing resistance", 0x2c63)); }
            else return 12;
            break;
        case 0x2c65:
            if (!__inited) { __lookup.insert(LookupMap::value_type("stuck resistance", 0x2c65)); }
            else return 13;
            break;
        case 0x2c67:
            if (!__inited) { __lookup.insert(LookupMap::value_type("confuse resistance", 0x2c67)); }
            else return 14;
            break;
        case 0x2c69:
            if (!__inited) { __lookup.insert(LookupMap::value_type("hold resistance", 0x2c69)); }
            else return 15;
            break;
        default:
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
                continue;
            }
            return 17;
        }
    }
}
// ===== _Z13G_EncryptToolv —— ORIG 0x0848e1c6 =====
// ORIG：mov 尾调 GlobalInstance<EncryptTool>::inst_ptr()（inst_ptr 0x848e1d3）。
EncryptTool* G_EncryptTool()
{
    return GlobalInstance<EncryptTool>::inst_ptr();
}

// ===== _ZN35FAIR_PVP_SKILL_TREE_SKILL_PER_POINT5clearEv —— ORIG 0x08a5e874 =====
// 反汇编：map@+0x00 clear、map@+0x18 clear、vector<pair>@+0x30 clear、int@+0x3c=0。
void FAIR_PVP_SKILL_TREE_SKILL_PER_POINT::clear()
{
    m_addSkills.clear();
    m_removeSkills.clear();
    m_giveSkills.clear();
    m_pointCost = 0;
}
// ORIG C1 0x08a5e7e6：构造成员后调 clear()。
FAIR_PVP_SKILL_TREE_SKILL_PER_POINT::FAIR_PVP_SKILL_TREE_SKILL_PER_POINT()
{
    clear();
}

// ============================================================================
// 脚本导入族（STPassiveObjectScript / STTileScript / DifficultPenaltyInfo）
// ============================================================================

// ---- DifficultPenaltyInfo（ORIG mangled R20DifficultPenaltyInfo；脚本
//      "[COMMON TRAP EXPLOSION]" 段读 5 个 int，反汇编 0x890a3d6 逐 4 字节写）----
struct DifficultPenaltyInfo
{
    int m_values[5];
};

// ===== _Z32ImportDifficultPenaltyInfoScriptPKcR20DifficultPenaltyInfo —— ORIG 0x0890a33e =====
// loadRDARScriptFile(dir="", path) 失败 → 返回 0x1bdb；逐行 ScanType(token, true)：
// 遇 "[COMMON TRAP EXPLOSION]" 段读取 5 个 int（ScanInt 成功才写入 out）。
int ImportDifficultPenaltyInfoScript(const char* path, DifficultPenaltyInfo& out)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0x1bdb;
    }
    std::string token;
    while (ScanType(token, true))
    {
        if (token == "[COMMON TRAP EXPLOSION]")
        {
            for (int i = 0; i <= 4; ++i)
            {
                bool ok = false;
                int v = ScanInt(&ok);
                if (ok)
                {
                    out.m_values[i] = v;
                }
            }
        }
    }
    return 0;
}

// ===== _Z14InitTileScriptv —— ORIG 0x089dcbac =====
// mov $1 直返。
int InitTileScript()
{
    return 1;
}

// ===== _Z16ImportTileScriptP12STTileScriptPKc —— ORIG 0x089dcbb6 =====
// loadRDARScriptFile("TILE", path) 失败 → printf("TILE LOAD FAIL! [%s]\n", path) 返回 0；
// 先 Clear()，随后按 token 分派：
//   [IMAGE]   → 循环 { pair<string,int>；ScanStr(&first)；second=ScanInt((bool*)0)；
//                 m_actions.push_back }
//   [tile type]→ ScanStr 比较 "[basic]"→m_flagC=0、"[extended]"→m_flagC=1
//   [img pos] → m_field1c = ScanInt((bool*)0)
//   [pass type]→ m_collisions 清空；循环 { n=ScanInt(&ok)；!ok 退出；
//                 row.clear()+push(n)；再读 13 个 ScanInt((bool*)0) 追加；push_back(row) }
// 返回 1。
int ImportTileScript(STTileScript* script, const char* path)
{
    if (!loadRDARScriptFile("TILE", path))
    {
        printf("TILE LOAD FAIL! [%s]\n", path);
        return 0;
    }
    script->Clear();

    std::string token;
    std::string str;
    while (ScanType(token, true))
    {
        if (token == "[IMAGE]")
        {
            std::pair<std::string, int> entry;
            ScanStr(&entry.first);
            entry.second = ScanInt((bool*)0);
            script->m_actions.push_back(entry);
        }
        else if (token == "[tile type]")
        {
            ScanStr(&str);
            if (str == "[basic]")
            {
                script->m_flagC = 0;
            }
            else if (str == "[extended]")
            {
                script->m_flagC = 1;
            }
        }
        else if (token == "[img pos]")
        {
            script->m_field1c = ScanInt((bool*)0);
        }
        else if (token == "[pass type]")
        {
            script->m_collisions.clear();
            for (;;)
            {
                bool ok = false;
                int n = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                std::vector<int> row;
                row.clear();
                row.push_back(n);
                for (int i = 1; i <= 0xd; ++i)
                {
                    row.push_back(ScanInt((bool*)0));
                }
                script->m_collisions.push_back(row);
            }
        }
    }
    return 1;
}



// ============================================================================
// ImportPassiveObjectScript —— ORIG 0x08a2c51a（0x92f4 字节巨型 token 分派）
// 反汇编全量转写（df_game_r）。静态 token 映射为 16 个文件级惰性 map
// （ORIG 各 __cxa_guard + SpinLock + inited 三件套；重建以同名 static 变量
// 聚合，语义等价：首次查询时锁内登记全部键值，随后重查）。
// 各 case 语义见 switch 内注释（[token] → ORIG case 值 → 写哪个字段）。
// ============================================================================

namespace
{

typedef std::map<const char*, int, less_TCHAR> TokenMap;

struct TokenEntry
{
    const char* m_key;
    int m_value;
};

// ORIG：每 map 独立 guard 变量（0x94ff778 等）+ SpinLock + inited 标志。
// 重建按 ORIG 地址序命名（s_lookupMain=0x94ff8f8、s_lockMain=0x94ff910、
// s_initedMain=0x94ff914；子 map 依此类推），惰性注册后重查本 token。
void EnsureTokenMap(TokenMap& map, SpinLock& lock, bool& inited,
                    const TokenEntry* entries, int count)
{
    if (inited)
    {
        return;
    }
    lock.enter();
    if (!inited)
    {
        for (int i = 0; i < count; ++i)
        {
            map.insert(std::make_pair(entries[i].m_key, entries[i].m_value));
        }
        inited = true;
    }
    lock.leave();
}

int LookupToken(TokenMap& map, SpinLock& lock, bool& inited,
                const TokenEntry* entries, int count, const char* token)
{
    EnsureTokenMap(map, lock, inited, entries, count);
    TokenMap::const_iterator it = map.find(token);
    if (it == map.end())
    {
        return -1;
    }
    return it->second;
}

// ---- 主 token 映射（0x94ff8f8，惰性登记；case 值与 ORIG switch 分支一致）----
TokenMap s_lookupMain;                       // ORIG 0x94ff8f8
SpinLock s_lockMain;                         // ORIG 0x94ff910
bool s_initedMain = false;                   // ORIG 0x94ff914

const TokenEntry s_mainEntries[] = {
    { "[name]", 0x1e0d }, { "[width]", 0x1e16 },
    { "[floating height]", 0x1e1f }, { "[layer]", 0x1e24 },
    { "[layer level]", 0x1e29 }, { "[burning death path]", 0x1e2e },
    { "[is hp by difficulty]", 0x1e32 }, { "[hp gage]", 0x1e37 },
    { "[pass type]", 0x1e3c }, { "[piercing power]", 0x1e62 },
    { "[piercing tolerance]", 0x1e67 }, { "[category]", 0x1e6c },
    { "[basic motion]", 0x1e83 }, { "[etc motion]", 0x1e8f },
    { "[basic action]", 0x1ea9 }, { "[last action]", 0x1ead },
    { "[etc action]", 0x1eb1 }, { "[under gravity]", 0x1ec6 },
    { "[vanish on move collision]", 0x1ecc }, { "[trap]", 0x1ed3 },
    { "[allow friend hit]", 0x1ed9 }, { "[team]", 0x1edf },
    { "[destroy particle]", 0x1ee5 }, { "[attack info]", 0x1ee9 },
    { "[etc attack info]", 0x1eed }, { "[int data]", 0x1f01 },
    { "[string data]", 0x1f18 }, { "[quest type]", 0x1f2e },
    { "[hp max]", 0x1f3c }, { "[hp destroy]", 0x1f42 },
    { "[object destroy condition]", 0x1f48 }, { "[sound category]", 0x1fc8 },
    { "[add object effect]", 0x200a }, { "[add particles]", 0x2022 },
    { "[homing]", 0x2066 }, { "[for tournament]", 0x20e7 },
    { "[for school]", 0x20ed }, { "[die when parent die]", 0x20f3 },
    { "[transparent on meet player]", 0x20f8 },
    { "[pvp room transparent value for observer]", 0x20fd },
    { "[create sound]", 0x2102 }, { "[is step on]", 0x2106 },
    { "[can beat index]", 0x210d }, { "[is blocking obj]", 0x2126 },
    { "[apply physics]", 0x212b }, { "[following immortal]", 0x2131 },
    { "[follow parent]", 0x213f }, { "[speed control passive objects]", 0x2152 },
    { "[objects]", 0x2193 }, { "[patterns]", 0x21e1 },
    { "[sequences]", 0x2241 }, { "[warning]", 0x2271 },
    { "[sniper type]", 0x22a0 }, { "[passive object type]", 0x22f6 },
    { "[random item fairy]", 0x231a }, { "[passive object icon]", 0x234c },
};

// [pass type] 子映射（0x94ff918）
TokenMap s_lookupPassType;                   // 0x94ff918
SpinLock s_lockPassType;                     // 0x94ff930
bool s_initedPassType = false;               // 0x94ff934
const TokenEntry s_passTypeEntries[] = {
    { "[pass all]", 0x1e42 }, { "[pass only float type]", 0x1e45 },
    { "[do not pass]", 0x1e48 }, { "[pass only red team]", 0x1e4b },
    { "[pass only blue team]", 0x1e4e }, { "[pass only character team]", 0x1e51 },
    { "[pass only monster team]", 0x1e54 }, { "[pass only character]", 0x1e59 },
};

// [object destroy condition] 子映射（0x94ff938）
TokenMap s_lookupDestroyCond;                // 0x94ff938
SpinLock s_lockDestroyCond;                  // 0x94ff950
bool s_initedDestroyCond = false;            // 0x94ff954
const TokenEntry s_destroyCondEntries[] = {
    { "[destroy condition]", 0x1f58 }, { "[destroy action]", 0x1f7a },
    { "[on attack]", 0x1f8b }, { "[add object index]", 0x1f9c },
    { "[add object particle file]", 0x1fa8 }, { "[add object index on attack]", 0x1fb7 },
};

// [sound category] 子映射（0x94ff958）
TokenMap s_lookupSoundCat;                   // 0x94ff958
SpinLock s_lockSoundCat;                     // 0x94ff970
bool s_initedSoundCat = false;               // 0x94ff974
const TokenEntry s_soundCatEntries[] = {
    { "[after object create]", 0x1fe2 }, { "[on frame]", 0x1fe5 },
    { "[on attack]", 0x1fe8 }, { "[on damage]", 0x1feb },
    { "[on destroyobject]", 0x1fee },
};

// [add particles] 子映射（0x94ff978）
TokenMap s_lookupParticle;                   // 0x94ff978
SpinLock s_lockParticle;                     // 0x94ff990
bool s_initedParticle = false;               // 0x94ff994
const TokenEntry s_particleEntries[] = {
    { "[file]", 0x203e }, { "[create delay]", 0x2043 },
    { "[create sound]", 0x2048 }, { "[follow parent]", 0x204d },
    { "[loop]", 0x2057 },
};

// [homing] 子映射（0x94ff998）
TokenMap s_lookupHoming;                     // 0x94ff998
SpinLock s_lockHoming;                       // 0x94ff9b0
bool s_initedHoming = false;                 // 0x94ff9b4
const TokenEntry s_homingEntries[] = {
    { "[homing use]", 0x2072 }, { "[homing follow]", 0x2078 },
    { "[homing velocity]", 0x2097 }, { "[homing check gap]", 0x20a2 },
    { "[sync animation rotation]", 0x20a8 }, { "[max rotation]", 0x20ae },
    { "[init rotation]", 0x20b4 }, { "[diff rotation]", 0x20ba },
    { "[homing time]", 0x20c0 }, { "[homing z axis]", 0x20c6 },
    { "[homing direction]", 0x20cc }, { "[homing keep distance]", 0x20d2 },
    { "[sync target move]", 0x20d9 }, { "[straight homing]", 0x20de },
};

// [speed control passive objects] 外层子映射（0x94ff9b8）
TokenMap s_lookupSpeedOuter;                 // 0x94ff9b8
SpinLock s_lockSpeedOuter;                   // 0x94ff9d0
bool s_initedSpeedOuter = false;             // 0x94ff9d4
const TokenEntry s_speedOuterEntries[] = {
    { "[passive object]", 0x2161 },
};

// [speed control passive objects] 内层子映射（0x94ff9d8）
TokenMap s_lookupSpeedInner;                 // 0x94ff9d8
SpinLock s_lockSpeedInner;                   // 0x94ff9f0
bool s_initedSpeedInner = false;             // 0x94ff9f4
const TokenEntry s_speedInnerEntries[] = {
    { "[index]", 0x2177 }, { "[speed]", 0x217c },
};

// [objects] 外层子映射（0x94ff9f8）
TokenMap s_lookupObjectOuter;                // 0x94ff9f8
SpinLock s_lockObjectOuter;                  // 0x94ffa10
bool s_initedObjectOuter = false;            // 0x94ffa14
const TokenEntry s_objectOuterEntries[] = {
    { "[object]", 0x21a1 },
};

// [objects] 内层子映射（0x94ffa18）
TokenMap s_lookupObjectInner;                // 0x94ffa18
SpinLock s_lockObjectInner;                  // 0x94ffa30
bool s_initedObjectInner = false;            // 0x94ffa34
const TokenEntry s_objectInnerEntries[] = {
    { "[index]", 0x21b5 }, { "[probability]", 0x21bc },
    { "[warning]", 0x21c3 },
};

// [objects]/[warning] 扫描串子映射（0x94ffa38：none/danger/recovery）
TokenMap s_lookupWarnKind;                   // 0x94ffa38
SpinLock s_lockWarnKind;                     // 0x94ffa50
bool s_initedWarnKind = false;               // 0x94ffa54
const TokenEntry s_warnKindEntries[] = {
    { "none", 0x21cb }, { "danger", 0x21cc }, { "recovery", 0x21cd },
};

// [patterns] 外层子映射（0x94ffa58）
TokenMap s_lookupPatternOuter;               // 0x94ffa58
SpinLock s_lockPatternOuter;                 // 0x94ffa70
bool s_initedPatternOuter = false;           // 0x94ffa74
const TokenEntry s_patternOuterEntries[] = {
    { "[pattern]", 0x21f0 },
};

// [patterns] 内层子映射（0x94ffa78）
TokenMap s_lookupPatternInner;               // 0x94ffa78
SpinLock s_lockPatternInner;                 // 0x94ffa90
bool s_initedPatternInner = false;           // 0x94ffa94
const TokenEntry s_patternInnerEntries[] = {
    { "[name]", 0x2203 }, { "[interval]", 0x220c },
    { "[columns]", 0x2214 }, { "[data]", 0x221c },
};

// [sequences] 子映射（0x94ffa98）
TokenMap s_lookupSequence;                   // 0x94ffa98
SpinLock s_lockSequence;                     // 0x94ffab0
bool s_initedSequence = false;               // 0x94ffab4
const TokenEntry s_sequenceEntries[] = {
    { "[pattern]", 0x2250 },
};

// [warning] 子映射（0x94ffab8）
TokenMap s_lookupWarn;                       // 0x94ffab8
SpinLock s_lockWarn;                         // 0x94ffad0
bool s_initedWarn = false;                   // 0x94ffad4
const TokenEntry s_warnEntries[] = {
    { "[min distance]", 0x227e }, { "[max distance]", 0x228b },
};

// [sniper type] 子映射（0x94ffad8）
TokenMap s_lookupSniper;                     // 0x94ffad8
SpinLock s_lockSniper;                       // 0x94ffaf0
bool s_initedSniper = false;                 // 0x94ffaf4
const TokenEntry s_sniperEntries[] = {
    { "[accelerate]", 0x22ae }, { "[init velocity]", 0x22b3 },
    { "[shot area]", 0x22b8 }, { "[shot distance]", 0x22c1 },
    { "[move check time]", 0x22c8 }, { "[invoke check time]", 0x22cd },
    { "[invoke delay time]", 0x22d2 }, { "[wait time]", 0x22d7 },
    { "[shake area]", 0x22dc }, { "[invoke time]", 0x22e4 },
    { "[invoke count]", 0x22e9 },
};

// [passive object type] 子映射（0x94ffaf8）
TokenMap s_lookupFairyType;                  // 0x94ffaf8
SpinLock s_lockFairyType;                    // 0x94ffb10
bool s_initedFairyType = false;              // 0x94ffb14
const TokenEntry s_fairyTypeEntries[] = {
    { "[missile fairy]", 0x22fd }, { "[random item fairy]", 0x2301 },
};

// [random item fairy] 子映射（0x94ffb18）
TokenMap s_lookupFairy;                      // 0x94ffb18
SpinLock s_lockFairy;                        // 0x94ffb30
bool s_initedFairy = false;                  // 0x94ffb34
const TokenEntry s_fairyEntries[] = {
    { "[/random item fairy]", 0x2325 }, { "[entry]", 0x232a },
    { "[/entry]", 0x232f }, { "[passiveobject]", 0x2334 },
};

}  // namespace

// ===== _Z25ImportPassiveObjectScriptP21STPassiveObjectScriptPKcb —— ORIG 0x08a2c51a =====
// loadRDARScriptFile(基目录, path) 失败 → false；随后 clear 三个子对象、
// getDirectoryPath 拼 m_s90，主 token 循环分派（未知 token → false；
// bRepair 且已读 [name] 时提前结束）。EOF 统一尾处理：
// FindIndexByFullScan(path) 写 m_idx 并返回 true。
bool ImportPassiveObjectScript(STPassiveObjectScript* script, const char* path,
                               bool bRepair)
{
    if (!loadRDARScriptFile(g_szPassiveObjectScriptBaseDirectory, path))
    {
        return false;
    }
    script->clear();
    script->m_destroy.clear();
    script->m_addEffect.clear();
    getDirectoryPath(script->m_s90, "%s%s",
                     g_szPassiveObjectScriptBaseDirectory, path);

    std::string token;
    std::string str;
    bool nameSet = false;   // ORIG -0x8d（[name] 已读标志）
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }
        const int v = LookupToken(s_lookupMain, s_lockMain, s_initedMain,
                                  s_mainEntries,
                                  sizeof(s_mainEntries) / sizeof(s_mainEntries[0]),
                                  token.c_str());
        switch (v)
        {
        case 0x1e0d:   // [name] → m_name；m_layer = m_name；nameSet = true
            ScanStr(&script->m_name);
            script->m_layer = script->m_name;
            nameSet = true;
            break;
        case 0x1e16:   // [width] → m_width、m_height
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_width = v;
            }
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_height = v;
            }
            break;
        case 0x1e1f:   // [floating height] → m_i14
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_i14 = v;
            }
            break;
        case 0x1e24:   // [layer] → getDrawLayer → m_i18
            ScanStr(&str);
            script->m_i18 = getDrawLayer(str.c_str());
            break;
        case 0x1e29:   // [layer level] → m_i15c
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_i15c = v;
            }
            break;
        case 0x1e2e:   // [burning death path] → m_s17c
            ScanStr(&script->m_s17c);
            break;
        case 0x1e32:   // [is hp by difficulty] → m_b158
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_b158 = (v != 0);
            }
            break;
        case 0x1e37:   // [hp gage] → m_b159
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_b159 = (v == 1);
            }
            break;
        case 0x1e3c:   // [pass type] → m_i1c（子映射 0..7）
        {
            ScanStr(&str);
            const int pt = LookupToken(s_lookupPassType, s_lockPassType,
                                       s_initedPassType, s_passTypeEntries,
                                       sizeof(s_passTypeEntries) / sizeof(s_passTypeEntries[0]),
                                       str.c_str());
            if (pt < 0)
            {
                return false;
            }
            script->m_i1c = pt;
            break;
        }
        case 0x1e62:   // [piercing power] → m_i20
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_i20 = v;
            }
            break;
        case 0x1e67:   // [piercing tolerance] → m_i24
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_i24 = v;
            }
            break;
        case 0x1e6c:   // [category] → m_category（getObjectCategory；0x2c 跳过）
        {
            std::string s;
            for (;;)
            {
                bool isType = false;
                if (!ScanTypeOrStr(isType, s, 1))
                {
                    return false;
                }
                if (isType)
                {
                    if (s != "[/category]")
                    {
                        return false;
                    }
                    break;
                }
                const int cat = getObjectCategory(s);
                if (cat == 0x2c)
                {
                    continue;
                }
                script->m_category.push_back(cat);
            }
            break;
        }
        case 0x1e83:   // [basic motion] → m_s34（isValidExtension ".ani" 校验）
        {
            std::string s;
            if (!ScanStr(&s))
            {
                return false;
            }
            if (isValidExtension(s, ".ani"))
            {
                return false;
            }
            script->m_s34 = s;
            break;
        }
        case 0x1e8f:   // [etc motion] → m_basicActions
        {
            std::string s;
            for (;;)
            {
                bool isType = false;
                if (!ScanTypeOrStr(isType, s, 1))
                {
                    return false;
                }
                if (isType)
                {
                    if (s != "[/etc motion]")
                    {
                        return false;
                    }
                    break;
                }
                if (isValidExtension(s, ".ani"))
                {
                    return false;
                }
                script->m_basicActions.push_back(s);
            }
            break;
        }
        case 0x1ea9:   // [basic action] → m_s44
            ScanStr(&script->m_s44);
            break;
        case 0x1ead:   // [last action] → m_s54
            ScanStr(&script->m_s54);
            break;
        case 0x1eb1:   // [etc action] → m_etcActions
        {
            std::string s;
            for (;;)
            {
                bool isType = false;
                if (!ScanTypeOrStr(isType, s, 1))
                {
                    return false;
                }
                if (isType)
                {
                    if (s != "[/etc action]")
                    {
                        return false;
                    }
                    break;
                }
                script->m_etcActions.push_back(s);
            }
            break;
        }
        case 0x1ec6:   // [under gravity] → m_b58
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_b58 = (v == 1);
            }
            break;
        case 0x1ecc:   // [vanish on move collision] → m_b61=1、m_b60
            script->m_b61 = 1;
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_b60 = (v == 1);
            }
            break;
        case 0x1ed3:   // [trap] → m_b5a
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_b5a = (v == 1);
            }
            break;
        case 0x1ed9:   // [allow friend hit] → m_b62
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_b62 = (v == 1);
            }
            break;
        case 0x1edf:   // [team] → m_i5c
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_i5c = v;
            }
            break;
        case 0x1ee5:   // [destroy particle] → m_s64
            ScanStr(&script->m_s64);
            break;
        case 0x1ee9:   // [attack info] → m_s68
            ScanStr(&script->m_s68);
            break;
        case 0x1eed:   // [etc attack info] → m_attackInfos
        {
            std::string s;
            for (;;)
            {
                bool isType = false;
                if (!ScanTypeOrStr(isType, s, 1))
                {
                    return false;
                }
                if (isType)
                {
                    if (s != "[/etc attack info]")
                    {
                        return false;
                    }
                    break;
                }
                script->m_attackInfos.push_back(s);
            }
            break;
        }
        case 0x1f01:   // [int data] → m_intData（ScanTypeOrInt 循环）
        {
            std::string s;
            script->m_intData.clear();
            for (;;)
            {
                bool isType = false;
                bool isInt = false;
                const int val = ScanTypeOrInt(isType, s, isInt, 1);
                if (!isInt)
                {
                    return false;
                }
                if (isType)
                {
                    if (s != "[/int data]")
                    {
                        return false;
                    }
                    break;
                }
                script->m_intData.push_back(val);
            }
            break;
        }
        case 0x1f18:   // [string data] → m_stringData
        {
            std::string s;
            script->m_stringData.clear();
            for (;;)
            {
                bool isType = false;
                if (!ScanTypeOrStr(isType, s, 1))
                {
                    return false;
                }
                if (isType)
                {
                    if (s != "[/string data]")
                    {
                        return false;
                    }
                    break;
                }
                script->m_stringData.push_back(s);
            }
            break;
        }
        case 0x1f2e:   // [quest type] → m_i94
        {
            ScanStr(&str);
            if (str == "[none]")
            {
                script->m_i94 = 0;
            }
            else if (str == "[must be destroyed]")
            {
                script->m_i94 = 1;
            }
            else if (str == "[chaos object]")
            {
                script->m_i94 = 2;
            }
            else
            {
                return false;
            }
            break;
        }
        case 0x1f3c:   // [hp max] → m_i98
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_i98 = v;
            }
            break;
        case 0x1f42:   // [hp destroy] → m_b59
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_b59 = (v == 1);
            }
            break;
        case 0x1f48:   // [object destroy condition] → m_destroy 各子字段
        {
            const int dv = LookupToken(s_lookupDestroyCond, s_lockDestroyCond,
                                       s_initedDestroyCond, s_destroyCondEntries,
                                       sizeof(s_destroyCondEntries) / sizeof(s_destroyCondEntries[0]),
                                       token.c_str());
            switch (dv)
            {
            case 0x1f58:   // [destroy condition] → m_type（子 token 0..4 / -1）
            {
                ScanStr(&str);
                if (str == "[on end of animation]")
                {
                    script->m_destroy.m_type = 0;
                }
                else if (str == "[z pos equal zero]")
                {
                    script->m_destroy.m_type = 1;
                }
                else if (str == "[time limite]")
                {
                    script->m_destroy.m_type = 2;
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_destroy.m_i3c = v;
                    }
                }
                else if (str == "[not destroy and z pos equal zero]")
                {
                    script->m_destroy.m_type = 3;
                }
                else if (str == "[parent dead]")
                {
                    script->m_destroy.m_type = 4;
                }
                else
                {
                    script->m_destroy.m_type = -1;
                }
                break;
            }
            case 0x1f7a:   // [destroy action] → m_flag04
            {
                ScanStr(&str);
                if (str == "[only destroy]")
                {
                    script->m_destroy.m_flag04 = 0;
                }
                else if (str == "[object create after destroy]")
                {
                    script->m_destroy.m_flag04 = 1;
                }
                else
                {
                    script->m_destroy.m_flag04 = -1;
                }
                break;
            }
            case 0x1f8b:   // [on attack] → m_i40
            {
                ScanStr(&str);
                if (str == "[only destroy]")
                {
                    script->m_destroy.m_i40 = 0;
                }
                else if (str == "[object create after destroy]")
                {
                    script->m_destroy.m_i40 = 1;
                }
                else
                {
                    script->m_destroy.m_i40 = -1;
                }
                break;
            }
            case 0x1f9c:   // [add object index] → m_ints44 × n
            {
                int n;
                if (!ScanInt(&n))
                {
                    return false;
                }
                for (int i = 0; i < n; ++i)
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_destroy.m_ints44.push_back(v);
                    }
                }
                break;
            }
            case 0x1fa8:   // [add object particle file] → m_strs14 × n
            {
                    int n;
                    if (!ScanInt(&n))
                    {
                        return false;
                    }
                script->m_destroy.m_strs14.reserve(n);
                for (int i = 0; i < n; ++i)
                {
                    std::string s;
                    ScanStr(&s);
                    script->m_destroy.m_strs14.push_back(s);
                }
                break;
            }
            case 0x1fb7:   // [add object index on attack] → m_ints44（reserve 版）
            {
                    int n;
                    if (!ScanInt(&n))
                    {
                        return false;
                    }
                script->m_destroy.m_ints44.reserve(n);
                for (int i = 0; i < n; ++i)
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_destroy.m_ints44.push_back(v);
                    }
                }
                break;
            }
            default:
                return false;
            }
            break;
        }
        case 0x1fc8:   // [sound category] → m_strs20/m_ints2c（5 槽）
        {
            script->m_destroy.m_strs20.clear();
            script->m_destroy.m_ints2c.clear();
            std::string s;
            script->m_destroy.m_strs20.resize(5, s);
            script->m_destroy.m_ints2c.resize(5, 0);
            for (;;)
            {
                bool isType = false;
                if (!ScanTypeOrStr(isType, s, 1))
                {
                    return false;
                }
                if (isType)
                {
                    if (s != "[/sound category]")
                    {
                        return false;
                    }
                    break;
                }
                int slot = LookupToken(s_lookupSoundCat, s_lockSoundCat,
                                       s_initedSoundCat, s_soundCatEntries,
                                       sizeof(s_soundCatEntries) / sizeof(s_soundCatEntries[0]),
                                       s.c_str());
                if (slot < 0)
                {
                    return false;
                }
                script->m_destroy.m_strs20[slot] = s;
                {
                    int v;
                    if (!ScanInt(&v))
                    {
                        return false;
                    }
                    script->m_destroy.m_ints2c[slot] = v;
                }
                if (slot == 1)   // [on frame] 额外 [on frame] 值 → m_i38
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_destroy.m_i38 = v;
                    }
                }
            }
            break;
        }
        case 0x200a:   // [add object effect] → m_addEffect
        {
            std::string s;
            for (;;)
            {
                bool isType = false;
                if (!ScanTypeOrStr(isType, s, 1))
                {
                    return false;
                }
                if (isType)
                {
                    if (s != "[/add object effect]")
                    {
                        return false;
                    }
                    break;
                }
                script->m_addEffect.m_strs00.push_back(s);
                {
                    int v;
                    if (!ScanInt(&v))
                    {
                        return false;
                    }
                    script->m_addEffect.m_ints0c.push_back(v);
                }
            }
            break;
        }
        case 0x2022:   // [add particles] → m_particles
        {
            std::string s1;
            std::string s2;
            std::string s3;
            for (;;)
            {
                if (!ScanType(s1, true))
                {
                    return false;
                }
                if (s1 == "[/add particles]")
                {
                    break;
                }
                STPassiveObjectAdditionalParticleInfo* obj =
                    new STPassiveObjectAdditionalParticleInfo();
                for (;;)
                {
                    if (!ScanType(s2, true))
                    {
                        delete obj;
                        return false;
                    }
                    if (s2 == "[/particle]")
                    {
                        break;
                    }
                    const int pv = LookupToken(s_lookupParticle, s_lockParticle,
                                               s_initedParticle, s_particleEntries,
                                               sizeof(s_particleEntries) / sizeof(s_particleEntries[0]),
                                               s2.c_str());
                    switch (pv)
                    {
                    case 0x203e:   // [file] → m_s00
                        ScanStr(&s3);
                        obj->m_s00 = s3;
                        break;
                    case 0x2043:   // [create delay] → m_i0c
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            obj->m_i0c = v;
                        }
                        break;
                    case 0x2048:   // [create sound] → m_s04
                        ScanStr(&s3);
                        obj->m_s04 = s3;
                        break;
                    case 0x204d:   // [follow parent] → m_s08 + m_i10/m_i14/m_i18
                        obj->m_s08 = s2;
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            obj->m_i10 = v;
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            obj->m_i14 = v;
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            obj->m_i18 = v;
                        }
                        break;
                    case 0x2057:   // [loop] → m_b1c
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            obj->m_b1c = (v != 0);
                        }
                        break;
                    default:
                        delete obj;
                        return false;
                    }
                }
                script->m_particles.push_back(obj);
            }
            break;
        }
        case 0x2066:   // [homing] → homing 字段组
        {
            const int hv = LookupToken(s_lookupHoming, s_lockHoming,
                                       s_initedHoming, s_homingEntries,
                                       sizeof(s_homingEntries) / sizeof(s_homingEntries[0]),
                                       token.c_str());
            switch (hv)
            {
            case 0x2072:   // [homing use] → m_b9c
                {
                    int v;
                    if (!ScanInt(&v))
                    {
                        return false;
                    }
                    script->m_b9c = (v != 0);
                }
                break;
            case 0x2078:   // [homing follow] → m_ia0/m_ia4
            {
                ScanStr(&str);
                if (str == "[ME]")
                {
                    script->m_ia0 = 0;
                }
                else if (str == "[ENEMY]")
                {
                    script->m_ia0 = 1;
                }
                else if (str == "[CHARACTER]")
                {
                    script->m_ia0 = 3;
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_ia4 = v;
                    }
                }
                else if (str == "[MONSTER]")
                {
                    script->m_ia0 = 2;
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_ia4 = v;
                    }
                }
                else
                {
                    return false;
                }
                break;
            }
            case 0x2097:   // [homing velocity] → m_fa8/m_fac
                {
                    float v;
                    if (!ScanFloat(&v))
                    {
                        return false;
                    }
                    script->m_fa8 = v;
                }
                {
                    float v;
                    if (!ScanFloat(&v))
                    {
                        return false;
                    }
                    script->m_fac = v;
                }
                break;
            case 0x20a2:   // [homing check gap] → m_ib0
                {
                    int v;
                    if (!ScanInt(&v))
                    {
                        return false;
                    }
                    script->m_ib0 = v;
                }
                break;
            case 0x20a8:   // [sync animation rotation] → m_bb4
                {
                    int v;
                    if (!ScanInt(&v))
                    {
                        return false;
                    }
                    script->m_bb4 = (v != 0);
                }
                break;
            case 0x20ae:   // [max rotation] → m_fb8
                {
                    float v;
                    if (!ScanFloat(&v))
                    {
                        return false;
                    }
                    script->m_fb8 = v;
                }
                break;
            case 0x20b4:   // [init rotation] → m_fbc
                {
                    float v;
                    if (!ScanFloat(&v))
                    {
                        return false;
                    }
                    script->m_fbc = v;
                }
                break;
            case 0x20ba:   // [diff rotation] → m_fc0
                {
                    float v;
                    if (!ScanFloat(&v))
                    {
                        return false;
                    }
                    script->m_fc0 = v;
                }
                break;
            case 0x20c0:   // [homing time] → m_ic4
                {
                    int v;
                    if (!ScanInt(&v))
                    {
                        return false;
                    }
                    script->m_ic4 = v;
                }
                break;
            case 0x20c6:   // [homing z axis] → m_ic8
                {
                    int v;
                    if (!ScanInt(&v))
                    {
                        return false;
                    }
                    script->m_ic8 = v;
                }
                break;
            case 0x20cc:   // [homing direction] → m_icc
                {
                    int v;
                    if (!ScanInt(&v))
                    {
                        return false;
                    }
                    script->m_icc = v;
                }
                break;
            case 0x20d2:   // [homing keep distance] → m_id0
                {
                    int v;
                    if (!ScanInt(&v))
                    {
                        return false;
                    }
                    script->m_id0 = v;
                }
                break;
            case 0x20d9:   // [sync target move] → m_bd4
                script->m_bd4 = 1;
                break;
            case 0x20de:   // [straight homing] → m_bd5
                script->m_bd5 = 1;
                break;
            default:
                return false;
            }
            break;
        }
        case 0x20e7:   // [for tournament] → m_b14c
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_b14c = (v != 0);
            }
            break;
        case 0x20ed:   // [for school] → m_b14d
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_b14d = (v != 0);
            }
            break;
        case 0x20f3:   // [die when parent die] → m_b14e
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_b14e = (v != 0);
            }
            break;
        case 0x20f8:   // [transparent on meet player] → m_ub14f
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_ub14f = (v != 0);
            }
            break;
        case 0x20fd:   // [pvp room transparent value for observer] → m_us150
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_us150 = static_cast<unsigned short>(v);
            }
            break;
        case 0x2102:   // [create sound] → m_s154
            ScanStr(&script->m_s154);
            break;
        case 0x2106:   // [is step on]：仅 ScanInt 校验，值丢弃（ORIG 0x8a31533）
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                
            }
            break;
        case 0x210d:   // [can beat index] → m_onAttackData
        {
            std::string s;
            for (;;)
            {
                bool isType = false;
                bool isInt = false;
                const int val = ScanTypeOrInt(isType, s, isInt, 1);
                if (!isInt)
                {
                    return false;
                }
                if (isType)
                {
                    if (s != "[/can beat index]")
                    {
                        return false;
                    }
                    break;
                }
                script->m_onAttackData.push_back(val);
            }
            break;
        }
        case 0x2126:   // [is blocking obj] → m_b16c
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                script->m_b16c = (v != 0);
            }
            break;
        case 0x212b:   // [apply physics] → m_f170
            {
                float v;
                if (!ScanFloat(&v))
                {
                    return false;
                }
                script->m_f170 = v;
            }
            break;
        case 0x2131:   // [following immortal] → m_i174（1；==1 时 2）
            script->m_i174 = 1;
            {
                int v;
                if (!ScanInt(&v))
                {
                    return false;
                }
                if (v == 1)
                {
                    script->m_i174 = 2;
                }
            }
            break;
        case 0x213f:   // [follow parent] → m_i178
        {
            ScanStr(&str);
            if (str == "[path gate]")
            {
                script->m_i178 = 1;
            }
            else if (str == "[parent]")
            {
                script->m_i178 = 2;
            }
            else if (str == "[from parent]")
            {
                script->m_i178 = 3;
            }
            else if (str == "[keep position]")
            {
                script->m_i178 = 4;
            }
            else
            {
                return false;
            }
            break;
        }
        case 0x2152:   // [speed control passive objects] → m_speedCtrls
        {
            std::string s;
            script->m_speedCtrls.clear();
            for (;;)
            {
                if (!ScanType(s, true))
                {
                    return false;
                }
                if (s == "[/speed control passive objects]")
                {
                    break;
                }
                const int sv = LookupToken(s_lookupSpeedOuter, s_lockSpeedOuter,
                                           s_initedSpeedOuter, s_speedOuterEntries,
                                           sizeof(s_speedOuterEntries) / sizeof(s_speedOuterEntries[0]),
                                           s.c_str());
                if (sv != 0 && sv != 0x2161)
                {
                    return false;
                }
                SpeedCtrlPassiveObjInfo info;
                for (;;)
                {
                    if (!ScanType(s, true))
                    {
                        return false;
                    }
                    if (s == "[/passive object]")
                    {
                        if (info.m_field0 == -1)
                        {
                            return false;
                        }
                        script->m_speedCtrls.push_back(info);
                        break;
                    }
                    const int iv = LookupToken(s_lookupSpeedInner, s_lockSpeedInner,
                                               s_initedSpeedInner, s_speedInnerEntries,
                                               sizeof(s_speedInnerEntries) / sizeof(s_speedInnerEntries[0]),
                                               s.c_str());
                    if (iv == 0x2177)   // [index] → m_field0
                    {
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            info.m_field0 = v;
                        }
                    }
                    else if (iv == 0x217c)   // [speed] → m_b4（[up]/[down]）
                    {
                        ScanStr(&str);
                        if (str == "[up]")
                        {
                            info.m_b4 = 1;
                        }
                        else if (str == "[down]")
                        {
                            info.m_b4 = 0;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            break;
        }
        case 0x2193:   // [objects] → m_genObjects
        {
            std::string s;
            script->m_genObjects.clear();
            for (;;)
            {
                if (!ScanType(s, true))
                {
                    return false;
                }
                if (s == "[/objects]")
                {
                    break;
                }
                const int ov = LookupToken(s_lookupObjectOuter, s_lockObjectOuter,
                                           s_initedObjectOuter, s_objectOuterEntries,
                                           sizeof(s_objectOuterEntries) / sizeof(s_objectOuterEntries[0]),
                                           s.c_str());
                if (ov != 0 && ov != 0x21a1)
                {
                    return false;
                }
                PassiveObjGeneratorObject obj;
                for (;;)
                {
                    if (!ScanType(s, true))
                    {
                        return false;
                    }
                    if (s == "[/object]")
                    {
                        script->m_genObjects.push_back(obj);
                        break;
                    }
                    const int iv = LookupToken(s_lookupObjectInner, s_lockObjectInner,
                                               s_initedObjectInner, s_objectInnerEntries,
                                               sizeof(s_objectInnerEntries) / sizeof(s_objectInnerEntries[0]),
                                               s.c_str());
                    switch (iv)
                    {
                    case 0x21b5:   // [index] → m_i00
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            obj.m_i00 = v;
                        }
                        break;
                    case 0x21bc:   // [probability] → m_f04
                        {
                            float v;
                            if (!ScanFloat(&v))
                            {
                                return false;
                            }
                            obj.m_f04 = v;
                        }
                        break;
                    case 0x21c3:   // [warning] → ScanStr → m_s08
                    {
                        std::string w;
                        ScanStr(&w);
                        const int wv = LookupToken(s_lookupWarnKind, s_lockWarnKind,
                                                   s_initedWarnKind, s_warnKindEntries,
                                                   sizeof(s_warnKindEntries) / sizeof(s_warnKindEntries[0]),
                                                   w.c_str());
                        if (wv < 0)
                        {
                            return false;
                        }
                        obj.m_s08 = w;
                        break;
                    }
                    default:
                        return false;
                    }
                }
            }
            break;
        }
        case 0x21e1:   // [patterns] → m_genPatterns
        {
            std::string s;
            script->m_genPatterns.clear();
            for (;;)
            {
                if (!ScanType(s, true))
                {
                    return false;
                }
                if (s == "[/patterns]")
                {
                    break;
                }
                const int pv = LookupToken(s_lookupPatternOuter, s_lockPatternOuter,
                                           s_initedPatternOuter, s_patternOuterEntries,
                                           sizeof(s_patternOuterEntries) / sizeof(s_patternOuterEntries[0]),
                                           s.c_str());
                if (pv != 0 && pv != 0x21f0)
                {
                    return false;
                }
                PassiveObjGeneratorPattern pat;
                for (;;)
                {
                    if (!ScanType(s, true))
                    {
                        return false;
                    }
                    if (s == "[/pattern]")
                    {
                        script->m_genPatterns.push_back(pat);
                        break;
                    }
                    const int iv = LookupToken(s_lookupPatternInner, s_lockPatternInner,
                                               s_initedPatternInner, s_patternInnerEntries,
                                               sizeof(s_patternInnerEntries) / sizeof(s_patternInnerEntries[0]),
                                               s.c_str());
                    switch (iv)
                    {
                    case 0x2203:   // [name] → m_s04
                        ScanStr(&s);
                        pat.m_s04 = s;
                        break;
                    case 0x220c:   // [interval] → m_i0c
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            pat.m_i0c = v;
                        }
                        break;
                    case 0x2214:   // [columns] → m_i08
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            pat.m_i08 = v;
                        }
                        break;
                    case 0x221c:   // [data] → m_b00=0 + m_v10 byte 流
                    {
                        std::string ds;
                        pat.m_b00 = 0;
                        for (;;)
                        {
                            bool isType = false;
                            bool isInt = false;
                            const int val = ScanTypeOrInt(isType, ds, isInt, 1);
                            if (!isInt)
                            {
                                return false;
                            }
                            if (isType)
                            {
                                if (ds != "[/data]")
                                {
                                    return false;
                                }
                                break;
                            }
                            pat.m_v10.push_back(static_cast<unsigned char>(val));
                        }
                        break;
                    }
                    default:
                        return false;
                    }
                }
            }
            break;
        }
        case 0x2241:   // [sequences] → m_genSequences
        {
            std::string s;
            script->m_genSequences.clear();
            for (;;)
            {
                if (!ScanType(s, true))
                {
                    return false;
                }
                if (s == "[/sequences]")
                {
                    break;
                }
                const int sv = LookupToken(s_lookupSequence, s_lockSequence,
                                           s_initedSequence, s_sequenceEntries,
                                           sizeof(s_sequenceEntries) / sizeof(s_sequenceEntries[0]),
                                           s.c_str());
                if (sv != 0 && sv != 0x2250)
                {
                    return false;
                }
                PassiveObjGeneratorSequence seq;
                for (;;)
                {
                    bool isType = false;
                if (!ScanTypeOrStr(isType, s, 1))
                    {
                        return false;
                    }
                    if (isType)
                    {
                        if (s != "[/pattern]")
                        {
                            return false;
                        }
                        script->m_genSequences.push_back(seq);
                        break;
                    }
                    seq.m_s00 = s;
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        seq.m_i04 = v;
                    }
                }
            }
            break;
        }
        case 0x2271:   // [warning] → m_warnDanger/m_warnRecovery
        {
            std::string s;
            for (;;)
            {
                if (!ScanType(s, true))
                {
                    return false;
                }
                if (s == "[/warning]")
                {
                    break;
                }
                const int wv = LookupToken(s_lookupWarn, s_lockWarn,
                                           s_initedWarn, s_warnEntries,
                                           sizeof(s_warnEntries) / sizeof(s_warnEntries[0]),
                                           s.c_str());
                if (wv == 0x227e)   // [min distance] → m_warnDanger 3 int
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_warnDanger.m_a = v;
                    }
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_warnDanger.m_b = v;
                    }
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_warnDanger.m_c = v;
                    }
                }
                else if (wv == 0x228b)   // [max distance] → m_warnRecovery 3 int
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_warnRecovery.m_a = v;
                    }
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_warnRecovery.m_b = v;
                    }
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_warnRecovery.m_c = v;
                    }
                }
                else
                {
                    return false;
                }
            }
            break;
        }
        case 0x22a0:   // [sniper type] → m_sniper
        {
            std::string s;
            PassiveIObjSniperInfo info;
            for (;;)
            {
                if (!ScanType(s, true))
                {
                    return false;
                }
                if (s == "[/sniper type]")
                {
                    script->m_sniper = info;
                    break;
                }
                const int sv = LookupToken(s_lookupSniper, s_lockSniper,
                                           s_initedSniper, s_sniperEntries,
                                           sizeof(s_sniperEntries) / sizeof(s_sniperEntries[0]),
                                           s.c_str());
                switch (sv)
                {
                case 0x22ae:   // [accelerate] → m_i30
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        info.m_i30 = v;
                    }
                    break;
                case 0x22b3:   // [init velocity] → m_i34
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        info.m_i34 = v;
                    }
                    break;
                case 0x22b8:   // [shot area] → m_b40=1 + 6 dword @+0x14..+0x28
                {
                    info.m_b40 = 1;
                    // ORIG 0x8a34880 循环写 info+0x14+i*4（i=0..5，覆盖
                    // m_vecMaxDist/m_vecField18/m_vecField24 指针槽区）。重建按
                    // 同偏移裸写（ORIG 即如此，勿触碰 vector 语义）。
                    int* p = reinterpret_cast<int*>(
                        reinterpret_cast<char*>(&info) + 0x14);
                    for (int i = 0; i <= 5; ++i)
                    {
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            p[i] = v;
                        }
                    }
                    break;
                }
                case 0x22c1:   // [shot distance] → m_b38=1、m_i3c
                    info.m_b38 = 1;
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        info.m_i3c = v;
                    }
                    break;
                case 0x22c8:   // [move check time] → m_i5c
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        info.m_i5c = v;
                    }
                    break;
                case 0x22cd:   // [invoke check time] → m_i60
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        info.m_i60 = v;
                    }
                    break;
                case 0x22d2:   // [invoke delay time] → m_i64
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        info.m_i64 = v;
                    }
                    break;
                case 0x22d7:   // [wait time] → m_i68
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        info.m_i68 = v;
                    }
                    break;
                case 0x22dc:   // [shake area] → 3 dword @+0x20..+0x28
                {
                    // ORIG 0x8a34c12 循环写 info+0x20+i*4（i=0..2，同上裸写）。
                    int* p = reinterpret_cast<int*>(
                        reinterpret_cast<char*>(&info) + 0x20);
                    for (int i = 0; i <= 2; ++i)
                    {
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            p[i] = v;
                        }
                    }
                    break;
                }
                case 0x22e4:   // [invoke time] → m_i6c
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        info.m_i6c = v;
                    }
                    break;
                case 0x22e9:   // [invoke count] → m_i70
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        info.m_i70 = v;
                    }
                    break;
                default:
                    return false;
                }
            }
            break;
        }
        case 0x22f6:   // [passive object type] → m_i270
        {
            ScanStr(&str);
            const int tv = LookupToken(s_lookupFairyType, s_lockFairyType,
                                       s_initedFairyType, s_fairyTypeEntries,
                                       sizeof(s_fairyTypeEntries) / sizeof(s_fairyTypeEntries[0]),
                                       str.c_str());
            if (tv == 0x22fd)   // [missile fairy]
            {
                script->m_i270 = 0;
            }
            else if (tv == 0x2301)   // [random item fairy]
            {
                script->m_i270 = 1;
            }
            else
            {
                return false;
            }
            break;
        }
        case 0x231a:   // [random item fairy] → m_itemFairies
        {
            bool active = true;
            RandomItemFairy fairy;
            for (;;)
            {
                if (!ScanType(token, true))
                {
                    break;
                }
                const int fv = LookupToken(s_lookupFairy, s_lockFairy,
                                           s_initedFairy, s_fairyEntries,
                                           sizeof(s_fairyEntries) / sizeof(s_fairyEntries[0]),
                                           token.c_str());
                switch (fv)
                {
                case 0x2325:   // [/random item fairy]
                    active = false;
                    break;
                case 0x232a:   // [entry]
                    fairy.clear();
                    break;
                case 0x232f:   // [/entry]
                    script->m_itemFairies.push_back(fairy);
                    break;
                case 0x2334:   // [passiveobject] → 4 int
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        fairy.m_i00 = v;
                    }
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        fairy.m_ints04[0] = v;
                    }
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        fairy.m_ints04[1] = v;
                    }
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        fairy.m_ints04[2] = v;
                    }
                    break;
                default:
                    return false;
                }
                if (!active)
                {
                    break;
                }
            }
            break;
        }
        case 0x234c:   // [passive object icon] → m_b280/m_s284/+0x288
            script->m_b280 = 1;
            ScanStr(&script->m_s284);
            // ORIG 0x8a35725 写 this+0x288（结构末 m_s284 之后的 int；重建按同偏移）。
            int v288;
            if (!ScanInt(&v288))
            {
                return false;
            }
            // ORIG 0x8a35725 写 this+0x288（结构末 m_s284 之后的 int；m_i288 具名）。
            script->m_i288 = v288;
            break;
        default:
            return false;
        }

        // 每 case 处理后的公共出口（ORIG 0x8a3577e）：bRepair 且 [name] 已读
        // 时提前结束；否则回到主 token 循环。
        if (bRepair && nameSet)
        {
            break;
        }
    }

    script->m_idx =
        reinterpret_cast<STScriptFileList*>(&g_stPassiveObjectScriptFileList)
            ->FindIndexByFullScan(path);
    return true;
}

// ===== _Z17GetSlicedIntervalPlS_S_S_lll —— ORIG 0x085732d8 =====
// 切片区间工具：arg6>0 时写 [0,arg6] 到 out1/out2（count=1）；
// 若 arg7+arg6 < arg5（总长），第二段 [arg7+arg6, arg5-(arg7+arg6)] 写
// out3/out4（count==0 时写 out1/out2），count 增 1。返回段数。
long GetSlicedInterval(long* out1, long* out2, long* out3, long* out4,
                       long total, long firstLen, long step)
{
    long count = 0;
    if (firstLen > 0)
    {
        *out1 = 0;
        *out2 = firstLen;
        count = 1;
    }
    if (step + firstLen < total)
    {
        if (count == 0)
        {
            *out1 = step + firstLen;
            *out2 = total - (step + firstLen);
        }
        else
        {
            *out3 = step + firstLen;
            *out4 = total - (step + firstLen);
        }
        ++count;
    }
    return count;
}

// ===== _Z24CheckDayScheduleTimeOveril —— ORIG 0x0846c370 =====
// time(NULL) + day * (-86400) > checkTime（0xfffeae80 == -86400 补码）。
bool CheckDayScheduleTimeOver(int day, long checkTime)
{
    const long now = static_cast<long>(time(0));
    return now + static_cast<long>(day) * (-86400L) > checkTime;
}

// ===== _Z29DecreaseTradeLimitCountByUserR5CUserii —— ORIG 0x0812017d =====
// curCharac 缺失 → false；GetInvenRef(space, slot) 缺失 → false；
// 否则委托 DecreaseTradeLimitCount(Inven_Item&)。
bool DecreaseTradeLimitCountByUser(CUser& user, int space, int slot)
{
    if (user.getCurCharacR() == 0)
    {
        return false;
    }
    CInventory* inven = user.getCurCharacInvenW();
    Inven_Item* item = inven->GetInvenRef(space, slot);
    if (item == 0)
    {
        return false;
    }
    return DecreaseTradeLimitCount(*item);
}

// ===== _Z18GiveItemReturnUserP5CUser —— ORIG 0x084bfd28 =====
// find_item(0x1f15) 失败直接返回；make_item 填局部 Inven_Item、set_add_info(1)。
// 按最后游玩 dungeon 是否存在分两路拼邮件：正文
// findString(4,"return_user_msg_1",0) + snprintf("%s", dungeonName) 或
// findString(4,"return_user_msg_2",0)；发件人 findString(4,"return_user_msg_title",0)；
// ReqDBSendNewSystemMail(title, item, 0, characNo, body, strlen(body), 7,
//                        serverGroup, false, false)。
void GiveItemReturnUser(CUser* user)
{
    Inven_Item item;
    const int itemIdx = 0x1f15;
    CItem* def = G_CDataManager()->find_item(itemIdx);
    if (def == 0)
    {
        return;
    }
    item.m_addInfo = itemIdx;
    def->make_item(item);
    item.set_add_info(1);

    const int dungeonIdx = user->getCurCharacLastPlayDungeonIndex();
    if (dungeonIdx != 0)
    {
        CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
        if (dungeon != 0)
        {
            char buf[0x100];
            OS_API::snprintf(
                buf, 0xff,
                g_scriptStringManager_.findString(4, "return_user_msg_1", 0),
                dungeon->GetDungeonName());
            const char* title =
                g_scriptStringManager_.findString(4, "return_user_msg_title", 0);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, item, 0, user->getCurCharacNo(), buf,
                static_cast<unsigned int>(strlen(buf)), 7,
                user->GetServerGroup(), false, false);
        }
    }
    else
    {
        const char* body =
            g_scriptStringManager_.findString(4, "return_user_msg_2", 0);
        const char* title =
            g_scriptStringManager_.findString(4, "return_user_msg_title", 0);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, item, 0, user->getCurCharacNo(), body,
            static_cast<unsigned int>(strlen(body)), 7,
            user->GetServerGroup(), false, false);
    }
}

// ===== _Z38GetInstanceDeathTowerPlaydataStatisticv —— ORIG 0x0860f451 =====
// ORIG：mov $0x94733a8 直返（.bss 静态实例，无 guard）。重建以文件级
// static 对象承载同一地址语义。
CDeathTowerPlayDataStatistic* GetInstanceDeathTowerPlaydataStatistic()
{
    static CDeathTowerPlayDataStatistic s_instance;
    return &s_instance;
}
