// ============================================================================
// df_game_r 还原 —— InitScriptProducers_8.cpp
// CDataManager::init()（ORIG 0x08358332..0x0835f1d5，docs/class_func_reports/
// CDataManager/init.md）依赖的 12 个脚本装载自由函数 + 被调依赖
// importVendingMachineInfoCore（本 TU 定义）。importDeathTowerScript 唯一定义
// 在 InitScriptProducers_6.cpp（重复强符号已清理），本 TU 仅声明供
// importTowerInfo 调用。逐函数自 ORIG objdump
// 反汇编与 docs/class_func_reports/_lt_global_gt_/ 对应报告转写，非固定返回：
//
//   importResealTable                _Z17importResealTablePKcP14reseal_table_t      T 0x089008b8
//     init.md +0x5431 调用 ("Etc/Reseal.etc", &m_resealTable)
//   importVillageAttackedScript      _Z27importVillageAttackedScriptPKcR19stVillageAttacked_t T 0x089093ee
//     init.md +0x1984 调用 ("Etc/VillageAttacked.etc", m_villageAttacked)，0=成功
//   loadPvpMapBasicDataScript        _Z25loadPvpMapBasicDataScriptPKcPSt6vectorI11PVPMAP_DATASaIS2_EE T 0x0890a427
//     init.md +0x37f6 调用 ("Etc/pvpmapbasicdata.etc", 栈局部 vector)
//   importVendingMachineInfoCore     _Z28importVendingMachineInfoCorePKcS0_R18stVendingMachine_t T 0x0890b2a3
//   importVendingMachineInfo         _Z24importVendingMachineInfoPKcS0_RSt6vectorI18stVendingMachine_tSaIS2_EE T 0x0890b74e
//     init.md +0x3300 调用 ("Etc/", "VendingMachine.lst", m_vendingMachine)，返回值弃
//   importTowerInfo                  _Z15importTowerInfoPKcS0_RSt3mapIj14stDeathTower_t.. T 0x0890b8d0
//     init.md +0x0b6c 调用 ("Etc/", "TowerList.lst", m_deathTowerMap)，0=成功
//   importUnlimitChallengeScript     _Z28importUnlimitChallengeScriptPKcP22stUnlimitChallengeInfo T 0x0890bc63
//     init.md +0x553e 调用 ("Etc/UnlimitChallenge.etc", &m_unlimitChallenge)
//   importRankSystemScript           _Z22importRankSystemScriptPKcP16stRankSystemInfo T 0x0890ed7f
//     init.md +0x0c2d 调用 ("Etc/RankSystemInfo.etc", &m_rankSystem)
//   importUltimateDungeonList        _Z25importUltimateDungeonListPKcRSt3setIiSt4lessIiESaIiEE T 0x8913c7e
//     init.md +0x173c 调用 ("Etc/UltimateDungeonList.etc", m_ultimateSet)
//   importSeriaRoomDecoEvent         _Z24importSeriaRoomDecoEventP24seriaRoomDecoEventScriptPKc T 0x0895e4bf
//     init.md +0x0501 调用 (&m_seriaRoomDeco, "Event/SeriaRoomDecoEvent.evt")
//   importUseJobFatigueGiftScript    _Z29importUseJobFatigueGiftScriptP22useCharacterJobFatiguePKc T 0x0895f817
//     init.md +0x0660 调用 (&m_useJobFatigue, "Event/JobFatigue.evt")
//   importSeriaRoomAnimationDecoEvent _Z33importSeriaRoomAnimationDecoEventP27seriaRoomDecoAniEventScriptPKc T 0x08960861
//     init.md +0x074a 调用 (&m_seriaAniDeco, "Event/SeriaRoomDecoAnimation.evt")
//   AvatarVariation::importAvatarColorVariation _ZN15AvatarVariation26importAvatarColorVariationEPNS_15AvatarColorInfoE T 0x088d444f
//     init.md +0x6a51 调用 (&m_avatarColor)——路径常量 "Etc/AvatarColorVariation.etc"
//
// 本 TU 同时提供上述域内 POD 的生命周期定义（ORIG W 弱符号逐条转写）：
//   seriaRoomDecoEventSoundInfo C1 0x089623ba / clear 0x089623ee / D1 0x083dd54a
//   useCharacterJobFatigue::fatigueMatchGift C1 0x0896251c
//   seriaRoomDecoAniInfo C1 0x089625e8 / clear 0x08962606 / D1 0x083cf9ba
//   stVendingMachineInfo_t C1 0x0891b36a / clear 0x0891b3fe / D1 0x083f2f3a
//   stVendingMachine_t C1 0x0891c820 / D1 0x083e636c
// 及 ORIG 数据符号 PVPMAP_BIG_IMAGE(0x93f4f00)/PVPMAP_SMALL_IMAGE(0x93f4f60)/
// PVPMAP_ONLY_TOURNAMENT_EXPOSURE(0x94f70f4)（其余三个 PvP 全局由 PvP_Room.cpp
// 提供，本 TU 仅 extern 引用，不重复定义）。
// ============================================================================

#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"        // 目标类型：reseal_table_t/stDeathTower_t/
                                 // stRankSystemInfo/stUnlimitChallengeInfo/
                                 // seriaRoomDeco*/useCharacterJobFatigue/
                                 // stVendingMachine_t/PVPMAP_DATA/AvatarColorInfo
#include "sync_script.h"        // STEquipmentPartSetPiece（DataManagerInitDepends.h 需要）
#include "LogManager.h"          // LogManager::logFormat
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt/ScanFloat/ScanStr
#include "CEnvironment.h"        // less_TCHAR/SpinLock（SpinLock.cpp 提供）

// ---- 外部被调（ORIG 真实自由函数，定义在各权威 TU）----
int getCharacterJobToEnumValue(std::string& name);   // _Z26getCharacterJobToEnumValueRSs T 0x089bbc0f
int getDrawLayer(const char* name);                  // _Z12getDrawLayerPKc T 0x088be12d
int importDeathTowerScript(const char* path, struct stDeathTower_t& out); // _Z22importDeathTowerScriptPKcR14stDeathTower_t T 0x089089f3（唯一定义 InitScriptProducers_6.cpp）


bool loadRDARScriptListFile(class STScriptFileList* list, const char* dir,
                            const char* listPath, std::map<int, int>* unused);
// _Z22loadRDARScriptListFile.. T 0x088bbd77（STScriptFileList.cpp，定义侧已 bool；
// P292：ORIG 调用点 `test %al,%al` 直判 ⇒ 本 TU 旧 int 声明产 test/sete 物化）。

// ---- PvP 全局表：定义于 PvP_Room.cpp 的三个，此处仅 extern 引用 ----
// [b28d-Main 代落·S165] 元素 unsigned char→bool（定义侧 PvP_Room.cpp:65 已改；
// 消 ODR 不一致；sizeof/地址 0x94f70e0 不变，写侧值域 0/1）。
extern bool PVPMAP_DEATHMATCH_MODE[0x14];                     // ORIG B 0x94f70e0
// [b28d-Main 代落·S215] 元素 char→bool（定义侧 PvP_Room.cpp 已改，P248 档②；
// 同符号同宽，读侧已按 bool 读 0/1）。
extern bool PVPMAP_ONLY_NORMAL_EXPOSURE[0x14];       // ORIG B 0x94f7108
extern bool PVPMAP_ONLY_TOURNAMENT_RANDOM[0x14];     // ORIG B 0x94f711c

// ---- PvP map 全局表（ORIG 数据符号；外部链接）----
// D 0x93f4f00 [0x50]：int[20]，仅 [0]=-1 其余 0（.data 字节实证）。
int PVPMAP_BIG_IMAGE[0x14] = {-1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// D 0x93f4f60 [0x50]：同上。
int PVPMAP_SMALL_IMAGE[0x14] = {-1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// B 0x94f70f4 [0x14]：char[20] 零初始化 BSS。
char PVPMAP_ONLY_TOURNAMENT_EXPOSURE[0x14] = {0};

// ============================================================================
// importResealTable —— ORIG 0x089008b8
// 报告 docs/class_func_reports/_lt_global_gt_/importResealTable.md
// [rarity weight]→m_arr1[0..5](+0xc)；[extended rarity weight]→m_arr2[0..5]
// (+0x24)；[grade]→m_vec push_back 直至 Scan 失败。Scan 失败仍写入后仅记日志。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 089008b8：clean 162/162（fresh .o 口径，
// ORIG 165 insns）；p167 CLEAR。已对齐：死 string×2（P171）、bool ok 独立槽
// （-0x2d/-0x2e/-0x2f）、三段 ScanInt(bool*) 循环、push_back(unsigned int)。
bool importResealTable(const char* path, reseal_table_t* out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    // ORIG 0x89008ef/0x89008fa：另两个 string 局部（构造+析构、全程未使用；
    // P171 死对象照抄，4.4 亦保留其 C1/D1 调用）。
    std::string deadToken1;
    std::string deadToken2;
    (void)deadToken1;
    (void)deadToken2;

    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[rarity weight]")                       // 0x8e122be 区字符串
        {
            bool ok;                                          // ORIG -0x2d
            int i;
            int value;
            for (i = 0; i < 6; ++i)
            {
                value = ScanInt(&ok);
                if (!ok)
                {
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                        0xa9d, "importResealTable - invalid rarity table");
                }
                out->m_arr1[i] = value;                       // ORIG 写 param_2+0xc+i*4
            }
        }
        else if (token == "[extended rarity weight]")
        {
            bool ok;                                          // ORIG -0x2e
            int i;
            int value;
            for (i = 0; i < 6; ++i)
            {
                value = ScanInt(&ok);
                if (!ok)
                {
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                        0xaaf,
                        "importResealTable - invalid extended rarity table");
                }
                out->m_arr2[i] = value;                       // ORIG 写 param_2+0x24+i*4
            }
        }
        else if (token == "[grade]")
        {
            bool ok;                                          // ORIG -0x2f
            int value;
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                out->m_vec.push_back(static_cast<unsigned int>(value));
            }
        }
    }
    return true;
}

// ============================================================================
// importUltimateDungeonList —— ORIG 0x08913c7e
// set 清空后扫描；[apply ultimate] 段内逐 int insert；遇到 [/apply ultimate]
// 或任意其他 token 连续吞至该结束标记；EOF 与正常结束均返回 1，打开失败返回 0。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 08913c7e：clean 89/89（ORIG 92 insns）；
// p167 CLEAR。已对齐：set::clear、ok 每轮重置、insert(int)、EOF 与 [/apply ultimate]
// 共享尾块 return 1、打开失败日志返回 0。
bool importUltimateDungeonList(const char* path, std::set<int>& out)  // [P330] 与 DataManagerInitDepends.h 同步
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(
            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
            0x3209,
            "ERROR : %s \xed\x8c\x8c\xec\x9d\xbc\xec\x9d\x84 \xec\xb0\xbe"
            "\xec\x9d\x84\xec\x88\x98 \xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88"
            "\xeb\x8b\xa4.",
            path);
        return 0;
    }

    std::string token;
    bool ok;
    int value;

    out.clear();                                   // ORIG set::clear
    for (;;)
    {
        if (!ScanType(token, true))
            break;                                 // EOF → 共享尾块 return 1
        ok = false;                                // ORIG local_19=false 每轮重置
        if (token == "[apply ultimate]")
        {
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                out.insert(value);
            }
            continue;
        }
        if (token == "[/apply ultimate]")
            break;                                 // 结束标记 → 直接收尾
        // 其他 token：ORIG do-while(!=)[/apply ultimate]) 继续吞并
    }
    return 1;
}

// ============================================================================
// importVillageAttackedScript —— ORIG 0x089093ee
// [attacked map info] 段内每行 4 个 int 构成 stAttackedMapInfo_t push_back；
// 第 2/3/4 列 Scan 失败分别返回 0x1990/0x1994/0x1998；打开失败 0x197f；
// 成功返回 0。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 089093ee：clean 96/96（ORIG 99 insns）；
// p167 CLEAR。已对齐：4 列 ScanInt 独立错误码 0x1990/0x1994/0x1998、首列失败回标签、
// stAttackedMapInfo_t 逐字段 + push_back、打开失败 0x197f。
int importVillageAttackedScript(const char* path, stVillageAttacked_t& out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
        return 0x197f;

    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[attacked map info]")
        {
            for (;;)
            {
                stAttackedMapInfo_t info;
                info.m_vill = ScanInt(&ok);
                if (!ok)
                    break;                         // 首列失败 → 回标签扫描
                info.m_area = ScanInt(&ok);
                if (!ok)
                    return 0x1990;
                info.m_field8 = ScanInt(&ok);
                if (!ok)
                    return 0x1994;
                info.m_fieldC = ScanInt(&ok);
                if (!ok)
                    return 0x1998;
                out.m_mapList.push_back(info);
            }
        }
    }
    return 0;
}

// ============================================================================
// loadPvpMapBasicDataScript —— ORIG 0x0890a427
// [pvp map order]：idx 入 PVPMAP_DATA::m_mapIdx（m_name ctor 后 clear）push_back；
// [pvp map basic data]：首列弃读 + 6 列入全局表（3 int 表 + 3 bool 化 char 表），
// 计满 0x14 条打印超限日志并仍返回成功；EOF 返回成功；打开失败返回 0。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 0890a427：clean 160/160（ORIG 164 insns）；
// p167 CLEAR。已对齐：首列弃读、6 列全局表写、entryIdx>0x13 日志 + break、
// [pvp map order] while(out!=0) 形、PVPMAP_DATA ctor + clear + push_back。
bool loadPvpMapBasicDataScript(const char* path, std::vector<PVPMAP_DATA>* out)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(
            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
            0x1c00,
            "error : %s \xed\x8c\x8c\xec\x9d\xbc\xec\x9d\x84 \xec\xb0\xbe"
            "\xec\x9d\x84 \xec\x88\x98 \xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88"
            "\xeb\x8b\xa4.",
            path);
        return false;
    }

    if (out != 0)
        out->clear();

    std::string token;
    int orderIdx = 0;
    bool ok = false;
    int entryIdx = 0;

    for (;;)
    {
        if (!ScanType(token, true))
            break;                                 // EOF → 共享尾块 return true

        if (token == "[pvp map basic data]")
        {
            const int firstCol = ScanInt((bool*)0); // ORIG 首列读入 -0xc 后弃用
            (void)firstCol;
            PVPMAP_BIG_IMAGE[entryIdx] = ScanInt((bool*)0);
            PVPMAP_SMALL_IMAGE[entryIdx] = ScanInt((bool*)0);
            PVPMAP_DEATHMATCH_MODE[entryIdx] =
                static_cast<char>(ScanInt((bool*)0) != 0);
            PVPMAP_ONLY_TOURNAMENT_EXPOSURE[entryIdx] =
                static_cast<char>(ScanInt((bool*)0) != 0);
            PVPMAP_ONLY_NORMAL_EXPOSURE[entryIdx] =
                static_cast<char>(ScanInt((bool*)0) != 0);
            PVPMAP_ONLY_TOURNAMENT_RANDOM[entryIdx] =
                static_cast<char>(ScanInt((bool*)0) != 0);
            ++entryIdx;
            if (entryIdx > 0x13)                   // ORIG cmpl $0x13,jle 继续
            {
                LogManager::logFormat(
                    0, "../RDARScriptEtcParameter.cpp",
                    __FUNCTION__, 0x1c2c,
                    "ERROR : %d \xea\xb0\x9c \xec\x9d\xb4\xec\x83\x81\xec\x9d"
                    "\x98 pvp map \xec\x9d\x84 \xec\xb6\x94\xea\xb0\x80\xed"
                    "\x95\xa0\xec\x88\x98\xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88"
                    "\xeb\x8b\xa4!!!",
                    0x14);
                break;
            }
        }
        else if (token == "[pvp map order]")
        {
            // ORIG do-while 布局：先 jmp 条件，`out != 0` 走 setne 物化。
            while (out != 0)
            {
                orderIdx = ScanInt(&ok);           // ORIG 复用 -0x14 槽
                if (!ok)
                    break;                         // → 回外层标签扫描
                PVPMAP_DATA data;                  // ctor：仅构造 m_name
                data.m_mapIdx = orderIdx;
                data.m_name.clear();               // ORIG push 前 string::clear
                out->push_back(data);
            }
        }
    }
    return true;
}

// ============================================================================
// importVendingMachineInfoCore —— ORIG 0x0890b2a3
// 单台售货机脚本解析。[item group]…[/item group] 为一栏：
//   [group num]/[material a b] 直写；[output item rate count flag] 与
//   [bonus output …] 分别累计 m_giveCount/m_bonusCount 并 push
//   {itemId, 新累计值, count}；flag==1 时 (itemId,count) 追加 m_lottery。
// 错误码：0x1e0b 打开失败、0x1e27/0x1e2d/0x1e30 组头、0x1e3d/0x1e40/0x1e44
// output、0x1e55/0x1e58/0x1e5c bonus；组间未知标签记日志继续；EOF 返回 0。
// 注意：ORIG clear() 不清 m_lottery（仅 vecGive/vecBonus），忠实保留。
// ============================================================================
namespace
{

const char* const kVendOutputTag = "[output]";
const char* const kVendBonusTag = "[bonus output]";

}  // namespace

// [b28d-solo-InitScriptProducers_8 N] ORIG 0890b2a3：p167 CLEAR（39/39 调用点全等）；
// clean 320/321（fresh .o 口径，ORIG 327 insns，净 1 条）。已对齐：删多余死 string
// （ORIG 无第二 string ⇒ 帧 0xb0→0xa0、ok 回 -0x35）、ok!=true 的 xor 物化 ×14 与
// ORIG 全等、output/bonus 改 for(;;)+if(!ok)break 形（P165 两份物理独立块）、
// make_pair<unsigned long&,unsigned int&>、push_back(O&&)、P171 越界读照抄。
// 残余为 4.1/4.4 块序（ORIG 两连 jmp 中继 vs 4.4 jump-thread）——N104/N121 封顶。
int importVendingMachineInfoCore(const char* dir, const char* file,
                                 stVendingMachine_t& vm)
{
    if (!loadRDARScriptFile(dir, file))
        return 0x1e0b;

    std::string token;
    bool ok = false;

    for (;;)
    {
    scan_again:
        // ---- 组外：未知标签记日志，直到 [item group] 或 EOF ----
        for (;;)
        {
            if (!ScanType(token, true))
                goto eof_exit;                     // ORIG jne 尾岛（890b714）
            if (token == "[item group]")
                break;
            goto unknown_tag;                      // ORIG 冷块落函数尾
        }

        stVendingMachineInfo_t info;                   // ctor 后自 clear

        for (;;)
        {
            if (!ScanType(token, true))
                break;                                 // EOF：析构后外层再扫失败→0
            if (token == "[/item group]")
            {
                vm.m_infoList.push_back(info);
                info.clear();
                break;
            }
            if (token == "[group num]")
            {
                info.m_groupNum = ScanInt(&ok);
                if (ok != true)
                    return 0x1e27;
                continue;
            }
            if (token == "[material]")
            {
                info.m_payType = ScanInt(&ok);
                if (ok != true)
                    return 0x1e2d;
                info.m_payAmount = ScanInt(&ok);
                if (ok != true)
                    return 0x1e30;
                continue;
            }
            // P165：ORIG 0x890b444/0x890b572 为两份物理独立块（各 4 次
            // ScanInt(bool*)，槽集不同）⇒ 源内展开两份，勿用 bonus 标志参数化。
            if (token == kVendOutputTag)
            {
                stVendingMachineOutput_t entry;
                for (;;)
                {
                    entry.m_itemId = ScanInt(&ok);
                    if (!ok)
                        break;
                    const int rate = ScanInt(&ok);
                    if (ok != true)
                        return 0x1e3d;
                    entry.m_count = ScanInt(&ok);
                    if (ok != true)
                        return 0x1e40;
                    if (ScanInt(&ok) == 1)
                        info.m_lottery.push_back(
                            std::make_pair(
                                reinterpret_cast<unsigned long&>(
                                    entry.m_itemId),
                                entry.m_count));
                    if (ok != true)
                        return 0x1e44;
                    info.m_giveCount += rate;
                    entry.m_cumWeight = info.m_giveCount;
                    info.m_vecGive.push_back(entry);
                }
                continue;
            }
            if (token == kVendBonusTag)
            {
                stVendingMachineOutput_t entry;
                for (;;)
                {
                    entry.m_itemId = ScanInt(&ok);
                    if (!ok)
                        break;
                    const int rate = ScanInt(&ok);
                    if (ok != true)
                        return 0x1e55;
                    entry.m_count = ScanInt(&ok);
                    if (ok != true)
                        return 0x1e58;
                    if (ScanInt(&ok) == 1)
                        info.m_lottery.push_back(
                            std::make_pair(
                                reinterpret_cast<unsigned long&>(
                                    entry.m_itemId),
                                entry.m_count));
                    if (ok != true)
                        return 0x1e5c;
                    info.m_bonusCount += rate;
                    entry.m_cumWeight = info.m_bonusCount;
                    info.m_vecBonus.push_back(entry);
                }
                continue;
            }
            // 组内其他标签：静默继续（ORIG 无日志路径）
        }
    }
unknown_tag:
    LogManager::logFormat(
        0, "../RDARScriptEtcParameter.cpp",
        __FUNCTION__, 0x1e66,
        "ERROR : Unknown tag error. %s", token.c_str());
    goto scan_again;
eof_exit:
    return 0;
}

// ============================================================================
// importVendingMachineInfo —— ORIG 0x0890b74e
// 列表文件 → map<idx,name>；逐项构造 stVendingMachine_t{键, Core 解析}，
// 成功 push_back，Core 失败返回 0x1e7c。空列表 0x1e73，列表加载失败 0x1e83。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 0890b74e：clean 118/118（ORIG 118 insns）；
// p167 CLEAR。已对齐：P214①/P257 正向包裹、iterator 双缓存、Core 结果内联实参（P0）。
int importVendingMachineInfo(const char* dir, const char* list,
                             std::vector<stVendingMachine_t>& out)
{
    STScriptFileList fileList;
    // P214①/P257：正向包裹复现 ORIG `call; test %al,%al; je <尾部失败块>`
    // （4.4 对 `if(!boolcall())` 恒产 xor $1 + 就地内联失败块）。
    if (loadRDARScriptListFile(&fileList, dir, list, 0))
    {
        if (fileList.m_fileMap.empty())
            return 0x1e73;

        // ORIG 70-72：++it 紧跟 push_back（body 内语句，先于 vm 析构）⇒ while 形。
        std::map<int, std::string>::iterator
            it = fileList.m_fileMap.begin(),
            end = fileList.m_fileMap.end();
        while (it != end)
        {
            stVendingMachine_t vm;                     // ctor：仅构造 +0x04 vector
            vm.m_id = it->first;
            // P0：ORIG 无 rc 槽往返（call; test %eax; setne）⇒ 内联实参。
            if (importVendingMachineInfoCore(dir, it->second.c_str(), vm) != 0)
                return 0x1e7c;
            out.push_back(vm);
            ++it;
        }
        return 0;
    }
    return 0x1e83;
}

// ============================================================================
// importTowerInfo —— ORIG 0x0890b8d0
// 列表文件遍历：sprintf("%s%s") 拼 per-file 路径 → importDeathTowerScript →
// insert(key, tower)。列表加载失败 0x1eab、空列表 0x1e8e、单文件失败 0x1ea2。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 0890b8d0：clean 160/160（ORIG 160 insns）；
// p167 CLEAR。已对齐：P214① 正向包裹、sprintf 路径拼接、importDeathTowerScript +
// insert(key,tower)、三档错误码 0x1eab/0x1e8e/0x1ea2。
int importTowerInfo(const char* dir, const char* list,
                    std::map<unsigned int, stDeathTower_t>& out)
{
    STScriptFileList fileList;
    // P214①/P257：正向包裹复现 ORIG `call; test %al,%al; je <尾部失败块>`。
    if (loadRDARScriptListFile(&fileList, dir, list, 0))
    {
        if (fileList.m_fileMap.empty())
            return 0x1e8e;

        std::map<int, std::string>::iterator
            it = fileList.m_fileMap.begin(),
            end = fileList.m_fileMap.end();
        while (it != end)
        {
            unsigned int key;                          // ORIG -0x197c（块首声明）
            stDeathTower_t tower;
            // P312/P214③：`={0}` 触发 4.4 内联 rep stos 慢路（ORIG 20+ 条）
            char path[0xff] = {0};
            key = static_cast<unsigned int>(it->first);
            std::sprintf(path, "%s%s", dir, it->second.c_str());

            if (importDeathTowerScript(path, tower) != 0)
                return 0x1ea2;
            out.insert(std::make_pair(key, tower));
            ++it;
        }
        return 0;
    }
    return 0x1eab;
}

// ============================================================================
// importRankSystemScript —— ORIG 0x0890ed7f
// [dungeon clear time average]：7 int 行 → m_vec38；前 2 列为 key、后 5 列为
//   value 写 m_dungeonRank.m_map0，key 已存在则整体返回失败；
// [dungeon clear time reduction bonus]：pair(a,b) 入 m_map44 且 {a,b} 入
//   m_dungeonRank.m_vec18；[rank grade]：逐值 push m_vec5c 且前 9 个写
//   m_rankLevel[0..8]；[combo interval boundaries]：4 int 入 m_vec68；
// [combo practice bonus inerval]：连续 int 覆写 m_field74 直至 Scan 失败。
// 打开失败返回 0，EOF 返回 1。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 gap0] ORIG 0890ed7f：p167 CLEAR（48/48）；fresh .o
// 379/379（gap=0）。本轮修：①[dungeon clear time average] 的重复键守卫改 P222 正向
// 包裹 `if(it==end){insert}else return false;`（ORIG 0x890efd8 call eq; test; je
// 失败块；旧 `if(!(it==end))` 多一条 xor $1）；②[combo practice bonus inerval] 由
// do-while 改为单值 `out->m_field74=ScanInt(&ok); if(!ok) return true;`（ORIG
// 0x890f293..2b3 只读一值即回主循环头 0x890edcd，非段内循环）。已对齐：死 string
// （-0x70，P171）、stClearTimeAverage ctor 声明（P104）、[rank grade]/[reduction
// bonus] for(;;)+if(!ok)break 形、用户空 dtor 删除（P317）。残余仅 ScanType 失败块
// 极性（ORIG jne 尾块 vs 4.4 je+就地物化，0x890f2b5）——N104 封顶。
bool importRankSystemScript(const char* path, stRankSystemInfo* out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    std::string deadToken;                             // ORIG -0x70 死 string（P171）
    (void)deadToken;
    bool ok = false;

    for (;;)
    {
        if (!ScanType(token, true))
            return true;                               // EOF → 成功

        if (token == "[dungeon clear time average]")
        {
            for (;;)
            {
                stRankSystemInfo::stDungeonClearTimeData data;
                // ORIG 0x890ee15..：逐列独立检查，各 `xor $1;test;je` + 共享 break 出口
                data.m_v[0] = ScanInt(&ok);
                if (ok != true)
                    break;
                data.m_v[1] = ScanInt(&ok);
                if (ok != true)
                    break;
                data.m_v[2] = ScanInt(&ok);
                if (ok != true)
                    break;
                data.m_v[3] = ScanInt(&ok);
                if (ok != true)
                    break;
                data.m_v[4] = ScanInt(&ok);
                if (ok != true)
                    break;
                data.m_v[5] = ScanInt(&ok);
                if (ok != true)
                    break;
                data.m_v[6] = ScanInt(&ok);
                if (ok != true)
                    break;
                out->m_vec38.push_back(data);

                stClearTimeAverageMapKey key;          // ORIG ctor 即 clear
                key.m_a = data.m_v[0];
                key.m_b = data.m_v[1];
                stClearTimeAverage average;
                average.m_v[0] = data.m_v[2];
                average.m_v[1] = data.m_v[3];
                average.m_v[2] = data.m_v[4];
                average.m_v[3] = data.m_v[5];
                average.m_v[4] = data.m_v[6];

                // ORIG：迭代器先默认构造后赋值（P50/P314）+ `==` 比较（P220 反极性）
                std::map<stClearTimeAverageMapKey,
                         stClearTimeAverage>::iterator it;
                it = out->m_dungeonRank.m_map0.find(key);
                // ORIG 0x890efd8..f02c：call eq; test; je <fail 块>，then 直落
                // insert（P222 正向包裹；写 `if (!(it==end))` 会多一条 xor $1）。
                if (it == out->m_dungeonRank.m_map0.end())
                {
                    out->m_dungeonRank.m_map0.insert(
                        std::make_pair(key, average));
                }
                else
                {
                    return false;                      // key 已存在 → 失败
                }
            }
        }
        else if (token == "[dungeon clear time reduction bonus]")
        {
            int a;
            int b;
            // ORIG 0x890f05b..：两列各自 ScanInt+ok!=true break，再 insert/
            // bonus 逐字段/vec18.push_back，尾部 jmp 回循环头。
            for (;;)
            {
                a = ScanInt(&ok);
                if (ok != true)
                    break;
                b = ScanInt(&ok);
                if (ok != true)
                    break;
                out->m_map44.insert(std::make_pair(a, b));
                stBonusPointValues bonus;
                bonus.m_a = a;
                bonus.m_b = b;
                out->m_dungeonRank.m_vec18.push_back(bonus);
            }
        }
        else if (token == "[rank grade]")
        {
            int rankIdx = 0;
            int grade;
            // ORIG 0x890f156..：ScanInt → ok!=true break → push_back →
            // rankIdx>8 break → m_rankLevel[rankIdx]=grade → ++rankIdx 回跳。
            for (;;)
            {
                grade = ScanInt(&ok);
                if (ok != true)
                    break;
                out->m_vec5c.push_back(grade);
                if (rankIdx > 8)
                    break;
                out->m_dungeonRank.m_rankLevel[rankIdx] =
                    static_cast<unsigned short>(grade);
                ++rankIdx;
            }
        }
        else if (token == "[combo interval boundaries]")
        {
            for (;;)
            {
                stRankSystemInfo::stComboIntervalBoundary boundary;
                boundary.m_v[0] = ScanInt(&ok);
                if (ok != true)
                    break;
                boundary.m_v[1] = ScanInt(&ok);
                if (ok != true)
                    break;
                boundary.m_v[2] = ScanInt(&ok);
                if (ok != true)
                    break;
                boundary.m_v[3] = ScanInt(&ok);
                if (ok != true)
                    break;
                out->m_vec68.push_back(boundary);
            }
        }
        else if (token == "[combo practice bonus inerval]")
        {
            // ORIG 0x890f293..0x890f2b3：只读一值即回主循环头（0x890f2ad
            // `je +0x4e`），非段内 do-while 循环；ok==false → return 1。
            out->m_field74 = ScanInt(&ok);
            if (!ok)
                return true;
        }
    }
}

// ============================================================================
// importUnlimitChallengeScript —— ORIG 0x0890bc63（报告
// _lt_global_gt_/importUnlimitChallengeScript.md 汇编+Ghidra C 双段转写）。
// 无 tag 静态表：直接字符串比较 if/else 链；七职业段为 7 个独立 else-if 块
// （ORIG 每块 info/key/item 临时各占独立栈槽，subIdx 立即数 0..6，
//  如 0x0890be87 swordman info @-0x118、0x0890bfee fighter @-0x124 …），
// 忠实按 ORIG 展开。reward_server 行 = [等级, int, int] → m_vec1c；
// reward_client 行 = 同三列 → multimap（make_pair(key&, item&) 左值模板，
// ORIG 0x0891fb9b _ZSt9make_pairIR22stUnlimitChallengeKeyR... 实证）。
// 各段 while 头四 int 链式置零（0x0890be2e `movl $0,-0x20;mov -0x20,%eax;
// mov %eax,-0x24;…` 链形 = 源码 `int a=0,b=a,c=b,d=c;`）。
// onoff：ScanInt 失败仍 return true（ORIG 0x0890bd1a jne → return 1）；
// 成功则 movb 0/1 写 m_field0（bool）。server_id/level 首列失败 = 段结束回
// 主循环；reward 段第二/三列失败 = return false。message 段 do-while：
// 每轮构造 msg1/msg2（0x0890cc53/-0x1bc），id=ScanInt 失败结束段。
// ============================================================================
// 残余 DIFF 归因（N 类豁免，语义已全等、指令数 1085/1093）：
//  ① 4.1 while/if 块排布镜像：ORIG `call ScanType; xor$1; test; jne
//     exit-island(nop;jmp)`（0x0890bcd3..bcd9）+ body fallthrough；4.4.7 O0
//     恒生成反向 `je body` + exit fallthrough（范式库负例“空 else nop;jmp 岛
//     vs jump-threading”，无合法源码形态可翻转 BB 排布）；
//  ② message 段 ebx 汇聚 flag（0x0890cd42 `mov $0x1,%ebx` / 0x0890cd6f
//     `test %ebx,%ebx`）= 4.1 统一 dtor epilogue；4.4 每路径独立 dtor
//     （负例“EH 双份 dtor epilogue vs merge”）；
//  ③ 其余为 .o 重定位地址显示差（链接后 AE 口径消除）。
// [b28d-solo-InitScriptProducers_8 N] ORIG 0890bc63：p167 CLEAR（121/121 调用点全等）；
// fresh .o 1093/1092（gap=-1）。本轮修：①全部 `if(ok!=true)` → `if(!ok)`（45 处；
// ORIG 0x890bd0e 等为 `movzbl; xor $1; test` 负极性物化）；②[reward_server]/
// [reward_client] 的 `while(ScanType(sub,true))` → `for(;;){ if(!ScanType(sub,true))
// break; ... }`（ORIG 0x890be0c 循环头 `call; xor $1; test; je body`）；③[message]
// 段改 `if(!ok) break;` 正向后置（ORIG 0x890cc8c je 继续）。残余仅 ScanType 失败块
// 极性（ORIG jne 尾块 vs 4.4 je+mov $1,%ebx 就地物化，0x0890bcd3/0x0890cdb4）与
// 循环入口旋转岛——N104/N121 封顶，语义全等。
bool importUnlimitChallengeScript(const char* path, stUnlimitChallengeInfo* out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;     // ORIG -0xfc（先构造）
    std::string sub;       // ORIG -0x100
    int zeroInit = 0;      // ORIG -0x34：movl $0x0 后不再读（帧槽对齐保留）
    bool ok = false;       // ORIG -0x101
    (void)zeroInit;

    for (;;)
    {
        if (!ScanType(token, true))
            return true;                               // EOF → 成功

        if (token == "[onoff]")
        {
            int value = ScanInt(&ok);                  // -0x30
            if (!ok)
                return true;                           // 0x0890bd1a
            if (value == 0)
                out->m_field0 = false;                 // movb $0,(%eax)
            else
                out->m_field0 = true;                  // movb $1,(%eax)
        }
        else if (token == "[server_id]")
        {
            int value;                                 // -0x108
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                out->m_vec4.push_back(value);
            }
        }
        else if (token == "[level]")
        {
            int value;                                 // -0x10c
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                out->m_vec10.push_back(value);
            }
        }
        else if (token == "[reward_server]")
        {
            for (;;)
            {
                if (!ScanType(sub, true))
                    break;
                int col1;                              // -0x2c（首列=等级）
                int link;                              // -0x28（链上死槽）
                int col2;                              // -0x24（第二列）
                int col3;                              // -0x20（第三列）
                // ORIG 0x0890be2e：movl $0,-0x20 → -0x24 → -0x28 → -0x2c 链式
                // 置零（4.1.2 先声明近 ebp；4.4.7 相反，故声明/赋值拆分）。
                col3 = 0;
                col2 = col3;
                link = col2;
                col1 = link;
                if (sub == "[swordman]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;                     // 段结束 → ScanType 下一子标签
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x118
                        info.m_jobLevel = col1;
                        info.m_subIdx = 0;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[fighter]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x124
                        info.m_jobLevel = col1;
                        info.m_subIdx = 1;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[gunner]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x130
                        info.m_jobLevel = col1;
                        info.m_subIdx = 2;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[mage]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x13c
                        info.m_jobLevel = col1;
                        info.m_subIdx = 3;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[priest]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x148
                        info.m_jobLevel = col1;
                        info.m_subIdx = 4;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[atgunner]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x154
                        info.m_jobLevel = col1;
                        info.m_subIdx = 5;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[thief]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x160
                        info.m_jobLevel = col1;
                        info.m_subIdx = 6;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
            }
        }
        else if (token == "[reward_client]")
        {
            for (;;)
            {
                if (!ScanType(sub, true))
                    break;
                int col1;                              // -0x1c（首列=等级）
                int link;                              // -0x18（链上死槽）
                int col2;                              // -0x14（第二列）
                int col3;                              // -0x10（第三列）
                // ORIG 0x0890c45d 链式置零（同 reward_server，声明/赋值拆分）。
                col3 = 0;
                col2 = col3;
                link = col2;
                col1 = link;
                if (sub == "[swordman]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x164
                        stUnlimitChallengeItem item;   // -0x16c
                        key.m_a = col1;
                        key.m_b = 0;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[fighter]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x170
                        stUnlimitChallengeItem item;   // -0x178
                        key.m_a = col1;
                        key.m_b = 1;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[gunner]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x17c
                        stUnlimitChallengeItem item;   // -0x184
                        key.m_a = col1;
                        key.m_b = 2;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[mage]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x188
                        stUnlimitChallengeItem item;   // -0x190
                        key.m_a = col1;
                        key.m_b = 3;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[priest]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x194
                        stUnlimitChallengeItem item;   // -0x19c
                        key.m_a = col1;
                        key.m_b = 4;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[atgunner]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x1a0
                        stUnlimitChallengeItem item;   // -0x1a8
                        key.m_a = col1;
                        key.m_b = 5;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[thief]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x1ac
                        stUnlimitChallengeItem item;   // -0x1b4
                        key.m_a = col1;
                        key.m_b = 6;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
            }
        }
        else if (token == "[message]")
        {
            for (;;)
            {
                int id = 0;                            // -0xc
                std::string msg1;                      // -0x1b8
                std::string msg2;                      // -0x1bc
                id = ScanInt(&ok);
                if (!ok)
                    break;                             // 0x0890cd6f test ebx → 退出段
                ScanStr(&msg1);
                ScanStr(&msg2);
                stUnlimitChallengeMessageInfo info;   // -0x1c8（C1 0x891c84a）
                info.m_id = static_cast<short>(id);
                info.m_msg1 = msg1;
                info.m_msg2 = msg2;
                out->m_vec28.push_back(info);
            }
        }
    }
}

// ============================================================================
// importSeriaRoomDecoEvent —— ORIG 0x0895e4bf
// [object max count]/[fatigue]/[reward item mail expire] 单 int 直写；
// [using item] 清 vector<int> 后逐 int push；[reward list] 二元组入
// m_matchEventMap（第二列 Scan 结果不校验）；[sound info] 清 m_soundList 后
// 每行“序号(弃)+int+int+string+int”构造元素 push。打开/空指针返回 0。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 0895e4bf：clean 212/212（ORIG 217 insns）；
// p167 CLEAR。已对齐：逐段 ScanInt/ScanStr 形、seriaRoomDecoEventSoundInfo 生命周期、
// 未知标签日志继续。
bool importSeriaRoomDecoEvent(class seriaRoomDecoEventScript* s,
                              const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;
    if (s == 0)
        return false;

    std::string token;
    std::string deadToken;                         // ORIG -0x24 死 string（P171）
    (void)deadToken;
    int value = 0;
    bool ok = false;                               // ORIG -0x9 死局部（P171）
    (void)ok;

    for (;;)
    {
        if (!ScanType(token, true))
            break;                                 // EOF → 共享尾块 return true
        if (token == "[object max count]")
        {
            ScanInt(&s->m_maxEventItemCount);
        }
        else if (token == "[using item]")
        {
            s->m_eventItemIdx.clear();
            for (;;)
            {
                // ORIG 0x895e58c：ScanInt(int*) 返回 bool（非 ScanInt(bool*)）
                if (!ScanInt(&value))
                    break;
                s->m_eventItemIdx.push_back(value);
            }
        }
        else if (token == "[fatigue]")
        {
            ScanInt(&s->m_fatigueCondition);
        }
        else if (token == "[reward list]")
        {
            for (;;)
            {
                // ORIG 0x895e5f0：std::pair<int,int> 局部（-0x30）+ 转换构造
                std::pair<int, int> p;
                if (!ScanInt(&p.first))
                    break;                             // 首列失败回标签扫描
                ScanInt(&p.second);                    // 第二列不校验
                s->m_matchEventMap.insert(p);
            }
        }
        else if (token == "[reward item mail expire]")
        {
            ScanInt(&s->m_rewardMailExpireDate);
        }
        else if (token == "[sound info]")
        {
            s->m_soundList.clear();
            seriaRoomDecoEventSoundInfo elem;
            for (;;)
            {
                elem.clear();                          // ORIG 循环首 clear
                // ORIG 0x895e6bd：ScanInt(int*)（行序号弃读）
                if (!ScanInt(&value))
                    break;
                ScanInt(&elem.m_field0);
                ScanInt(&elem.m_field4);
                ScanStr(&elem.m_name);
                ScanInt(&elem.m_fieldC);
                s->m_soundList.push_back(elem);
            }
        }
    }
    return true;
}

// ============================================================================
// importUseJobFatigueGiftScript —— ORIG 0x0895f817
// 函数级静态 tag→id 表（ORIG __lookup 0x94f75f8/guard+__init_lock
// 0x94f7610/__inited 0x94f7614）：[/job fatigue]=0x217、[job type]=0x21b、
// [fatigue title]=0x229、[fatigue text]=0x22d、[reward list]=0x231、
// [/job type]=0x252。[/job type] 时 insert(jobType,gift) 并复位累计态。
// [reward list] 行校验：第 2 列 >=0、第 3 列 >0，违规立即返回失败。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 gap1] ORIG 0895f817：p167 CLEAR（62/62）；fresh .o
// 472/473（gap=1）。本轮按 P333 状态机重写惰性注册链：首轮 v=0 沿 case 0→0x217→
// 0x21b→0x229→0x22d→0x231→0x252 fallthrough 逐条 insert（ORIG 0x895fae7..fe75），
// 每 case 负极性 `if(!__inited){insert}else{...break;}`（ORIG `xor $1; test; je`；
// 旧一次性建表形净缺 45 条）；case 0/0x217 命中即 `return true`（ORIG 0x895fb44；
// 旧"无操作"是语义错）；EOF 改 break+循环外 `return ok`（P164 配套，ORIG 尾共享块
// 0x895ffaf 返回 ok 槽）。已对齐：value_type 转发构造（P315）、三件套移入循环体
// （P177）、fatigueMatchGift ctor throw()（P216）。残余仅循环入口旋转 jmp/nop 岛
// 与尾部岛——N104/N121 封顶。
bool importUseJobFatigueGiftScript(class useCharacterJobFatigue* out,
                                   const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;
    if (out == 0)
        return false;

    std::string token;
    bool ok = false;
    useCharacterJobFatigue::fatigueMatchGift gift;          // ctor 全清零
    int jobType = -1;                                       // ORIG local_128

    for (;;)
    {
        // ORIG 0x895f89f jne 0x895ffaf：EOF 走共享尾块返回 ok 槽（-0x11d）
        // 当前值（P164 配套：break + 循环外统一 return 得 ORIG 尾岛形）。
        if (!ScanType(token, true))
            break;

        // ORIG 0x895f8a5：三件套声明落循环体内（每轮穿 guard，P177）。
        static std::map<const char*, int, less_TCHAR> __lookup;  // ORIG __lookup
        static SpinLock __init_lock;                              // ORIG __init_lock
        static bool __inited = false;                             // ORIG __inited

    retry:
        // P333 状态机：首轮（未建表）v=0 进 case 0，沿 fallthrough 链逐条
        // 建表（ORIG 0x895fae7/0x895fb4e/0x895fc09/0x895fc7c/0x895fcef/
        // 0x895fe75），链尾置 __inited 后 goto retry 重查；此后 v=find 结果。
        int v;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator found =
                __lookup.find(token.c_str());
            v = (found != __lookup.end()) ? found->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto retry;
            }
            v = 0;                                           // 建表哨兵
        }

        // P315：ORIG 用 value_type 转发构造（pair<const char* const,int>
        // C1<char const(&)[N],int>），非 make_pair（后者多一层转换构造+EH）。
        typedef std::map<const char*, int, less_TCHAR>::value_type V;
        switch (v)
        {
        case 0:
        case 0x217:                                          // [/job fatigue]
            // ORIG 0x895faee：负极性 `xor $1; test; je`（P168a②：写 if(!__inited)）。
            if (!__inited)
                __lookup.insert(V("[/job fatigue]", 0x217));
            else
                return true;                                 // ORIG 0x895fb44
            // fallthrough
        case 0x21b:                                          // [job type]
            if (!__inited)
            {
                __lookup.insert(V("[job type]", 0x21b));
            }
            else
            {
                std::string jobName;
                ScanStr(&jobName);
                jobType = getCharacterJobToEnumValue(jobName);   // ORIG 0x895fbe0
                break;
            }
            // fallthrough
        case 0x229:                                          // [fatigue title]
            if (!__inited)
            {
                __lookup.insert(V("[fatigue title]", 0x229));
            }
            else
            {
                ScanStr(&gift.m_msg1);                       // +0x1c
                break;
            }
            // fallthrough
        case 0x22d:                                          // [fatigue text]
            if (!__inited)
            {
                __lookup.insert(V("[fatigue text]", 0x22d));
            }
            else
            {
                ScanStr(&gift.m_msg2);                       // +0x20
                break;
            }
            // fallthrough
        case 0x231:                                          // [reward list]
            if (!__inited)
            {
                __lookup.insert(V("[reward list]", 0x231));
            }
            else
            {
                for (;;)
                {
                    int first = 0;
                    int second = 0;
                    int third = 0;
                    first = ScanInt(&ok);
                    if (!ok)
                        break;
                    second = ScanInt(&ok);
                    if (!ok)
                        break;
                    if (second < 0)
                        return false;                        // ORIG jns 校验
                    third = ScanInt(&ok);
                    if (!ok)
                        break;
                    if (third <= 0)
                        return false;                        // ORIG jg 校验
                    gift.m_field18 = third;                  // mov %eax,-0x140
                    gift.m_rewardMap.insert(std::make_pair(first, second));
                }
                break;
            }
            // fallthrough
        case 0x252:                                          // [/job type]
            if (!__inited)
            {
                __lookup.insert(V("[/job type]", 0x252));
            }
            else
            {
                out->m_giftMap.insert(
                    std::make_pair(jobType, gift));          // ORIG make_pairIRiRN..E
                gift.m_field18 = 0;                          // movl $0,-0x140
                gift.m_rewardMap.clear();                    // map::clear(-0x158)
                jobType = -1;                                // movl $-1,-0x124
                break;
            }
            break;
        default:
            break;
        }

        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto retry;
        }
    }
    return ok;
}

// ============================================================================
// importSeriaRoomAnimationDecoEvent —— ORIG 0x08960861
// [using item] pair 序列清+填 m_itemMap；[reward item] 第三列弃读的二元组
// push m_rewardList；[ani list] 行 “key,string,string(layer 名),int×3” 经
// getDrawLayer 转层号入 m_aniList；[finish sound] 直写 m_finishSound；
// [finish ani] 循环 “[front]/[back]” 选层（静态表 0x318/0x31b），随后行
// string(name)+string(layer)+int×3 入 m_finishAniMap；未知层名返回失败。
// 该段仅能经 EOF（ScanStr 失败）退出。打开/空指针返回 0，EOF 返回 1。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 gap0] ORIG 08960861：p167 CLEAR（90/90）；fresh .o
// 628/628（gap=0）。本轮修真 bug：①[reward item] 第三列由 `(void)ScanInt(&value)`
// 改为 `if(!ScanInt(&reward.m_field8)) return false;`（ORIG 0x8960a36..a53 写对象
// +0x08 并检查返回值——旧码写错目标且缺失败门禁）；②key/val 提升为函数级局部
// （ORIG 0x89608bd/c7 入口初始化 -0xec/-0xf0，[using item]/[ani list] 复用）。
// 并按 P333 重写 [finish ani] 惰性注册链：首轮 v=0 沿 case 0→0x318→0x31b
// fallthrough 建表（ORIG 0x8960efb/0x8960f55），case 体负极性 if(!__inited)，
// default 在 __inited 时 return false（未知层名），三件套声明移入子循环（P177，
// ORIG 重扫 0x8960fc9 jmp 0x8960cf2）。残余仅 ScanType/ScanInt 失败块极性（ORIG
// jne 尾块 vs 4.4 je 就地物化）——N104 封顶。
bool importSeriaRoomAnimationDecoEvent(class seriaRoomDecoAniEventScript* s,
                                       const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;
    if (s == 0)
        return false;

    std::string token;
    std::string layerName;
    int key = 0;                                        // ORIG -0xec（[using item]/[ani list] 复用）
    int val = 0;                                        // ORIG -0xf0（[using item]）
    bool ok = false;

    for (;;)
    {
        if (!ScanType(token, true))
            return true;                                    // EOF → 成功

        if (token == "[using item]")
        {
            s->m_itemMap.clear();
            for (;;)
            {
                // ORIG：ScanInt(int*)（p167 MISSING ScanInt(int*)×3）；key/val 为
                // 函数级局部（ORIG 0x89608bd/0x89608c7 入口初始化 -0xec/-0xf0）。
                if (!ScanInt(&key))
                    break;
                if (!ScanInt(&val))
                    return false;
                // ORIG pair<int,int> 转发构造（非 make_pair）
                s->m_itemMap.insert(std::pair<int, int>(key, val));
            }
        }
        else if (token == "[reward item]")
        {
            s->m_rewardList.clear();
            for (;;)
            {
                seriaRoomDecoRewardInfo reward;
                if (!ScanInt(&reward.m_itemIdx))
                    break;
                if (!ScanInt(&reward.m_reward))
                    return false;
                // ORIG 0x8960a36..a53：第三列写 reward.m_field8（-0xfc+8）且
                // 检查 ScanInt 返回值（失败 → return false）；旧 `(void)ScanInt
                // (&value)` 既写错目标又缺失败门禁。
                if (!ScanInt(&reward.m_field8))
                    return false;
                s->m_rewardList.push_back(reward);
            }
        }
        else if (token == "[ani list]")
        {
            s->m_aniList.clear();
            seriaRoomDecoAniInfo info;
            for (;;)
            {
                info.clear();
                if (!ScanInt(&key))
                    break;
                if (!ScanStr(&info.m_name))
                    return false;
                if (!ScanStr(&layerName))
                    return false;
                info.m_layer = getDrawLayer(layerName.c_str());
                if (!ScanInt(&info.m_a))
                    return false;
                if (!ScanInt(&info.m_b))
                    return false;
                if (!ScanInt(&info.m_c))
                    return false;
                s->m_aniList.insert(std::make_pair(key, info));
            }
        }
        else if (token == "[finish sound]")
        {
            ScanStr(&s->m_finishSound);
        }
        else if (token == "[finish ani]")
        {
            seriaRoomDecoAniInfo info;
            for (;;)
            {
                info.clear();
                if (!ScanStr(&layerName))              // 层选择 token
                    break;                             // EOF → 回主循环（成功）

                // ORIG 0x8960cf2：三件套声明落子循环体内（每轮穿 guard，P177）。
                static std::map<const char*, int, less_TCHAR> __lookup;   // ORIG __lookup 0x94f75b8
                static SpinLock __init_lock;                              // ORIG __init_lock 0x94f75d0
                static bool __inited = false;                             // ORIG __inited 0x94f75d4

            retry:
                // P333 状态机：首轮 v=0 沿 case 0→0x31b fallthrough 建表
                // （ORIG 0x8960efb/0x8960f55），链尾置 __inited 后 goto retry
                // 重查（ORIG 0x8960fc9 jmp 0x8960cf2）；此后 v=find 结果。
                int v;
                if (__inited)
                {
                    // ORIG 8960e8c..8960ee5：ne 判定后 found->second 只读一次
                    // （ptEv×1，结果落 -0x1c 槽）再与 0x318/0x31b 比较。
                    std::map<const char*, int, less_TCHAR>::const_iterator found =
                        __lookup.find(layerName.c_str());
                    v = (found != __lookup.end()) ? found->second : -1;
                }
                else
                {
                    __init_lock.enter();
                    if (__inited)
                    {
                        __init_lock.leave();
                        goto retry;
                    }
                    v = 0;                             // 建表哨兵
                }

                int layerId;
                typedef std::map<const char*, int, less_TCHAR>::value_type V;
                switch (v)
                {
                case 0:
                case 0x318:                            // [front]
                    // ORIG 0x8960f02：负极性 `xor $1; test; je`（P168a②）。
                    if (!__inited)
                    {
                        __lookup.insert(V("[front]", 0x318));
                    }
                    else
                    {
                        layerId = 0;
                        break;
                    }
                    // fallthrough
                case 0x31b:                            // [back]
                    if (!__inited)
                    {
                        __lookup.insert(V("[back]", 0x31b));
                    }
                    else
                    {
                        layerId = 1;
                        break;
                    }
                    // fallthrough
                default:
                    if (__inited)
                        return false;                  // 未命中/未知层名 → 失败
                    break;
                }

                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                    goto retry;
                }

                if (!ScanStr(&info.m_name))
                    return false;
                if (!ScanStr(&layerName))
                    return false;
                info.m_layer = getDrawLayer(layerName.c_str());
                if (!ScanInt(&info.m_a))
                    return false;
                if (!ScanInt(&info.m_b))
                    return false;
                if (!ScanInt(&info.m_c))
                    return false;
                // ORIG make_pair<aniLayer::T&, seriaRoomDecoAniInfo&>（左值实参）
                seriaRoomDecoAniEventScript::aniLayer::T layer =
                    static_cast<seriaRoomDecoAniEventScript::aniLayer::T>(
                        layerId);
                s->m_finishAniMap.insert(std::make_pair(layer, info));
            }
        }
    }
}

// ============================================================================
// AvatarVariation::importAvatarColorVariation —— ORIG 0x088d444f
// 固定路径 "Etc/AvatarColorVariation.etc"；清 m_colorMap 后 [avatar color map]
// 段每行 “key r g b name”（RGB 字节宽度 mov %al 实证）；name 扫描成功时同时
// insert colorMap/nameMap，失败退出该段；主循环 EOF 返回成功。
// ============================================================================
namespace AvatarVariation
{

// [b28d-solo-InitScriptProducers_8 AE] ORIG 088d444f：clean 192/192（ORIG 194 insns）；
// p167 CLEAR。已对齐：m_colorMap/m_nameMap 双 insert、RGB 字节宽度 mov %al 实证、
// name 扫描失败退段。
bool importAvatarColorVariation(AvatarColorInfo* out)
{
    if (!loadRDARScriptFile("", "Etc/AvatarColorVariation.etc"))
        return false;

    out->m_colorMap.clear();

    std::string token;
    bool ok = false;
    int dead = 0;                                      // ORIG -0xc 死写入（P171）
    (void)dead;

    for (;;)
    {
        if (!ScanType(token, true))
            return true;

        if (token == "[avatar color map]")
        {
            for (;;)
            {
                colorRGB rgb;                          // ctor：三字节置 0xff
                const int key = ScanInt(&ok);
                if (!ok)
                    break;
                rgb.m_r = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                    break;
                rgb.m_g = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                    break;
                rgb.m_b = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                    break;

                std::string name;                      // ORIG 循环内 -0x4c
                ok = ScanStr(&name);
                if (!ok)
                    break;
                out->m_colorMap.insert(std::make_pair(key, rgb));
                out->m_nameMap.insert(std::make_pair(key, name));
            }
        }
    }
}

}  // namespace AvatarVariation

// ============================================================================
// 域内 POD 生命周期定义（ORIG W 符号转写；声明见 CDataManager.h）
// ============================================================================

// ORIG C1 0x089623ba：+0/+4/+0xc 清零，string 默认构造。
seriaRoomDecoEventSoundInfo::seriaRoomDecoEventSoundInfo()
    : m_field0(0), m_field4(0)   // m_name(+8) 自动构造先于 m_fieldC，故 m0/m4 入 init-list、m_fieldC 留 body(ORIG 序)
{
    m_fieldC = 0;
}

seriaRoomDecoEventSoundInfo::~seriaRoomDecoEventSoundInfo() {}

// ORIG clear 0x089623ee：string.clear 后三 int 清零。
void seriaRoomDecoEventSoundInfo::clear()
{
    m_name.clear();
    m_field0 = 0;
    m_field4 = 0;
    m_fieldC = 0;
}

// ORIG fatigueMatchGift C1 0x0896251c：成员默认构造后 +0x18=0、map/string clear。
useCharacterJobFatigue::fatigueMatchGift::fatigueMatchGift() throw()
    : m_field18(0)   // 声明先于 m_msg1(+1c)/m_msg2(+20) string 自动构造，故置 init-list(ORIG 序)
{
    m_rewardMap.clear();
    m_msg1.clear();
    m_msg2.clear();
}

// ORIG C1 0x089625e8：string 默认构造后调 clear（+0x04=7，其余 int 清零）。
seriaRoomDecoAniInfo::seriaRoomDecoAniInfo() throw()
{
    clear();
}

seriaRoomDecoAniInfo::~seriaRoomDecoAniInfo() {}

// ORIG clear 0x08962606：+0x04 置 7（无层哨兵），+0x08/+0x0c/+0x10 清零；
// 不触碰 m_name。
void seriaRoomDecoAniInfo::clear()
{
    m_layer = 7;
    m_a = 0;
    m_b = 0;
    m_c = 0;
}

// ORIG C1 0x0891b36a：三个 vector 成员默认构造后调 clear（尾部 call 0x891b3fe）。
stVendingMachineInfo_t::stVendingMachineInfo_t()
{
    clear();
}

stVendingMachineInfo_t::~stVendingMachineInfo_t() {}

// ORIG clear 0x0891b3fe：+0=0、+4=-1、+8=0、+0xc=0、vecGive.clear()、
// +0x1c=0、vecBonus.clear()。注意：不清 m_lottery（+0x2c），忠实保留。
void stVendingMachineInfo_t::clear()
{
    m_groupNum = 0;
    m_payType = -1;
    m_payAmount = 0;
    m_giveCount = 0;
    m_vecGive.clear();
    m_bonusCount = 0;
    m_vecBonus.clear();
}

// ORIG C1 0x0891c820：仅构造 +0x04 vector（m_id 保持未初始化）。
stVendingMachine_t::stVendingMachine_t() {}

// ORIG D1 0x083e636c：vector 析构（隐式等价）。
stVendingMachine_t::~stVendingMachine_t() {}
