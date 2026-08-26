// GameSecurity.cpp —— 包安全自由函数实现（PacketDispatcher 叶子批次）。
// 逐指令转写自 ORIG df_game_r（objdump -d）：
//   DenyPacket(int)                          ORIG T 0x859092a (0x104)
//   _NS_PI_Check_CMD(...)                    ORIG T 0x8274778 (0x1fd)
//   _NS_PI_2ND_MakeChecksumTo1Byte2(...)     ORIG T 0x808cc4b (0x1f)

#include "PacketSecurity.h"

#include "PacketBuf.h"
#include "CUser.h"      // WongWork::ENUM_HACKTYPE / CHackAnalyzer / SetETC / 组队判定
#include "CParty.h"     // CParty::get_member_count（组队误判豁免）
#include "md5.h"        // md5_context/md5_starts/md5_update/md5_finish（_NS_MakeHash_）

#include <map>
#include <utility>
#include <time.h>
#include <string.h>

// ---------------------------------------------------------------------------
// PI 安全域函数（ORIG 符号经 nm 核实；真实定义在本 TU 后段）：
//   bool _NS_MakeHash_(char*, short, int&)
//       _Z13_NS_MakeHash_PcsRi            T 0x808c8b5（报告 _lt_global_gt_/_NS_MakeHash_.md）
//   void _NS_MakeHash_(char*, unsigned short, unsigned int&)
//       _Z13_NS_MakeHash_PctRj            T 0x0808c976（报告 _NS_MakeHash__0808c976.md，薄包装）
//   char PI_GetType_CS(char, short)
//       _Z13PI_GetType_CScs               T 0x808c5d8（报告 _lt_global_gt_/PI_GetType_CS.md）
//   void PIAddServerHackCnt(CUser*, WongWork::ENUM_HACKTYPE, int)
//       _Z18PIAddServerHackCntP5CUserN8WongWork13ENUM_HACKTYPEEi  W 0x82772b7
//   int PI_GetHackTypes(short)
//       _Z14PI_GetHackTypes               W 0x82758e3
//       注意：ORIG 符号不带参数编码（非 _Z15..v），但函数体读取调用方压栈的
//       short（0x8(%ebp)，0x82758e9 处 mov 0x8(%ebp),%eax → jump table）。
//       用 asm 标签把带 short 形参的声明指到 ORIG 原始符号，保持压参行为。
bool _NS_MakeHash_(char* data, short len, int& out);
char PI_GetType_CS(char type, short cmd);
int PI_GetHackTypes(short hackType) __asm__("_Z14PI_GetHackTypes");
void PIAddServerHackCnt(CUser* user, WongWork::ENUM_HACKTYPE type, int count);
int _NS_PI_2ND_GetHackType(unsigned int type, unsigned char b, unsigned short c);


// ---------------------------------------------------------------------------
// DenyPacket  ORIG T 0x859092a (0x104)
// 纯谓词：id 命中封禁集合返回 true。二分比较树结构与 ORIG 一致：
//   true 集合 = {0x05..0x06, 0x1a..0x1b, 0x1d, 0x42, 0x44, 0x53..0x54,
//                0x61, 0x66, 0x69, 0x8a, 0xac, 0xb9..0xc1, 0xcc..0xcd,
//                0xe1, 0xee, 0x100, 0x104, 0x107, 0x10f..0x111,
//                0x120, 0x13c, 0x145}
// ---------------------------------------------------------------------------
bool DenyPacket(int id)
{
    if (id <= 0xac)
    {
        if (id <= 0x44)
        {
            if (id <= 0x1b)
            {
                if (id >= 0x1a)  // 0x1a..0x1b
                    return true;
                // sub $0x5 / cmp $0x1 / ja → 0x05..0x06
                return static_cast<unsigned int>(id - 0x5) <= 0x1u;
            }
            return id == 0x1d || id == 0x42;
        }
        if (id <= 0x61)
        {
            if (id == 0x61)
                return true;
            // sub $0x53 / cmp $0x1 / ja → 0x53..0x54
            return static_cast<unsigned int>(id - 0x53) <= 0x1u;
        }
        return id == 0x69 || id == 0x8a || id == 0x66;
    }

    if (id <= 0x100)
    {
        if (id <= 0xcd)
        {
            if (id >= 0xcc)  // 0xcc..0xcd
                return true;
            // sub $0xb9 / cmp $0x8 / ja → 0xb9..0xc1
            return static_cast<unsigned int>(id - 0xb9) <= 0x8u;
        }
        return id == 0xe1 || id == 0xee;
    }

    if (id <= 0x111)
    {
        if (id >= 0x10f)  // 0x10f..0x111
            return true;
        return id == 0x104 || id == 0x107;
    }
    return id == 0x13c || id == 0x145 || id == 0x120;
}

// ---------------------------------------------------------------------------
// _NS_PI_2ND_MakeChecksumTo1Byte2  ORIG T 0x808cc4b (0x1f)
// ORIG：*a = movzbl(*a)；*b = movzbl(*b)（各截低 8 位回写 int）。
// ---------------------------------------------------------------------------
void _NS_PI_2ND_MakeChecksumTo1Byte2(unsigned int& checksum1,
                                     unsigned int& checksum2)
{
    checksum1 = static_cast<unsigned int>(static_cast<unsigned char>(checksum1));
    checksum2 = static_cast<unsigned int>(static_cast<unsigned char>(checksum2));
}

// ---------------------------------------------------------------------------
// _NS_PI_Check_CMD  ORIG T 0x8274778 (0x1fd)
// 命令级包完整性校验：
//   1) packetClass 0/1/2 → 类型字节 t（ORIG: 0→2, 1→1, 其余→0）；
//   2) PI_GetType_CS(t, cmd) 为 0 → 放行返回；
//   3) data = get_packet(0)+0xd，idx = get_index()-0xd（包体区）；
//   4) get_int(expected)（返回值忽略）；
//   5) PICS map<pair<char,char>,int>（CUser::GetPICSMap）：无该 key 先置 0，
//      计数 >0x64 → 放行返回；
//   6) _NS_MakeHash_(data, idx, hash) 失败 → 返回；hash==expected → 放行；
//   7) 否则计数 +1，penalty = (t%10)*10+0xc9（ORIG 魔数除法 0x67/5 序列），
//      PIAddServerHackCnt(user, PI_GetHackTypes(cmdType), penalty)。
// ---------------------------------------------------------------------------
void _NS_PI_Check_CMD(ENUM_PACKETCLASS packetClass, unsigned short cmd,
                      PacketBuf& packet, CUser* user)
{
    if (user == 0)
        return;

    char type = 0;  // -0x29(%ebp)
    if (packetClass == 0)
        type = 2;
    else if (packetClass == 1)
        type = 1;

    char cmdType = PI_GetType_CS(type, static_cast<short>(cmd));  // -0x2a(%ebp)
    if (cmdType == 0)
        return;

    char* data = packet.get_packet(0) + 0xd;                        // -0x20
    short idx = static_cast<short>(packet.get_index() - 0xd);       // -0x1a

    int expected = 0;  // -0x30
    packet.get_int(expected);

    std::pair<char, char> key = std::make_pair(type, cmdType);      // -0x32

    std::map<std::pair<char, char>, int>* pics =
        (std::map<std::pair<char, char>, int>*)user->GetPICSMap();  // -0x18

    if (pics->find(key) == pics->end())
        (*pics)[key] = 0;

    int& cnt = (*pics)[key];  // -0x14
    if (cnt > 0x64)
        return;

    int hash = 0;  // -0x38
    if (!_NS_MakeHash_(data, idx, hash))
        return;

    if (hash == expected)
        return;

    ++cnt;

    // ORIG 0x82748fb：movzbl t → imul $0x67 魔数除 5 取余 → r*10 + 0xc9。
    int penalty = static_cast<int>(type % 10) * 10 + 0xc9;  // -0x10
    int hackType = PI_GetHackTypes(static_cast<short>(cmdType));  // -0xc
    PIAddServerHackCnt(user,
                       static_cast<WongWork::ENUM_HACKTYPE>(hackType),
                       penalty);
}

// ===========================================================================
// _NS_MakeHash_  ORIG T 0x808c8b5（报告 _lt_global_gt_/_NS_MakeHash_.md）
// len 越界（<=0 或 >0x190）返回 false；否则 MD5 后按 ORIG 逐字节 xor 折叠：
//   d[0] ^= d[8]^d[10]^d[13] ^ 0x81   d[1] ^= d[5]^d[9]^d[12]  ^ 0x78
//   d[2] ^= d[4]^d[6]^d[14]  ^ 0x1a   d[3] ^= d[7]^d[11]^d[15] ^ 0xbf
// 结果以小端 dword 写 *out（ORIG mov (%eax),%edx 读 4 字节）。
// ===========================================================================
bool _NS_MakeHash_(char* data, short len, int& out)
{
    if (len <= 0 || len > 0x190)
        return false;

    unsigned char digest[16];
    md5_context ctx;
    md5_starts(&ctx);
    md5_update(&ctx, reinterpret_cast<unsigned char*>(data), len);
    md5_finish(&ctx, digest);

    digest[0] = static_cast<unsigned char>(digest[0] ^ digest[8] ^ digest[10] ^
                                           digest[13] ^ 0x81);
    digest[1] = static_cast<unsigned char>(digest[1] ^ digest[5] ^ digest[9] ^
                                           digest[12] ^ 0x78);
    digest[2] = static_cast<unsigned char>(digest[2] ^ digest[4] ^ digest[6] ^
                                           digest[14] ^ 0x1a);
    digest[3] = static_cast<unsigned char>(digest[3] ^ digest[7] ^ digest[11] ^
                                           digest[15] ^ 0xbf);

    out = static_cast<int>(digest[0]) | (static_cast<int>(digest[1]) << 8) |
          (static_cast<int>(digest[2]) << 16) | (static_cast<int>(digest[3]) << 24);
    return true;
}

// ---------------------------------------------------------------------------
// _NS_MakeHash_  ORIG T 0x0808c976（报告 _NS_MakeHash__0808c976.md）
// unsigned 形参薄包装：len 经 cwtl 符号扩展为 short 转调 int& 版本。
// ---------------------------------------------------------------------------
void _NS_MakeHash_(char* data, unsigned short len, unsigned int& out)
{
    _NS_MakeHash_(data, static_cast<short>(len),
                  reinterpret_cast<int&>(out));
}

// ===========================================================================
// PI_GetType_CS  ORIG T 0x808c5d8（报告 _lt_global_gt_/PI_GetType_CS.md）
// type!=1 → 0；type==1 时按 cmd 查表（未命中 0）。
// ===========================================================================
char PI_GetType_CS(char type, short cmd)
{
    if (type != 1)
        return 0;

    switch (cmd)
    {
        case 0x11b: return 1;
        case 0x11c: return 2;
        case 0x11e: return 3;
        case 0x1f:  return 10;
        case 0x91:  return 0x21;
        case 0x29:  return 0x23;
        case 0x108: return 0x24;
        case 0x1e:  return 0x25;
        case 0x127: return 0x26;
        case 0x126: return 0x28;
        case 0x13f: return 0x29;
        case 0x140: return 0x2a;
        case 0x2a:  return 0x2b;
        case 0x31:  return 0x2d;
        case 0x152: return 0x2e;
        case 0x16f: return 0x31;
        case 0x14e: return 0x32;
        default:    return 0;
    }
}

// ===========================================================================
// PI_GetHackTypes  ORIG W 0x82758e3
// jump table（.rodata 0x8bff0b4，55 项）逐项转写：idx 0→0x514；
// 1..9→0x515..0x51d；10→0x54a；11..52→0x51e..0x547；53→0x548；54→0x549；
// 越界/负值（movswl 符号扩展后 ja）→ 0x514。
// ===========================================================================
int PI_GetHackTypes(short hackType)
{
    int v = hackType;
    if (static_cast<unsigned int>(v) > 0x36u)
        return 0x514;

    switch (v)
    {
        case 0:  return 0x514;
        case 1:  return 0x515;
        case 2:  return 0x516;
        case 3:  return 0x517;
        case 4:  return 0x518;
        case 5:  return 0x519;
        case 6:  return 0x51a;
        case 7:  return 0x51b;
        case 8:  return 0x51c;
        case 9:  return 0x51d;
        case 10: return 0x54a;
        case 11: return 0x51e;
        case 12: return 0x51f;
        case 13: return 0x520;
        case 14: return 0x521;
        case 15: return 0x522;
        case 16: return 0x523;
        case 17: return 0x524;
        case 18: return 0x525;
        case 19: return 0x526;
        case 20: return 0x527;
        case 21: return 0x528;
        case 22: return 0x529;
        case 23: return 0x52a;
        case 24: return 0x52b;
        case 25: return 0x52c;
        case 26: return 0x52d;
        case 27: return 0x52e;
        case 28: return 0x52f;
        case 29: return 0x530;
        case 30: return 0x531;
        case 31: return 0x532;
        case 32: return 0x533;
        case 33: return 0x534;
        case 34: return 0x535;
        case 35: return 0x536;
        case 36: return 0x537;
        case 37: return 0x538;
        case 38: return 0x539;
        case 39: return 0x53a;
        case 40: return 0x53b;
        case 41: return 0x53c;
        case 42: return 0x53d;
        case 43: return 0x53e;
        case 44: return 0x53f;
        case 45: return 0x540;
        case 46: return 0x541;
        case 47: return 0x542;
        case 48: return 0x543;
        case 49: return 0x544;
        case 50: return 0x545;
        case 51: return 0x546;
        case 52: return 0x547;
        case 53: return 0x548;
        case 54: return 0x549;
        default: return 0x514;
    }
}

// ===========================================================================
// PIAddServerHackCnt  ORIG W 0x082772b7
// type==0x524 且在组队（CParty 成员数>1）时跳过（防组队场景误判）；
// 否则 SetETC((short)type, count) 记 ETC 并把回写值交给 CHackAnalyzer：
//   addServerHackCnt(user, type, 1, etcValue, 0)
// ===========================================================================
void PIAddServerHackCnt(CUser* user, WongWork::ENUM_HACKTYPE type, int count)
{
    if (static_cast<int>(type) == 0x524 && user->CheckInParty())
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0 && party->get_member_count() > 1)
        {
            return;
        }
    }
    int etcValue = user->SetETC(static_cast<short>(type), count);
    user->getHackAnalyzer()->addServerHackCnt(user, type, 1u,
        static_cast<unsigned int>(etcValue), 0u);
}
// ===========================================================================
// HP_GetHackType  ORIG T 0x82756cf（jump table .rodata 0x8bfefc8，41 项）
// type<=0x29 查表；表项 idx 0/越界 → 0x384；idx 1..9 → 0x385..0x38d；
// idx 10..40 → 0x578..0x596。
// ===========================================================================
int HP_GetHackType(unsigned char type)
{
    switch (type)
    {
        case 0:   return 0x384;
        case 1:   return 0x385;
        case 2:   return 0x386;
        case 3:   return 0x387;
        case 4:   return 0x388;
        case 5:   return 0x389;
        case 6:   return 0x38a;
        case 7:   return 0x38b;
        case 8:   return 0x38c;
        case 9:   return 0x38d;
        case 10:  return 0x578;
        case 11:  return 0x579;
        case 12:  return 0x57a;
        case 13:  return 0x57b;
        case 14:  return 0x57c;
        case 15:  return 0x57d;
        case 16:  return 0x57e;
        case 17:  return 0x57f;
        case 18:  return 0x580;
        case 19:  return 0x581;
        case 20:  return 0x582;
        case 21:  return 0x583;
        case 22:  return 0x584;
        case 23:  return 0x585;
        case 24:  return 0x586;
        case 25:  return 0x587;
        case 26:  return 0x588;
        case 27:  return 0x589;
        case 28:  return 0x58a;
        case 29:  return 0x58b;
        case 30:  return 0x58c;
        case 31:  return 0x58d;
        case 32:  return 0x58e;
        case 33:  return 0x58f;
        case 34:  return 0x590;
        case 35:  return 0x591;
        case 36:  return 0x592;
        case 37:  return 0x593;
        case 38:  return 0x594;
        case 39:  return 0x595;
        case 40:  return 0x596;
        default:  return 0x384;
    }
}

// ===========================================================================
// HIP_GetHackType  ORIG T 0x827585c（jump table .rodata 0x8bff070，17 项）
// type<=0x10 查表：idx 0→0x5dc；1..4→0x5dd..0x5e0；5→0x5dc；6..8→0x5e2..0x5e4；
// 9..11→0x641..0x643；12→0x5dc；13..15→0x646..0x648；16→0x679；越界→0x5dc。
// ===========================================================================
int HIP_GetHackType(unsigned char type)
{
    switch (type)
    {
        case 0:  return 0x5dc;
        case 1:  return 0x5dd;
        case 2:  return 0x5de;
        case 3:  return 0x5df;
        case 4:  return 0x5e0;
        case 5:  return 0x5dc;
        case 6:  return 0x5e2;
        case 7:  return 0x5e3;
        case 8:  return 0x5e4;
        case 9:  return 0x641;
        case 10: return 0x642;
        case 11: return 0x643;
        case 12: return 0x5dc;
        case 13: return 0x646;
        case 14: return 0x647;
        case 15: return 0x648;
        case 16: return 0x679;
        default: return 0x5dc;
    }
}

// ===========================================================================
// MI_GetHackType  ORIG T 0x8274ad7：value = (signed char)c + 0x6a4，
// 上限 0x707（jle 保持）。
// ===========================================================================
int MI_GetHackType(char c)
{
    int v = static_cast<int>(static_cast<signed char>(c)) + 0x6a4;
    if (v > 0x707)
        v = 0x707;
    return v;
}

// ===========================================================================
// _NS_IPScan  ORIG T 0x8274676：读 1 字节 → HIP_GetHackType →
// addServerHackCnt(user, type, 1, 0, 0)。读取失败返回 1。
// ===========================================================================
int _NS_IPScan(CUser* user, PacketBuf& packet)
{
    unsigned char a = 0;
    if (!packet.get_byte(a))
        return 1;

    int hackType = HIP_GetHackType(a);
    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(hackType), 1, 0, 0);
    return 0;
}

// ===========================================================================
// _NS_ProcessScan  ORIG T 0x8274523：读 3 字节（b%10 + c%10*10 编码值）→
// HP_GetHackType(a) → SetETC → addServerHackCnt(user, type, 1, etc, 0)。
// 各步读取失败依次返回 1/2/3。
// ===========================================================================
int _NS_ProcessScan(CUser* user, PacketBuf& packet)
{
    char a = 0, b = 0, c = 0;
    if (!packet.get_byte(a))
        return 1;
    if (!packet.get_byte(b))
        return 2;
    if (!packet.get_byte(c))
        return 3;

    int value = static_cast<int>(b % 10) + static_cast<int>(c % 10) * 10;
    int hackType = HP_GetHackType(static_cast<unsigned char>(a));
    int etc = user->SetETC(static_cast<short>(hackType), value);
    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(hackType), 1,
        static_cast<unsigned int>(etc), 0);
    return 0;
}

// ===========================================================================
// _NS_PacketIntegrity  ORIG T 0x8274975：读 4 字节，编码值
// d%5 + b%5*10 + a%5*100 → PI_GetHackTypes(c) → PIAddServerHackCnt。
// user 为空返回 4；各步读取失败依次返回 1/2/3/4。
// ===========================================================================
int _NS_PacketIntegrity(CUser* user, PacketBuf& packet)
{
    unsigned char a = 0, b = 0, c = 0, d = 0;
    if (!packet.get_byte(a))
        return 1;
    if (!packet.get_byte(b))
        return 2;
    if (!packet.get_byte(c))
        return 3;
    if (!packet.get_byte(d))
        return 4;
    if (user == 0)
        return 4;

    int value = static_cast<int>(d % 5) + static_cast<int>(b % 5) * 10 +
                static_cast<int>(a % 5) * 100;
    int hackType = PI_GetHackTypes(static_cast<short>(c));
    PIAddServerHackCnt(user, static_cast<WongWork::ENUM_HACKTYPE>(hackType),
                       value);
    return 0;
}

// ===========================================================================
// _NS_MI_RecvMemoryIntegrityModulation  ORIG T 0x8274b09：读 2 字节 →
// MI_GetHackType(a) → SetETC(type, (signed char)b) → addServerHackCnt。
// 读取失败依次返回 1/2。
// ===========================================================================
int _NS_MI_RecvMemoryIntegrityModulation(CUser* user, PacketBuf& packet)
{
    char a = 0, b = 0;
    if (!packet.get_byte(a))
        return 1;
    if (!packet.get_byte(b))
        return 2;

    int hackType = MI_GetHackType(a);
    int etc = user->SetETC(static_cast<short>(hackType),
                           static_cast<int>(static_cast<signed char>(b)));
    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(hackType), 1,
        static_cast<unsigned int>(etc), 0);
    return 0;
}

// ===========================================================================
// _NS_NonClient_GetResponse  ORIG T 0x8274cc3：置 NonClientFlag；读随机数+结果
// 字节。结果为 0 时校验 user 随机数的 _NS_MakeHash_(4 字节)，不匹配 → hack
// 0x33b；结果 1 → hack 0x33d；其它 → hack 0x33e。user 空返回 0；读取失败
// 返回 1/2。
// ===========================================================================
int _NS_NonClient_GetResponse(CUser* user, PacketBuf& packet)
{
    if (user == 0)
        return 0;

    user->SetNonClientFlag(true);
    int randInt = 0;
    if (!packet.get_int(randInt))
        return 1;
    char result = 0;
    if (!packet.get_byte(result))
        return 2;

    if (result == 0)
    {
        int local = user->GetNonClientRandInt();
        int hash = 0;
        if (!_NS_MakeHash_(reinterpret_cast<char*>(&local), 4, hash))
            return 0;
        if (hash != randInt)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, static_cast<WongWork::ENUM_HACKTYPE>(0x33b), 1, 0, 0);
        }
    }
    else if (result == 1)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, static_cast<WongWork::ENUM_HACKTYPE>(0x33d), 1, 0, 0);
    }
    else
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, static_cast<WongWork::ENUM_HACKTYPE>(0x33e), 1, 0, 0);
    }
    return 0;
}

// ===========================================================================
// _NS_PI_2ND_RecvPacketIntegrityModulation_C2C  ORIG T 0x8275290：读 ushort，
// v = v + 0xe11；v > 0xf9e 则 0xf9f；addServerHackCnt(user, v, 1, 0, 0)。
// ===========================================================================
int _NS_PI_2ND_RecvPacketIntegrityModulation_C2C(CUser* user,
                                                 PacketBuf& packet)
{
    unsigned short a = 0;
    if (!packet.get_short(a))
        return 1;

    int v = static_cast<int>(a) + 0xe11;
    if (v > 0xf9e)
        v = 0xf9f;
    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(v), 1, 0, 0);
    return 0;
}

// ===========================================================================
// _NS_PI_2ND_RecvPacketIntegrityModulation_S2C  ORIG T 0x8275140：读 byte+short
// → _NS_PI_2ND_GetHackType(1, a, b) → addServerHackCnt(user, type, 1, 0, 0)。
// ===========================================================================
int _NS_PI_2ND_RecvPacketIntegrityModulation_S2C(CUser* user,
                                                 PacketBuf& packet)
{
    unsigned char a = 0;
    unsigned short b = 0;
    if (!packet.get_byte(a))
        return 1;
    if (!packet.get_short(b))
        return 2;

    int hackType = _NS_PI_2ND_GetHackType(1, a, b);
    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(hackType), 1, 0, 0);
    return 0;
}

// ===========================================================================
// checkNewMemberHacklog  ORIG T 0x8275315：hackType 编码为 YYYYMMDD。
// 与当前时间差（天）：<0 / ==0 → hack 0x134；1..3 → 0x135；4..7 → 0x136；
// 8..0x1e → 0x137；其它不记。SetETC(type, p3) 后 addServerHackCnt
// (user, type, 1, etc, hackType)。
// ===========================================================================
void checkNewMemberHacklog(unsigned int hackType, CUser* user,
                           unsigned int p3)
{
    if (user == 0)
        return;

    struct tm tmv;
    memset(&tmv, 0, sizeof(tmv));
    tmv.tm_year = static_cast<int>(hackType / 10000) - 0x76c;
    tmv.tm_mon = static_cast<int>((hackType / 100) % 100) - 1;
    tmv.tm_mday = static_cast<int>(hackType % 100);

    time_t t = mktime(&tmv);
    time_t now = time(0);
    int days = static_cast<int>(difftime(static_cast<double>(now),
                                         static_cast<double>(t)) / 86400.0);

    int hackId = -1;
    if (days < 0)
        hackId = 0x134;
    else if (days == 0)
        hackId = 0x134;
    else if (days <= 3)
        hackId = 0x135;
    else if (days <= 7)
        hackId = 0x136;
    else if (days <= 0x1e)
        hackId = 0x137;

    if (hackId < 0)
        return;

    int etc = user->SetETC(static_cast<short>(hackId),
                           static_cast<int>(p3));
    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(hackId), 1,
        static_cast<unsigned int>(etc), hackType);
}

// ===========================================================================
// _NS_PI_2ND_GetHackTypeEnum  ORIG T 0x8275af3
// type==1 走 cmd 表（<=0x25e，.rodata 0x8bff190 逐项转写）；type==0 走
// cmd0 表（<=0x232，.rodata 0x8bffb0c 逐项转写）；其它类型返回 0。
// ===========================================================================
static const unsigned short s_pi2ndHackTypeEnumCmd[607] = {
    0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc,
    0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24,
    0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c,
    0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54,
    0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x0,
    0x71, 0x0, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83,
    0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x0, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x0, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b,
    0x9c, 0x9d, 0x9e, 0x9f, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8,
    0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0x0, 0x0, 0x0, 0xc0,
    0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc,
    0xcd, 0xce, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0x0,
    0xdb, 0xdc, 0x0, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0x0, 0x0,
    0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0x0, 0x0, 0xf0, 0xf1, 0xf2,
    0x0, 0x0, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0x0, 0xfd, 0xfe,
    0xff, 0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108, 0x0, 0x10a,
    0x10b, 0x10c, 0x10d, 0x10e, 0x10f, 0x110, 0x111, 0x112, 0x113, 0x114, 0x115, 0x0,
    0x117, 0x118, 0x119, 0x11a, 0x11b, 0x11c, 0x0, 0x11e, 0x11f, 0x120, 0x121, 0x122,
    0x123, 0x0, 0x0, 0x0, 0x0, 0x0, 0x129, 0x12a, 0x12b, 0x0, 0x12d, 0x12e,
    0x12f, 0x130, 0x131, 0x132, 0x133, 0x0, 0x135, 0x136, 0x137, 0x138, 0x139, 0x13a,
    0x13b, 0x13c, 0x0, 0x0, 0x0, 0x0, 0x142, 0x143, 0x144, 0x145, 0x146, 0x147,
    0x148, 0x149, 0x14a, 0x14b, 0x14c, 0x14d, 0x14e, 0x14f, 0x150, 0x0, 0x0, 0x0,
    0x0, 0x151, 0x152, 0x0, 0x0, 0x155, 0x156, 0x157, 0x158, 0x159, 0x15a, 0x15b,
    0x15c, 0x0, 0x15e, 0x15f, 0x160, 0x161, 0x162, 0x163, 0x164, 0x0, 0x0, 0x167,
    0x168, 0x169, 0x16a, 0x16b, 0x16c, 0x16d, 0x0, 0x0, 0x170, 0x0, 0x172, 0x0,
    0x0, 0x175, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x176, 0x177, 0x178, 0x17a,
};
static const unsigned short s_pi2ndHackTypeEnumCmd0[563] = {
    0x17b, 0x17c, 0x17d, 0x17e, 0x17f, 0x180, 0x181, 0x182, 0x183, 0x184, 0x185, 0x186,
    0x187, 0x188, 0x189, 0x18a, 0x18b, 0x18c, 0x18d, 0x18e, 0x18f, 0x190, 0x191, 0x192,
    0x193, 0x194, 0x195, 0x196, 0x197, 0x198, 0x199, 0x19a, 0x19b, 0x19c, 0x19d, 0x19e,
    0x19f, 0x1a0, 0x1a1, 0x1a2, 0x1a3, 0x1a4, 0x1a5, 0x1a6, 0x1a7, 0x1a8, 0x1a9, 0x1aa,
    0x1ab, 0x1ac, 0x1ad, 0x1ae, 0x1af, 0x1b0, 0x1b1, 0x1b2, 0x1b3, 0x1b4, 0x1b5, 0x1b6,
    0x1b7, 0x1b8, 0x1b9, 0x1ba, 0x1bb, 0x1bc, 0x1bd, 0x1be, 0x1bf, 0x1c0, 0x1c1, 0x1c2,
    0x1c3, 0x1c4, 0x1c5, 0x1c6, 0x1c7, 0x1c8, 0x1c9, 0x1ca, 0x1cb, 0x1cc, 0x1cd, 0x1ce,
    0x1cf, 0x1d0, 0x1d1, 0x1d2, 0x1d3, 0x1d4, 0x1d5, 0x1d6, 0x1d7, 0x1d8, 0x1d9, 0x1da,
    0x1db, 0x1dc, 0x1dd, 0x1de, 0x1df, 0x1e0, 0x1e1, 0x1e2, 0x1e3, 0x1e4, 0x1e5, 0x1e6,
    0x1f3, 0x1f4, 0x1f5, 0x1f6, 0x1f7, 0x1f8, 0x1f9, 0x1fa, 0x0, 0x1fc, 0x1fd, 0x1fe,
    0x1ff, 0x200, 0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207, 0x208, 0x209, 0x20a,
    0x20b, 0x20c, 0x20d, 0x20e, 0x20f, 0x210, 0x211, 0x212, 0x213, 0x214, 0x215, 0x216,
    0x217, 0x218, 0x219, 0x21a, 0x21b, 0x21c, 0x21d, 0x21e, 0x21f, 0x220, 0x0, 0x222,
    0x223, 0x224, 0x225, 0x226, 0x227, 0x228, 0x229, 0x22a, 0x0, 0x22c, 0x22d, 0x0,
    0x22f, 0x230, 0x232, 0x233, 0x234, 0x236, 0x237, 0x238, 0x239, 0x23a, 0x23b, 0x23c,
    0x23d, 0x23e, 0x23f, 0x240, 0x0, 0x0, 0x243, 0x0, 0x245, 0x246, 0x247, 0x248,
    0x249, 0x0, 0x24b, 0x24c, 0x0, 0x24e, 0x0, 0x250, 0x251, 0x252, 0x253, 0x254,
    0x0, 0x256, 0x257, 0x258, 0x259, 0x25a, 0x25b, 0x25c, 0x25d, 0x25e, 0x25f, 0x260,
    0x261, 0x262, 0x263, 0x264, 0x265, 0x266, 0x0, 0x268, 0x269, 0x26a, 0x26b, 0x26c,
    0x26d, 0x0, 0x26f, 0x270, 0x0, 0x272, 0x0, 0x0, 0x275, 0x276, 0x277, 0x278,
    0x279, 0x27a, 0x27b, 0x27c, 0x27d, 0x0, 0x27f, 0x280, 0x281, 0x282, 0x283, 0x284,
    0x285, 0x286, 0x287, 0x288, 0x289, 0x0, 0x0, 0x28c, 0x28d, 0x28e, 0x28f, 0x290,
    0x291, 0x292, 0x293, 0x294, 0x295, 0x296, 0x297, 0x298, 0x299, 0x29a, 0x0, 0x0,
    0x29d, 0x29e, 0x29f, 0x2a0, 0x2a1, 0x2a2, 0x2a3, 0x0, 0x0, 0x0, 0x0, 0x2a8,
    0x0, 0x2aa, 0x2ab, 0x2ac, 0x2ad, 0x2ae, 0x2af, 0x2b0, 0x2b1, 0x0, 0x2b3, 0x2b4,
    0x2b5, 0x2b6, 0x0, 0x0, 0x2b9, 0x2ba, 0x0, 0x0, 0x2bd, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2be, 0x2bf, 0x2c0,
};


int _NS_PI_2ND_GetHackTypeEnum(unsigned char type, short cmd)
{
    if (type == 1)
    {
        if (static_cast<unsigned int>(static_cast<int>(cmd)) > 0x25eu)
            return 0;
        return s_pi2ndHackTypeEnumCmd[cmd];
    }
    if (type == 0)
    {
        if (static_cast<unsigned int>(static_cast<int>(cmd)) > 0x232u)
            return 0;
        return s_pi2ndHackTypeEnumCmd0[cmd];
    }
    return 0;
}

// ===========================================================================
// _NS_PI_2ND_GetHackType  ORIG W 0x8277359：v = 2*(enum + 0x3e8) + type，
// 上限 0xe0f（jbe 保持）。
// ===========================================================================
int _NS_PI_2ND_GetHackType(unsigned int type, unsigned char b, unsigned short c)
{
    int v = 2 * (_NS_PI_2ND_GetHackTypeEnum(b, static_cast<short>(c)) + 0x3e8)
            + static_cast<int>(type);
    if (v > 0xe0e)
        return 0xe0f;
    return v;
}
