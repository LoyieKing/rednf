// ============================================================================
// InitScriptProducers_1.cpp —— CDataManager::init()（ORIG 0x08358332）依赖的
// 脚本装载自由函数 · 第 1 批（批次归属经 Main 裁定 + 兄弟批次广播确认）。
//
// 本 TU 承载（全部为 ORIG nm 真实符号，地址见各函数注释；实现语义逐指令反推自
// docs/class_func_reports/_lt_global_gt_/ 对应报告 + objdump 复核，无固定返回）：
//   importAccountCargoScript        0x088c7e80
//   AccountCargoScript::clear       0x08372442 (W)
//   importChannelScript             0x088dc008
//   applyChannelScript              0x088dc069
//   channel_info_t C1/D1/op=        0x088dd2a8 / 0x083f11da / 0x088dd3c6 (W)
//   channel_dungeon_info_t C1/D1/=  0x088dd34a / 0x083d8b2c / 0x088dd390 (W)
//   ImportWorldDrop                 0x089022be
//   ImportSeriaBlessingScript       0x0891063b
//   ImportLevelUpRewardItemScript   0x089107e9
//   ImportGrowthCapsuleScript       0x0891120c
//   ImportNewAccountLeventUpToJobScript 0x089179eb
//   LoadRewardDataByNoType          0x08917c99
//   deleteTagBracketString          0x08917db3
//   stStepRewardStackableItemScript::clear/set_data 0x08371508/0x0891bd0e (W)
//   ImportBroadCastItemScript       0x08918f24
//
//   loadRDARScriptFile/ScanType/ScanInt/ScanStr/ScanFloat —— DNFLexWrapper.h
//   getCharacterJobToEnumValue 0x089bbc0f —— CDataManager.cpp
//   LoadRewardDataByGrowType 0x08917f44 —— InitScripts_2 批次（Main 裁定归属）
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
bool importChannelScript(channel_script_t* script)
{
    if (!loadRDARScriptFile("", "Etc/channel_info.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__, 0x38,
                              "importChannelScript - can't find channel_info.etc");
        return 0;
    }
    return applyChannelScript(script);   // ORIG 直返 raw eax（P9）
}

// ============================================================================
// AccountCargoScript（Etc/AccountCargo.etc 域）
// ============================================================================

// ORIG W 0x08372442：+0x00=0、+0x04=0、std::map::clear(+0x08)。
void AccountCargoScript::clear()
{
    m_reqLevel = 0;
    m_itemIdx = 0;
    m_upgradeMap.clear();
}

// b28d-solo-InitScriptProducers_1: DIFF→N 封顶（p167 20/20 CLEAR，语义闭合）。
// 本波复核（clean 归一后 132=132 指令，仅 3 处）：①我方多 1 条入口 jmp（ORIG
// `jmp+nop` 自跳转被归一剥离，P164 do-while 入口布局）；②ORIG do-while 条件
// `call operator==; test %al,%al; je <循环头>` 无 xor，4.4 恒插 `xor $0x1`
// （P320/S138：4.1 循环条件取反直接反转跳转）；③极性 je↔jne（由②派生）。
// 四形实测 `!()`/`==false`/`==0`/体内 break = 137/137/137/139 vs ORIG 135 均不可翻。
// ORIG T 0x088c7e80。段结构：[required level] / [required item] /
// [upgrade info](grade 与 UpgradeInfo 首字段同一栈位重叠，随后再扫三 int) /
// [account cargo end]。未识别 token 直接跳过；ScanInt 断流按 ORIG 精确分支处理。
bool importAccountCargoScript(AccountCargoScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    // ORIG 0x088c7eac：script 先 spill 到具名指针局部 -0x10，之后全部经它访问（P0）。
    AccountCargoScript* s = script;
    s->clear();  // ORIG +0x032 调用 AccountCargoScript::clear(0x08372442)

    std::string token;
    int deadFlag = 0;                            // ORIG -0xc 只写一次的 int 局部
    (void)deadFlag;
    bool ok = false;

    do
    {
        while (true)
        {
            while (true)
            {
                while (true)
                {
                    if (!ScanType(token, true))
                        goto exitLoop;                      // ORIG +0x19d：以 ok 当前值返回
                    if (token == "[required level]")
                    {
                        s->m_reqLevel = ScanInt((bool*)0);
                        continue;                           // 回到 ScanType
                    }
                    break;
                }
                if (token == "[required item]")
                {
                    s->m_itemIdx = ScanInt((bool*)0);
                    continue;
                }
                break;
            }

            if (token == "[upgrade info]")
            {
                // ORIG +0xe4..+0x177：grade 扫描失败退出本段；其后三个 int 不做 ok 检查。
                // ORIG info 落 -0x58..-0x48 且 key 与 info.m_field0 同栈位（pair 构造两参
                // 同为 &local_58 实证）⇒ 无独立 grade 局部，直接用 info.m_field0。
                while (true)
                {
                    UpgradeInfo info;
                    info.m_field0 = ScanInt(&ok);
                    if (!ok)
                        break;
                    info.m_field4 = ScanInt(&ok);
                    info.m_field8 = ScanInt(&ok);
                    info.m_fieldC = ScanInt(&ok);
                    // b28d-lane25：P167 差集实证 ORIG 调 pair<int,UpgradeInfo>::C1<int&,UpgradeInfo&>
                    // 直构（P192），非 make_pair。
                    s->m_upgradeMap.insert(std::pair<int, UpgradeInfo>(info.m_field0, info));
                }
                continue;                                   // 段毕回主循环（do 底部仅对未知 token 生效）
            }
            break;
        }
    } while ((token == "[account cargo end]") == 0);         // ORIG do-while 判定

    // ORIG 0x088c8017：正常退出（命中 end tag）无条件置 ok=true 后返回。
    ok = true;

exitLoop:
    return ok;
}

// ============================================================================
// Channel（Etc/channel_info.etc 域）
// ============================================================================

// ORIG W 0x088dd2a8：发射序=三个 string 构造（+0x00/+0x08/+0x0c）**先于**
// 标量清零（+0x04/+0x10/+0x14）⇒ 4.1 类成员构造分组在前；4.4 初始化列表按
// 声明序混合发射，故标量改 body 赋值复现分组序。
// 注意：m_gridPercent() 值初始化会被 gcc 展开成 9 次循环（36B），与 ORIG
// memset 0x28（40B）不一致，故显式 memset 全部 10 个 float。
channel_info_t::channel_info_t()
    : m_name(),
      m_dungeonName(),
      m_dungeonKey()
{
    m_channelType = 0;
    m_pBonusRates = 0;
    m_bonusRate = 0;
    memset(m_gridPercent, 0, sizeof(m_gridPercent));
}

// ORIG W 0x083f11da：逆序析构 +0x0c/+0x08/+0x00 三个 string（其余 POD 无动作）。
channel_info_t::~channel_info_t()
{
}

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
bool applyChannelScript(channel_script_t* script)
{
    std::string token;

    for (;;)
    {
        if (!ScanType(token, true))
            return 1;                                    // +0x778→+0x779 esi=1

        // b28d-lane25：P167 差集实证 ORIG 用 operator==（非 !=）且结构为
        // if==[dungeon]/else if==[server]/else{log;return 1}（0x088dc034 je +0x19a）。
        if (token == "[dungeon]")
        {
            // +0x05a..+0x136：两个 STR 后连读 int 至断流，逐条打日志并入表。
            std::string name;
            channel_dungeon_info_t info;

            if (!ScanStr(&name))
                return 1;                                // ebx=0 → esi=1
            if (!ScanStr(&info.m_keyName))
                return 1;

            bool more = true;
            while (true)
            {
                int value = ScanInt(&more);
                if (!more)
                    break;
                LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__,
                                      0x65, "[DUNGEON LIST] %s\t%d", name.c_str(), value);
                info.m_intList.push_back(value);
            }

            script->m_channelDungeonMap[name] = info;     // operator[] + operator=
            continue;                                     // ebx=1 → 主循环
        }

        // else if (token == "[server]")：+0x1b5..+0x736：先 serverNo；行循环
        // chNo→name→type→分类→bonusRate→float[10]；chNo 断流时把已积累 innerMap
        // 以 serverNo 提交到 +0x18 外层 map。
        else if (token == "[server]")
        {
        std::map<int, channel_info_t> channelInfos;
        int serverNo = 0;
        bool ok = false;

        serverNo = ScanInt(&ok);
        if (!ok)
            return 1;                                     // +0x71f ebx=0 → esi=1

        enum RowState { ROW_INSERT = 0, ROW_ERROR = 1, ROW_NEXT = 2 };

        for (;;)
        {
            channel_info_t info;
            RowState state;

            int chNo = ScanInt(&ok);
            if (!ok)
            {
                state = ROW_INSERT;                       // 频道段结束
            }
            else if (channelInfos.find(chNo) != channelInfos.end())
            {
                LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__,
                                      0x85,
                                      "importChannelScript - invalid channel number. %d already exists",
                                      chNo);
                state = ROW_ERROR;
            }
            else
            {
                if (!ScanStr(&info.m_name))
                {
                    LogManager::logFormat(0, "../RDARScriptChannel.cpp",
                                          __FUNCTION__, 0x8c,
                                          "importChannelScript - invalid channel name (requires STR)");
                    state = ROW_ERROR;
                }
                else
                {
                    info.m_channelType = ScanInt(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(0, "../RDARScriptChannel.cpp",
                                              __FUNCTION__, 0x9c,
                                              "importChannelScript - invalid channel type (requires INT)");
                        state = ROW_ERROR;
                    }
                    else if (!ScanStr(&info.m_dungeonName))
                    {
                        LogManager::logFormat(0, "../RDARScriptChannel.cpp",
                                              __FUNCTION__, 0xa3,
                                              "importChannelScript - invalid dungeon classification (requires STR)");
                        state = ROW_ERROR;
                    }
                    else
                    {
                        LogManager::logFormat(0, "../RDARScriptChannel.cpp",
                                              __FUNCTION__, 0xab,
                                              "[CHANNEL LIST] %d\t%s\t%d", chNo,
                                              info.m_dungeonName.c_str(), info.m_channelType);

                        if (info.m_dungeonName != "[none]")
                        {
                            // b28d-lane25：P167 差集实证 ORIG 走 iterator::C1Ev + find 两段
                            // （P50 反例复活：先声明后赋值，禁初始化式合并）。
                            std::map<std::string, channel_dungeon_info_t>::iterator dit;
                            dit = script->m_channelDungeonMap.find(info.m_dungeonName);
                            if (dit == script->m_channelDungeonMap.end())
                            {
                                LogManager::logFormat(
                                    0, "../RDARScriptChannel.cpp", __FUNCTION__, 0xb6,
                                    "importChannelScript - invalid dungeon classification. %s does not exist",
                                    info.m_dungeonName.c_str());
                                state = ROW_ERROR;
                            }
                            else
                            {
                                // +0x4ec..+0x524：+0x10 存分类区间 vector 地址（别名非拷贝），
                                // +0x0c 复制其键名字符串。
                                info.m_pBonusRates = &dit->second.m_intList;
                                info.m_dungeonKey = dit->second.m_keyName;
                            }
                        }

                        if (state != ROW_ERROR)
                        {
                            // +0x14 字节槽（ORIG 为 4 字节写入，高位属填充不参与读取）
                            info.m_bonusRate = (unsigned char)ScanInt(&ok);
                            if (!ok)
                            {
                                LogManager::logFormat(0, "../RDARScriptChannel.cpp",
                                                      __FUNCTION__, 0xc3,
                                                      "importChannelScript - invalid dungeon bonus rate. (required INT)");
                                state = ROW_ERROR;
                            }
                            else
                            {
                                bool rowOk = true;
                                for (int i = 0; i < 10; ++i)
                                {
                                    float percent = ScanFloat(&ok);   // 先存后检（fstps 序）
                                    info.m_gridPercent[i] = percent;
                                    if (!ok)
                                    {
                                        LogManager::logFormat(
                                            0, "../RDARScriptChannel.cpp", __FUNCTION__,
                                            0xcc,
                                            "importChannelScript - invalid level GridPercent(%d). (required Float)",
                                            i);
                                        rowOk = false;
                                        break;
                                    }
                                }
                                if (rowOk)
                                {
                                    channelInfos[chNo] = info;
                                    state = ROW_NEXT;
                                }
                                else
                                {
                                    state = ROW_ERROR;
                                }
                            }
                        }
                    }
                }
            }

            if (state == ROW_NEXT)
                continue;                                 // 下一频道行

            if (state == ROW_INSERT)
            {
                script->m_channelMap.insert(               // +0x18 外层 map
                    std::make_pair(serverNo, channelInfos));
                break;                                     // ebx=2 → 主循环
            }
            return 0;                                      // ROW_ERROR → esi=0
        }
        }
        else
        {
            LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__,
                                  0xef,
                                  "importChannelScript - invalid type. %s is not acceptable",
                                  token.c_str());
            return 1;                                      // +0x776→+0x779 esi=1
        }
    }
}

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
int getChannelType(const char* name)
{
    for (int i = 0; i <= 0x13; ++i)
    {
        if (strcmp(name, g_ChannelTypeString[i]) == 0)
            return i;
    }
    return -1;
}

// b28d-solo-InitScriptProducers_1: NEAR→AE（258=258 全对齐）。修：两个内层循环
// 下标提至函数头（P212/P331：块内 for-init 声明在 4.4 槽方向与 4.1 相反；全头部
// 声明按 ORIG 槽深序 jDungeon(-0x20)→jChannel(-0x1c) 即逐条复现）。三迭代器 C1Ev
// 已在函数头，声明序 it→inner→outer 复现 ORIG 0x088dc846..0x088dc86c 与槽
// -0x40/-0x44/-0x48。
// ORIG T 0x088dc81b。遍历打印：dungeon 段（+0x00 map）逐项
// "[DUNGEON] <键名> as <键> : " + 区间 int 列表；server 段（+0x18 外层 map）
// "[CHANNEL] Server %d" + 每频道 "\t%d : %s type(%d) name(%s) tag(%s) " +
// bonusRate 表。日志行号 0x107/0x10c/0x11a 与 ORIG 一致。
void traversalChannelScript(channel_script_t* script)
{
    char buf[0x200];

    // b28d-lane25：P167 差集实证 ORIG 增量段走 ppEi（后置++，P28 增量侧推论）+
    // 各迭代器独立 C1Ev 且三条 C1Ev 在函数头连续发射（ORIG 0x088dc846..0x088dc86c，
    // 槽 -0x40/-0x44/-0x48）⇒ 三个迭代器一律"先声明后赋值"且全部声明在函数头，
    // 声明序 it → inner → outer。
    std::map<std::string, channel_dungeon_info_t>::iterator it;
    std::map<int, channel_info_t>::iterator inner;
    std::map<int, std::map<int, channel_info_t> >::iterator outer;
    // b28d-lane25 复测：ORIG 两个内层循环下标槽 -0x20（dungeon 段）/ -0x1c（channel
    // 段）——块内 for-init 声明在 4.4 下槽方向相反（P212），提至函数头按 ORIG 槽深序
    // 声明即可复现。
    unsigned int jDungeon;
    unsigned int jChannel;

    for (it = script->m_channelDungeonMap.begin();
         it != script->m_channelDungeonMap.end(); it++)
    {
        snprintf(buf, sizeof(buf), "[DUNGEON] %s as %s : ",
                 it->second.m_keyName.c_str(), it->first.c_str());
        for (jDungeon = 0; jDungeon < it->second.m_intList.size(); ++jDungeon)
            snprintf(buf, sizeof(buf), "%s%d ", buf, it->second.m_intList[jDungeon]);
        LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__, 0x107, buf);
    }

    for (outer = script->m_channelMap.begin();
         outer != script->m_channelMap.end(); outer++)
    {
        LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__, 0x10c,
                              "[CHANNEL] Server %d", outer->first);
        for (inner = outer->second.begin();
             inner != outer->second.end(); inner++)
        {
            channel_info_t& ci = inner->second;
            snprintf(buf, sizeof(buf), "\t%d : %s type(%d) name(%s) tag(%s) ",
                     inner->first, ci.m_name.c_str(), ci.m_channelType,
                     ci.m_dungeonKey.c_str(), ci.m_dungeonName.c_str());
            if (ci.m_pBonusRates != 0)
            {
                for (jChannel = 0; jChannel < ci.m_pBonusRates->size(); ++jChannel)
                    snprintf(buf, sizeof(buf), "%s%d ", buf, ci.m_pBonusRates->at(jChannel));
            }
            LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__, 0x11a,
                                  buf);
        }
    }
}

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
bool ImportWorldDrop(WorldDropInfo* out, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        // 字面量直传（不建具名 static 数组）：ORIG nm 无
        // `_ZZ15ImportWorldDropP13WorldDropInfoPKcE8kFailMsg`，字符串为匿名 .rodata
        // （ORIG 0x08e0fafc），仅 `__FUNCTION__` 局部静态符号存在（§4.1 仅NEW 清理）。
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp", __FUNCTION__, 0xd36,
                              "CRITICAL ERROR : \xbf\xf9\xb5\xe5\xb5\xe5\xb7\xd3 "
                              "\xbd\xba\xc5\xa9\xb8\xb3\xc6\xae \xb7\xce\xb5\xe5 \xbd\xc7\xc6\xd0!");
        return false;
    }

    std::string token;                         // ORIG local_1c
    int dead18 = 0; (void)dead18;              // ORIG local_18 prologue $0 写
    bool ok = false;                           // ORIG local_1d

    // ORIG 0x08902342：`call ScanType; xor $1; test; jne` ⇒ bool callee 取反早退形。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[world drop]")
        {
            int slot;
            // b28d-lane25 复测：ORIG 条件块在循环体**顶部**（+0xa6 读 slot → +0xb4
            // `movzbl ok; xor $1; test; je 体`），体尾 +0x1ca 无条件回跳条件；4.4 对
            // 逗号 while 恒旋转成底部条件。改 while(true)+`if (ok != true) break;` 复现。
            while (true)
            {
                slot = ScanInt(&ok);
                if (ok != true)
                    break;
                out[slot].m_first = ScanInt(&ok);       // +0x00（无 ok 检查，忠实）
                out[slot].m_total = 0;                  // +0x10
                int n = 0;                              // ORIG local_10
                WorldDropItemChunck chunk;              // ORIG local_28/local_24 相邻槽
                // ORIG 0x08902449：`cmpl $0xff,-0xc; setle %al; test %al,%al; jne` ⇒
                // 边界条件独立 while + 内层 itemIdx 早退，无具名 bool 局部（P0）。
                while (n <= 0xff)
                {
                    chunk.m_itemIdx = ScanInt(&ok);
                    if (chunk.m_itemIdx == -1)
                        break;
                    chunk.m_rate = ScanInt(&ok);        // ORIG local_24
                    out[slot].m_total += chunk.m_rate;  // ORIG 每次重算 slot*0x14（无缓存）
                    out[slot].m_chunks.push_back(chunk);
                    if (ok != true)
                    {
                        break;
                    }
                    n = n + 1;
                }
                out[slot].m_first = out[slot].m_total;  // 收敛写回
            }
        }
    }
    return true;
}

// ============================================================================
// Seria Blessing（Event/SeriaBlessing.evt）
// ============================================================================

// ORIG T 0x0891063b。[Seria Blessing] 段每行 key + 5 int 写满 map 值；
// 任一 int 断流即按 ORIG 行号常量返回（init.md 非 0 = Fail）。
int ImportSeriaBlessingScript(const char* path, stSeriaBlessingScript& out)
{
    if (!loadRDARScriptFile("", path))
        return 0x2a74;

    std::string token;
    bool ok = false;

    for (;;)                                   // ORIG 4.1 while 落点形（check 在头）
    {
        if (!ScanType(token, true))
            break;
        stSeriaBlessingScript::Value value;      // 每轮构造（ORIG 同；随即全覆盖）

        // b28d-lane25：P167 差集实证 ORIG 调 operator==（非 !=）；P191/P222 正向包裹。
        if (token == "[Seria Blessing]")
        {
            while (true)
            {
                int key = ScanInt(&ok);
                if (!ok)
                    break;

                value.m_minLv = ScanInt(&ok);
                if (!ok) return 0x2a89;
                value.m_maxLv = ScanInt(&ok);
                if (!ok) return 0x2a8d;
                value.m_field8 = ScanInt(&ok);
                if (!ok) return 0x2a91;
                value.m_fieldC = ScanInt(&ok);
                if (!ok) return 0x2a95;
                value.m_field10 = ScanInt(&ok);
                if (!ok) return 0x2a99;

                out.m_map0[key] = value;         // operator[] 值初始化为 0（同 Value ctor）后覆盖
            }
        }
    }
    return 0;
}

// ============================================================================
// LevelUpRewardItem（Event/LevelUpRewardItem.evt）
// ============================================================================


// ORIG T 0x089107e9。八个职业分支各自携带独立内联解析循环与临时对象槽（ORIG
// 汇编中 make_pair/pair/insert 临时槽按 swordman..thief 分支序自 -0xf4 依次
// 分配至 -0x14，即每分支一条独立 insert(std::make_pair(key,data)) 语句）；
// key/data 对象构造在 ScanType 主循环前（ORIG +0x35..+0x5b）。未知 token 跳过。
// 收敛口径：orig=ours=607，助记符序列全等；仅 8 个 token 串/空串字面量在 .o
// 阶段以未解析重定位显示（.rodata 实测内容逐条一致），链接后按 AE 口径判等。
int ImportLevelUpRewardItemScript(const char* path, stLevelUpRewardItemScript& script)
{
    if (!loadRDARScriptFile("", path))
        return 0x2aa7;

    std::string token;
    bool ok = false;
    LevelUpRewardItemTableKey key;   // ORIG +0x38 ctor（-0x112，循环外唯一构造）
    LevelUpRewardItemData data;      // ORIG +0x5b ctor（-0x11c）

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        if (token == "[swordman]")
        {
            key.m_job = 0;                       // ORIG +0xa2 movb $0,-0x110
            key.m_grow = ScanInt(&ok);           // ORIG +0xab mov %al,-0x10f
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2ac4;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2ac8;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
        else if (token == "[fighter]")
        {
            key.m_job = 1;
            key.m_grow = ScanInt(&ok);
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2add;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2ae1;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
        else if (token == "[at fighter]")
        {
            key.m_job = 7;
            key.m_grow = ScanInt(&ok);
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2af6;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2afa;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
        else if (token == "[gunner]")
        {
            key.m_job = 2;
            key.m_grow = ScanInt(&ok);
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2b0f;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2b13;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
        else if (token == "[at gunner]")
        {
            key.m_job = 5;
            key.m_grow = ScanInt(&ok);
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2b28;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2b2c;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
        else if (token == "[mage]")
        {
            key.m_job = 3;
            key.m_grow = ScanInt(&ok);
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2b41;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2b45;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
        else if (token == "[priest]")
        {
            key.m_job = 4;
            key.m_grow = ScanInt(&ok);
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2b5a;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2b5e;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
        else if (token == "[thief]")
        {
            key.m_job = 6;
            key.m_grow = ScanInt(&ok);
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2b73;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2b77;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
    }
    return 0;
}

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
int ImportGrowthCapsuleScript(const char* path, stGrowthCapsuleScript& script)
{
    if (!loadRDARScriptFile("", path))
        return 0x2ba2;

    std::string token;
    bool ok = false;

    for (;;)
    {
        if (!ScanType(token, true))
            goto failEof;                        // ORIG 失败块 0x089113b5 落在循环体之后

        if (token == "[gage percent]")
        {
            script.m_gagePercent = ScanInt(&ok);
            if (!ok) return 0x2bb0;
        }
        else if (token == "[max gage exp]")
        {
            script.m_maxGageExp = ScanInt(&ok);
            if (!ok) return 0x2bb6;
        }
        else if (token == "[capsule id]")
        {
            script.m_capsuleId = ScanInt(&ok);
            if (!ok) return 0x2bbc;
        }
        else if (token == "[capsule exp]")
        {
            script.m_capsuleExp = ScanInt(&ok);
            if (!ok) return 0x2bc2;
        }
        // b28d-lane25：P167 差集实证 ORIG 调 operator==（非 !=）；正向包裹。
        else if (token == "[capsule limit lv]")
        {
            script.m_capsuleLimitLv = ScanInt(&ok);
            if (!ok)
                return 0x2bc8;
        }
    }
failEof:
    return 0;
}

// ============================================================================
// NewAccountLevelUpToJob（Event/NewAccountLevelUpToJob.evt）
// ============================================================================

// ORIG W 0x08371508：m_map0.clear()。
void stStepRewardStackableItemScript::clear()
{
    m_map0.clear();
}

// ORIG W 0x0891bd0e：find 命中则向既有 vector push_back，否则新建单元素 vector 插入。
// b28-34：比较写 `m_map0.end() == it`（临时为 this、it 为形参）——ORIG 先装载
// 0x4(%esp)=&it 后装载 (%esp)=&end 临时；写 `it == m_map0.end()` 装载序反成 DIFF。
// b28d-lane25：ORIG 0x0891bd0e。P167 差集实证 ORIG 调 make_pair<IRKi,RSt6vector…>
// （键为 const int& 绑定形参本身）⇒ 形参顶层 const（不入 mangled），定义处还原。
void stStepRewardStackableItemScript::set_data(const int step, StackableItemData& data)
{
    std::map<int, std::vector<StackableItemData> >::iterator it = m_map0.find(step);
    if (m_map0.end() == it)
    {
        std::vector<StackableItemData> items;
        items.push_back(data);
        m_map0.insert(std::make_pair(step, items));
    }
    else
    {
        it->second.push_back(data);
    }
}
// ---- StackableItemData（ORIG C1 0x0891bccc / clear 0x0891bce0，W）----
// clear() 定义在 CtorProducers_1.cpp（NL4 unit-at-a-time 塌缩机制：callee 同 TU
// 可见时 4.4 将本 ctor outgoing 帧塌缩 sub $0x4，ORIG 0x86b4144 域族 sub $0x18）。

StackableItemData::StackableItemData()
{
    clear();
}

// b28d-solo-InitScriptProducers_1: AE（修：删 result 局部改直接 return + `goto
// failZero` 尾标签，复现 ORIG 失败块落循环回跳之后 `nop; mov $0,%ebx; jmp`；槽组
// -0xc/-0x10/-0x11/-0x1c/-0x28 与帧 0x30 全对齐）。
// ORIG T 0x08917c99。endTag 形参在 ORIG 函数体内零引用（段终止靠 ScanInt 断流，
// 由调用方主循环消费闭合 tag），保留签名并显式标注。
int LoadRewardDataByNoType(stStepRewardStackableItemScript& script, std::string endTag)
{
    (void)endTag;

    int step;                                    // ORIG -0xc
    std::string deadTok; (void)deadTok;          // ORIG -0x10 默认 ctor 死串（prologue 首 ctor）
    bool ok = false;                             // ORIG local_15
    StackableItemData data;                      // ORIG local_20 函数级 ctor
    std::vector<StackableItemData> vec;          // ORIG local_2c 函数级 vector（每轮 clear）

    for (;;)
    {
        step = 0;                                // ORIG prologue-of-body $0 写
        data.clear();                            // ORIG StackableItemData::clear
        vec.clear();                             // ORIG vector::clear
        step = ScanInt(&ok);
        if (ok != true)
            goto failZero;                       // ORIG 失败块 0x08917d5b 落在循环回跳之后
        data.m_field0 = ScanInt(&ok);
        if (ok != true)
            return 0x3662;
        data.m_field4 = ScanInt(&ok);
        if (ok != true)
            return 0x3666;
        script.set_data(step, data);
    }
failZero:
    return 0;
}

// b28d-solo-InitScriptProducers_1: DIFF→AE（0x2ae=0x2ae）。修：共享的链外
// `if (err != 0) return err;` 移入三个分支体各一份 —— ORIG 每分支独立
// `cmpl $0,-0xc; je <岛>` + fall-through return（0x08917af8/0x08917ba7/0x08917c53，
// 岛 0x08917a36/39/3f 各 `nop; jmp 循环头`）；共享检查在 4.4 下 tail-merge 成单回边。
// err 具名 int（ORIG -0xc）与 token→err→clear 声明序已复原。
// ORIG T 0x089179eb。首成员 clear 后按三段分发至共享 loader；loader 非 0 即透传。
int ImportNewAccountLeventUpToJobScript(const char* path,
                                        stNewAccountLevelUpToJobScript& script)
{
    if (!loadRDARScriptFile("", path))
        return 0x3629;

    std::string token;
    int err = 0;                                 // ORIG -0xc（token ctor 后、clear 前清零）
    script.m_stepReward.clear();                 // ORIG +0x044（0x08371508）

    for (;;)                                     // ORIG 0x08917a53：bool callee 取反形
    {
        if (!ScanType(token, true))
            break;

        if (token == "[reward stackable item]")
        {
            // b28d-lane25：P167 差集实证 ORIG 直接传字面量（C1EPKc 临时，无 C1ERKSs 拷贝）。
            err = LoadRewardDataByNoType(script.m_stepReward, "[/reward stackable item]");
            if (err != 0)
                return err;
        }
        else if (token == "[reward equip item]")
        {
            err = LoadRewardDataByGrowType(script.m_levelUp18, "[/reward equip item]");
            if (err != 0)
                return err;
        }
        else if (token == "[reward avatar item]")
        {
            err = LoadRewardDataByGrowType(script.m_levelUp30, "[/reward avatar item]");
            if (err != 0)
                return err;
        }
        else
        {
            continue;
        }
    }
    return 0;
}

// ============================================================================
// BroadCastItem（Etc/BroadCastItem.etc）
// ============================================================================

// ORIG T 0x08918f24。注意：无论装载成败一律返回 0（ORIG 两出口 ebx 均为 0）。
int ImportBroadCastItemScript(const char* path, stBroadCastItemScript& script)
{
    if (!loadRDARScriptFile("", path))
        return 0;

    std::string token;
    bool ok = false;

    for (;;)                                   // ORIG 4.1 while 落点形：check 在头
    {
        if (!ScanType(token, true))
            break;
        if (token == "[common items]")
        {
            while (true)
            {
                int itemIdx = ScanInt(&ok);
                if (!ok)
                    break;
                script.m_vec0.push_back(itemIdx);
            }
        }
        else if (token == "[uncommon items]")
        {
            int group = ScanInt(&ok);
            if (!ok)
                break;                           // ORIG：直接退出主循环

            std::vector<int> items;
            while (true)
            {
                int itemIdx = ScanInt(&ok);
                if (!ok)
                    break;
                items.push_back(itemIdx);
            }
            script.m_mapC.insert(std::make_pair(group, items));
        }
    }
    return 0;
}

// ============================================================================
// 共享字符串工具（LoadRewardDataByGrowType 依赖，Main 裁定本批承载）
// ============================================================================

// b28d-solo-InitScriptProducers_1: AE（修：循环变量 size_type→int，复现 ORIG
// `cmp -0xc(%ebp),%eax; setg` 有符号边界；size_type 生 seta）。
// ORIG T 0x08917db3：剔除 '[' 与 ']'（ORIG 以 tag="[]" 逐字符比对）；仅当确有
// 剔除时回写（ORIG local_11 门禁）。
void deleteTagBracketString(std::string& text)
{
    std::string tag = "[]";
    std::string filtered;
    bool replaced = false;

    // ORIG 0x08917ecd：cmp -0xc(%ebp),%eax; setg ⇒ 有符号 int 边界比较（size_type
    // 无符号会产生 seta）。
    for (int i = 0; i < (int)text.size(); ++i)
    {
        // b28d-lane25：P167 差集实证 ORIG 无 char 缓存——text[i] 在条件两臂与
        // push_back 处各读一次（operator[] 共 3 次）。
        if (text[i] == tag[0] || text[i] == tag[1])
        {
            replaced = true;
        }
        else
        {
            filtered.push_back(text[i]);
        }
    }

    if (replaced)
        text = filtered;
}
