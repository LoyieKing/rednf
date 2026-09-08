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
//   char _NS_MakeHash_(char*, short, int&)
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
// 返回类型 char（非 bool/int）：ORIG 调用点 _NS_PI_MakeHash_NOTI(0x82746f3) 为
// `xor $0x1,%eax; test %al,%al; jne` —— 只测低字节，char 值域的 ==1 判据；
// ORIG 本体(0x0808c8b5) 两条返回 `mov $0x0,%eax`/`mov $0x1,%eax` 严格 0/1。
// mangled 不含返回类型 ⇒ ABI 无影响（Main 授权，batch-23 S 线）。
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
        if (id == 0x100)
            return true;
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
// len 越界（<=0 或 >0x190）返回 0；否则一步 md5() 后按 ORIG 逐字节 xor 折叠：
//   d[0] ^= d[8]^d[10]^d[13] ^ 0x81   d[1] ^= d[5]^d[9]^d[12]  ^ 0x78
//   d[2] ^= d[4]^d[6]^d[14]  ^ 0x1a   d[3] ^= d[7]^d[11]^d[15] ^ 0xbf
// 结果以小端 dword 写 *out（ORIG mov (%eax),%edx 读 4 字节）。
// ===========================================================================
// [b23S3C] ORIG 尾 `mov $0x1,%eax; leave; ret`+调用点 xor$0x1 直测低字节 ⇒ 返回 char；
// 体 ORIG 单 call _Z3md5PhiS_（0x80b3d57），勿展开 starts/update/finish 三 call。
bool _NS_MakeHash_(char* data, short len, int& out)
{
    if (len <= 0 || len > 0x190)
        return 0;

    unsigned char digest[16];
    md5(reinterpret_cast<unsigned char*>(data), len, digest);

    digest[0] = static_cast<unsigned char>(digest[0] ^ digest[8] ^ digest[10] ^
                                           digest[13] ^ 0x81);
    digest[1] = static_cast<unsigned char>(digest[1] ^ digest[5] ^ digest[9] ^
                                           digest[12] ^ 0x78);
    digest[2] = static_cast<unsigned char>(digest[2] ^ digest[4] ^ digest[6] ^
                                           digest[14] ^ 0x1a);
    digest[3] = static_cast<unsigned char>(digest[3] ^ digest[7] ^ digest[11] ^
                                           digest[15] ^ 0xbf);

    // ORIG lea digest,%eax; mov (%eax),%edx; mov %edx,(out) —— 小端 dword 直读，非逐字节 or。
    out = *reinterpret_cast<int*>(digest);
    return 1;
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
    switch (type)
    {
        case 1:
            switch (cmd)
            {
        case 0x11b: return 1;
        case 0x11c: return 2;
        case 0x11e: return 3;
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
                case 0x1f:  return 10;
            }
            break;
    }
    return 0;
}

// ===========================================================================
// PI_GetHackTypes  ORIG W 0x82758e3
// jump table（.rodata 0x8bff0b4，55 项）逐项转写：idx 0→0x514；
// 1..9→0x515..0x51d；10→0x54a；11..52→0x51e..0x547；53→0x548；54→0x549；
// 越界/负值（movswl 符号扩展后 ja）→ 0x514。
// ===========================================================================
int PI_GetHackTypes(short hackType)
{
    if (static_cast<unsigned int>(hackType) > 0x36u)
        return 0x514;

    switch (hackType)
    {
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
    if (static_cast<int>(type) == 0x524)
    {
        if (user->CheckInParty())
        {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            if (party->get_member_count() > 1)
                return;
        }
        }
    }
    user->getHackAnalyzer()->addServerHackCnt(user, type, 1u,
        static_cast<unsigned int>(user->SetETC(static_cast<short>(type), count)), 0u);
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
        // case 0 与 default 合并（ORIG 跳转表槽 0 直指 default 标签，单处 0x384）
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
        case 41:  return 0x597;
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
        case 1:  return 0x5dd;
        case 2:  return 0x5de;
        case 3:  return 0x5df;
        case 4:  return 0x5e0;
        case 6:  return 0x5e2;
        case 7:  return 0x5e3;
        case 8:  return 0x5e4;
        case 9:  return 0x641;
        case 10: return 0x642;
        case 11: return 0x643;
        case 13: return 0x646;
        case 14: return 0x647;
        case 15: return 0x648;
        case 16: return 0x679;
        default: return 0x5dc;
    }
}

// ===========================================================================
// MI_GetHackType  ORIG T 0x8274ad7：value = (unsigned char)c + 0x6a4，
// 上限 0x707（jle 保持）。
// ===========================================================================
int MI_GetHackType(char c)
{
    int v = (c & 0xff) + 0x6a4;
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

    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(HIP_GetHackType(a)), 1, 0, 0);
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
// [b28c-L23 N(P75 实参装载时刻) 残余 4 行] PIAddServerHackCnt 的 0x8/0x4(%esp)
// 两实参：ORIG @0x08274... 先载 value(-0x10)→%eax→0x8(%esp)，再载 hackType(-0xc)→
// %eax→0x4(%esp)（严格 R2L、同一寄存器复用）；4.4 因 hackType 是刚 call 完的结果
// 而先取 %eax、value 走 %edx（P75 跨版本装载时刻调度，压栈顺序/调用约定/语义全同）。
// 117=117 条数与助记符序列一致。
{
    unsigned char a, b, c, d;
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

    int value = d % 10 + (b % 10) * 10 + (a % 10) * 100;
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
    char a, b;
    if (!packet.get_byte(a))
        return 1;
    if (!packet.get_byte(b))
        return 2;

    unsigned int hackType = static_cast<unsigned int>(MI_GetHackType(a));
    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(hackType), 1,
        static_cast<unsigned int>(user->SetETC(static_cast<short>(hackType),
            static_cast<int>(static_cast<signed char>(b)))), 0);
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
    int randInt;
    if (!packet.get_int(randInt))
        return 1;
    char result;
    if (!packet.get_byte(result))
        return 2;

    switch (result)
    {
        case 0:
        {
            int local = user->GetNonClientRandInt();
            int hash;
            if (!_NS_MakeHash_(reinterpret_cast<char*>(&local), 4, hash))
                return 0;
            if (hash != randInt)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, static_cast<WongWork::ENUM_HACKTYPE>(0x33b), 1, 0, 0);
            }
            break;
        }
        case 1:
            user->getHackAnalyzer()->addServerHackCnt(
                user, static_cast<WongWork::ENUM_HACKTYPE>(0x33d), 1, 0, 0);
            break;
        default:
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
    unsigned short a;
    if (!packet.get_short(a))
        return 1;

    unsigned int v = a + 0xe11;
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
    unsigned char a;
    unsigned short b;
    if (!packet.get_byte(a))
        return 1;
    if (!packet.get_short(b))
        return 2;

    unsigned int hackType = static_cast<unsigned int>(_NS_PI_2ND_GetHackType(1, a, b));
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
// _NS_PI_2ND_GetHackTypeEnum  ORIG W 0x08275af3 (0x17b6)
// §6.2 巨件核销：真修——原 static 前缀表查找版 mangled 为 _ZL26…（32 条），
// ORIG 权威符号为 weak _Z26…（跳表展开 1230 条）。去 static 并按 ORIG 结构
// 全量重建：if(type==1)/else if(type==0) + 内层 switch(cmd)（报告缺失，
// nm 定址后 objdump -d 0x08275af3..0x082772a9 补证）。
//   · type==1 跳表 @0x8bff190（0x25f 项）、type==0 跳表 @0x8bffb0c（0x233 项）；
//   · case 按返回值分组（同值 cmd 共享叶子 mov$N;jmp）；
//   · 指向 default（0x082772a2 mov$0）的 285/282 个 cmd 不列 case——
//     ORIG 源码未写这些 case，与越界共用 return 0；
//   · cmd→枚举映射逐项对账一致。
// ===========================================================================
// ===========================================================================
// _NS_PI_2ND_GetHackType  ORIG W 0x8277359：v = 2*(enum + 0x3e8) + type，
// 上限 0xe0f（jbe 保持）。
// 【P210】定义前置：调用者须在 _NS_PI_2ND_GetHackTypeEnum 定义之前，
// 否则 4.4 按 callee 帧精确继承 outgoing，0x38 保守大帧塌缩为 0x20。
// ===========================================================================
int _NS_PI_2ND_GetHackTypeEnum(unsigned char type, short cmd);

int _NS_PI_2ND_GetHackType(unsigned int type, unsigned char b, unsigned short c)
{
    unsigned int e =
        static_cast<unsigned int>(_NS_PI_2ND_GetHackTypeEnum(b, static_cast<short>(c)));
    unsigned int v =
        2 * (e + 0x3e8u) +
        type;
    if (v > 0xe0e)
        return 0xe0f;
    return v;
}

int _NS_PI_2ND_GetHackTypeEnum(unsigned char type, short cmd)
{
    if (type == 1)
    {
        switch (cmd)
        {
            case 0x0:
                return 0x1;
            case 0x1:
                return 0x2;
            case 0x2:
                return 0x3;
            case 0x3:
                return 0x4;
            case 0x4:
                return 0x5;
            case 0x5:
                return 0x6;
            case 0x6:
                return 0x7;
            case 0x7:
                return 0x8;
            case 0x8:
                return 0x9;
            case 0x9:
                return 0xa;
            case 0xa:
                return 0xb;
            case 0xb:
                return 0xc;
            case 0xc:
                return 0xd;
            case 0xd:
                return 0xe;
            case 0xe:
                return 0xf;
            case 0xf:
                return 0x10;
            case 0x10:
                return 0x11;
            case 0x11:
                return 0x12;
            case 0x12:
                return 0x13;
            case 0x13:
                return 0x14;
            case 0x14:
                return 0x15;
            case 0x15:
                return 0x16;
            case 0x16:
                return 0x17;
            case 0x17:
                return 0x18;
            case 0x18:
                return 0x19;
            case 0x19:
                return 0x1a;
            case 0x1a:
                return 0x1b;
            case 0x1b:
                return 0x1c;
            case 0x1c:
                return 0x1d;
            case 0x1d:
                return 0x1e;
            case 0x1e:
                return 0x1f;
            case 0x1f:
                return 0x20;
            case 0x20:
                return 0x21;
            case 0x21:
                return 0x22;
            case 0x22:
                return 0x23;
            case 0x23:
                return 0x24;
            case 0x24:
                return 0x25;
            case 0x25:
                return 0x26;
            case 0x26:
                return 0x27;
            case 0x27:
                return 0x28;
            case 0x28:
                return 0x29;
            case 0x29:
                return 0x2a;
            case 0x2a:
                return 0x2b;
            case 0x2b:
                return 0x2c;
            case 0x2c:
                return 0x2d;
            case 0x2d:
                return 0x2e;
            case 0x2e:
                return 0x2f;
            case 0x2f:
                return 0x30;
            case 0x30:
                return 0x31;
            case 0x31:
                return 0x32;
            case 0x32:
                return 0x33;
            case 0x33:
                return 0x34;
            case 0x34:
                return 0x35;
            case 0x35:
                return 0x36;
            case 0x36:
                return 0x37;
            case 0x37:
                return 0x38;
            case 0x38:
                return 0x39;
            case 0x39:
                return 0x3a;
            case 0x3a:
                return 0x3b;
            case 0x3b:
                return 0x3c;
            case 0x3c:
                return 0x3d;
            case 0x3d:
                return 0x3e;
            case 0x3e:
                return 0x3f;
            case 0x3f:
                return 0x40;
            case 0x40:
                return 0x41;
            case 0x41:
                return 0x42;
            case 0x42:
                return 0x43;
            case 0x43:
                return 0x44;
            case 0x44:
                return 0x45;
            case 0x45:
                return 0x46;
            case 0x46:
                return 0x47;
            case 0x47:
                return 0x48;
            case 0x48:
                return 0x49;
            case 0x49:
                return 0x4a;
            case 0x4a:
                return 0x4b;
            case 0x4b:
                return 0x4c;
            case 0x4c:
                return 0x4d;
            case 0x4d:
                return 0x4e;
            case 0x4e:
                return 0x4f;
            case 0x4f:
                return 0x50;
            case 0x50:
                return 0x51;
            case 0x51:
                return 0x52;
            case 0x52:
                return 0x53;
            case 0x53:
                return 0x54;
            case 0x54:
                return 0x55;
            case 0x55:
                return 0x56;
            case 0x56:
                return 0x57;
            case 0x57:
                return 0x58;
            case 0x58:
                return 0x59;
            case 0x59:
                return 0x5a;
            case 0x5a:
                return 0x5b;
            case 0x5b:
                return 0x5c;
            case 0x5c:
                return 0x5d;
            case 0x5d:
                return 0x5e;
            case 0x5e:
                return 0x5f;
            case 0x5f:
                return 0x60;
            case 0x60:
                return 0x61;
            case 0x61:
                return 0x62;
            case 0x62:
                return 0x63;
            case 0x63:
                return 0x64;
            case 0x64:
                return 0x65;
            case 0x65:
                return 0x66;
            case 0x66:
                return 0x67;
            case 0x67:
                return 0x68;
            case 0x68:
                return 0x69;
            case 0x69:
                return 0x6a;
            case 0x6a:
                return 0x6b;
            case 0x6c:
                return 0x71;
            case 0x6e:
                return 0x7a;
            case 0x6f:
                return 0x7b;
            case 0x70:
                return 0x7c;
            case 0x71:
                return 0x7d;
            case 0x72:
                return 0x7e;
            case 0x73:
                return 0x7f;
            case 0x74:
                return 0x80;
            case 0x75:
                return 0x81;
            case 0x76:
                return 0x82;
            case 0x77:
                return 0x83;
            case 0x78:
                return 0x84;
            case 0x79:
                return 0x85;
            case 0x7a:
                return 0x86;
            case 0x7b:
                return 0x87;
            case 0x7c:
                return 0x88;
            case 0x7d:
                return 0x89;
            case 0x7e:
                return 0x8a;
            case 0x80:
                return 0x8c;
            case 0x81:
                return 0x8d;
            case 0x82:
                return 0x8e;
            case 0x83:
                return 0x8f;
            case 0x84:
                return 0x90;
            case 0x85:
                return 0x91;
            case 0x86:
                return 0x92;
            case 0x87:
                return 0x93;
            case 0x89:
                return 0x95;
            case 0x8a:
                return 0x96;
            case 0x8b:
                return 0x97;
            case 0x8c:
                return 0x98;
            case 0x8d:
                return 0x99;
            case 0x8e:
                return 0x9a;
            case 0x8f:
                return 0x9b;
            case 0x90:
                return 0x9c;
            case 0x91:
                return 0x9d;
            case 0x92:
                return 0x9e;
            case 0x93:
                return 0x9f;
            case 0x94:
                return 0xa1;
            case 0x95:
                return 0xa2;
            case 0x96:
                return 0xa3;
            case 0x97:
                return 0xa4;
            case 0x98:
                return 0xa5;
            case 0x99:
                return 0xa6;
            case 0x9a:
                return 0xa7;
            case 0x9b:
                return 0xa8;
            case 0x9c:
                return 0xa9;
            case 0x9d:
                return 0xaa;
            case 0x9e:
                return 0xab;
            case 0x9f:
                return 0xac;
            case 0xa0:
                return 0xad;
            case 0xa1:
                return 0xae;
            case 0xa2:
                return 0xaf;
            case 0xa3:
                return 0xb0;
            case 0xa4:
                return 0xb1;
            case 0xa5:
                return 0xb2;
            case 0xa6:
                return 0xb3;
            case 0xa7:
                return 0xb4;
            case 0xa8:
                return 0xb5;
            case 0xa9:
                return 0xb6;
            case 0xaa:
                return 0xb7;
            case 0xab:
                return 0xb8;
            case 0xac:
                return 0xb9;
            case 0xad:
                return 0xba;
            case 0xae:
                return 0xbb;
            case 0xaf:
                return 0xbc;
            case 0xb3:
                return 0xc0;
            case 0xb4:
                return 0xc1;
            case 0xb5:
                return 0xc2;
            case 0xb6:
                return 0xc3;
            case 0xb7:
                return 0xc4;
            case 0xb8:
                return 0xc5;
            case 0xb9:
                return 0xc6;
            case 0xba:
                return 0xc7;
            case 0xbb:
                return 0xc8;
            case 0xbc:
                return 0xc9;
            case 0xbd:
                return 0xca;
            case 0xbe:
                return 0xcb;
            case 0xbf:
                return 0xcc;
            case 0xc0:
                return 0xcd;
            case 0xc1:
                return 0xce;
            case 0xc2:
                return 0xd1;
            case 0xc3:
                return 0xd2;
            case 0xc4:
                return 0xd3;
            case 0xc5:
                return 0xd4;
            case 0xc6:
                return 0xd5;
            case 0xc7:
                return 0xd6;
            case 0xc8:
                return 0xd7;
            case 0xc9:
                return 0xd8;
            case 0xca:
                return 0xd9;
            case 0xcc:
                return 0xdb;
            case 0xcd:
                return 0xdc;
            case 0xcf:
                return 0xde;
            case 0xd0:
                return 0xdf;
            case 0xd1:
                return 0xe0;
            case 0xd2:
                return 0xe1;
            case 0xd3:
                return 0xe2;
            case 0xd4:
                return 0xe3;
            case 0xd5:
                return 0xe4;
            case 0xd8:
                return 0xe7;
            case 0xd9:
                return 0xe8;
            case 0xda:
                return 0xe9;
            case 0xdb:
                return 0xea;
            case 0xdc:
                return 0xeb;
            case 0xdd:
                return 0xec;
            case 0xde:
                return 0xed;
            case 0xe1:
                return 0xf0;
            case 0xe2:
                return 0xf1;
            case 0xe3:
                return 0xf2;
            case 0xe6:
                return 0xf5;
            case 0xe7:
                return 0xf6;
            case 0xe8:
                return 0xf7;
            case 0xe9:
                return 0xf8;
            case 0xea:
                return 0xf9;
            case 0xeb:
                return 0xfa;
            case 0xec:
                return 0xfb;
            case 0xee:
                return 0xfd;
            case 0xef:
                return 0xfe;
            case 0xf0:
                return 0xff;
            case 0xf1:
                return 0x100;
            case 0xf2:
                return 0x101;
            case 0xf3:
                return 0x102;
            case 0xf4:
                return 0x103;
            case 0xf5:
                return 0x104;
            case 0xf6:
                return 0x105;
            case 0xf7:
                return 0x106;
            case 0xf8:
                return 0x107;
            case 0xf9:
                return 0x108;
            case 0xfb:
                return 0x10a;
            case 0xfc:
                return 0x10b;
            case 0xfd:
                return 0x10c;
            case 0xfe:
                return 0x10d;
            case 0xff:
                return 0x10e;
            case 0x100:
                return 0x10f;
            case 0x101:
                return 0x110;
            case 0x102:
                return 0x111;
            case 0x103:
                return 0x112;
            case 0x104:
                return 0x113;
            case 0x105:
                return 0x114;
            case 0x106:
                return 0x115;
            case 0x108:
                return 0x117;
            case 0x109:
                return 0x118;
            case 0x10a:
                return 0x119;
            case 0x10b:
                return 0x11a;
            case 0x10c:
                return 0x11b;
            case 0x10d:
                return 0x11c;
            case 0x10f:
                return 0x11e;
            case 0x110:
                return 0x11f;
            case 0x111:
                return 0x120;
            case 0x112:
                return 0x121;
            case 0x113:
                return 0x122;
            case 0x114:
                return 0x123;
            case 0x11a:
                return 0x129;
            case 0x11b:
                return 0x12a;
            case 0x11c:
                return 0x12b;
            case 0x11e:
                return 0x12d;
            case 0x11f:
                return 0x12e;
            case 0x120:
                return 0x12f;
            case 0x121:
                return 0x130;
            case 0x122:
                return 0x131;
            case 0x123:
                return 0x132;
            case 0x124:
                return 0x133;
            case 0x126:
                return 0x135;
            case 0x127:
                return 0x136;
            case 0x128:
                return 0x137;
            case 0x129:
                return 0x138;
            case 0x12a:
                return 0x139;
            case 0x12b:
                return 0x13a;
            case 0x12c:
                return 0x13b;
            case 0x12d:
                return 0x13c;
            case 0x132:
                return 0x142;
            case 0x133:
                return 0x143;
            case 0x134:
                return 0x144;
            case 0x135:
                return 0x145;
            case 0x136:
                return 0x146;
            case 0x137:
                return 0x147;
            case 0x138:
                return 0x148;
            case 0x139:
                return 0x149;
            case 0x13a:
                return 0x14a;
            case 0x13b:
                return 0x14b;
            case 0x13c:
                return 0x14c;
            case 0x13d:
                return 0x14d;
            case 0x13e:
                return 0x14e;
            case 0x13f:
                return 0x14f;
            case 0x140:
                return 0x150;
            case 0x145:
                return 0x151;
            case 0x146:
                return 0x152;
            case 0x149:
                return 0x155;
            case 0x14a:
                return 0x156;
            case 0x14b:
                return 0x157;
            case 0x14c:
                return 0x158;
            case 0x14d:
                return 0x159;
            case 0x14e:
                return 0x15a;
            case 0x14f:
                return 0x15b;
            case 0x150:
                return 0x15c;
            case 0x152:
                return 0x15e;
            case 0x153:
                return 0x15f;
            case 0x154:
                return 0x160;
            case 0x155:
                return 0x161;
            case 0x156:
                return 0x162;
            case 0x157:
                return 0x163;
            case 0x158:
                return 0x164;
            case 0x15b:
                return 0x167;
            case 0x15c:
                return 0x168;
            case 0x15d:
                return 0x169;
            case 0x15e:
                return 0x16a;
            case 0x15f:
                return 0x16b;
            case 0x160:
                return 0x16c;
            case 0x161:
                return 0x16d;
            case 0x164:
                return 0x170;
            case 0x166:
                return 0x172;
            case 0x169:
                return 0x175;
            case 0x25b:
                return 0x176;
            case 0x25c:
                return 0x177;
            case 0x25d:
                return 0x178;
            case 0x25e:
                return 0x17a;
        }
    }
    else if (type == 0)
    {
        switch (cmd)
        {
            case 0x0:
                return 0x17b;
            case 0x1:
                return 0x17c;
            case 0x2:
                return 0x17d;
            case 0x3:
                return 0x17e;
            case 0x4:
                return 0x17f;
            case 0x5:
                return 0x180;
            case 0x6:
                return 0x181;
            case 0x7:
                return 0x182;
            case 0x8:
                return 0x183;
            case 0x9:
                return 0x184;
            case 0xa:
                return 0x185;
            case 0xb:
                return 0x186;
            case 0xc:
                return 0x187;
            case 0xd:
                return 0x188;
            case 0xe:
                return 0x189;
            case 0xf:
                return 0x18a;
            case 0x10:
                return 0x18b;
            case 0x11:
                return 0x18c;
            case 0x12:
                return 0x18d;
            case 0x13:
                return 0x18e;
            case 0x14:
                return 0x18f;
            case 0x15:
                return 0x190;
            case 0x16:
                return 0x191;
            case 0x17:
                return 0x192;
            case 0x18:
                return 0x193;
            case 0x19:
                return 0x194;
            case 0x1a:
                return 0x195;
            case 0x1b:
                return 0x196;
            case 0x1c:
                return 0x197;
            case 0x1d:
                return 0x198;
            case 0x1e:
                return 0x199;
            case 0x1f:
                return 0x19a;
            case 0x20:
                return 0x19b;
            case 0x21:
                return 0x19c;
            case 0x22:
                return 0x19d;
            case 0x23:
                return 0x19e;
            case 0x24:
                return 0x19f;
            case 0x25:
                return 0x1a0;
            case 0x26:
                return 0x1a1;
            case 0x27:
                return 0x1a2;
            case 0x28:
                return 0x1a3;
            case 0x29:
                return 0x1a4;
            case 0x2a:
                return 0x1a5;
            case 0x2b:
                return 0x1a6;
            case 0x2c:
                return 0x1a7;
            case 0x2d:
                return 0x1a8;
            case 0x2e:
                return 0x1a9;
            case 0x2f:
                return 0x1aa;
            case 0x30:
                return 0x1ab;
            case 0x31:
                return 0x1ac;
            case 0x32:
                return 0x1ad;
            case 0x33:
                return 0x1ae;
            case 0x34:
                return 0x1af;
            case 0x35:
                return 0x1b0;
            case 0x36:
                return 0x1b1;
            case 0x37:
                return 0x1b2;
            case 0x38:
                return 0x1b3;
            case 0x39:
                return 0x1b4;
            case 0x3a:
                return 0x1b5;
            case 0x3b:
                return 0x1b6;
            case 0x3c:
                return 0x1b7;
            case 0x3d:
                return 0x1b8;
            case 0x3e:
                return 0x1b9;
            case 0x3f:
                return 0x1ba;
            case 0x40:
                return 0x1bb;
            case 0x41:
                return 0x1bc;
            case 0x42:
                return 0x1bd;
            case 0x43:
                return 0x1be;
            case 0x44:
                return 0x1bf;
            case 0x45:
                return 0x1c0;
            case 0x46:
                return 0x1c1;
            case 0x47:
                return 0x1c2;
            case 0x48:
                return 0x1c3;
            case 0x49:
                return 0x1c4;
            case 0x4a:
                return 0x1c5;
            case 0x4b:
                return 0x1c6;
            case 0x4c:
                return 0x1c7;
            case 0x4d:
                return 0x1c8;
            case 0x4e:
                return 0x1c9;
            case 0x4f:
                return 0x1ca;
            case 0x50:
                return 0x1cb;
            case 0x51:
                return 0x1cc;
            case 0x52:
                return 0x1cd;
            case 0x53:
                return 0x1ce;
            case 0x54:
                return 0x1cf;
            case 0x55:
                return 0x1d0;
            case 0x56:
                return 0x1d1;
            case 0x57:
                return 0x1d2;
            case 0x58:
                return 0x1d3;
            case 0x59:
                return 0x1d4;
            case 0x5a:
                return 0x1d5;
            case 0x5b:
                return 0x1d6;
            case 0x5c:
                return 0x1d7;
            case 0x5d:
                return 0x1d8;
            case 0x5e:
                return 0x1d9;
            case 0x5f:
                return 0x1da;
            case 0x60:
                return 0x1db;
            case 0x61:
                return 0x1dc;
            case 0x62:
                return 0x1dd;
            case 0x63:
                return 0x1de;
            case 0x64:
                return 0x1df;
            case 0x65:
                return 0x1e0;
            case 0x66:
                return 0x1e1;
            case 0x67:
                return 0x1e2;
            case 0x68:
                return 0x1e3;
            case 0x69:
                return 0x1e4;
            case 0x6a:
                return 0x1e5;
            case 0x6b:
                return 0x1e6;
            case 0x6c:
                return 0x1f3;
            case 0x6d:
                return 0x1f4;
            case 0x6e:
                return 0x1f5;
            case 0x6f:
                return 0x1f6;
            case 0x70:
                return 0x1f7;
            case 0x71:
                return 0x1f8;
            case 0x72:
                return 0x1f9;
            case 0x73:
                return 0x1fa;
            case 0x75:
                return 0x1fc;
            case 0x76:
                return 0x1fd;
            case 0x77:
                return 0x1fe;
            case 0x78:
                return 0x1ff;
            case 0x79:
                return 0x200;
            case 0x7a:
                return 0x201;
            case 0x7b:
                return 0x202;
            case 0x7c:
                return 0x203;
            case 0x7d:
                return 0x204;
            case 0x7e:
                return 0x205;
            case 0x7f:
                return 0x206;
            case 0x80:
                return 0x207;
            case 0x81:
                return 0x208;
            case 0x82:
                return 0x209;
            case 0x83:
                return 0x20a;
            case 0x84:
                return 0x20b;
            case 0x85:
                return 0x20c;
            case 0x86:
                return 0x20d;
            case 0x87:
                return 0x20e;
            case 0x88:
                return 0x20f;
            case 0x89:
                return 0x210;
            case 0x8a:
                return 0x211;
            case 0x8b:
                return 0x212;
            case 0x8c:
                return 0x213;
            case 0x8d:
                return 0x214;
            case 0x8e:
                return 0x215;
            case 0x8f:
                return 0x216;
            case 0x90:
                return 0x217;
            case 0x91:
                return 0x218;
            case 0x92:
                return 0x219;
            case 0x93:
                return 0x21a;
            case 0x94:
                return 0x21b;
            case 0x95:
                return 0x21c;
            case 0x96:
                return 0x21d;
            case 0x97:
                return 0x21e;
            case 0x98:
                return 0x21f;
            case 0x99:
                return 0x220;
            case 0x9b:
                return 0x222;
            case 0x9c:
                return 0x223;
            case 0x9d:
                return 0x224;
            case 0x9e:
                return 0x225;
            case 0x9f:
                return 0x226;
            case 0xa0:
                return 0x227;
            case 0xa1:
                return 0x228;
            case 0xa2:
                return 0x229;
            case 0xa3:
                return 0x22a;
            case 0xa5:
                return 0x22c;
            case 0xa6:
                return 0x22d;
            case 0xa8:
                return 0x22f;
            case 0xa9:
                return 0x230;
            case 0xaa:
                return 0x232;
            case 0xab:
                return 0x233;
            case 0xac:
                return 0x234;
            case 0xad:
                return 0x236;
            case 0xae:
                return 0x237;
            case 0xaf:
                return 0x238;
            case 0xb0:
                return 0x239;
            case 0xb1:
                return 0x23a;
            case 0xb2:
                return 0x23b;
            case 0xb3:
                return 0x23c;
            case 0xb4:
                return 0x23d;
            case 0xb5:
                return 0x23e;
            case 0xb6:
                return 0x23f;
            case 0xb7:
                return 0x240;
            case 0xba:
                return 0x243;
            case 0xbc:
                return 0x245;
            case 0xbd:
                return 0x246;
            case 0xbe:
                return 0x247;
            case 0xbf:
                return 0x248;
            case 0xc0:
                return 0x249;
            case 0xc2:
                return 0x24b;
            case 0xc3:
                return 0x24c;
            case 0xc5:
                return 0x24e;
            case 0xc7:
                return 0x250;
            case 0xc8:
                return 0x251;
            case 0xc9:
                return 0x252;
            case 0xca:
                return 0x253;
            case 0xcb:
                return 0x254;
            case 0xcd:
                return 0x256;
            case 0xce:
                return 0x257;
            case 0xcf:
                return 0x258;
            case 0xd0:
                return 0x259;
            case 0xd1:
                return 0x25a;
            case 0xd2:
                return 0x25b;
            case 0xd3:
                return 0x25c;
            case 0xd4:
                return 0x25d;
            case 0xd5:
                return 0x25e;
            case 0xd6:
                return 0x25f;
            case 0xd7:
                return 0x260;
            case 0xd8:
                return 0x261;
            case 0xd9:
                return 0x262;
            case 0xda:
                return 0x263;
            case 0xdb:
                return 0x264;
            case 0xdc:
                return 0x265;
            case 0xdd:
                return 0x266;
            case 0xdf:
                return 0x268;
            case 0xe0:
                return 0x269;
            case 0xe1:
                return 0x26a;
            case 0xe2:
                return 0x26b;
            case 0xe3:
                return 0x26c;
            case 0xe4:
                return 0x26d;
            case 0xe6:
                return 0x26f;
            case 0xe7:
                return 0x270;
            case 0xe9:
                return 0x272;
            case 0xec:
                return 0x275;
            case 0xed:
                return 0x276;
            case 0xee:
                return 0x277;
            case 0xef:
                return 0x278;
            case 0xf0:
                return 0x279;
            case 0xf1:
                return 0x27a;
            case 0xf2:
                return 0x27b;
            case 0xf3:
                return 0x27c;
            case 0xf4:
                return 0x27d;
            case 0xf6:
                return 0x27f;
            case 0xf7:
                return 0x280;
            case 0xf8:
                return 0x281;
            case 0xf9:
                return 0x282;
            case 0xfa:
                return 0x283;
            case 0xfb:
                return 0x284;
            case 0xfc:
                return 0x285;
            case 0xfd:
                return 0x286;
            case 0xfe:
                return 0x287;
            case 0xff:
                return 0x288;
            case 0x100:
                return 0x289;
            case 0x103:
                return 0x28c;
            case 0x104:
                return 0x28d;
            case 0x105:
                return 0x28e;
            case 0x106:
                return 0x28f;
            case 0x107:
                return 0x290;
            case 0x108:
                return 0x291;
            case 0x109:
                return 0x292;
            case 0x10a:
                return 0x293;
            case 0x10b:
                return 0x294;
            case 0x10c:
                return 0x295;
            case 0x10d:
                return 0x296;
            case 0x10e:
                return 0x297;
            case 0x10f:
                return 0x298;
            case 0x110:
                return 0x299;
            case 0x111:
                return 0x29a;
            case 0x114:
                return 0x29d;
            case 0x115:
                return 0x29e;
            case 0x116:
                return 0x29f;
            case 0x117:
                return 0x2a0;
            case 0x118:
                return 0x2a1;
            case 0x119:
                return 0x2a2;
            case 0x11a:
                return 0x2a3;
            case 0x11f:
                return 0x2a8;
            case 0x121:
                return 0x2aa;
            case 0x122:
                return 0x2ab;
            case 0x123:
                return 0x2ac;
            case 0x124:
                return 0x2ad;
            case 0x125:
                return 0x2ae;
            case 0x126:
                return 0x2af;
            case 0x127:
                return 0x2b0;
            case 0x128:
                return 0x2b1;
            case 0x12a:
                return 0x2b3;
            case 0x12b:
                return 0x2b4;
            case 0x12c:
                return 0x2b5;
            case 0x12d:
                return 0x2b6;
            case 0x130:
                return 0x2b9;
            case 0x131:
                return 0x2ba;
            case 0x134:
                return 0x2bd;
            case 0x230:
                return 0x2be;
            case 0x231:
                return 0x2bf;
            case 0x232:
                return 0x2c0;
        }
    }
    return 0;
}
// ============================================================================
// OnlyOrigBatch11 落地：_NS_HS_NoResponseToHackLog —— ORIG T 0x08274f31
// user->getHackAnalyzer()->addServerHackCnt(user, 0x33c, 1, 0, 0)
// （addServerHackCnt 五参：CUser*, ENUM_HACKTYPE, uint, uint, uint）。
// ============================================================================
void _NS_HS_NoResponseToHackLog(CUser* user)
{
    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(0x33c), 1, 0, 0);
}
