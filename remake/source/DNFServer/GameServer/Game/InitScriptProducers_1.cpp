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

// ORIG T 0x088c7e80。段结构：[required level] / [required item] /
// [upgrade info](grade 与 UpgradeInfo 首字段同一栈位重叠，随后再扫三 int) /
// [account cargo end]。未识别 token 直接跳过；ScanInt 断流按 ORIG 精确分支处理。
bool importAccountCargoScript(AccountCargoScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    bool ok = false;
    bool done = false;

    script->clear();  // ORIG +0x032 调用 AccountCargoScript::clear(0x08372442)

    do
    {
        while (true)
        {
            while (true)
            {
                while (true)
                {
                    if (!ScanType(token, true))
                    {
                        done = false;                       // ORIG +0x19d：以当前标志返回
                        goto exitLoop;
                    }
                    if (token == "[required level]")
                    {
                        script->m_reqLevel = ScanInt((bool*)0);
                        continue;                           // 回到 ScanType
                    }
                    break;
                }
                if (token == "[required item]")
                {
                    script->m_itemIdx = ScanInt((bool*)0);
                    continue;
                }
                break;
            }

            if (token == "[upgrade info]")
            {
                // ORIG +0xe4..+0x177：grade 扫描失败退出本段；其后三个 int 不做 ok 检查。
                // key 与 value.m_field0 同栈位（pair 构造两参同为 &local_58 实证）。
                while (true)
                {
                    int grade = ScanInt(&ok);
                    if (!ok)
                        break;
                    int f4 = ScanInt(&ok);
                    int f8 = ScanInt(&ok);
                    int fC = ScanInt(&ok);

                    UpgradeInfo info;
                    info.m_field0 = grade;
                    info.m_field4 = f4;
                    info.m_field8 = f8;
                    info.m_fieldC = fC;
                    script->m_upgradeMap.insert(std::make_pair(grade, info));
                }
                continue;                                   // 段毕回主循环（do 底部仅对未知 token 生效）
            }
            break;
        }

        done = (token == "[account cargo end]");            // ORIG do-while 判定
    } while (!done);

exitLoop:
    return done;
}

// ============================================================================
// Channel（Etc/channel_info.etc 域）
// ============================================================================

// ORIG W 0x088dd2a8：三个 string 构造（+0x00/+0x08/+0x0c）+ type/pBonusRates 清零
// + bonusRate 字节清零 + memset(m_gridPercent, 0, 0x28)。初始化列表等价转写。
// 注意：m_gridPercent() 值初始化会被 gcc 展开成 9 次循环（36B），与 ORIG
// memset 0x28（40B）不一致，故显式 memset 全部 10 个 float。
channel_info_t::channel_info_t()
    : m_name(),
      m_channelType(0),
      m_dungeonName(),
      m_dungeonKey(),
      m_pBonusRates(0),
      m_bonusRate(0)
{
    memset(m_gridPercent, 0, sizeof(m_gridPercent));
}

// ORIG W 0x083f11da：逆序析构 +0x0c/+0x08/+0x00 三个 string（其余 POD 无动作）。
channel_info_t::~channel_info_t()
{
}

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
    for (int i = 0; i < 10; ++i)
        m_gridPercent[i] = o.m_gridPercent[i];
    return *this;
}

// ORIG W 0x088dd34a：string(+0x00) + vector<int>(+0x04) 默认构造。
channel_dungeon_info_t::channel_dungeon_info_t()
    : m_keyName(),
      m_intList()
{
}

// ORIG W 0x083d8b2c：逆序析构 vector/string（成员析构由编译器生成，等价空体）。
channel_dungeon_info_t::~channel_dungeon_info_t()
{
}

// ORIG W 0x088dd390：键名 string 赋值 + vector 整体赋值。
channel_dungeon_info_t& channel_dungeon_info_t::operator=(const channel_dungeon_info_t& o)
{
    m_keyName = o.m_keyName;
    m_intList = o.m_intList;
    return *this;
}

// ORIG T 0x088dc069。主循环分 [dungeon]/[server] 两段：
//  返回 1＝正常结束/EOF/未知类型（均 esi=1 出口）；0＝[server] 行解析错误（esi=0）。
int applyChannelScript(channel_script_t* script)
{
    std::string token;

    for (;;)
    {
        if (!ScanType(token, true))
            return 1;                                    // +0x778→+0x779 esi=1

        if (token != "[dungeon]" && token != "[server]")
        {
            LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__,
                                  0xef,
                                  "importChannelScript - invalid type. %s is not acceptable",
                                  token.c_str());
            return 1;                                    // +0x776→+0x779 esi=1
        }

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

        // token == "[server]"
        // +0x1b5..+0x736：先 serverNo；行循环 chNo→name→type→分类→bonusRate→float[10]；
        // chNo 断流时把已积累 innerMap 以 serverNo 提交到 +0x18 外层 map。
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
                            std::map<std::string, channel_dungeon_info_t>::iterator dit =
                                script->m_channelDungeonMap.find(info.m_dungeonName);
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
}

// ORIG T 0x088dc008：loadRDARScriptFile("","Etc/channel_info.etc") 门禁 +
// LogManager 报错 + 转发 applyChannelScript 返回值。
int importChannelScript(channel_script_t* script)
{
    if (!loadRDARScriptFile("", "Etc/channel_info.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__, 0x38,
                              "importChannelScript - can't find channel_info.etc");
        return 0;
    }
    return applyChannelScript(script);   // ORIG 直返 raw eax（P9）
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

// ORIG T 0x088dc81b。遍历打印：dungeon 段（+0x00 map）逐项
// "[DUNGEON] <键名> as <键> : " + 区间 int 列表；server 段（+0x18 外层 map）
// "[CHANNEL] Server %d" + 每频道 "\t%d : %s type(%d) name(%s) tag(%s) " +
// bonusRate 表。日志行号 0x107/0x10c/0x11a 与 ORIG 一致。
void traversalChannelScript(channel_script_t* script)
{
    char buf[0x200];

    for (std::map<std::string, channel_dungeon_info_t>::iterator it =
             script->m_channelDungeonMap.begin();
         it != script->m_channelDungeonMap.end(); ++it)
    {
        snprintf(buf, sizeof(buf), "[DUNGEON] %s as %s : ",
                 it->second.m_keyName.c_str(), it->first.c_str());
        for (unsigned int j = 0; j < it->second.m_intList.size(); ++j)
            snprintf(buf, sizeof(buf), "%s%d ", buf, it->second.m_intList[j]);
        LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__, 0x107, buf);
    }

    for (std::map<int, std::map<int, channel_info_t> >::iterator outer =
             script->m_channelMap.begin();
         outer != script->m_channelMap.end(); ++outer)
    {
        LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__, 0x10c,
                              "[CHANNEL] Server %d", outer->first);
        for (std::map<int, channel_info_t>::iterator inner = outer->second.begin();
             inner != outer->second.end(); ++inner)
        {
            channel_info_t& ci = inner->second;
            snprintf(buf, sizeof(buf), "\t%d : %s type(%d) name(%s) tag(%s) ",
                     inner->first, ci.m_name.c_str(), ci.m_channelType,
                     ci.m_dungeonKey.c_str(), ci.m_dungeonName.c_str());
            if (ci.m_pBonusRates != 0)
            {
                for (unsigned int j = 0; j < ci.m_pBonusRates->size(); ++j)
                    snprintf(buf, sizeof(buf), "%s%d ", buf, ci.m_pBonusRates->at(j));
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

// ORIG T 0x089022be。[world drop] 段：slot 步长 0x14 寻址；槽内首值写 +0x00，
// 总量清零后成对读 (itemIdx, rate)：rate 累加 +0x10、chunck 入 +0x04 vector；
// 段内所有出口统一执行 +0x00 = +0x10。itemIdx==-1 即终止本槽。slot 无越界检查（忠实）。
bool ImportWorldDrop(WorldDropInfo* out, const char* path)
{
    static const char kFailMsg[] =
        "CRITICAL ERROR : \xbf\xf9\xb5\xe5\xb5\xe5\xb7\xd3 "
        "\xbd\xba\xc5\xa9\xb8\xb3\xc6\xae \xb7\xce\xb5\xe5 \xbd\xc7\xc6\xd0!";

    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                              0xd36, kFailMsg);
        return false;
    }

    std::string token;
    bool ok = false;

    for (;;)                                   // ORIG 4.1 while 落点形（check 在头）
    {
        if (!ScanType(token, true))
            break;
        if (token != "[world drop]")
            continue;

        while (true)
        {
            int slot = ScanInt(&ok);
            if (!ok)
                break;

            WorldDropInfo& entry = out[slot];
            entry.m_first = ScanInt(&ok);        // +0x00 首扫描值（无 ok 检查，忠实）
            entry.m_total = 0;                   // +0x10

            for (int j = 0; j <= 255; ++j)
            {
                int itemIdx = ScanInt(&ok);
                if (itemIdx == -1)
                    break;

                int rate = ScanInt(&ok);
                entry.m_total += rate;

                WorldDropItemChunck chunk;       // 栈上相邻两 int 构成 8 字节元素
                chunk.m_itemIdx = itemIdx;
                chunk.m_rate = rate;
                entry.m_chunks.push_back(chunk);

                if (!ok)
                    break;
            }

            entry.m_first = entry.m_total;       // 收敛写回（所有出口一致）
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

        if (token != "[Seria Blessing]")
            continue;

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

            out.m_map0[key] = value;             // operator[] 值初始化为 0（同 Value ctor）后覆盖
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
            return 0;

        if (token == "[gage percent]")
        {
            script.m_gagePercent = ScanInt(&ok);
            if (!ok) return 0x2bb0;
            continue;
        }
        if (token == "[max gage exp]")
        {
            script.m_maxGageExp = ScanInt(&ok);
            if (!ok) return 0x2bb6;
            continue;
        }
        if (token == "[capsule id]")
        {
            script.m_capsuleId = ScanInt(&ok);
            if (!ok) return 0x2bbc;
            continue;
        }
        if (token == "[capsule exp]")
        {
            script.m_capsuleExp = ScanInt(&ok);
            if (!ok) return 0x2bc2;
            continue;
        }
        if (token != "[capsule limit lv]")
            continue;

        script.m_capsuleLimitLv = ScanInt(&ok);
        if (!ok)
            return 0x2bc8;
    }
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
void stStepRewardStackableItemScript::set_data(int step, StackableItemData& data)
{
    std::map<int, std::vector<StackableItemData> >::iterator it = m_map0.find(step);
    if (it == m_map0.end())
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
void StackableItemData::clear()
{
    m_field0 = -1;   // +0x00 movl $0xffffffff
    m_field4 = 0;    // +0x04
}

StackableItemData::StackableItemData()
{
    clear();
}

// ORIG T 0x08917c99。endTag 形参在 ORIG 函数体内零引用（段终止靠 ScanInt 断流，
// 由调用方主循环消费闭合 tag），保留签名并显式标注。
int LoadRewardDataByNoType(stStepRewardStackableItemScript& script, std::string endTag)
{
    (void)endTag;

    std::string token;   // ORIG 局部 string（构造/析构，无观察效应）
    bool ok = false;

    while (true)
    {
        StackableItemData data;                  // 每轮重置（ORIG ctor+clear 等价）
        int step = ScanInt(&ok);
        if (!ok)
            return 0;

        data.m_field0 = ScanInt(&ok);
        if (!ok) return 0x3662;
        data.m_field4 = ScanInt(&ok);
        if (!ok) return 0x3666;

        script.set_data(step, data);
    }
}

// ORIG T 0x089179eb。首成员 clear 后按三段分发至共享 loader；loader 非 0 即透传。
int ImportNewAccountLeventUpToJobScript(const char* path,
                                        stNewAccountLevelUpToJobScript& script)
{
    if (!loadRDARScriptFile("", path))
        return 0x3629;

    script.m_stepReward.clear();                 // ORIG +0x044（0x08371508）

    std::string token;
    while (ScanType(token, true))
    {
        int err;

        if (token == "[reward stackable item]")
        {
            std::string endTag("[/reward stackable item]");
            err = LoadRewardDataByNoType(script.m_stepReward, endTag);
        }
        else if (token == "[reward equip item]")
        {
            std::string endTag("[/reward equip item]");
            err = LoadRewardDataByGrowType(script.m_levelUp18, endTag);
        }
        else if (token == "[reward avatar item]")
        {
            std::string endTag("[/reward avatar item]");
            err = LoadRewardDataByGrowType(script.m_levelUp30, endTag);
        }
        else
        {
            continue;
        }

        if (err != 0)
            return err;
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

// ORIG T 0x08917db3：剔除 '[' 与 ']'（ORIG 以 tag="[]" 逐字符比对）；仅当确有
// 剔除时回写（ORIG local_11 门禁）。
void deleteTagBracketString(std::string& text)
{
    std::string tag = "[]";
    std::string filtered;
    bool replaced = false;

    for (std::string::size_type i = 0; i < text.size(); ++i)
    {
        char c = text[i];
        if (c == tag[0] || c == tag[1])
        {
            replaced = true;
        }
        else
        {
            filtered.push_back(c);
        }
    }

    if (replaced)
        text = filtered;
}
