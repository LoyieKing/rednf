#ifndef DNF_GAME_APSYSTEM_TYPES_H_
#define DNF_GAME_APSYSTEM_TYPES_H_


#include <string>
#include <vector>

// CUser 完整定义见 CUser.h（本头仅签名引用）
class CUser;

// APSystem 共享类型（CActionPointEtcParameter 成员元素 / DB 签名）。
// 由 MoreTypes.h 与 CDataManager.h 共同包含，避免重复定义。

class MSG_CHARAC_SELECT;   // 全局作用域（PacketTypes.h 权威定义；仅引用）
namespace APSystem
{

// ============================================================================
// APSystem::_ActionGroupIndex —— 动作点「组号」类型。
// ORIG 实证（df_game_r 全符号表）：不存在 _ZN8APSystem17_ActionGroupIndexC1E*
// / D1 / 任何成员函数符号；ClearActionAndSendtoUser / SendActionClear /
// IsValidActionGroupIndex 的全部调用点都是一条 movl $<组号>,0x4(%esp) 直压
// 立即数（如 CParty::ClearDungeon 0x085a9330 的 0x18/0x19/0x1b/0x1c/0xf/0xd）。
// ⇒ 该形参类型是 4 字节枚举（Itanium ABI 下与 class 同名同 mangling
//    NS_17_ActionGroupIndexE），而非带构造函数的类；建模成类会在每个调用点
//    多出一次 C1Ev/C1Ei 构造调用（eager-ctor 冗余）。
// ============================================================================
enum _ActionGroupIndex
{
    _ACTION_GROUP_INDEX_NONE = 0   // 组号由调用点以整型强制转换直传 [1,0x2d]
};


// ============================================================================
// APSystem::_TodayRewardItem —— 顶层类（ORIG W 0x08895736 C1/C2 同体，
// memset(this,0,0xe)；D1/D2 0x08897d76 空体）。
// CActionPointEtcParameter::m_todayRewards 的元素类型；import 段
// [action point today reward] 栈对象 -0x116 同构（GetTodayRewardAndSendMail
// 0x08122104 元素偏移 +0/+4/+6/+8/+0xc 实证）。
// ============================================================================
#pragma pack(push, 1)
class _TodayRewardItem
{
public:
    // 2026-08-31：ORIG memset(this,0,0xe)、元素分配 14B（new_allocator 14B）
    // ——pack(1) sizeof 0xe；原默认对齐 0x10。
    __attribute__((used)) _TodayRewardItem();   // ORIG W 0x08895736
    // [Q8 2026-09-11 P342 triviality 轴] 删用户空析构（旧判词「须保留」**已推翻**）：
    // ORIG 该类型走 `_Destroy_auxILb1EE`/`__copy_move_backward<true,true>`（trivial 路径）
    // 而我方用户析构使其退化 ILb0（ScriptLeafA2 观测：`_Destroy_auxILb0EE…_TodayRewardItem`
    // + `importActionPointSystemEtcScript` EXTRA `D*Ev`×2 与 ORIG 不符）。
    // ORIG D1/D2 W 0x08897d76（5B 空体）由隐式析构 odr-use 发射（唯一 call 点
    // 0x08897d88 在 `new_allocator<_TodayRewardItem>::destroy` 体内）；
    // 本组 TU 中 ScriptLeafA2（栈对象 `APSystem::_TodayRewardItem today;`、
    // vector 成员 CDataManager.h:1736）提供 odr-use 链 ⇒ 删后 nm 实测 D1/D2 仍发射。
    // overlay 实测：ScriptLeafA2 659/1087/0/13/319 → 661/1087/0/13/318；
    // APSystemOnlyOrig 49/93/0/6/29 → 49/92/0/6/28；
    // `importActionPointSystemEtcScript` 812 → 792（ORIG 794，gap −18 → +2）。

    int m_itemIdx;              // +0x00 today reward 序号 [0..3]
    unsigned short m_minAp;     // +0x04
    unsigned short m_maxAp;     // +0x06
    int m_rewardItemIdx;        // +0x08（find_item 实参）
    unsigned short m_count;     // +0x0c（邮件附件数量）
};                              // sizeof 0xe
#pragma pack(pop)

// APSystem::_ActionPoint —— 行动点 DB 行记录。
// ORIG W 0x0848f5b4 _ZN8APSystem12_ActionPointC1Ev/C2Ev（ctor 体=memset(this,0,0xb)，
// objdump 转写，实现见 APSystemLeafC3.cpp）。步长 0xb 实证：_SIG_LOAD_ACTION_POINT
// ctor 0x0848f5d8 自 +0xd 起按 0xb 步长构造 300 个元素；DB_LoadActionPoint::
// ParseActionClearState(…,_ActionPoint*,…) 同步长遍历、组索引 @+0x02。
// int 落在未对齐 +0x02 槽 → 必须 pack(1)，sizeof == 0xb。ORIG 无 D1/D2 符号（平凡析构）。
class __attribute__((packed)) _ActionPoint
{
public:
    _ActionPoint();

    unsigned short m_actionNo;   // +0x00
    int m_groupIndex;            // +0x02（IsValidActionGroupIndex 实参来源）
    unsigned char m_days[5];     // +0x06..+0x0a
};                               // sizeof 0xb

// ============================================================================
// _SIG_LOAD_ACTION_POINT —— ORIG W 0x0848f5d8 C1/C2 同体（sizeof 0xcf1 =
// 0xd 头 + 300×0xb）。SendResult put_binary(sig,0xcf1)、DB_LoadActionPoint::
// Select、CActionPointManager::Set memcpy(this+8,sig,0xcf1) 多点实证。
// 嵌套 _TodayRewardItem 为另一类型：IsValidTodayRewardItem
// (NS_22_SIG_LOAD_ACTION_POINT16_TodayRewardItemE，按值 4 字节；
// 体仅读 [ebp+8]∈[0,3]，0x08894976 objdump)，无独立生命周期符号。
// ============================================================================
class __attribute__((packed)) _SIG_LOAD_ACTION_POINT
{
public:
    class _TodayRewardItem
    {
    public:
        int m_itemIdx;   // +0x00（today 序号 [0..3]，单 int 按 4 字节传）
    };                   // sizeof 4

    _SIG_LOAD_ACTION_POINT();   // ORIG W 0x0848f5d8 C1/C2 同体

    unsigned int m_characNo;             // +0x00（请求侧角色号；Update where 条件）
    unsigned int m_apSum;                // +0x04（select ap_sum）
    unsigned char m_isRewardMedal;       // +0x08
    // [g04-P357 2026-09-11 Main 批准] unsigned char → bool：ORIG 该字段消费点
    // `movzbl 0x11(%edx,%eax,1),%eax; xor $0x1,%eax; test %al,%al; je`
    // （GetTodayRewardAndSendMail 的 `if (!f)`）⇒ ORIG 侧字段为 bool
    // （探针 /tmp/ap_probe6.cpp gS3：仅 bool 字段 + `if(!f)` 才出 xor）。
    // 布局零变化：bool 与 unsigned char 同为 1 字节，数组 4 元素 ⇒ 恒 4 字节，
    // sizeof/成员偏移/mangled 全不变；usize 点（memset sizeof / printf 提升 /
    // `= (flag != 0)` 赋值）全兼容。
    bool m_isRewardItem[4];              // +0x09..+0x0c
    _ActionPoint m_actionPoints[300];    // +0x0d，步长 0xb
};                                       // sizeof 0xcf1
typedef char _assert_sig_load_action_point_size[(sizeof(_SIG_LOAD_ACTION_POINT) == 0xcf1) ? 1 : -1];

// 【batch-17 三联动结论（§6.2 口径，UpC11 实测复核）】
//   copy ctor 0x08895ebc / operator= 0x0889603a 的逐成员拷贝集合已完全对齐：
//   [+0x00 dword][+0x04 dword][+0x08 word][+0x0a byte][+0x0c word][+0x0e]
//   [+0x0f][+0x10] → vector(+0x14) → [+0x20 dword] → string(+0x24) →
//   string(+0x28) → [+0x2c byte] → **止于 +0x2c**；+0x2d..+0x2f 全区域零引用
//   （工单原记"copy ctor 拷 0x2d/0x2f"系 CCMC8 把 OURS 反产行的 + 前缀误读，
//   已双向复核修正）。据此删除 m_pad2d[3]（隐式拷贝会拆成 movzwl 0x2d +
//   movzbl 0x2f，+4~6 insn；类有效对齐 4 ⇒ 自动补齐，sizeof 0x30 断言不变）。
//   残余 copy 84/74、assign 61/54 的 10 条差 = ORIG 从 +0x0c 起每成员重载
//   0xc(%ebp)/0x8(%ebp) 的 4 条形，而 4.4 全程寄存器缓存（N18a/N76 命中，
//   非语义、不可源码控制）⇒ 判语义完成、byte 层封顶。
//   import 侧（0x08894996）现 794/798（gap -4），取数/写入粒度见各消费行注释。
// 元素布局（importActionPointSystemEtcScript 0x08894996 汇编实证，sizeof 0x30）：
//   B+0x00 ushort / B+0x02 int(IsValidActionGroupIndex 参数) / B+0x06 byte[5]
//   （写 -(char)v；其中 +8/+9 槽随后被独立扫描覆写）/ B+0x0c word（map 键）/
//   B+0x10 byte / B+0x14 vector<int>（"," 分割 atoi）/ B+0x20 int（weekday，
//   失败=7）/ B+0x24 string / B+0x28 string / B+0x2c byte。
//
// [PoolI 2026-09-11 布局正解] ORIG 真含 _ActionPoint 子对象（+0x00，11B）：
//   ① C1Ev 0x0889566e 首段 `mov this,%eax; mov %eax,(%esp); call
//      _ZN8APSystem12_ActionPointC1Ev`（子对象构造；_ActionPoint packed sizeof 0xb）；
//   ② copy ctor 0x08895ebc / operator= 0x0889603a 逐成员序列 =
//      4B@0x00 / 4B@0x04 / 2B@0x08 / 1B@0x0a（11B 块对齐剥离 4+4+2+1）
//      / 2B@0x0c / 1B@0x0e / 1B@0x0f / 1B@0x10（派生成员）。
//   旧「union 模拟（ushort@0 + packed ActionPointRecord@2 + m_bytes02 视图）」
//   使 4.4 隐式拷贝产 2B@0 + 18B packed 块（4+4+4+4+2，gap +10）且含 ORIG 不存在的
//   尾字段（m_field0b@0x0b / m_field11[3]@0x11..0x13）⇒ 已废。
//   0x0b 与 0x11..0x13 是 padding（ORIG 拷贝不触及），不得声明成员（P26/P213）。
struct CActionPointEx
{
    CActionPointEx();

    // ORIG 的 _ActionPoint 子对象：m_actionNo@+0x00(ushort)、m_groupIndex@+0x02(int)、
    // m_days[5]@+0x06；整体 11B（packed，ORIG C1Ev memset 0xb 实证）。
    _ActionPoint m_ap;               // +0x00..+0x0a

    unsigned short m_mapKey;         // +0x0c（action-point map 键）
    unsigned char m_field0e;         // +0x0e（报告未读取）
    unsigned char m_levelMin;        // +0x0f（等级下限）
    unsigned char m_levelMax;        // +0x10（等级上限）

    std::vector<int> m_itemList;     // +0x14（"," 分割 atoi）
    int m_weekday;                   // +0x20（ConvertWeekdayStringToInt 失败置 7）
    std::string m_str24;             // +0x24
    std::string m_str28;             // +0x28
    unsigned char m_field2c;         // +0x2c
    // +0x2d..+0x2f 是**不参与拷贝的尾隙**：ORIG copy ctor(0x08895ebc) 与
    // operator=(0x0889603a) 的逐成员序列都止于 `movzbl 0x2c / mov %dl,0x2c`，
    // 全区域 objdump 无任何 0x2d/0x2f 引用。声明显式 char[3] 会被隐式拷贝
    // 拆成 movzwl 0x2d + movzbl 0x2f 两段（+4 insn，P26 同源）；类有效对齐为 4
    // （含 int/vector/string 成员）⇒ 编译器自动补齐至 sizeof 0x30，下方断言守住。
};                                   // sizeof 0x30
typedef char _assert_action_point_ex_size[
    (sizeof(CActionPointEx) == 0x30) ? 1 : -1];

// 布局（importActionPointSystemEtcScript [action point medal reward] 实证，
// 栈对象 -0x116..-0x109：ushort/ushort/int/ushort，sizeof 0xc；
// ctor 0x08895712=memset(this,0,0xa)，尾 pad 不清零）。
#pragma pack(push, 1)
struct _MedalRewardItem
{
    // 2026-08-31：ORIG ctor 0x8895712 memset(this,0,0xa)、元素分配 10B
    // （new_allocator 10B）——pack(1) sizeof 0xa；原默认对齐 0xc。
    _MedalRewardItem();   // ORIG W 0x08895712

    unsigned short m_field00;   // +0x00
    unsigned short m_field02;   // +0x02
    int m_field04;              // +0x04
    unsigned short m_field08;   // +0x08
};                              // sizeof 0xa
#pragma pack(pop)


// ORIG 0x08121616：GetActionPointFromMemory（纯空函数）
// ORIG T 0x08121608：RequestLoadActionPointFromDBAndES（ORIG 体仅 return 1，
// 13 字节；SelectCharac::process 调用）。
class CDBAndExchangeServerProc
{
public:
    static void GetActionPointFromMemory(const CUser& user, _SIG_LOAD_ACTION_POINT& out);
    static bool RequestLoadActionPointFromDBAndES(CUser& user, MSG_CHARAC_SELECT& msg);
};
}

// ============================================================================
// zlib 包装全局函数（本批唯一实现属主：APSystemOnlyOrig.cpp；Main 授权）。
// ORIG T 0x086b2102 uncompress_zip / T 0x086b201f compress_zip（objdump 全文转写）。
bool uncompress_zip(char* out, unsigned int& outLen, const char* in, unsigned int inLen);
bool compress_zip(char* out, unsigned int& outLen, const char* in, unsigned int inLen);

#endif  // DNF_GAME_APSYSTEM_TYPES_H_
