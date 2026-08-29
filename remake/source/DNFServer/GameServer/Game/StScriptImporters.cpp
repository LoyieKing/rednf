// StScriptImporters.cpp —— st* 脚本结构 W 符号实现 + 全局导入函数（OrigStStructs 独占）。
//
// 覆盖 ORIG 真实符号（nm 实证均为 W = 头内联强度，本 TU 以类外定义提供同名符号）：
//   stQPGuideScript::stRecommendQPData  C1 0x891ba22 / C1ERKS_ 0x892f730 / D1 0x891ba6a
//                                      op= 0x892f7ea / clear 0x891ba80
//   stPartyMatchingInfo                 C1 0x891b78e / C1ERKS_ 0x892ead6 / D1 0x891ca12
//                                      op= 0x892ebe0 / clear 0x891b81c
//   stNeoplePCBANG                      C1 0x891acce / C1ERKS_ 0x89276f8 / D1 0x891c6c2
//                                      op= 0x89277be / clear 0x891ac9a
//   stBloodWarBgmTable                  C1 0x891a95c / C1ERKS_ 0x892d3b6 / D1 0x891c9fc
//                                      op= 0x892d464 / clear 0x891a9a4
//   stBloodPoint                        C1 0x891a9c4 / clear 0x891a9d8（importBloodWarEtcScript 依赖）
//   stUltimateWarMonsterInfo            C1 0x891aa04 / clear 0x891aa18（同上）
// 以及 ORIG T 导入函数：
//   importQPGuideScript    0x0890ff93 / importPartyMatchingInfo 0x0890f311
//   importPCBANGScript     0x08905773 / importBloodWarEtcScript 0x0890e3f8
//
// 报告：docs/class_func_reports/_lt_global_gt_/{importQPGuideScript,importPartyMatchingInfo,
//       importPCBANGScript,importBloodWarEtcScript}.md；W 符号无报告，objdump 直转
//       （汇编已逐条核对：clear/C1/C1ERKS_/D1/op= 各成员序与 ORIG 一致）。

#include <utility>
#include <vector>

#include "stQPGuideScript.h"
#include "stPartyMatchingInfo.h"
#include "stNeoplePCBANG.h"
#include "stBloodWarBgmTable.h"

#include "NpcScript.h"          // loadRDARScriptFile / getCharacterJob
#include "DNFLexWrapper.h"      // ScanType / ScanStr / ScanInt

// getGrowTypeIndex 定义于 InitScriptProducers_6.cpp（ORIG T 0x088be63d）；无权威头，
// 照 InitScriptProducers_7.cpp 局部声明模式。
int getGrowTypeIndex(const char* name);

// stBloodWarGrowTable / stBloodWarBloodPointTable —— importBloodWarEtcScript 形参类型
// （mangled P19stBloodWarGrowTable / P25stBloodWarBloodPointTable）。GCC 4.4 对
// typedef 到模板实例会展开 typedef 名，唯派生结构体才能产生 ORIG mangled。
// stBloodWarGrowTable 亦定义于 WarRoom.cpp（ImportWarRoomWPTable 形参，布局一致；
// 纯派生 struct 无成员函数，链接无符号冲突）。此处为 importBloodWarEtcScript
// 提供本 TU 视图。
struct stBloodWarGrowTable
    : public std::vector<std::vector<std::pair<int, int> > >
{
};
struct stBloodWarBloodPointTable
    : public std::vector<stBloodPoint>
{
};

// ============================================================================
// stQPGuideScript::stRecommendQPData（报告缺失，objdump 直转）
// ============================================================================

// ORIG C1 0x891ba22（0x48）：vector<int>(+0x8) C1 + clear()。
stQPGuideScript::stRecommendQPData::stRecommendQPData() : m_items()
{
    clear();
}

// ORIG D1 0x891ba6a（0x16）：仅 vector<int> 析构。
stQPGuideScript::stRecommendQPData::~stRecommendQPData()
{
}

// ORIG C1ERKS_ 0x892f730（0x36）：两 int + vector<int> 复制。
stQPGuideScript::stRecommendQPData::stRecommendQPData(stRecommendQPData const& other)
    : m_job(other.m_job), m_grow(other.m_grow), m_items(other.m_items)
{
}

// ORIG op= 0x892f7ea（0x39）：两 int + vector<int> 赋值。
stQPGuideScript::stRecommendQPData& stQPGuideScript::stRecommendQPData::operator=(
    stRecommendQPData const& other)
{
    m_job = other.m_job;
    m_grow = other.m_grow;
    m_items = other.m_items;
    return *this;
}

// ORIG clear 0x891ba80（0x3f）：+0x00/+0x04 int=-1、+0x08 vector clear + reserve(4)。
void stQPGuideScript::stRecommendQPData::clear()
{
    m_job = -1;
    m_grow = -1;
    m_items.clear();
    m_items.reserve(4);
}

// ============================================================================
// stPartyMatchingInfo（报告缺失，objdump 直转）
// ============================================================================

// ORIG C1 0x891b78e（0x8e）：两 string + vector<int> 构造后 clear()。
stPartyMatchingInfo::stPartyMatchingInfo()
    : m_channelName(), m_regionName(), m_dungeonIdx()
{
    clear();
}

// ORIG D1 0x891ca12（0x6d）：vector + 两 string 逆序析构。
stPartyMatchingInfo::~stPartyMatchingInfo()
{
}

// ORIG C1ERKS_ 0x892ead6（0x86）：两 string + vector<int> 复制。
stPartyMatchingInfo::stPartyMatchingInfo(stPartyMatchingInfo const& other)
    : m_channelName(other.m_channelName),
      m_regionName(other.m_regionName),
      m_dungeonIdx(other.m_dungeonIdx)
{
}

// ORIG op= 0x892ebe0（0x4d）：string op= ×2 + vector op=。
stPartyMatchingInfo& stPartyMatchingInfo::operator=(stPartyMatchingInfo const& other)
{
    m_channelName = other.m_channelName;
    m_regionName = other.m_regionName;
    m_dungeonIdx = other.m_dungeonIdx;
    return *this;
}

// ORIG clear 0x891b81c（0x2f）：两 string clear + vector<int> clear。
void stPartyMatchingInfo::clear()
{
    m_channelName.clear();
    m_regionName.clear();
    m_dungeonIdx.clear();
}

// ============================================================================
// stNeoplePCBANG（报告缺失，objdump 直转）
// ============================================================================

// ORIG C1 0x891acce（0x48）：vector<pair>(+0xc) C1 + clear()。
stNeoplePCBANG::stNeoplePCBANG() : m_status()
{
    clear();
}

// ORIG D1 0x891c6c2（0x16）：仅 vector<pair> 析构。
stNeoplePCBANG::~stNeoplePCBANG()
{
}

// ORIG C1ERKS_ 0x89276f8（0x42）：三 int + vector<pair> 复制。
stNeoplePCBANG::stNeoplePCBANG(stNeoplePCBANG const& other)
    : m_field0(other.m_field0),
      m_level(other.m_level),
      m_peopleCount(other.m_peopleCount),
      m_status(other.m_status)
{
}

// ORIG op= 0x89277be（0x45）：三 int + vector<pair> 赋值。
stNeoplePCBANG& stNeoplePCBANG::operator=(stNeoplePCBANG const& other)
{
    m_field0 = other.m_field0;
    m_level = other.m_level;
    m_peopleCount = other.m_peopleCount;
    m_status = other.m_status;
    return *this;
}

// ORIG clear 0x891ac9a（0x33）：+0x00/+0x04/+0x08 int=0 + vector<pair> clear。
void stNeoplePCBANG::clear()
{
    m_field0 = 0;
    m_level = 0;
    m_peopleCount = 0;
    m_status.clear();
}

// ============================================================================
// stBloodWarBgmTable（报告缺失，objdump 直转）
// ============================================================================

// ORIG C1 0x891a95c（0x48）：string(+0x4) C1 + clear()。
stBloodWarBgmTable::stBloodWarBgmTable() : m_name()
{
    clear();
}

// ORIG D1 0x891c9fc（0x16）：仅 string 析构。
stBloodWarBgmTable::~stBloodWarBgmTable()
{
}

// ORIG C1ERKS_ 0x892d3b6（0x2a）：int + string 复制。
stBloodWarBgmTable::stBloodWarBgmTable(stBloodWarBgmTable const& other)
    : m_id(other.m_id), m_name(other.m_name)
{
}

// ORIG op= 0x892d464（0x2d）：int + string 赋值。
stBloodWarBgmTable& stBloodWarBgmTable::operator=(stBloodWarBgmTable const& other)
{
    m_id = other.m_id;
    m_name = other.m_name;
    return *this;
}

// ORIG clear 0x891a9a4（0x1f）：+0x00 int=-1 + string clear。
void stBloodWarBgmTable::clear()
{
    m_id = -1;
    m_name.clear();
}

// ============================================================================
// stBloodPoint / stUltimateWarMonsterInfo（importBloodWarEtcScript 依赖，报告缺失）
// ============================================================================

// ORIG C1 0x891a9c4（0x13）= clear()；D1 0x893f604 trivial（5B）。
stBloodPoint::stBloodPoint()
{
    clear();
}

// ORIG clear 0x891a9d8（0x2c）：+0x00=4、+0x04/+0x08/+0x0c=0。
void stBloodPoint::clear()
{
    m_type = 4;
    m_v1 = 0;
    m_v2 = 0;
    m_v3 = 0;
}

// ORIG C1 0x891aa04（0x13）= clear()；D1 0x893f858 trivial（5B）。
stUltimateWarMonsterInfo::stUltimateWarMonsterInfo()
{
    clear();
}

// ORIG clear 0x891aa18（0x22）：三 int=0。
void stUltimateWarMonsterInfo::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
}

// ============================================================================
// importQPGuideScript —— ORIG T 0x0890ff93（报告全文转写）
// ============================================================================
int importQPGuideScript(const char* path, stQPGuideScript& out)
{
    if (!loadRDARScriptFile("", path))
        return 0x28b6;
    std::string token;
    bool bRec = true;    // [qp recommendation] 段可解析标志（-0x17）
    bool bRec2 = true;   // [qp recommendation] 记录循环标志（-0x16）
    bool bTool = true;   // [qp tooltip mapping] 循环标志（-0x15）
    stQPGuideScript::stRecommendQPData data;

    for (;;)
    {
        if (bRec && ScanType(token, true))
        {
            if (token == "[qp recommendation]")
            {
                while (bRec2)
                {
                    data.clear();
                    int job = -1;
                    std::string jobStr;
                    if (ScanStr(&jobStr))
                    {
                        job = getCharacterJob(jobStr.c_str());
                        if (job < 0 || job > 10)
                            return 0;
                        int grow = -1;
                        data.m_job = job;
                        std::string growStr;
                        if (ScanStr(&growStr))
                        {
                            grow = getGrowTypeIndex(growStr.c_str());
                            if (grow == 0xffff)
                                return 0;
                            int count = 0;
                            data.m_grow = grow;
                            if (ScanInt(&count))
                            {
                                if (count >= 1)
                                {
                                    int itemIdx = -1;
                                    for (int i = 0; i < count; ++i)
                                    {
                                        if (!ScanInt(&itemIdx))
                                            return 0;
                                        data.m_items.push_back(itemIdx);
                                    }
                                    if (job < 0 || job > 10)
                                        return 0;
                                    if (out.m_vec0.size() == 0xb)
                                        out.m_vec0[job].push_back(data);
                                    else
                                        return 0;
                                }
                                else
                                {
                                    return 0;
                                }
                            }
                            else
                            {
                                return 0;
                            }
                        }
                        else
                        {
                            return 0;
                        }
                    }
                    else
                    {
                        // ScanStr 失败：out[10] 已填充则结束记录段，否则返回 0。
                        if (out.m_vec0.at(10).empty())
                            return 0;
                        bRec2 = false;
                    }
                }
                continue;
            }
            if (token == "[qp tooltip mapping]")
            {
                while (bTool)
                {
                    int key = -1;
                    std::string val;
                    if (!ScanInt(&key))
                    {
                        if (out.m_tooltipMapC.empty())
                            return 0;
                        bTool = false;
                        bRec = false;
                        break;
                    }
                    if (key < 0)
                        return 0;
                    if (!ScanStr(&val))
                        return 0;
                    if (val != "")
                    {
                        if (out.m_tooltipMapC.find(key) == out.m_tooltipMapC.end())
                        {
                            out.m_tooltipMapC.insert(std::make_pair(key, val.c_str()));
                        }
                    }
                }
                continue;
            }
        }
        else
        {
            if (bRec || bRec2 || bTool)
                return 0;
            return 1;
        }
    }
}

// ============================================================================
// importPartyMatchingInfo —— ORIG T 0x0890f311（报告全文转写）
// ============================================================================
int importPartyMatchingInfo(const char* path, std::vector<stPartyMatchingInfo>* out)
{
    if (!loadRDARScriptFile("", path))
        return 0;
    std::string token;
    bool ok = false;
    while (ScanType(token, true))
    {
        if (token == "[common dungeon index]")
        {
            int v;
            do
            {
                v = ScanInt(&ok);
            } while (ok);
        }
        else if (token == "[channel name]")
        {
            stPartyMatchingInfo info;
            ScanStr(&info.m_channelName);
            while (true)
            {
                if (ScanType(token, true) && token == "[/channel name]")
                    break;
                if (token == "[region name]")
                {
                    ScanStr(&info.m_regionName);
                }
                while (true)
                {
                    if (ScanType(token, true) && token == "[/region name]")
                        break;
                    if (token == "[dungeon index]")
                    {
                        int idx;
                        while (idx = ScanInt(&ok), ok)
                        {
                            info.m_dungeonIdx.push_back(idx);
                        }
                        out->push_back(info);
                    }
                }
            }
        }
    }
    return 1;
}

// ============================================================================
// importPCBANGScript —— ORIG T 0x08905773（报告全文转写）
// ============================================================================
int importPCBANGScript(const char* path, std::vector<stNeoplePCBANG>& out)
{
    if (!loadRDARScriptFile("", path))
        return 0x1417;
    std::string token;
    out.clear();
    stNeoplePCBANG data;
    std::pair<ENUM_CHANGE_STATUS_TYPE, int> statusPair;
    while (ScanType(token, true))
    {
        if (token == "[level]")
        {
            data.m_level = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[people count]")
        {
            data.m_peopleCount = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[physical attack]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[physical defense]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(3);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[magical attack]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(1);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[magical defense]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(2);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[fire resistance]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x14);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[water resistance]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x15);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[dark resistance]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x16);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[light resistance]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x17);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[HP regen speed]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0xe);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[MP regen speed]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x1d);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[move speed]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0xb);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[attack speed]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(10);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[cast speed]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0xc);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[hit recovery]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x22);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[jump power]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(8);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[HP MAX]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0xd);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[MP MAX]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x30);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[/level]")
        {
            out.push_back(data);
            data.clear();
        }
    }
    return 1;
}

// ============================================================================
// importBloodWarEtcScript —— ORIG T 0x0890e3f8（报告全文转写）
// ============================================================================
int importBloodWarEtcScript(const char* path,
                            stBloodWarGrowTable* growTable,
                            stBloodWarBloodPointTable* bloodPoint,
                            std::vector<stBloodWarBgmTable>* bgm,
                            std::vector<stUltimateWarMonsterInfo>* monsterInfo)
{
    if (!loadRDARScriptFile("", path))
        return 0;
    std::string token;
    std::string str;
    bool ok = false;
    while (ScanType(token, true))
    {
        if (token == "[BGM]")
        {
            bool cont;
            do
            {
                stBloodWarBgmTable bgmTable;
                bgmTable.m_id = ScanInt(&ok);
                cont = false;
                if (ok)
                {
                    if (ScanStr(&str))
                    {
                        bgmTable.m_name = str;
                        if (bgm)
                            bgm->push_back(bgmTable);
                        cont = true;
                    }
                }
            } while (cont);
        }
        else if (token == "[BP TABLE]")
        {
            bool cont;
            do
            {
                std::vector<std::pair<int, int> > rows;
                for (int i = 0; i < 5; ++i)
                {
                    std::pair<int, int> p;
                    p.first = ScanInt(&ok);
                    if (!ok)
                        break;
                    p.second = ScanInt(&ok);
                    if (!ok)
                        break;
                    rows.push_back(p);
                }
                cont = false;
                if (ok)
                {
                    if (growTable)
                        growTable->push_back(rows);
                    cont = true;
                }
            } while (cont);
        }
        else if (token == "[BLOOD POINT]")
        {
            while (ScanStr(&str))
            {
                int type = 4;
                if (str == "named")
                    type = 0;
                else if (str == "crazy")
                    type = 1;
                else if (str == "round")
                    type = 2;
                else if (str == "nomal")
                    type = 3;
                int v1 = ScanInt(&ok);
                if (!ok)
                    break;
                int v2 = ScanInt(&ok);
                if (!ok)
                    break;
                int v3 = ScanInt(&ok);
                if (!ok)
                    break;
                if (bloodPoint && type != 4)
                {
                    stBloodPoint bp;
                    bp.m_type = type;
                    bp.m_v1 = v1;
                    bp.m_v2 = v2;
                    bp.m_v3 = v3;
                    bloodPoint->push_back(bp);
                }
            }
        }
        else if (token == "[BLOOD INFO]")
        {
            stUltimateWarMonsterInfo info;
            info.m_field0 = ScanInt(&ok);
            if (!ok)
                break;
            info.m_field4 = ScanInt(&ok);
            if (!ok)
                break;
            info.m_field8 = ScanInt(&ok);
            if (!ok)
                break;
            monsterInfo->push_back(info);
        }
    }
    return 1;
}
