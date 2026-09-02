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


int loadRDARScriptListFile(class STScriptFileList* list, const char* dir,
                           const char* listPath, std::map<int, int>* unused);
// _Z22loadRDARScriptListFile.. T 0x088bbd77（STScriptFileList.cpp）

// ---- PvP 全局表：定义于 PvP_Room.cpp 的三个，此处仅 extern 引用 ----
extern char PVPMAP_DEATHMATCH_MODE[0x14];            // ORIG B 0x94f70e0
extern char PVPMAP_ONLY_NORMAL_EXPOSURE[0x14];       // ORIG B 0x94f7108
extern char PVPMAP_ONLY_TOURNAMENT_RANDOM[0x14];     // ORIG B 0x94f711c

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
bool importResealTable(const char* path, reseal_table_t* out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    bool ok = false;
    int value = 0;

    while (ScanType(token, true))
    {
        if (token == "[rarity weight]")                       // 0x8e122be 区字符串
        {
            for (int i = 0; i < 6; ++i)
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
            for (int i = 0; i < 6; ++i)
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
            while ((value = ScanInt(&ok), ok))
                out->m_vec.push_back(static_cast<unsigned int>(value));
        }
    }
    return true;
}

// ============================================================================
// importUltimateDungeonList —— ORIG 0x08913c7e
// set 清空后扫描；[apply ultimate] 段内逐 int insert；遇到 [/apply ultimate]
// 或任意其他 token 连续吞至该结束标记；EOF 与正常结束均返回 1，打开失败返回 0。
// ============================================================================
int importUltimateDungeonList(const char* path, std::set<int>& out)
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
    bool ok = false;
    int value = 0;

    out.clear();                                   // ORIG set::clear
    for (;;)
    {
        ok = false;                                // ORIG local_19=false 每轮重置
        if (!ScanType(token, true))
            return 1;                              // EOF → 成功
        if (token == "[apply ultimate]")
        {
            while ((value = ScanInt(&ok), ok))
                out.insert(value);
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
int importVillageAttackedScript(const char* path, stVillageAttacked_t& out)
{
    std::string token;
    bool ok = false;

    if (!loadRDARScriptFile("", path))
        return 0x197f;

    while (ScanType(token, true))
    {
        if (token != "[attacked map info]")
            continue;
        for (;;)
        {
            stAttackedMapInfo_t info;
            ok = false;
            info.m_vill = ScanInt(&ok);
            if (!ok)
                break;                             // 首列失败 → 回标签扫描
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
    return 0;
}

// ============================================================================
// loadPvpMapBasicDataScript —— ORIG 0x0890a427
// [pvp map order]：idx 入 PVPMAP_DATA::m_mapIdx（m_name ctor 后 clear）push_back；
// [pvp map basic data]：首列弃读 + 6 列入全局表（3 int 表 + 3 bool 化 char 表），
// 计满 0x14 条打印超限日志并仍返回成功；EOF 返回成功；打开失败返回 0。
// ============================================================================
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
    bool ok = false;
    int orderIdx = 0;
    int entryIdx = 0;

    for (;;)
    {
        if (!ScanType(token, true))
            return true;                           // EOF → 成功

        if (token == "[pvp map basic data]")
        {
            (void)ScanInt((bool*)0);               // ORIG 首列读入 -0xc 后弃用
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
                return true;
            }
        }
        else if (token == "[pvp map order]")
        {
            if (out == 0)
                continue;                          // ORIG 循环条件带 NULL 检查
            for (;;)
            {
                ok = false;
                const int mapIdx = ScanInt(&ok);
                if (!ok)
                    break;                         // → 回标签扫描
                PVPMAP_DATA data;                  // ctor：仅构造 m_name
                data.m_mapIdx = mapIdx;
                data.m_name.clear();               // ORIG push 前 string::clear
                out->push_back(data);
            }
        }
    }
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

int importVendingMachineInfoCore(const char* dir, const char* file,
                                 stVendingMachine_t& vm)
{
    if (!loadRDARScriptFile(dir, file))
        return 0x1e0b;

    std::string token;
    bool ok = false;
    int result = 0;

    for (;;)
    {
        // ---- 组外：未知标签记日志，直到 [item group] 或 EOF ----
        for (;;)
        {
            if (!ScanType(token, true))
                return 0;
            if (token == "[item group]")
                break;
            LogManager::logFormat(
                0, "../RDARScriptEtcParameter.cpp",
                __FUNCTION__, 0x1e66,
                "ERROR : Unknown tag error. %s", token.c_str());
        }

        stVendingMachineInfo_t info;                   // ctor 后自 clear

        bool cont = true;
        while (cont)
        {
            if (!ScanType(token, true))
            {
                cont = false;                          // EOF：析构后外层再扫失败→0
                break;
            }
            if (token == "[/item group]")
            {
                vm.m_infoList.push_back(info);
                info.clear();
                cont = false;
                break;
            }
            if (token == "[group num]")
            {
                info.m_groupNum = ScanInt(&ok);
                if (!ok)
                {
                    result = 0x1e27;
                    cont = false;
                    break;
                }
                continue;
            }
            if (token == "[material]")
            {
                info.m_payType = ScanInt(&ok);
                if (!ok)
                {
                    result = 0x1e2d;
                    cont = false;
                    break;
                }
                info.m_payAmount = ScanInt(&ok);
                if (!ok)
                {
                    result = 0x1e30;
                    cont = false;
                    break;
                }
                continue;
            }
            if (token == kVendOutputTag || token == kVendBonusTag)
            {
                const bool bonus = (token == kVendBonusTag);
                for (;;)
                {
                    ok = false;
                    const int itemId = ScanInt(&ok);
                    if (!ok)
                        break;                         // → 回组内标签扫描
                    const int rate = ScanInt(&ok);
                    if (!ok)
                    {
                        result = bonus ? 0x1e55 : 0x1e3d;
                        cont = false;
                        break;
                    }
                    const int count = ScanInt(&ok);
                    if (!ok)
                    {
                        result = bonus ? 0x1e58 : 0x1e40;
                        cont = false;
                        break;
                    }
                    const int flag = ScanInt(&ok);
                    if (flag == 1)
                        info.m_lottery.push_back(
                            std::make_pair(itemId, count));
                    if (!ok)
                    {
                        result = bonus ? 0x1e5c : 0x1e44;
                        cont = false;
                        break;
                    }
                    if (!bonus)
                    {
                        info.m_giveCount += rate;
                        stVendingMachineOutput_t entry;
                        entry.m_itemId = itemId;
                        entry.m_cumWeight = info.m_giveCount;
                        entry.m_count = static_cast<unsigned int>(count);
                        info.m_vecGive.push_back(entry);
                    }
                    else
                    {
                        info.m_bonusCount += rate;
                        stVendingMachineOutput_t entry;
                        entry.m_itemId = itemId;
                        entry.m_cumWeight = info.m_bonusCount;
                        entry.m_count = static_cast<unsigned int>(count);
                        info.m_vecBonus.push_back(entry);
                    }
                }
                continue;
            }
            // 组内其他标签：静默继续（ORIG 无日志路径）
        }
        if (!cont && result != 0)
            return result;                             // 中途失败：错误码直返
    }
}

// ============================================================================
// importVendingMachineInfo —— ORIG 0x0890b74e
// 列表文件 → map<idx,name>；逐项构造 stVendingMachine_t{键, Core 解析}，
// 成功 push_back，Core 失败返回 0x1e7c。空列表 0x1e73，列表加载失败 0x1e83。
// ============================================================================
int importVendingMachineInfo(const char* dir, const char* list,
                             std::vector<stVendingMachine_t>& out)
{
    STScriptFileList fileList;
    if (!loadRDARScriptListFile(&fileList, dir, list, 0))
        return 0x1e83;
    if (fileList.m_fileMap.empty())
        return 0x1e73;

    for (std::map<int, std::string>::iterator it = fileList.m_fileMap.begin();
         it != fileList.m_fileMap.end(); ++it)
    {
        stVendingMachine_t vm;                         // ctor：仅构造 +0x04 vector
        vm.m_id = it->first;
        const int rc =
            importVendingMachineInfoCore(dir, it->second.c_str(), vm);
        if (rc != 0)
            return 0x1e7c;
        out.push_back(vm);
    }
    return 0;
}

// ============================================================================
// importTowerInfo —— ORIG 0x0890b8d0
// 列表文件遍历：sprintf("%s%s") 拼 per-file 路径 → importDeathTowerScript →
// insert(key, tower)。列表加载失败 0x1eab、空列表 0x1e8e、单文件失败 0x1ea2。
// ============================================================================
int importTowerInfo(const char* dir, const char* list,
                    std::map<unsigned int, stDeathTower_t>& out)
{
    STScriptFileList fileList;
    if (!loadRDARScriptListFile(&fileList, dir, list, 0))
        return 0x1eab;
    if (fileList.m_fileMap.empty())
        return 0x1e8e;

    char path[0xff];
    for (std::map<int, std::string>::iterator it = fileList.m_fileMap.begin();
         it != fileList.m_fileMap.end(); ++it)
    {
        std::memset(path, 0, sizeof(path));            // ORIG memset 0xff 字节
        std::sprintf(path, "%s%s", dir, it->second.c_str());

        stDeathTower_t tower;
        if (importDeathTowerScript(path, tower) != 0)
            return 0x1ea2;
        out.insert(std::make_pair(static_cast<unsigned int>(it->first), tower));
    }
    return 0;
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
bool importRankSystemScript(const char* path, stRankSystemInfo* out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
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
                ok = false;
                data.m_v[0] = ScanInt(&ok);
                if (!ok) break;
                data.m_v[1] = ScanInt(&ok);
                if (!ok) break;
                data.m_v[2] = ScanInt(&ok);
                if (!ok) break;
                data.m_v[3] = ScanInt(&ok);
                if (!ok) break;
                data.m_v[4] = ScanInt(&ok);
                if (!ok) break;
                data.m_v[5] = ScanInt(&ok);
                if (!ok) break;
                data.m_v[6] = ScanInt(&ok);
                if (!ok) break;
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

                // ORIG：find 命中（!= end）即整体失败
                if (out->m_dungeonRank.m_map0.find(key) !=
                    out->m_dungeonRank.m_map0.end())
                    return false;
                out->m_dungeonRank.m_map0.insert(
                    std::make_pair(key, average));
            }
        }
        else if (token == "[dungeon clear time reduction bonus]")
        {
            for (;;)
            {
                ok = false;
                const int a = ScanInt(&ok);
                if (!ok) break;
                const int b = ScanInt(&ok);
                if (!ok) break;
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
            for (;;)
            {
                ok = false;
                const int grade = ScanInt(&ok);
                if (!ok)
                    break;
                out->m_vec5c.push_back(grade);
                if (!(rankIdx < 9))                    // ORIG i<9（含下标 8）
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
                if (!ok) break;
                boundary.m_v[1] = ScanInt(&ok);
                if (!ok) break;
                boundary.m_v[2] = ScanInt(&ok);
                if (!ok) break;
                boundary.m_v[3] = ScanInt(&ok);
                if (!ok) break;
                out->m_vec68.push_back(boundary);
            }
        }
        else if (token == "[combo practice bonus inerval]")
        {
            do
            {
                out->m_field74 = ScanInt(&ok);
            } while (ok);
        }
    }
}

// ============================================================================
// importUnlimitChallengeScript —— ORIG 0x0890bc63
// [onoff]（Scan 失败仍返回成功）、[server_id]/[level]、
// [reward_server]/[reward_client] 七职业段（swordman..thief → 子索引 0..6）、
// [message]。reward_server 行 → stUnlimitChallengeRewardInfo{u16,u8,int,int}
// push m_vec1c；reward_client 行 → multimap<(u16 key,u8 sub),(int,int)>。
// 打开/中途 Scan 失败返回 0，EOF 返回 1。
// ============================================================================
namespace
{

const char* const kUncJobTags[7] = {
    "[swordman]", "[fighter]", "[gunner]", "[mage]",
    "[priest]", "[atgunner]", "[thief]"
};

}  // namespace

bool importUnlimitChallengeScript(const char* path, stUnlimitChallengeInfo* out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    std::string sub;
    bool ok = false;

    for (;;)
    {
        if (!ScanType(token, true))
            return true;                               // EOF → 成功

        if (token == "[onoff]")
        {
            const int flag = ScanInt(&ok);
            if (!ok)
                return true;                           // ORIG quirk：仍返回成功
            out->m_field0 = (flag == 0) ? 0 : 1;
        }
        else if (token == "[server_id]")
        {
            for (;;)
            {
                const int id = ScanInt(&ok);
                if (!ok)
                    break;
                out->m_vec4.push_back(id);
            }
        }
        else if (token == "[level]")
        {
            for (;;)
            {
                const int level = ScanInt(&ok);
                if (!ok)
                    break;
                out->m_vec10.push_back(level);
            }
        }
        else if (token == "[reward_server]" || token == "[reward_client]")
        {
            const bool client = (token == "[reward_client]");
            while (ScanType(sub, true))
            {
                int jobIdx = -1;
                for (int j = 0; j < 7; ++j)
                {
                    if (sub == kUncJobTags[j])
                    {
                        jobIdx = j;
                        break;
                    }
                }
                if (jobIdx < 0)
                    break;                             // 未知子标签回主循环

                for (;;)
                {
                    ok = false;
                    const int a = ScanInt(&ok);
                    if (!ok)
                        break;                         // 段结束
                    const int b = ScanInt(&ok);
                    if (!ok)
                        return false;
                    const int c = ScanInt(&ok);
                    if (!ok)
                        return false;

                    if (!client)
                    {
                        stUnlimitChallengeRewardInfo info;
                        info.m_jobLevel = static_cast<unsigned short>(a);
                        info.m_subIdx = static_cast<unsigned char>(jobIdx);
                        info.m_a = b;
                        info.m_b = c;
                        out->m_vec1c.push_back(info);
                    }
                    else
                    {
                        stUnlimitChallengeKey key;
                        key.m_a = static_cast<unsigned short>(a);
                        key.m_b = static_cast<unsigned char>(jobIdx);
                        stUnlimitChallengeItem item;
                        item.m_a = b;
                        item.m_b = c;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
            }
        }
        else if (token == "[message]")
        {
            for (;;)
            {
                ok = false;
                const int id = ScanInt(&ok);
                if (!ok)
                    break;
                std::string msg1;
                std::string msg2;
                ScanStr(&msg1);
                ScanStr(&msg2);
                stUnlimitChallengeMessageInfo info;
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
bool importSeriaRoomDecoEvent(class seriaRoomDecoEventScript* s,
                              const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;
    if (s == 0)
        return false;

    std::string token;
    bool ok = false;
    int value = 0;

    while (ScanType(token, true))
    {
        if (token == "[object max count]")
        {
            ScanInt(&s->m_maxEventItemCount);
        }
        else if (token == "[using item]")
        {
            s->m_eventItemIdx.clear();
            for (;;)
            {
                value = ScanInt(&ok);                  // ScanInt(int*) 返回 bool
                if (!ok)
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
                int first = 0;
                int second = 0;
                if (!ScanInt(&first))
                    break;                             // 首列失败回标签扫描
                ScanInt(&second);                      // 第二列不校验
                s->m_matchEventMap.insert(std::make_pair(first, second));
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
                ok = false;
                value = ScanInt(&ok);                  // 行序号（弃）
                if (!ok)
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
bool importUseJobFatigueGiftScript(class useCharacterJobFatigue* out,
                                   const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;
    if (out == 0)
        return false;

    static std::map<const char*, int, less_TCHAR> __lookup;   // ORIG __lookup
    static SpinLock __init_lock;                               // ORIG __init_lock
    static bool __inited = false;                             // ORIG __inited

    std::string token;
    bool ok = false;
    int jobType = -1;                                       // ORIG local_128
    useCharacterJobFatigue::fatigueMatchGift gift;          // ctor 全清零

    while (ScanType(token, true))
    {
        if (!__inited)
        {
            __init_lock.enter();
            if (!__inited)
            {
                // ORIG 插入链 0x895fc3c/0x895fca6/0x895fd18/0x895fa10/
                //                 0x895fb70/0x895fe83
                __lookup.insert(std::make_pair("[/job fatigue]", 0x217));
                __lookup.insert(std::make_pair("[job type]", 0x21b));
                __lookup.insert(std::make_pair("[fatigue title]", 0x229));
                __lookup.insert(std::make_pair("[fatigue text]", 0x22d));
                __lookup.insert(std::make_pair("[reward list]", 0x231));
                __lookup.insert(std::make_pair("[/job type]", 0x252));
                __init_lock.leave();
                __inited = true;
            }
            else
            {
                __init_lock.leave();
            }
        }

        std::map<const char*, int, less_TCHAR>::const_iterator found =
            __lookup.find(token.c_str());
        const int id = (found != __lookup.end()) ? found->second : -1;

        if (id == 0x217)
        {
            // [/job fatigue]：无操作
        }
        else if (id == 0x21b)
        {
            std::string jobName;
            ScanStr(&jobName);
            jobType = getCharacterJobToEnumValue(jobName);   // ORIG 0x895fbe0
        }
        else if (id == 0x229)
        {
            ScanStr(&gift.m_msg1);                           // +0x1c
        }
        else if (id == 0x22d)
        {
            ScanStr(&gift.m_msg2);                           // +0x20
        }
        else if (id == 0x231)
        {
            for (;;)
            {
                ok = false;
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
                    return false;                            // ORIG jns 校验
                third = ScanInt(&ok);
                if (!ok)
                    break;
                if (third <= 0)
                    return false;                            // ORIG jg 校验
                gift.m_field18 = third;                      // mov %eax,-0x140
                gift.m_rewardMap.insert(std::make_pair(first, second));
            }
        }
        else if (id == 0x252)
        {
            out->m_giftMap.insert(
                std::make_pair(jobType, gift));              // ORIG make_pairIRiRN..E
            gift.m_field18 = 0;                              // movl $0,-0x140
            gift.m_rewardMap.clear();                        // map::clear(-0x158)
            jobType = -1;                                    // movl $-1,-0x124
        }
        // 未命中（id==-1）：跳过继续
    }
    return true;
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
bool importSeriaRoomAnimationDecoEvent(class seriaRoomDecoAniEventScript* s,
                                       const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;
    if (s == 0)
        return false;

    static std::map<const char*, int, less_TCHAR> __lookup;   // ORIG __lookup 0x94f75b8
    static SpinLock __init_lock;                               // ORIG __init_lock 0x94f75d0
    static bool __inited = false;                             // ORIG __inited 0x94f75d4

    std::string token;
    std::string layerName;
    bool ok = false;
    int value = 0;

    for (;;)
    {
        if (!ScanType(token, true))
            return true;                                    // EOF → 成功

        if (token == "[using item]")
        {
            s->m_itemMap.clear();
            for (;;)
            {
                ok = false;
                const int key = ScanInt(&ok);
                if (!ok)
                    break;
                int val = 0;
                if (!ScanInt(&val))
                    return false;
                s->m_itemMap.insert(std::make_pair(key, val));
            }
        }
        else if (token == "[reward item]")
        {
            s->m_rewardList.clear();
            for (;;)
            {
                seriaRoomDecoRewardInfo reward;
                ok = false;
                reward.m_itemIdx = ScanInt(&ok);
                if (!ok)
                    break;
                if (!ScanInt(&reward.m_reward))
                    return false;
                (void)ScanInt(&value);                  // ORIG 第三列弃读
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
                ok = false;
                int key = 0;
                key = ScanInt(&ok);
                if (!ok)
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
                if (!__inited)
                {
                    __init_lock.enter();
                    if (!__inited)
                    {
                        __lookup.insert(std::make_pair("[front]", 0x318));
                        __lookup.insert(std::make_pair("[back]", 0x31b));
                        __init_lock.leave();
                        __inited = true;
                    }
                    else
                    {
                        __init_lock.leave();
                    }
                }
                std::map<const char*, int, less_TCHAR>::const_iterator found =
                    __lookup.find(layerName.c_str());
                int layerId;
                if (found == __lookup.end())
                    return false;                      // ORIG 未命中 → 失败
                if (found->second == 0x318)
                    layerId = 0;                       // [front]
                else if (found->second == 0x31b)
                    layerId = 1;                       // [back]
                else
                    return false;

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
                s->m_finishAniMap.insert(
                    std::make_pair(
                        static_cast<seriaRoomDecoAniEventScript::aniLayer::T>(
                            layerId),
                        info));
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

bool importAvatarColorVariation(AvatarColorInfo* out)
{
    if (!loadRDARScriptFile("", "Etc/AvatarColorVariation.etc"))
        return false;

    out->m_colorMap.clear();

    std::string token;
    std::string name;
    bool ok = false;

    for (;;)
    {
        if (!ScanType(token, true))
            return true;

        if (token != "[avatar color map]")
            continue;

        for (;;)
        {
            colorRGB rgb;                              // ctor：三字节置 0xff
            ok = false;
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

            ok = ScanStr(&name);
            if (ok)
            {
                out->m_colorMap.insert(std::make_pair(key, rgb));
                out->m_nameMap.insert(std::make_pair(key, name));
            }
            if (!ok)
                break;                                 // ORIG do-while(ok)
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
useCharacterJobFatigue::fatigueMatchGift::fatigueMatchGift()
{
    m_field18 = 0;
    m_rewardMap.clear();
    m_msg1.clear();
    m_msg2.clear();
}

// ORIG C1 0x089625e8：string 默认构造后调 clear（+0x04=7，其余 int 清零）。
seriaRoomDecoAniInfo::seriaRoomDecoAniInfo()
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
