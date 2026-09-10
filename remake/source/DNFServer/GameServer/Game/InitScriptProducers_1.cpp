// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 InitScriptProducers_1/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CDataManager.h"
#include "sync_script.h"   // STEquipmentPartSetPiece 完整类型（DataManagerInitDepends.h 前置依赖）
#include "DataManagerInitDepends.h"
#include "DNFLexWrapper.h"
#include "LogManager.h"

#include <cstdio>
#include <cstring>

// ---- 外部定义 ----
int  getCharacterJobToEnumValue(std::string& job);                     // CDataManager.cpp 0x89bbc0f
int  LoadRewardDataByGrowType(stLevelUpRewardItemScript& script,
                              std::string endTag);                     // InitScriptProducers_2.cpp

// ---- 布局自证（与 ORIG sizeof 一致性锚点）----
namespace {

struct LayoutAsserts
{
    // LevelUpRewardItemTableKey：ctor/clear 0x0891bac0/0x0891bad4 → {ushort,uchar,uchar}
    char check_key[sizeof(LevelUpRewardItemTableKey) == 4 ? 1 : -1];
    // LevelUpRewardItemData：clear 0x081b88d0 写两 int（-1/0）
    char check_data[sizeof(LevelUpRewardItemData) == 8 ? 1 : -1];
    // StackableItemData：LoadRewardDataByNoType 栈上两连续 int
    char check_stackable[sizeof(StackableItemData) == 8 ? 1 : -1];
    // stGrowthCapsuleScript：C1 0x0837114a 五个 int
    char check_capsule[sizeof(stGrowthCapsuleScript) == 0x14 ? 1 : -1];
    // stSeriaBlessingScript::Value：C1 0x08693aa0 五个 int
    char check_seria_value[sizeof(stSeriaBlessingScript::Value) == 0x14 ? 1 : -1];
    // WorldDropInfo：槽位步长 0x14（m_worldDrop1[201]+0x6734 → m_worldDrop2@+0x76e8）
    char check_drop[sizeof(WorldDropInfo) == 0x14 ? 1 : -1];
    // channel_info_t / channel_dungeon_info_t：ctor 0x088dd2a8/0x088dd34a
    char check_channel_info[sizeof(channel_info_t) == 0x40 ? 1 : -1];
    char check_channel_dungeon[sizeof(channel_dungeon_info_t) == 0x10 ? 1 : -1];
};

}  // namespace

// ORIG T 0x088dc008：loadRDARScriptFile("","Etc/channel_info.etc") 门禁 +
// LogManager 报错 + 转发 applyChannelScript 返回值。
// [P248 档①] 返回 bool：CDataManager::init 调用点 ORIG 形
// `call; xor $1,%eax; test %al,%al; je`（报告 idx 3963-3966）；bool 化后尾调
// 透传形态与 ORIG 0x088dc062 `call applyChannelScript; leave; ret` 逐条一致。
#include "InitScriptProducers_1/importChannelScript.inc"

// ============================================================================
// AccountCargoScript（Etc/AccountCargo.etc 域）
// ============================================================================

// ORIG W 0x08372442：+0x00=0、+0x04=0、std::map::clear(+0x08)。
#include "InitScriptProducers_1/clear.inc"

// b28d-solo-InitScriptProducers_1: DIFF→N 封顶（p167 20/20 CLEAR，语义闭合）。
// 本波复核（clean 归一后 132=132 指令，仅 3 处）：①我方多 1 条入口 jmp（ORIG
// `jmp+nop` 自跳转被归一剥离，P164 do-while 入口布局）；②ORIG do-while 条件
// `call operator==; test %al,%al; je <循环头>` 无 xor，4.4 恒插 `xor $0x1`
// （P320/S138：4.1 循环条件取反直接反转跳转）；③极性 je↔jne（由②派生）。
// 四形实测 `!()`/`==false`/`==0`/体内 break = 137/137/137/139 vs ORIG 135 均不可翻。
// ORIG T 0x088c7e80。段结构：[required level] / [required item] /
// [upgrade info](grade 与 UpgradeInfo 首字段同一栈位重叠，随后再扫三 int) /
// [account cargo end]。未识别 token 直接跳过；ScanInt 断流按 ORIG 精确分支处理。
#include "InitScriptProducers_1/importAccountCargoScript.inc"

// ============================================================================
// Channel（Etc/channel_info.etc 域）
// ============================================================================

// ORIG W 0x088dd2a8：发射序=三个 string 构造（+0x00/+0x08/+0x0c）**先于**
// 标量清零（+0x04/+0x10/+0x14）⇒ 4.1 类成员构造分组在前；4.4 初始化列表按
// 声明序混合发射，故标量改 body 赋值复现分组序。
// 注意：m_gridPercent() 值初始化会被 gcc 展开成 9 次循环（36B），与 ORIG
// memset 0x28（40B）不一致，故显式 memset 全部 10 个 float。
#include "InitScriptProducers_1/channel_info_t.inc"

// ORIG W 0x083f11da：逆序析构 +0x0c/+0x08/+0x00 三个 string（其余 POD 无动作）。
#include "InitScriptProducers_1/dtor_channel_info_t.inc"

// b28d-solo-InitScriptProducers_1: AE（修：m_gridPercent 加 union 块视图
// `m_gridBlock = o.m_gridBlock`，复现 ORIG 0x088dd457 src 缓存 %edx + %ecx 中转的
// 0x28 字节结构块拷贝（P246）；逐元素循环生 scaled 索引 + 循环变量槽致帧 0x28）。
// ORIG W 0x088dd3c6：逐成员拷贝（string×3 赋值 + type + pBonusRates + bonusRate +
// floats 按 dword 循环拷贝），与编译器逐字段序一致。
channel_info_t& channel_info_t::operator=(const channel_info_t& o)
{
    m_name = o.m_name;
    m_channelType = o.m_channelType;
    m_dungeonName = o.m_dungeonName;
    m_dungeonKey = o.m_dungeonKey;
    m_pBonusRates = o.m_pBonusRates;
    m_bonusRate = o.m_bonusRate;
    // ORIG 0x088dd457..0x088dd472：src 缓存 %edx + %ecx 中转的 0x28 字节整体块
    // 拷贝（P246）⇒ 结构视图整体赋值，非逐元素循环。
    m_gridBlock = o.m_gridBlock;
    return *this;
}

// channel_dungeon_info_t 的 C1/D1/op= 为 ORIG W 隐式合成（P299/CWG434：ORIG
// map::operator[] 0x088dd490 在构造临时对象前发 4×movl $0 前置零初始化）⇒
// 本 TU 不得写用户定义体（SweepInitProd6-2 已删 CDataManager.h 内声明）；
// 由 applyChannelScript 的 `channel_dungeon_info_t info;` 与 `map::operator[]`
// 使用点发射同名 W 符号（ORIG 0x45/0x45/0x35）。

// b28d-solo-InitScriptProducers_1: DIFF→N 封顶（p167 66/66 CLEAR，语义闭合）。
// 残余 = 帧 0x120 vs 0x110 + 槽组整体平移 0xc（N82/N95：4.4 聚合对象优先占低槽 +
// 标量分区排布）+ 条件取反 xor 物化；逐块控制流/字段偏移/日志行号常量已核。
// 本波复核：jcc 目标下标逐位全等（666 件审计含本件）；p167 差集仅 C1↔C2/D1↔D2
// 同址别名（ORIG 调用点用 C2/D2，4.1 同址发射；P93 伪差集）；无 call*（无虚槽）；
// 残余帧差 = 我方多 1 个具名 state 槽（RowState，ORIG 以 ebx 表达式承载）。
// ORIG T 0x088dc069。主循环分 [dungeon]/[server] 两段：
//  返回 1＝正常结束/EOF/未知类型（均 esi=1 出口）；0＝[server] 行解析错误（esi=0）。
// [P248 档①] 返回 bool（全部 return 为常量 0/1 ⇒ 机器码与 int 版逐条相同）。
#include "InitScriptProducers_1/applyChannelScript.inc"

// ---- Channel 类型名表（ORIG static _ZL19g_ChannelTypeString d 0x093f4e40，
//      20 项 ASCII 字符串指针，getChannelType/toChannelTypeString 共用）----
static const char* const g_ChannelTypeString[20] = {
    "CHANNEL_TEEN",                     // 0
    "CHANNEL_GENERAL",                  // 1
    "CHANNEL_BEGINNER",                 // 2
    "CHANNEL_TRADE",                    // 3
    "CHANNEL_PVP",                      // 4
    "CHANNEL_FREEPVP",                  // 5
    "CHANNEL_GUILD_WAR_PVP",            // 6
    "CHANNEL_TOURNAMENT_PVP",           // 7
    "CHANNEL_INTEGRATED_PVP",           // 8
    "CHANNEL_SCHOOL_LEAGUE_PVP",        // 9
    "CHANNEL_SCHOOL_TOURNAMENT_PVP",    // 10
    "CHANNEL_GORGE_OF_THE_DEAD",        // 11
    "CHANNEL_CASUAL_PVP",               // 12
    "CHANNEL_INTEGRATED_FREEPVP",       // 13
    "CHANNEL_ONLINE_PRELIMINARY_PVP",   // 14
    "CHANNEL_FAIR_PVP",                 // 15
    "CHANNEL_FAIR_FREEPVP",             // 16
    "CHANNEL_DEBUGGING",                // 17
    "CHANNEL_ATTACK_ZONE",              // 18
    "CHANNEL_MAX",                      // 19
};

// ORIG T 0x088dd190：越界返回 "UNKNOWN_CHANNEL_TYPE"，否则返回类型名字符串。
const char* toChannelTypeString(int type)
{
    if (type >= 0 && type <= 0x13)
        return g_ChannelTypeString[type];
    return "UNKNOWN_CHANNEL_TYPE";
}

// ORIG T 0x088dd1b2：strcmp 线性查表，命中返回下标，未命中返回 -1。
#include "InitScriptProducers_1/getChannelType.inc"

// b28d-solo-InitScriptProducers_1: NEAR→AE（258=258 全对齐）。修：两个内层循环
// 下标提至函数头（P212/P331：块内 for-init 声明在 4.4 槽方向与 4.1 相反；全头部
// 声明按 ORIG 槽深序 jDungeon(-0x20)→jChannel(-0x1c) 即逐条复现）。三迭代器 C1Ev
// 已在函数头，声明序 it→inner→outer 复现 ORIG 0x088dc846..0x088dc86c 与槽
// -0x40/-0x44/-0x48。
// ORIG T 0x088dc81b。遍历打印：dungeon 段（+0x00 map）逐项
// "[DUNGEON] <键名> as <键> : " + 区间 int 列表；server 段（+0x18 外层 map）
// "[CHANNEL] Server %d" + 每频道 "\t%d : %s type(%d) name(%s) tag(%s) " +
// bonusRate 表。日志行号 0x107/0x10c/0x11a 与 ORIG 一致。
#include "InitScriptProducers_1/traversalChannelScript.inc"

// ============================================================================
// WorldDrop（Etc/WorldDrop.etc、Etc/WorldDropWarArea.etc）
// ============================================================================

// ============================================================================
// WorldDrop（Etc/WorldDrop.etc、Etc/WorldDropWarArea.etc）
// ============================================================================

// b28d-solo-InitScriptProducers_1: DIFF→AE（0x206=0x206，指令数全等）。修：外层
// slot 循环由逗号条件 `while ((slot=ScanInt(&ok), ok==true))`（4.4 旋转成底部条件）
// 改为 `while (true) { slot = ScanInt(&ok); if (ok != true) break; ... }`，复现
// ORIG 条件块置顶 + 体尾无条件回跳（P25/P283；ORIG 0x08902364 读 slot →
// 0x08902372 `movzbl ok; xor $1; test; je 体`）。
// ORIG T 0x089022be。[world drop] 段：slot 步长 0x14 寻址；槽内首值写 +0x00，
// 总量清零后成对读 (itemIdx, rate)：rate 累加 +0x10、chunck 入 +0x04 vector；
// 段内所有出口统一执行 +0x00 = +0x10。itemIdx==-1 即终止本槽。slot 无越界检查（忠实）。
#include "InitScriptProducers_1/ImportWorldDrop.inc"

// ============================================================================
// Seria Blessing（Event/SeriaBlessing.evt）
// ============================================================================

// ORIG T 0x0891063b。[Seria Blessing] 段每行 key + 5 int 写满 map 值；
// 任一 int 断流即按 ORIG 行号常量返回（init.md 非 0 = Fail）。
#include "InitScriptProducers_1/ImportSeriaBlessingScript.inc"

// ============================================================================
// LevelUpRewardItem（Event/LevelUpRewardItem.evt）
// ============================================================================


// ORIG T 0x089107e9。八个职业分支各自携带独立内联解析循环与临时对象槽（ORIG
// 汇编中 make_pair/pair/insert 临时槽按 swordman..thief 分支序自 -0xf4 依次
// 分配至 -0x14，即每分支一条独立 insert(std::make_pair(key,data)) 语句）；
// key/data 对象构造在 ScanType 主循环前（ORIG +0x35..+0x5b）。未知 token 跳过。
// 收敛口径：orig=ours=607，助记符序列全等；仅 8 个 token 串/空串字面量在 .o
// 阶段以未解析重定位显示（.rodata 实测内容逐条一致），链接后按 AE 口径判等。
#include "InitScriptProducers_1/ImportLevelUpRewardItemScript.inc"

// ============================================================================
// GrowthCapsule（Event/GrowthCapsule.evt）
// ============================================================================

// b28d-solo-InitScriptProducers_1: DIFF→AE（0x1e0=0x1e0）。修：5 个独立 if 改为
// if/else-if 链并删显式 continue —— ORIG 每 case 尾 `je <独立 nop;jmp 循环头>`
// （0x08911249/4c/4f/52/55）是 4.1 对「else-if 链出口」的独立中继；4.4 对显式
// continue/goto 恒 jump-thread（5 标签串 135/单标签 140），而 else-if 链直落
// 逐岛复现（144 条 vs ORIG 143，AE 归一后全等）。
// ORIG T 0x0891120c。五命名段任意顺序；[capsule limit lv] 读入成功后继续主循环，
// EOF 返回 0；各字段断流按 ORIG 行号常量返回。
#include "InitScriptProducers_1/ImportGrowthCapsuleScript.inc"

// ============================================================================
// NewAccountLevelUpToJob（Event/NewAccountLevelUpToJob.evt）
// ============================================================================

// ORIG W 0x08371508：m_map0.clear()。
#include "InitScriptProducers_1/clear_1.inc"

// ORIG W 0x0891bd0e：find 命中则向既有 vector push_back，否则新建单元素 vector 插入。
// b28-34：比较写 `m_map0.end() == it`（临时为 this、it 为形参）——ORIG 先装载
// 0x4(%esp)=&it 后装载 (%esp)=&end 临时；写 `it == m_map0.end()` 装载序反成 DIFF。
// b28d-lane25：ORIG 0x0891bd0e。P167 差集实证 ORIG 调 make_pair<IRKi,RSt6vector…>
// （键为 const int& 绑定形参本身）⇒ 形参顶层 const（不入 mangled），定义处还原。
#include "InitScriptProducers_1/set_data.inc"
// ---- StackableItemData（ORIG C1 0x0891bccc / clear 0x0891bce0，W）----
// clear() 定义在 CtorProducers_1.cpp（NL4 unit-at-a-time 塌缩机制：callee 同 TU
// 可见时 4.4 将本 ctor outgoing 帧塌缩 sub $0x4，ORIG 0x86b4144 域族 sub $0x18）。

#include "InitScriptProducers_1/StackableItemData.inc"

// b28d-solo-InitScriptProducers_1: AE（修：删 result 局部改直接 return + `goto
// failZero` 尾标签，复现 ORIG 失败块落循环回跳之后 `nop; mov $0,%ebx; jmp`；槽组
// -0xc/-0x10/-0x11/-0x1c/-0x28 与帧 0x30 全对齐）。
// ORIG T 0x08917c99。endTag 形参在 ORIG 函数体内零引用（段终止靠 ScanInt 断流，
// 由调用方主循环消费闭合 tag），保留签名并显式标注。
#include "InitScriptProducers_1/LoadRewardDataByNoType.inc"

// b28d-solo-InitScriptProducers_1: DIFF→AE（0x2ae=0x2ae）。修：共享的链外
// `if (err != 0) return err;` 移入三个分支体各一份 —— ORIG 每分支独立
// `cmpl $0,-0xc; je <岛>` + fall-through return（0x08917af8/0x08917ba7/0x08917c53，
// 岛 0x08917a36/39/3f 各 `nop; jmp 循环头`）；共享检查在 4.4 下 tail-merge 成单回边。
// err 具名 int（ORIG -0xc）与 token→err→clear 声明序已复原。
// ORIG T 0x089179eb。首成员 clear 后按三段分发至共享 loader；loader 非 0 即透传。
#include "InitScriptProducers_1/ImportNewAccountLeventUpToJobScript.inc"

// ============================================================================
// BroadCastItem（Etc/BroadCastItem.etc）
// ============================================================================

// ORIG T 0x08918f24。注意：无论装载成败一律返回 0（ORIG 两出口 ebx 均为 0）。
#include "InitScriptProducers_1/ImportBroadCastItemScript.inc"

// ============================================================================
// 共享字符串工具（LoadRewardDataByGrowType 依赖，Main 裁定本批承载）
// ============================================================================

// b28d-solo-InitScriptProducers_1: AE（修：循环变量 size_type→int，复现 ORIG
// `cmp -0xc(%ebp),%eax; setg` 有符号边界；size_type 生 seta）。
// ORIG T 0x08917db3：剔除 '[' 与 ']'（ORIG 以 tag="[]" 逐字符比对）；仅当确有
// 剔除时回写（ORIG local_11 门禁）。
#include "InitScriptProducers_1/deleteTagBracketString.inc"
