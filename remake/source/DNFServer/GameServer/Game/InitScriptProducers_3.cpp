// ============================================================================
// df_game_r 还原 —— InitScriptProducers_3.cpp
//
// CDataManager::init（ORIG 0x08358332..0x0835f1d5，报告 init.md 全量转写）
// 依赖的脚本装载自由函数 · 第 3 组（Guild/Power/Quest 参数、称号簿、PvP 频道、
// 旧装备属性、屏蔽字、物品组表）。
//
// 本 TU 实现的符号（均经 df_game_r nm 核实为真实 T 符号；逐指令转写自
// docs/class_func_reports/_lt_global_gt_/ 对应报告，调用点行号引自
// docs/class_func_reports/CDataManager/init.md）：
//   importOldEquipmentStatInfo   _Z26importOldEquipmentStatInfoPKc        0x08a58174（init.md +0x2af）
//   importPVPChannelList         _Z20importPVPChannelListPcP18live_server_info_ti
//                                                                         0x08901f10（init.md +0x1ce1）
//   importGuildParameterScript   _Z26importGuildParameterScriptP20GuildParameterScriptPKc
//                                                                         0x08978d48（init.md +0x1d56）
//   importPowerParameterScript   _Z26importPowerParameterScriptP20PowerParameterScriptPKc
//                                                                         0x08a592bc（init.md +0x1dcb）
//   importTitleBookEtcScript     _Z24importTitleBookEtcScriptPKcP15stTitleBookInfo
//                                                                         0x08aa8110（init.md +0x1f2a）
//   importQuestParameterScript   _Z26importQuestParameterScriptP20QuestParameterScriptPKc
//                                                                         0x08a6b660（init.md +0x207d）
//   importSlangName              _Z15importSlangNamePcP13SlangNameData    0x089081a3（init.md +0x2204）
//   importItmeGroupTable         _Z20importItmeGroupTablePKcS0_RSt3mapISsi..
//                                                                         0x0890bb25（init.md +0x3e40）
//
// 声明唯一出处 DataManagerInitDepends.h（本文件在 CDataManager.h 之后包含之）。
// ============================================================================

#include "CDataManager.h"
#include "sync_script.h"         // CSyncScript/STEquipmentPartSetPiece（须先于 deps 头，与 CDataManager.cpp 口径一致）
#include "DataManagerInitDepends.h"
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt/ScanFloat/ScanStr/ScanTypeOrStr
#include "LogManager.h"          // LogManager::logFormat

#include <map>
#include <string>
#include <utility>
#include <vector>

// ---- 外部依赖声明（ORIG T 符号，定义归属各自批次；此处仅引用）----

// _Z13importRewardsPSt6vectorIP18stTitleRewardBasisSaIS1_EESs 0x08aa8b79
void importRewards(std::vector<stTitleRewardBasis*>* out, std::string token);
// _Z19importTitleSectionP14stTitleSectioni 0x08aa8d65（返回 bool）
bool importTitleSection(stTitleSection* section, int idx);
// _Z18getTitleCollectionP15stTitleBookInfoi 0x08aa94e1
std::vector<stTitleElement>* getTitleCollection(stTitleBookInfo* book, int tab);
// _Z26convertTabTypeStringToEnumPKc 0x08aa7d98
int convertTabTypeStringToEnum(const char* text);
// _Z17loadAppendageTypeRKSs 0x0889253b / _Z13loadValueTypeRKSs 0x08892ac4
int loadAppendageType(const std::string& token);
int loadValueType(const std::string& token);

// g_equipmentStatInfoMap —— ORIG 未修饰名 B 0x095000a0（RDARScriptOldEquip 域
// 全局，本 TU 为其唯一定义点；消费方经同名 extern "C" 引用）。
// 类型证据：报告 _lt_global_gt_/__static_initialization_and_destruction_0_
// 08a588b8.md —— ORIG 静态初始化为 map<unsigned int, STEquipmentStatInfo>
// 默认构造并 __cxa_atexit 注册析构（本定义的动态初始化与之等价）。
extern "C"
{
std::map<unsigned int, STEquipmentStatInfo> g_equipmentStatInfoMap;
}

// ============================================================================
// importOldEquipmentStatInfo —— ORIG 0x08a58174（../RDARScriptOldEquip.cpp）
// 报告 _lt_global_gt_/importOldEquipmentStatInfo.md。
// 逐条读取 itemIndex + STEquipmentStatInfo 全部 26 个 int 字段；任一缺失即
// logFormat 并返回 false；重复 index 打印 Redundant 后跳过插入；
// ScanInt 收尾失败（正常 EOF）返回 true。
// ============================================================================
bool importOldEquipmentStatInfo(const char* dir)
{
    if (!loadRDARScriptFile(dir, "equipment/OldEquipmentStatInfoList.dat"))
    {
        LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                              __FUNCTION__, 0x12,
                              "CRITICAL ERROR : old equipment stat info load failed!");
        return false;
    }

    unsigned int itemIndex;
    bool ok = false;

    while ((itemIndex = ScanInt(&ok), ok))
    {
        STEquipmentStatInfo info;
        info.m_hpMax = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x29,
                                  "Invalid Old Equipment Stat Info(missing hpMax)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_mpMax = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x30,
                                  "Invalid Old Equipment Stat Info(missing mpMax)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_phyAtt = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x37,
                                  "Invalid Old Equipment Stat Info(missing physical attack)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_phyDef = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x3d,
                                  "Invalid Old Equipment Stat Info(missing physical defense)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_magAtt = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x43,
                                  "Invalid Old Equipment Stat Info(missing magical attack)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_magDef = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x49,
                                  "Invalid Old Equipment Stat Info(missing magical defense)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_equipPhyAtt = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x50,
                                  "Invalid Old Equipment Stat Info(missing equipment physical attack)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_equipPhyDef = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x57,
                                  "Invalid Old Equipment Stat Info(missing equipment physical defense)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_equipMagAtt = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x5e,
                                  "Invalid Old Equipment Stat Info(missing magical attack)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_equipMagDef = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x65,
                                  "Invalid Old Equipment Stat Info(missing magical defense)! itemIndex = %d", itemIndex);
            return false;
        }
        // 元素耐性 4 项（+0x28..+0x34）：ORIG 以 base+(i+8)*4 寻址。
        int* elementTolerance = &info.m_refFire;
        for (int i = 0; i < 4; ++i)
        {
            elementTolerance[i] = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                      __FUNCTION__, 0x6e,
                                      "Invalid Old Equipment Stat Info(missing element tolerance %d)! itemIndex = %d", i, itemIndex);
                return false;
            }
        }
        info.m_refAll = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x76,
                                  "Invalid Old Equipment Stat Info(missing all element tolerance)! itemIndex = %d", itemIndex);
            return false;
        }
        // 状态异常耐性 17 项（+0x3c..+0x7c）：ORIG 以 base+(j+12)*4 寻址。
        int* activeProperty = &info.m_refSlow;
        for (int j = 0; j < 0x11; ++j)
        {
            activeProperty[j] = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                      __FUNCTION__, 0x7f,
                                      "Invalid Old Equipment Stat Info(missing active property %d)! itemIndex = %d", j, itemIndex);
                return false;
            }
        }
        info.m_refAllState = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x87,
                                  "Invalid Old Equipment Stat Info(missing all activeStatus tolerance)! itemIndex = %d", itemIndex);
            return false;
        }

        if (g_equipmentStatInfoMap.find(itemIndex) != g_equipmentStatInfoMap.end())
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x92,
                                  "Redundant Old Equipment Stat Info! itemIndex = %d", itemIndex);
        }
        else
        {
            g_equipmentStatInfoMap.insert(std::make_pair(itemIndex, info));
        }
    }
    return true;
}

// ============================================================================
// importPVPChannelList —— ORIG 0x08901f10
// 报告 _lt_global_gt_/importPVPChannelList.md；调用点 init.md +0x1cce：
//   importPVPChannelList("Etc/pvpChannelList.etc", this+0xb4ec, 0)。
// [PVP CHANNEL INFO] 段：grade=ScanInt、count=ScanInt；每条 ScanStr(名字) +
// pvp_channel_info_t 的 9 个 int（扫描序→偏移见 CDataManager.h 注释）+
// 尾随 ScanStr；完成后把 map[grade]=vector insert 进 live_server_info_t 的
// m_pvpChannelMap。第三形参 flag 在 ORIG 函数体内未使用。
// 任一 channel 字段缺失 → 清理后返回 false。
// ============================================================================
bool importPVPChannelList(char* path, live_server_info_t* info, int flag)
{
    (void)flag;
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[PVP CHANNEL INFO]")
        {
            while (true)
            {
                int grade;
                if (!(grade = ScanInt(&ok), ok))
                    break;
                int count;
                if (!(count = ScanInt(&ok), ok))
                    break;

                std::vector<pvp_channel_info_t> channels;
                std::string name;

                for (int i = 0; i < count; ++i)
                {
                    ScanStr(&name);

                    pvp_channel_info_t channel;
                    channel.m_field0 = ScanInt(&ok);           // 扫描序 1 → +0x00
                    if (!ok)
                        return false;
                    channel.m_minGrade = ScanInt(&ok);          // 扫描序 2 → +0x04
                    if (!ok)
                        return false;
                    channel.m_maxGrade = ScanInt(&ok);         // 扫描序 3 → +0x08
                    if (!ok)
                        return false;
                    channel.m_channelType = ScanInt(&ok);      // 扫描序 4 → +0x10
                    if (!ok)
                        return false;
                    channel.m_upgradeRevision = ScanInt(&ok);  // 扫描序 5 → +0x0c
                    if (!ok)
                        return false;
                    channel.m_minLevel = ScanInt(&ok);         // 扫描序 6 → +0x14
                    if (!ok)
                        return false;
                    channel.m_maxLevel = ScanInt(&ok);        // 扫描序 7 → +0x18
                    if (!ok)
                        return false;
                    channel.m_field1c = ScanInt(&ok);          // 扫描序 8 → +0x1c
                    if (!ok)
                        return false;
                    channel.m_field20 = ScanInt(&ok);          // 扫描序 9 → +0x20
                    if (!ok)
                        return false;

                    channels.push_back(channel);
                    ScanStr(&name);
                }

                info->m_pvpChannelMap.insert(std::make_pair(grade, channels));
            }
        }
    }
    return true;
}

// ============================================================================
// importGuildParameterScript —— ORIG 0x08978d48
// 报告 _lt_global_gt_/importGuildParameterScript.md；调用点 init.md +0x1d53：
//   importGuildParameterScript(this+0xa50c, "Etc/Guild.etc")。
// 返回 bool：仅当读到 [guild parameter end] 为 true；加载失败 / ScanType 失败 /
// [guild exp weight] 遇 m_field8==0 均返回 false（ORIG 一致）。
// ============================================================================
bool importGuildParameterScript(GuildParameterScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    script->clear();

    std::string token;
    bool done = false;

    while (true)
    {
        while (true)
        {
            while (true)
            {
                while (true)
                {
                    while (true)
                    {
                        if (!ScanType(token, true))
                            goto scan_end;

                        if (token == "[guild create cost]")
                        {
                            script->m_field0 = ScanInt((bool*)0);
                            continue;
                        }
                        break;
                    }
                    if (token == "[guild levelup cost]")
                    {
                        // 最多 17 行：level cost1 cost2；level>0x11 放弃余下行。
                        // ORIG 写序（asm +0x107/+0x126）：cost1→[level-1][1]、
                        // cost2→[level-1][0]，m_levelUpCost 首址 +0x5c。
                        for (int count = 0; count < 0x11; ++count)
                        {
                            int level = ScanInt((bool*)0);
                            if (level > 0x11)
                                break;
                            int cost1 = ScanInt((bool*)0);
                            int cost2 = ScanInt((bool*)0);
                            script->m_levelUpCost[level - 1][1] = cost1;
                            script->m_levelUpCost[level - 1][0] = cost2;
                        }
                        continue;
                    }
                    break;
                }
                if (token == "[guild point acquirement ratio]")
                {
                    script->m_field4 = ScanInt((bool*)0);
                    continue;
                }
                break;
            }
            if (token == "[guild exp prob]")
            {
                script->m_field8 = static_cast<char>(ScanInt((bool*)0));
                continue;
            }
            break;
        }

        if (token == "[guild exp weight]")
        {
            // asm +0x1c7：prob==0 时直接以 false 结束整个函数。
            if (script->m_field8 == 0)
                goto scan_end;
            for (char w = 0; w < script->m_field8; ++w)
            {
                int rangeMin = ScanInt((bool*)0);
                int rangeMax = ScanInt((bool*)0);
                int ratio = ScanInt((bool*)0);
                // asm +0x24d..+0x295：insert(make_pair(lv,ratio))，
                // .second==false 时终止内层循环。
                for (int lv = rangeMin; lv <= rangeMax; ++lv)
                {
                    if (!script->m_map44.insert(std::make_pair(lv, ratio)).second)
                        break;
                }
            }
            continue;
        }

        if (token == "[guild mail]")
        {
            script->m_mail = ScanInt((bool*)0);
            continue;
        }
        if (token == "[guild level demand for agit]")
        {
            script->m_fieldC = ScanInt((bool*)0);
            continue;
        }
        if (token == "[guild exp book]")
        {
            // 固定 3 对 key/value 插入 m_map10（asm +0x36f..+0x3f3）。
            for (int i = 0; i < 3; ++i)
            {
                int key = ScanInt((bool*)0);
                int value = ScanInt((bool*)0);
                script->m_map10.insert(std::make_pair(key, value));
            }
            continue;
        }
        if (token == "[guild level demand for buying guild exp book]")
        {
            script->m_field28 = ScanInt((bool*)0);
            continue;
        }
        if (token == "[agit upgrade info]")
        {
            STAgitUpgradeInfo entry;
            for (int i = 0; i < 5; ++i)
            {
                entry.m_field0 = ScanInt((bool*)0);
                entry.m_field4 = ScanInt((bool*)0);
                entry.m_field8 = ScanInt((bool*)0);
                entry.m_fieldC = ScanInt((bool*)0);
                script->m_vec2c.push_back(entry);
            }
            continue;
        }
        if (token == "[guild storage upgrade info]")
        {
            STGuildStorageUpgradeInfo entry;
            while ((entry.m_field0 = ScanInt(&done), done))
            {
                done = false;
                entry.m_field4 = ScanInt((bool*)0);
                entry.m_field8 = ScanInt((bool*)0);
                entry.m_fieldC = ScanInt((bool*)0);
                entry.m_field10 = ScanInt((bool*)0);
                script->m_vec38.push_back(entry);
            }
            done = false;
            continue;
        }
        if (token == "[guild exp member weight]")
        {
            int memberCount;
            double weight;
            while (((memberCount = ScanInt(&done)), done))
            {
                done = false;
                weight = ScanFloat((bool*)0);
                script->m_vecE4.push_back(std::make_pair(memberCount, weight));
            }
            done = false;
            continue;
        }
        if (token == "[guild levelup price]")
        {
            int guildLevel;
            while (((guildLevel = ScanInt(&done)), done))
            {
                done = false;
                std::vector<int> prices;
                prices.reserve(0x11);
                for (int j = 0; j < 0x11; ++j)
                    prices.push_back(ScanInt((bool*)0));
                script->m_vecF0.push_back(std::make_pair(guildLevel, prices));
            }
            done = false;
            continue;
        }
        if (token == "[today guild member weight]")
        {
            script->m_fieldFC = ScanFloat((bool*)0);
            continue;
        }
        if (token == "[guild fund condition]")
        {
            script->m_field104 = ScanInt((bool*)0);
            script->m_field108 = ScanInt((bool*)0);
            script->m_field10c = ScanInt((bool*)0);
            script->m_field110 = ScanFloat((bool*)0);
            continue;
        }
        if (token == "[guild parameter end]")
        {
            done = true;
            break;
        }
    }

scan_end:
    return done;
}

// ============================================================================
// importPowerParameterScript —— ORIG 0x08a592bc
// 报告 _lt_global_gt_/importPowerParameterScript.md；调用点 init.md +0x1dc8：
//   importPowerParameterScript(this+0xa628, "Etc/Power.etc")。
// 返回 bool：仅当读到 [power parameter end] 为 true；加载失败 / ScanType 失败 /
// [power war channel] 第二值缺失 返回 false（ORIG 一致）。
// ============================================================================
bool importPowerParameterScript(PowerParameterScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    script->clear();

    std::string token;
    bool done = false;

    while (true)
    {
        if (!ScanType(token, true))
            goto scan_end;

        if (token == "[power join cost]")                      { script->m_powerJoinCost = ScanInt((bool*)0); continue; }
        if (token == "[power join guild level]")               { script->m_powerJoinGuildLevel = ScanInt((bool*)0); continue; }
        if (token == "[power join quest]")                     { script->m_powerJoinQuest = ScanInt((bool*)0); continue; }
        if (token == "[kill point to end power war]")          { script->m_killPointToEndPowerWar = ScanInt((bool*)0); continue; }
        if (token == "[exp reward]")                           { script->m_expReward = ScanInt((bool*)0); continue; }
        if (token == "[easy assault range]")                   { script->m_easyAssaultRange = ScanInt((bool*)0); continue; }
        if (token == "[ghost revival base time]")              { script->m_ghostTime = ScanInt((bool*)0); continue; }
        if (token == "[respon penalty time]")                  { script->m_penaltyThreshold = ScanInt((bool*)0); continue; }
        if (token == "[powerwar point multiple number]")       { script->m_powerWarPointMultiple = ScanInt((bool*)0); continue; }
        if (token == "[refresh power war process info]")       { script->m_refreshProcessInfo = ScanInt((bool*)0); continue; }
        if (token == "[abuser ban time]")                      { script->m_abuserBanTime = ScanInt((bool*)0); continue; }
        if (token == "[power up ratio]")                       { script->m_powerUpRatio = ScanInt((bool*)0); continue; }
        if (token == "[power up assault count]")               { script->m_powerUpAssaultCount = ScanInt((bool*)0); continue; }
        if (token == "[power up assault victory ratio]")       { script->m_powerUpAssaultVictoryRatio = ScanInt((bool*)0); continue; }
        if (token == "[power up generate period]")             { script->m_powerUpGeneratePeriod = ScanInt((bool*)0); continue; }
        if (token == "[straight victories]")                   { script->m_straightVictories = ScanInt((bool*)0); continue; }
        if (token == "[item initial cooltime]")                { script->m_itemInitialCooltime = ScanInt((bool*)0); continue; }
        if (token == "[basic powerwar point]")                 { script->m_basicPowerWarPoint = ScanInt((bool*)0); continue; }
        if (token == "[first grade bonus powerwar point]")     { script->m_firstGradeBonusPoint = ScanInt((bool*)0); continue; }
        if (token == "[ranking bonus grade]")                  { script->m_rankingBonusGrade = ScanInt((bool*)0); continue; }
        if (token == "[ranking bonus decrease point]")         { script->m_rankingBonusDecreasePoint = ScanInt((bool*)0); continue; }
        if (token == "[decrease powerwar point per day]")      { script->m_decreasePointPerDay = ScanInt((bool*)0); continue; }
        if (token == "[required user powerwar point 4 dungeon]") { script->m_requiredUserPoint4Dungeon = ScanInt((bool*)0); continue; }
        if (token == "[power war dungeon info]")
        {
            script->m_powerWarDungeonInfo0 = ScanInt((bool*)0);
            script->m_powerWarDungeonInfo1 = ScanInt((bool*)0);
            continue;
        }
        if (token == "[pc room bonus]")
        {
            // asm +0x60a..+0x625：读入即写入 +0x44，ok 标志被忽略。
            script->m_pcRoomBonus = ScanInt(&done);
            done = false;
            continue;
        }
        if (token == "[time assault super power state on join powerwar]")
        {
            script->m_timeAssaultSuperPowerStateOnJoin = ScanInt((bool*)0);
            continue;
        }
        if (token == "[time assault super power state after powerwar]")
        {
            script->m_timeAssaultSuperPowerStateAfter = ScanInt((bool*)0);
            continue;
        }
        if (token == "[powerwar enter level limit]")           { script->m_minEnterLevel = ScanInt((bool*)0); continue; }
        if (token == "[powerwar enter age limit]")             { script->m_minEnterAge = ScanInt((bool*)0); continue; }
        if (token == "[power up point mod]")                   { script->m_powerUpPointMod = ScanInt((bool*)0); continue; }
        if (token == "[victorious reward time]")               { script->m_victoriousRewardTime = ScanInt((bool*)0); continue; }
        if (token == "[power rejoin panalty]")                 { script->m_powerWarCooldownMinutes = ScanInt((bool*)0); continue; }

        if (token == "[power up noti time]")
        {
            int value;
            while (((value = ScanInt(&done)), done))
            {
                done = false;
                script->m_vec20.push_back(value);
            }
            done = false;
            continue;
        }
        if (token == "[kill count effect range]")
        {
            // asm +0x3e6..+0x434：首值带 ok 校验，次值不校验。
            while (true)
            {
                std::pair<int, int> range;
                range.first = ScanInt(&done);
                if (!done)
                    break;
                done = false;
                range.second = ScanInt((bool*)0);
                script->m_vec70.push_back(range);
            }
            done = false;
            continue;
        }
        if (token == "[power up bonus]")
        {
            int value;
            while (((value = ScanInt(&done)), done))
            {
                done = false;
                script->m_vec38.push_back(value);
            }
            done = false;
            continue;
        }
        if (token == "[power war channel]")
        {
            while (true)
            {
                int channel;
                if (!(channel = ScanInt(&done), done))
                    break;
                int serverGroup = ScanInt(&done);
                if (!done)
                    return false;                    // asm +0x739：第二值缺失即整体失败
                done = false;
                script->m_powerWarServerList.push_back(
                    std::make_pair(channel, serverGroup));
            }
            done = false;
            continue;
        }
        if (token == "[power join cost modify]")
        {
            int value;
            while (((value = ScanInt(&done)), done))
            {
                done = false;
                script->m_vecAC.push_back(value);
            }
            done = false;
            continue;
        }
        if (token == "[straight victory penalty]")
        {
            stVictoryPenalty penalty;
            penalty.m_field0 = ScanInt(&done);
            if (done)
            {
                penalty.m_field4 = ScanInt(&done);
                if (done)
                {
                    penalty.m_field8 = ScanInt(&done);
                    if (done)
                    {
                        penalty.m_fieldC = ScanInt(&done);
                        if (done)
                        {
                            done = false;
                            script->m_vecBC.push_back(penalty);
                            continue;
                        }
                    }
                }
            }
            done = false;
            continue;
        }
        if (token == "[straight victory break bonus]")
        {
            int value;
            while (((value = ScanInt(&done)), done))
            {
                done = false;
                script->m_vecC8.push_back(value);
            }
            done = false;
            continue;
        }
        if (token == "[power parameter end]")
        {
            done = true;
            break;
        }
    }

scan_end:
    return done;
}

// ============================================================================
// importQuestParameterScript —— ORIG 0x08a6b660
// 报告 _lt_global_gt_/importQuestParameterScript.md；调用点 init.md +0x207a：
//   importQuestParameterScript(this+0x8cc8, "n_Quest/questParameter.etc")。
// [difficulty] 用 ScanTypeOrStr：类型令牌取 sub[0] 为键、ScanInt 为值插入
// m_map0；字符串令牌仅识别 [/difficulty] 终止符。[piece reward] 中首个
// ScanInt 失败即整体返回 false（asm +0x445）。
// ============================================================================
bool importQuestParameterScript(QuestParameterScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    script->clear();

    std::string token;
    bool done = false;

    while (true)
    {
        if (!ScanType(token, true))
            goto scan_end;

        if (token == "[difficulty]")
        {
            std::string sub;
            while (true)
            {
                bool isStr = false;
                if (!ScanTypeOrStr(isStr, sub, true))
                    break;
                if (isStr)
                {
                    if (sub == "[/difficulty]")
                        break;
                    continue;
                }
                int value = ScanInt((bool*)0);
                char key = sub.c_str()[0];
                script->m_map0.insert(std::make_pair(key, value));
            }
            continue;
        }
        if (token == "[green level penalty]")   { script->m_field18 = ScanInt((bool*)0); continue; }
        if (token == "[grey level penalty]")    { script->m_field1c = ScanInt((bool*)0); continue; }
        if (token == "[bal kun penalty]")       { script->m_field20 = ScanInt((bool*)0); continue; }
        if (token == "[exp reward table]")
        {
            script->m_vec24.reserve(200);
            int value;
            for (int i = 0; i < 200; ++i)
            {
                if (!(value = ScanInt(&done), done))
                    break;
                done = false;
                script->m_vec24.push_back(value);
            }
            done = false;
            continue;
        }
        if (token == "[gold reward table]")
        {
            script->m_vec30.reserve(200);
            int value;
            for (int i = 0; i < 200; ++i)
            {
                if (!(value = ScanInt(&done), done))
                    break;
                done = false;
                script->m_vec30.push_back(value);
            }
            done = false;
            continue;
        }
        if (token == "[piece reward table]")
        {
            while (true)
            {
                int key = ScanInt(&done);
                if (!done)
                    break;
                int value = ScanInt(&done);
                if (!done)
                    break;
                done = false;
                script->m_map3c.insert(std::make_pair(key, value));
            }
            done = false;
            continue;
        }
        if (token == "[piece reward]")
        {
            int questIdx = ScanInt(&done);
            if (!done)
                return false;
            done = false;

            std::vector<pieceQuestRewardStatus> statuses;
            std::string sub;
            while (true)
            {
                if (!ScanType(sub, true))
                    break;
                if (sub == "[/piece reward]")
                    break;

                int appendage = loadAppendageType(sub);
                if (appendage != -1)
                {
                    pieceQuestRewardStatus status;
                    status.m_appendageType = appendage;
                    std::string typeText;
                    ScanStr(&typeText);
                    status.m_valueType = loadValueType(typeText);
                    status.m_value = ScanFloat((bool*)0);
                    statuses.push_back(status);
                }
            }
            script->m_map54.insert(std::make_pair(questIdx, statuses));
            continue;
        }
        if (token == "[quest parameter end]")
        {
            done = true;
            break;
        }
    }

scan_end:
    return done;
}

// ============================================================================
// importTitleBookEtcScript —— ORIG 0x08aa8110
// 报告 _lt_global_gt_/importTitleBookEtcScript.md；调用点 init.md +0x1f23：
//   importTitleBookEtcScript("Etc/TitleBook.etc", this+0xa788)。
// 双遍结构（ORIG __lookup/__init_lock/__inited 三个函数级静态）：
//   首令牌触发 6 个 (键,id) 登记（0x39/0x4d/0x7f/0xc6/0xe4/0xf4），置位
//   __inited 后以当前令牌重新查表派发——因此首令牌同样会被正常处理。
// ============================================================================
bool importTitleBookEtcScript(const char* path, stTitleBookInfo* out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    static std::map<const char*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static char __inited = 0;

    std::string token;
    std::string tok2;
    std::string tok3;
    bool ok = false;

    while (true)
    {
        while (true)
        {
            if (!ScanType(token, true))
                goto scan_end;

            if (!__inited)
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;
                }
                break;                          // 以未初始化态进入派发
            }

            tok2 = token;
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(tok2.c_str());
            if (it != __lookup.end())
                goto dispatch;
            else
                goto next_token;                // 未知键：忽略该令牌
        }

        // ---- 首遍：登记全部键值（顺序与 ORIG 级联一致）----
        __lookup.insert(std::make_pair("[default reward]", 0x39));
        __lookup.insert(std::make_pair("[title collection info]", 0x4d));
        __lookup.insert(std::make_pair("[section]", 0x7f));
        __lookup.insert(std::make_pair("[custom condition message]", 0xc6));
        __lookup.insert(std::make_pair("[adjust list]", 0xe4));
        __lookup.insert(std::make_pair("[view titlebook eventmark]", 0xf4));

dispatch:
        {
            int id;
            if (!__inited)
            {
                id = 0;
            }
            else
            {
                tok2 = token;
                std::map<const char*, int, less_TCHAR>::const_iterator it =
                    __lookup.find(tok2.c_str());
                id = (it != __lookup.end()) ? it->second : -1;
            }

            switch (id)
            {
            case 0x39:                           // [default reward]
            case 0:
            {
                if (__inited)
                {
                    while (true)
                    {
                        if (!ScanType(tok3, true))
                        {
                            __init_lock.leave();
                            __inited = 1;
                            goto scan_end_true;
                        }
                        if (tok3 == "[/default reward]")
                            break;
                        importRewards(&out->m_vec48, tok3);
                    }
                }
                else if (id != 0)
                {
                    __lookup.insert(std::make_pair("[default reward]", 0x39));
                }
                break;
            }
            case 0x4d:                           // [title collection info]
            {
                if (__inited)
                {
                    ScanStr(&tok3);
                    int tab = convertTabTypeStringToEnum(tok3.c_str());
                    int count = ScanInt((bool*)0);
                    for (int i = 0; i < count; ++i)
                    {
                        ScanInt((bool*)0);              // ORIG 丢弃值（asm +0x468）
                        stTitleElement element;
                        element.m_index = ScanInt((bool*)0);
                        if (element.m_index != -1)
                        {
                            element.m_achievementIdx = ScanInt((bool*)0);
                            int jobCount = ScanInt((bool*)0);
                            for (int j = 0; j < jobCount; ++j)
                                element.m_fitJobs.push_back(ScanInt((bool*)0));
                        }
                        out->m_map18[tab].push_back(element);
                    }
                }
                else if (id != 0)
                {
                    __lookup.insert(std::make_pair("[title collection info]", 0x4d));
                }
                break;
            }
            case 0x7f:                           // [section]
            {
                if (__inited)
                {
                    ScanStr(&tok3);
                    int tab = convertTabTypeStringToEnum(tok3.c_str());
                    bool keepGoing;
                    do
                    {
                        stTitleSection section;
                        section.m_category = tab;

                        // 统计同 tab 已有章节数作为段内索引（asm +0x622..+0x668）。
                        int sameCount = 0;
                        for (std::vector<stTitleSection>::size_type k = 0;
                             k < out->m_vecC.size(); ++k)
                        {
                            if (out->m_vecC[k].m_category == tab)
                                ++sameCount;
                        }

                        keepGoing = importTitleSection(&section, sameCount);
                        if (!keepGoing)
                            break;

                        // 收缩区间到首个空槽（asm +0x69d..+0x70b）：
                        // endIdx(+0x10) > i 且 coll[i].m_index==-1 时 endIdx=i。
                        std::vector<stTitleElement>* collection =
                            getTitleCollection(out, tab);
                        if (collection != 0)
                        {
                            for (int i = section.m_startIdx;
                                 section.m_endIdx > i; ++i)
                            {
                                if ((int)collection->size() > i &&
                                    (*collection)[i].m_index == -1)
                                {
                                    section.m_endIdx = i;
                                }
                            }
                        }
                        out->m_vecC.push_back(section);
                    } while (keepGoing);
                }
                else if (id != 0)
                {
                    __lookup.insert(std::make_pair("[section]", 0x7f));
                }
                break;
            }
            case 0xc6:                           // [custom condition message]
            {
                if (__inited)
                {
                    stCustomConditionMsg entry;
                    out->m_vec30.clear();
                    while (ScanStr(&tok3))
                    {
                        entry.m_tabType = convertTabTypeStringToEnum(tok3.c_str());
                        entry.m_value = ScanInt((bool*)0);
                        ScanStr(&entry.m_msg);
                        out->m_vec30.push_back(entry);
                    }
                }
                else if (id != 0)
                {
                    __lookup.insert(
                        std::make_pair("[custom condition message]", 0xc6));
                }
                break;
            }
            case 0xe4:                           // [adjust list]
            {
                if (__inited)
                {
                    stAdjust entry;
                    while ((entry.m_questIdx = ScanInt(&ok), ok))
                    {
                        ok = false;
                        entry.m_field4 = ScanInt((bool*)0);
                        entry.m_achievementIdx = ScanInt((bool*)0);
                        out->m_vec3c.push_back(entry);
                    }
                    ok = false;
                }
                else if (id != 0)
                {
                    __lookup.insert(std::make_pair("[adjust list]", 0xe4));
                }
                break;
            }
            case 0xf4:                           // [view titlebook eventmark]
            {
                if (__inited)
                {
                    int value = 0;
                    if (ScanInt(&value))         // ORIG 使用 ScanInt(int*) 重载
                        out->m_viewEventMark = (value != 0);
                }
                else if (id != 0)
                {
                    __lookup.insert(
                        std::make_pair("[view titlebook eventmark]", 0xf4));
                }
                break;
            }
            default:
                break;
            }

            if (!__inited)
            {
                __init_lock.leave();
                __inited = 1;
                continue;                        // 以当前令牌重新派发
            }
        }

next_token:
        ;
    }

scan_end:
    return true;

scan_end_true:
    return true;
}

// ============================================================================
// importSlangName —— ORIG 0x089081a3
// 报告 _lt_global_gt_/importSlangName.md；调用点 init.md +0x21fd：
//   importSlangName("Etc/slangname.etc", this+0x86a0)。
// [SLANG]/[one byte skip char]/[two byte skip char] 三段分别回填
// SlangNameData 的 vector<string>(+0x00)/vector<uchar>(+0x0c)/
// vector<ushort>(+0x18)。
// ============================================================================
bool importSlangName(char* path, SlangNameData* out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    std::string text;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[SLANG]")
        {
            while (ScanStr(&text))
                out->m_slangList.push_back(text);
        }
        if (token == "[one byte skip char]")
        {
            unsigned char ch;
            while (((ch = static_cast<unsigned char>(ScanInt(&ok))), ok))
            {
                ok = false;
                out->m_charList.push_back(ch);
            }
            ok = false;
        }
        if (token == "[two byte skip char]")
        {
            unsigned short ch;
            while (((ch = static_cast<unsigned short>(ScanInt(&ok))), ok))
            {
                ok = false;
                out->m_shortList.push_back(ch);
            }
            ok = false;
        }
    }
    return true;
}

// ============================================================================
// importItmeGroupTable —— ORIG 0x0890bb25
// 报告 _lt_global_gt_/importItmeGroupTable.md；调用点 init.md +0x3e40：
//   importItmeGroupTable("Etc/", "ItemGroupName.tbl", getItemGroupNameTable())。
// 循环读 (id, 名称) 对插入 table；返回 0=成功、0x1ebd=ScanStr 失败、
// 0x1eb2=加载失败（错误码与 ORIG 一致）。
// ============================================================================
int importItmeGroupTable(const char* dir, const char* file,
                         std::map<std::string, int>& table)
{
    if (!loadRDARScriptFile(dir, file))
        return 0x1eb2;

    std::string name;
    bool ok = false;

    while (true)
    {
        int id = ScanInt(&ok);
        if (!ok)
            break;
        ok = false;
        if (!ScanStr(&name))
            return 0x1ebd;
        table.insert(std::make_pair(name, id));
    }
    return 0;
}
