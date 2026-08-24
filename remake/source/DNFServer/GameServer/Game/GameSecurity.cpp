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
    if (type == static_cast<WongWork::ENUM_HACKTYPE>(0x524) &&
        user->CheckInParty())
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0 && party->get_member_count() > 1)
            return;
    }

    int etcValue = user->SetETC(static_cast<short>(type), count);
    WongWork::CHackAnalyzer* analyzer = user->getHackAnalyzer();
    // ORIG 压参顺序：0x8(%esp)=user、0xc=1、0x10=etcValue、0x14=0。
    analyzer->addServerHackCnt(user, type, 1u,
                               static_cast<unsigned int>(etcValue), 0u);
}
