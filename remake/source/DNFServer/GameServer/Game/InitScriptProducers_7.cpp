// ============================================================================
// df_game_r Game/ —— CDataManager::init() 脚本装载自由函数（第 7 批）。
//
// 本 TU 承载 DataManagerInitDepends.h 声明的以下 12 个 import*（全部为 ORIG nm
// 真实 T 符号；逐函数按 objdump 反汇编忠实转写，调用点见
// docs/class_func_reports/CDataManager/init.md）：
//
//   importEventAvatarListScript        0x0895e1ed  init.md +0x688b
//   importEventEtcScript               0x0895e30f  init.md +0x6998
//   importFairPVPEquipment             0x088ffbfc  init.md +0x2328
//   importGrowthEquipmentScript        0x0895eb3f  init.md +0x0576
//   importHellPartyScript              0x08909857  init.md +0x0d30
//   importInGameAdvertisementEtcScript 0x0897e9c4  init.md +0x048c
//   importInGameEventInfoList          0x0897eb59  init.md +0x674d
//   importLimitItemUsageInfo           0x0890d871  init.md +0x562b
//   importMercenarySystemEtcScript     0x089f6c50  init.md +0x6b19
//   importNPCRelationEventScript       0x0890d44f  init.md +0x5a5f
//   importPcBangEquipmentRendar        0x08915606  init.md +0x6be8
//   importQuestShopScript              0x0890fa3d  init.md +0x10d4
//
// 另提供上述函数直接依赖、当前无任何定义的 ORIG 小符号：
//   makeFairPVPEquipmentPackageKey 由 InitScripts_2 提供（勿在此重复定义）
//   tokenizeStr / ConvertStringToTime / getGrowTypeIndex 由 InitScripts_6 提供
//   loadAppendageType / loadValueType 沿用 extern 声明（定义归其报告域批次）
//   stLimitItemUsageInfo::Add 0x891b4a8 / stResetItemInfo::Add 0x891b4dc /
//   stRefillItemInfo::Add 0x891b502
//   stQuestShopStatus C1 0x891b948 / clear 0x891b8cc / D1 0x83cd826
//   stStatusLevelInfo C1 0x891b8b4；stQuestShoptInitCost C1 0x837109a
//   stNPCRelationEventInfo C1 0x891c946
//   stMercenaryRewardGroup C1 0x89f7a9c / stMercenaryCompetitionArea C1 0x89f7b28
//   stHellPartyMonsterGroupList C1 0x891b1d6 / stHellParty_t C1 0x891b242 D1 0x891c7e2
// ============================================================================

#include "CDataManager.h"        // DataManagerInitDepends.h 形参类型宿主，必须先包含

#include "DataManagerInitDepends.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include "DNFLexWrapper.h"
#include "EventAvatarListScript.h"

#include "CEventScriptMng.h"   // InGameEventInfo / InGameEventInfoList

// ---- 外部脚本助手（定义在兄弟 TU，此处仅引用）----
int loadAppendageType(const std::string& token);                       // 0x0889253b
int loadValueType(const std::string& token);                           // 0x08892ac4
void tokenizeStr(const std::string& str, std::vector<std::string>& out,
                 const std::string& delim);                            // 0x088945e2（InitScripts_6）
int getCharacterJob(const char* name);                                 // 0x088be57e（NpcScript.h 域）
bool ConvertStringToTime(std::string str, long& out);                  // 0x088c2341（InitScripts_6）
int getGrowTypeIndex(const char* name);                                // 0x088be63d（InitScripts_6）

namespace
{

// ORIG 各装载器内的「静态名→id 表 + SpinLock 双检」在单线程 init 下等价于
// 固定常量表查找。表内容逐条来自反汇编中的 insert 对（字符串地址→id）。
struct TokenId
{
    const char* name;
    int id;
};

// importInGameEventInfoList 0x897eb59 插入对（0x94f7ba8 表）。
const TokenId kInGameEventTokenIds[] =
{
    {"[/event_list]", 0x24},
    {"[idx]",         0x27},
    {"[record type]", 0x2b},
    {"[start]",       0x2e},
    {"[end]",         0x31},
    {"[state]",       0x34},
    {"[/event]",      0x37},
};

int lookupTokenId(const std::string& token, const TokenId* table, unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        if (token == table[i].name)
        {
            return table[i].id;
        }
    }
    return -1;
}

} // namespace

// ============================================================================
// importEventAvatarListScript — ORIG 0x0895e1ed（init.md +0x688b 调用，
// "Event/EventAvatarList.evt" → m_eventAvatarList）。
// 语义：loadRDARScriptFile("",path)；clear；扫描 "[avatar list]" 后连续
// ScanInt 入 set（ScanInt 失败回主循环）；"[event avatar list end]" 置返回
// 标记。返回该标记（未见到 end 段返回 false）。
// ============================================================================
bool importEventAvatarListScript(EventAvatarListScript* s, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    s->clear();

    std::string token;
    bool seenEnd = false;
    int value = -1;
    int unused0 = 0; (void)unused0;   // ORIG 栈槽 -0xc 同款死变量（-O0 保留存储）

    while (ScanType(token, true))
    {
        if (token == "[avatar list]")
        {
            value = -1;
            for (;;)
            {
                value = ScanInt(&seenEnd);          // 复用同一 bool（ORIG 同栈槽）
                if (!seenEnd)
                {
                    break;
                }
                s->m_avatars.insert(value);
            }
        }
        else if (token == "[event avatar list end]")
        {
            seenEnd = true;
        }
    }
    return seenEnd;
}

// ============================================================================
// importEventEtcScript — ORIG 0x0895e30f（init.md +0x6998，"Event/EventEtc.evt"
// → m_eventEtc）。"[seria room tree material list]" 连续 int 入 m_vec0；
// "[seria room deco material list]" 按 (值, 序号) 写 m_mapC（序号自增）；
// "[christmas event end]" 结束并返回 true。
// ============================================================================
bool importEventEtcScript(EventEtcScript* s, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    s->clear();

    std::string token;
    bool run = true;

    while (run && ScanType(token, true))
    {
        if (token == "[seria room tree material list]")
        {
            int value = -1;
            while (ScanInt(&value))
            {
                s->m_vec0.push_back(value);
            }
        }
        else if (token == "[seria room deco material list]")
        {
            int key = 0;
            int index = 0;
            for (;;)
            {
                if (!ScanInt(&key))
                {
                    break;
                }
                s->m_mapC.insert(std::map<unsigned int, unsigned int>::value_type(
                    static_cast<unsigned int>(key), static_cast<unsigned int>(index)));
                ++index;
            }
        }
        else if (token == "[christmas event end]")
        {
            run = false;
        }
    }
    return !run;
}

// ============================================================================
// importFairPVPEquipment — ORIG 0x088ffbfc（init.md +0x2328，
// "Etc/FairPVPEquipment.etc" → m_stackableMap）。
// "[group]"：ScanStr 职业→getCharacterJob；ScanInt((bool*)0) 段位；
// key=makeFairPVPEquipmentPackageKey(job,grade)。随后收集若干
// "[package selection]" 包（成对 itemId/count 直至 ScanInt 失败），整组赋给
// (*out)[key]。[/fair pvp package] 或流结束均返回 true。
// ============================================================================
bool importFairPVPEquipment(char* path, std::map<int, std::vector<std::vector<STStackablePackageData> > >* out)
{
    if (out == NULL)
    {
        return false;
    }
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;
    std::string jobName;

    while (ScanType(token, true))
    {
        if (token == "[group]")
        {
            if (!ScanStr(&jobName))
            {
                return false;
            }
            const int job = getCharacterJob(jobName.c_str());
            const int grade = ScanInt((bool*)NULL);
            const int key = makeFairPVPEquipmentPackageKey(job, grade);

            std::vector<std::vector<STStackablePackageData> > groups;
            ScanType(token, true);
            while (token == "[package selection]")
            {
                std::vector<STStackablePackageData> pack;
                int itemId = 0;
                while (ScanInt(&itemId))
                {
                    STStackablePackageData data;
                    data.m_field0 = itemId;
                    data.m_field4 = ScanInt((bool*)NULL);
                    pack.push_back(data);
                }
                groups.push_back(pack);
                ScanType(token, true);
            }
            (*out)[key] = groups;
        }
        else if (token == "[/fair pvp package]")
        {
            break;
        }
    }
    return true;
}

// ============================================================================
// importGrowthEquipmentScript — ORIG 0x0895eb3f（init.md +0x0576，
// "Event/GrowthWeapon.evt" → m_growthWeapon）。五个标量段直写字段（任一
// ScanInt 失败提前结束但返回 true）；"[growth equipment reward list]"
// 成对 (idx, reward) 入 m_rewardMap。
// 字段偏移与 growthEquipmentScript 布局一致（+0x18/+0x1c/+0x20/+0x22/+0x24）。
// ============================================================================
bool importGrowthEquipmentScript(growthEquipmentScript* s, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    if (s == NULL)
    {
        return false;
    }

    std::string token;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[gift growth weapon box]")
        {
            s->m_giftBox = ScanInt(&ok);
            if (!ok) { return true; }        // ORIG：失败跳出解析但仍返回 1
        }
        else if (token == "[growth material item]")
        {
            s->m_materialItem = ScanInt(&ok);
            if (!ok) { return true; }
        }
        else if (token == "[growth material item max]")
        {
            s->m_materialMax = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok) { return true; }
        }
        else if (token == "[event bubble max]")
        {
            s->m_eventBubbleMax = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok) { return true; }
        }
        else if (token == "[event sand max]")
        {
            s->m_eventSandMax = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok) { return true; }
        }
        else if (token == "[growth equipment reward list]")
        {
            for (;;)
            {
                std::pair<int, int> entry;
                entry.first = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                entry.second = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                s->m_rewardMap.insert(entry);
            }
        }
    }
    return true;
}

// ============================================================================
// importInGameAdvertisementEtcScript — ORIG 0x0897e9c4（init.md +0x048c，
// "Etc/ingame_advertisement.etc" → m_inGameAd）。clear() 将前 151 字节置 1；
// "[in game advertisement expose condition of teen]" 后每个 idx（ScanInt
// 成功且 <=150）清 0 一字节，idx>150 结束本段。
// ============================================================================
bool importInGameAdvertisementEtcScript(InGameAdvertisementScript* s, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    s->clear();

    std::string token;
    bool ok = false;
    int idx = 0;

    while (ScanType(token, true))
    {
        if (token == "[in game advertisement expose condition of teen]")
        {
            for (;;)
            {
                idx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (idx > 150)
                {
                    break;
                }
                s->m_exposeConditionOfTeen[idx] = 0;
            }
        }
    }
    return true;
}

// ============================================================================
// importLimitItemUsageInfo — ORIG 0x0890d871（init.md +0x562b，
// "Etc/Chn_Server_LimititemUsageinfo.etc" → m_limitItemUsage）。
// 三段分别填充 Ex 结构的三个子表；日期串经 ConvertStringToTime 转秒。
// ============================================================================
bool importLimitItemUsageInfo(char* path, stLimitItemUsageInfoEx* out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;
    std::string dateText;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[Limit Item Usage Info]")
        {
            for (;;)
            {
                const int kind = ScanInt(&ok);
                if (!ok)
                {
                    break;                                   // 回主循环
                }
                const int a = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                const int b = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                out->m_info0.Add(static_cast<unsigned char>(kind), a, b);
            }
        }
        else if (token == "[reset item]")
        {
            for (;;)
            {
                const int itemIdx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (!ScanStr(&dateText))
                {
                    return false;
                }
                long endTime = 0;
                if (!ConvertStringToTime(dateText, endTime))
                {
                    return false;
                }
                out->m_resetC.Add(itemIdx, endTime);
            }
        }
        else if (token == "[refill item]")
        {
            for (;;)
            {
                const int itemIdx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                const int count = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                if (!ScanStr(&dateText))
                {
                    return false;
                }
                long endTime = 0;
                if (!ConvertStringToTime(dateText, endTime))
                {
                    return false;
                }
                out->m_refill18.Add(itemIdx, count, endTime);
            }
        }
    }
    return true;
}

// ============================================================================
// importNPCRelationEventScript — ORIG 0x0890d44f（init.md +0x5a5f，
// "Event/NPCRelationEvent.evt" → m_npcRelation）。
// 顶层仅接受 "[NPC]"（其他 token 直接成功返回）；块内 "[index]" new 0x48 节点
// 并 AddNPCInfo；"[good]/[soso]" 进入好感段：每 kind token 先 GetFavorIndex，
// "[item]" 成对入 AddFavorItemInfo、"[reward]" 单条入 AddRewardItemInfo；
// "[can not give item]" 连续 id 入 AddDislikeItemInfo。
// ============================================================================
bool importNPCRelationEventScript(const char* path, CNPCRelationEventManager* out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    if (out == NULL)
    {
        return false;
    }

    std::string token;
    std::string sub;
    bool ok = false;
    int curIdx = 0;

    while (ScanType(token, true))
    {
        curIdx = 0;
        if (token != "[NPC]")
        {
            return true;                        // ORIG：非 [NPC] 顶层 token 即结束
        }

        bool moreSub = true;
        while (moreSub)
        {
            if (!ScanType(sub, true))
            {
                break;
            }

            if (sub == "[index]")
            {
                curIdx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                stNPCRelationEventInfo* info = new stNPCRelationEventInfo;
                out->AddNPCInfo(static_cast<unsigned short>(curIdx), info);
                continue;
            }

            const bool favorKind =
                (sub == "[good]") || (sub == "[soso]") || (sub == "[bad]");
            if (!favorKind)
            {
                if (sub != "[can not give item]")
                {
                    continue;                   // 未知子 token 跳过
                }
                for (;;)
                {
                    const int itemId = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    out->AddDislikeItemInfo(static_cast<unsigned short>(curIdx),
                                            static_cast<unsigned int>(itemId));
                }
                continue;
            }

            // ---- 好感段：kind token 循环 ----
            for (;;)
            {
                std::string kindToken;
                if (!ScanType(kindToken, true))
                {
                    break;
                }
                const ENUM_NPC_RELATION_EVENT_FAVOR_KIND favor =
                    out->GetFavorIndex(kindToken.c_str());

                if (kindToken == "[item]")
                {
                    for (;;)
                    {
                        const int itemId = ScanInt(&ok);
                        if (!ok)
                        {
                            break;
                        }
                        const int count = ScanInt(&ok);
                        if (!ok)
                        {
                            break;
                        }
                        out->AddFavorItemInfo(static_cast<unsigned short>(curIdx), favor,
                                              static_cast<unsigned int>(itemId),
                                              static_cast<unsigned int>(count));
                    }
                }
                else if (kindToken == "[reward]")
                {
                    const int a = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    const int b = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    out->AddRewardItemInfo(static_cast<unsigned short>(curIdx), favor,
                                           static_cast<unsigned int>(a),
                                           static_cast<unsigned int>(b));
                    break;                      // ORIG：reward 仅一条即回子循环
                }
                else
                {
                    break;                      // 未知 kind token 结束好感段
                }
            }
        }
    }
    return true;
}

// ============================================================================
// importQuestShopScript — ORIG 0x0890fa3d（init.md +0x10d4，
// "Etc/questshop.etc" → m_questShop）。返回 0=成功，否则错误码：
//   0x2815 装载失败 / 0x2829 [quest status] 序号 ScanInt 失败 /
//   0x284f loadAppendageType==-1 / 0x286d [quest point] ScanInt 失败。
// "[initialization costs]" 三值写 m_initCost（失败按 ORIG 返回 0）。
// ============================================================================
int importQuestShopScript(const char* path, stQuestShop& out)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0x2815;
    }

    std::string token;
    std::string text;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[initialization costs]")
        {
            stQuestShoptInitCost cost;
            cost.m_cost0 = ScanInt(&ok);
            if (!ok) { return 0; }
            cost.m_cost1 = ScanInt(&ok);
            if (!ok) { return 0; }
            cost.m_cost2 = ScanInt(&ok);
            if (!ok) { return 0; }
            out.m_initCost.m_cost0 = cost.m_cost0;
            out.m_initCost.m_cost1 = cost.m_cost1;
            out.m_initCost.m_cost2 = cost.m_cost2;
        }
        if (token != "[quest status]")
        {
            continue;
        }

        // ---- 单个 [quest status] 记录 ----
        stQuestShopStatus status;
        status.clear();
        const int idx = ScanInt(&ok);
        if (!ok)
        {
            return 0x2829;
        }
        if (out.m_field18 < idx)
        {
            out.m_field18 = idx;
        }

        bool closeRecord = false;
        while (!closeRecord)
        {
            if (!ScanType(token, true))
            {
                closeRecord = true;             // ORIG：流结束也先入库再回主循环
                break;
            }
            if (token == "[/quest status]")
            {
                closeRecord = true;
                break;
            }
            if (token == "[name]")
            {
                ScanStr(&status.m_name);
            }
            else if (token == "[name2]")
            {
                ScanStr(&status.m_name2);
            }
            else if (token == "[explain]")
            {
                ScanStr(&status.m_explain);
            }
            else if (token == "[icon]")
            {
                ScanStr(&status.m_icon);
                status.m_field10 = ScanInt((bool*)NULL);
            }
            else if (token == "[quest status info]")
            {
                if (!ScanStr(&text))
                {
                    return 0x284f;
                }
                const int appendage = loadAppendageType(text);
                if (appendage == -1)
                {
                    return 0x284f;
                }
                if (!ScanStr(&text))
                {
                    return 0x284f;
                }
                const int valueType = loadValueType(text);
                (void)valueType;
                const float value = ScanFloat((bool*)NULL);
                (void)value;
                // ORIG：out+0x28+appendage = (char)idx（appendage 槽位标记已收录状态）
                out.m_pad28[appendage] = static_cast<char>(idx);
            }
            else if (token == "[level info]")
            {
                stStatusLevelInfo info;
                info.m_second = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    continue;
                }
                info.m_first = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    continue;
                }
                status.m_levelInfos.push_back(info);
            }
            else if (token == "[quest point]")
            {
                const int point = ScanInt(&ok);
                if (!ok)
                {
                    return 0x286d;
                }
                status.m_questPoint = static_cast<unsigned char>(point);
            }
        }

        out.m_map0.insert(
            std::map<int, stQuestShopStatus>::value_type(idx, status));
    }
    return 0;
}

// ============================================================================
// importPcBangEquipmentRendar — ORIG 0x08915606（init.md +0x6be8，
// "Etc/PcBangRentalService.etc" → m_pcBangRentar）。
// "[section]"：EventRewardSection 三字节 + count 个 uchar 入 m_vec0；
// "[group]"：job/grade→key，收集 "[package selection]" 包（itemId、count、
// short、"逗号串"→tokenize/getGrowTypeIndex 逐项 push_back），[/group] 时
// insert 进 m_mapC。
// ============================================================================
bool importPcBangEquipmentRendar(const char* path, PcBangItemRentarData* out)
{
    if (out == NULL)
    {
        return false;
    }
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;
    std::string nameText;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[section]")
        {
            EventRewardSection section;
            section.m_field0 = static_cast<unsigned char>(ScanInt(&ok));
            if (!ok)
            {
                return false;
            }
            section.m_field1 = static_cast<unsigned char>(ScanInt((bool*)NULL));
            section.m_field2 = static_cast<unsigned char>(ScanInt((bool*)NULL));
            const int count = ScanInt((bool*)NULL);
            for (int i = 0; i < count; ++i)
            {
                const unsigned char value = static_cast<unsigned char>(ScanInt((bool*)NULL));
                section.m_items.push_back(value);
            }
            out->m_vec0.push_back(section);
        }
        else if (token == "[group]")
        {
            if (!ScanStr(&nameText))
            {
                return false;
            }
            const int job = getCharacterJob(nameText.c_str());
            const int grade = ScanInt((bool*)NULL);
            const int key = makeFairPVPEquipmentPackageKey(job, grade);

            std::vector<std::vector<EventRewardItemInfo> > groups;
            for (;;)
            {
                if (!ScanType(token, true))
                {
                    break;
                }
                if (token == "[package selection]")
                {
                    std::vector<EventRewardItemInfo> items;
                    int itemIdx = 0;
                    while (ScanInt(&itemIdx))
                    {
                        EventRewardItemInfo item;
                        item.m_field0 = itemIdx;
                        item.m_short8 = static_cast<short>(ScanInt((bool*)NULL));
                        item.m_field4 = ScanInt((bool*)NULL);

                        std::string growText;
                        ScanStr(&growText);
                        std::vector<std::string> parts;
                        tokenizeStr(growText, parts, ",");
                        for (unsigned int p = 0; p < parts.size(); ++p)
                        {
                            const int growType = getGrowTypeIndex(parts[p].c_str());
                            item.m_growTypes.push_back(growType);
                        }
                        items.push_back(item);
                    }
                    groups.push_back(items);
                }
                else if (token == "[/group]")
                {
                    out->m_mapC.insert(std::map<int, std::vector<std::vector<EventRewardItemInfo> > >::value_type(
                        key, groups));
                    break;
                }
            }
        }
    }
    return true;
}

// ============================================================================
// importMercenarySystemEtcScript — ORIG 0x089f6c50（init.md +0x6b19，
// "Etc/mercenary.etc" → m_mercenarySystem）。返回 0=成功，否则错误码
// （9 装载失败 / 0xc 空指针 / 0x29..0xd1 各段 Scan 失败，见 case 注释）。
// ============================================================================
int importMercenarySystemEtcScript(const char* path, stMercenarySystemInfo* out)
{
    if (!loadRDARScriptFile("", path))
        return 9;
    if (out == NULL)
        return 0xc;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;                 // ORIG -0x11c
    std::string sub;                   // ORIG -0x120
    std::string sub2;                  // ORIG -0x124
    std::string unused;                // ORIG -0x128（构造/析构但零引用槽）
    bool ok = false;                   // ORIG -0x129
    short lv = 0;                      // ORIG -0x12c（movw $0；%ax 写）
    unsigned int gold = 0;             // ORIG -0x130
    unsigned int item = 0;             // ORIG -0x134
    unsigned int avatarIdx = 0;        // ORIG -0x138
    unsigned int period = 0;           // ORIG -0x13c
    float rate = 0;                    // ORIG -0x140
    unsigned int count = 0;            // ORIG -0x144
    unsigned int index = 0;            // ORIG -0x148

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        // ORIG 重派发跳回 +0xf0（__lookup guard）：静态声明位于主循环体内。
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        for (;;)
        {
            int id;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                id = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;
                }
                id = 0;
            }

            switch (id)
            {
            case 0x25:                      // [base time unit]
            case 0:
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[base time unit]", 0x25));
                }
                else
                {
                    period = ScanInt(&ok);
                    if (!ok) return 0x29;
                    if (period == 0) return 0x2c;
                    out->m_field0 = period;
                    break;
                }
                /* fall through（ORIG +0x383 jmp +0x3d5） */
            case 0x32:                      // [level base GoldItem Prob]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[level base GoldItem Prob]", 0x32));
                }
                else
                {
                    lv = ScanInt(&ok);
                    if (!ok) return 0x36;
                    gold = ScanInt(&ok);
                    if (!ok) return 0x3a;
                    item = ScanInt(&ok);
                    if (!ok) return 0x3e;
                    out->m_vec8.push_back(std::make_pair(lv, std::make_pair(gold, item)));
                    if (out->m_field4 > (unsigned short)lv)   // ORIG 16 位 cmp 形：保留最小值
                        out->m_field4 = (unsigned short)lv;
                    break;
                }
                /* fall through（+0x430 jmp +0x55e） */
            case 0x46:                      // [mercenary reward per period]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[mercenary reward per period]", 0x46));
                }
                else
                {
                    while (true)
                    {
                        period = ScanInt(&ok);
                        if (!ok) break;
                        rate = ScanFloat(&ok);
                        if (!ok) return 0x50;
                        out->m_vec14.push_back(std::make_pair(period, rate));
                    }
                    break;
                }
                /* fall through（+0x5b9 jmp +0x66c） */
            case 0x57:                      // [mercenary reward avatar bonus]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[mercenary reward avatar bonus]", 0x57));
                }
                else
                {
                    while (true)
                    {
                        avatarIdx = ScanInt(&ok);
                        if (!ok) break;
                        rate = ScanFloat(&ok);
                        if (!ok) return 0x61;
                        out->m_vec20.push_back(std::make_pair(avatarIdx, rate));
                    }
                    break;
                }
                /* fall through（+0x6c7 jmp +0x774） */
            case 0x69:                      // [mercenary reward critical]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[mercenary reward critical]", 0x69));
                }
                else
                {
                    if (!ScanStr(&out->m_str2c))
                        return 0x6c;
                    out->m_field3c = 0;
                    while (true)
                    {
                        gold = ScanInt(&ok);
                        if (!ok) break;
                        rate = ScanFloat(&ok);
                        if (!ok) return 0x77;
                        out->m_field3c += gold;      // ORIG 累计和
                        out->m_vec30.push_back(std::make_pair(gold, rate));
                    }
                    break;
                }
                /* fall through（+0x7c0 jmp +0x8a9） */
            case 0x7f:                      // [competition area]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[competition area]", 0x7f));
                }
                else
                {
                    stMercenaryCompetitionArea area;
                    int groupIdSum = 0;              // ORIG -0x1c
                    while (true)
                    {
                        if (!ScanType(sub, true))
                            break;
                        if (sub == "[reward group]")
                        {
                            stMercenaryRewardGroup group;
                            group.m_groupId = ScanInt(&ok);
                            groupIdSum += group.m_groupId;   // ORIG 先累加后检查 ok
                            if (!ok) return 0x8f;
                            if (!ScanStr(&group.m_name))
                                return 0x92;
                            while (true)
                            {
                                if (!ScanType(sub2, true))
                                    return 0x97;
                                if (sub2 == "[mob index]")
                                {
                                    index = ScanInt(&ok);
                                    if (!ok) continue;
                                    count = ScanInt(&ok);
                                    if (!ok) return 0xa3;
                                    group.m_field20 = group.m_field20 + count;   // group+0x20 累计（ORIG 两段物化）
                                    group.m_mobList.push_back(std::make_pair(count, index));
                                    continue;
                                }
                                if (sub2 == "[item index]")
                                {
                                    index = ScanInt(&ok);
                                    if (!ok) continue;
                                    count = ScanInt(&ok);
                                    if (!ok) return 0xb3;
                                    group.m_field24 = group.m_field24 + count;   // group+0x24 累计（ORIG 两段物化）
                                    group.m_itemList.push_back(std::make_pair(count, index));
                                    continue;
                                }
                                if (sub2 == "[/reward group]")
                                {
                                    area.m_groups.push_back(group);
                                    break;
                                }
                            }
                        }
                        if (sub == "[world map]")
                        {
                            area.m_worldMapIdx = ScanInt(&ok);  // ORIG 无 ok 检查
                            continue;
                        }
                        if (sub == "[area level]")
                        {
                            area.m_areaLevel = ScanInt(&ok);    // ORIG 无 ok 检查
                            continue;
                        }
                        if (sub == "[/competition area]")
                        {
                            area.m_groupIdSum = groupIdSum;
                            out->m_vec40.push_back(area);
                            break;
                        }
                        return 0xd1;
                    }
                    break;
                }
                /* fall through */
            default:
                break;
            }

            // ORIG LAB_089f795a 公共尾：注册链走完释放锁置位重派发；正常态下一轮。
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
            }
            else
                break;
        }
    }
    return 0;
}

// ============================================================================
// importHellPartyScript — ORIG 0x08909857（init.md +0x0d30，
// "Etc/HellParty.etc" → *Get_instance_stHellParty_t()）。
// 返回 0=成功；错误码段：0x1a24 装载 / 0x1a2e 首.ScanType /
// 0x1a37..0x1aa3 基础值各 Scan / 0x1aba 组下标 / 0x1acd..0x1aed 组名 /
// 0x1aed 未知组名 / 0x1b08 组下标重复 / 0x1b1a..0x1b30 difficulty 段。
// ============================================================================
int importHellPartyScript(const char* path, stHellParty_t& out)
{
    std::string token;
    std::string sub;
    bool ok = false;

    if (!loadRDARScriptFile("", path))
    {
        return 0x1a24;
    }

    for (;;)
    {
        // ---- 顶层分派（ORIG 外层头：每次取一 token，流结束 → 0x1a2e）----
        if (!ScanType(token, true))
        {
            return 0x1a2e;
        }

        if (token == "[hellparty script end]")
        {
            return 0;
        }

        // ================= [hellparty basic values] =================
        if (token == "[hellparty basic values]")
        {
            for (;;)
            {
                if (!ScanType(sub, true))
                {
                    return 0x1a37;
                }
                if (sub == "[/hellparty basic values]")
                {
                    break;                          // 回顶层
                }

                if (sub == "[max base value]")
                {
                    out.m_maxBaseValue = ScanInt(&ok);
                    if (out.m_maxBaseValue == 0) { return 0x1a41; }
                    if (!ok) { return 0x1a43; }
                }
                else if (sub == "[min base value]")
                {
                    out.m_minBaseValue = ScanInt(&ok);
                    if (!ok) { return 0x1a4c; }
                }
                else if (sub == "[base value]")
                {
                    out.m_baseValue = ScanInt(&ok);
                    if (!ok) { return 0x1a53; }
                }
                else if (sub == "[sub base rates]")
                {
                    out.m_subBaseRates = ScanFloat(&ok);
                    if (!ok) { return 0x1a5a; }
                }
                else if (sub == "[pass sub base rates]")
                {
                    out.m_passSubBaseRates = ScanFloat(&ok);
                    if (!ok) { return 0x1a61; }
                }
                else if (sub == "[base rising value]")
                {
                    out.m_baseRisingValue = ScanInt(&ok);
                    if (!ok) { return 0x1a68; }
                }
                else if (sub == "[level difference rates]")
                {
                    // 三连读，仅以最后一次 ScanInt 结果判定（ORIG 同栈槽）。
                    out.m_levelDiffRates[1] = ScanFloat(&ok);
                    out.m_levelDiffRates[2] = ScanFloat(&ok);
                    out.m_levelDiffRates[3] = ScanFloat(&ok);
                    if (!ok) { return 0x1a72; }
                }
                else if (sub == "[difficulty rates]")
                {
                    out.m_difficultyRates[0] = ScanFloat(&ok);
                    out.m_difficultyRates[1] = ScanFloat(&ok);
                    out.m_difficultyRates[2] = ScanFloat(&ok);
                    out.m_difficultyRates[3] = ScanFloat(&ok);
                    out.m_difficultyRates[4] = ScanFloat(&ok);
                    if (!ok) { return 0x1a88; }
                }
                else if (sub == "[party rates]")
                {
                    // 首值经 ScanFloat(3,&ok) 以刻度 3 装载（ORIG _Z9ScanFloatiPb）。
                    out.m_partyRates[0] = ScanFloat(3, &ok);
                    out.m_partyRates[1] = ScanFloat(&ok);
                    out.m_partyRates[2] = ScanFloat(&ok);
                    out.m_partyRates[3] = ScanFloat(&ok);
                    if (!ok) { return 0x1a93; }
                }
                else if (sub == "[ancient dungeon add rate]")
                {
                    out.m_ancientDungeonAddRate = ScanFloat(&ok);
                    if (!ok) { return 0x1a9b; }
                }
                else if (sub == "[guide line value]")
                {
                    out.m_guideLineValue = ScanInt(&ok);
                    if (!ok) { return 0x1aa3; }
                }
                // 其余子 token 忽略
            }
            continue;
        }

        // ================ [hellparty monster group] ================
        if (token == "[hellparty monster group]")
        {
            for (;;)
            {
                if (!ScanType(sub, true))
                {
                    return 0x1ab2;
                }
                if (sub == "[/hellparty monster group]")
                {
                    break;                          // 回顶层
                }

                if (sub == "[group index]")
                {
                    const int idx = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x1aba;
                    }
                    out.m_groupIndex = static_cast<unsigned short>(idx);
                }
                else if (sub == "[group]")
                {
                    stHellPartyMonsterGroupList list;
                    std::string groupName;
                    if (!ScanStr(&groupName))
                    {
                        return 0x1ac9;
                    }

                    // 'A'..'E' 选择难度槽 tag 1..5；容量/计数取自 difficulty 表。
                    int tag = 0;
                    if (groupName == "A")      { tag = 1; }
                    else if (groupName == "B") { tag = 2; }
                    else if (groupName == "C") { tag = 3; }
                    else if (groupName == "D") { tag = 4; }
                    else if (groupName == "E") { tag = 5; }
                    else
                    {
                        return 0x1aed;
                    }
                    const int slotCount = out.m_field72[tag];

                    for (int i = 0; i < slotCount; ++i)
                    {
                        const int monsterIdx = ScanInt(&ok);
                        if (!ok) { break; }
                        const int rate = ScanInt(&ok);
                        if (!ok) { break; }
                        list.m_monsterIdx[i] = monsterIdx;
                        list.m_rate[i] = static_cast<unsigned char>(rate);
                    }

                    if (out.m_groupMap.find(out.m_groupIndex) != out.m_groupMap.end())
                    {
                        return 0x1b08;              // 组下标重复
                    }
                    out.m_groupMap.insert(
                        std::map<unsigned short, stHellPartyMonsterGroupList>::value_type(
                            out.m_groupIndex, list));
                }
                else
                {
                    return 0x1aed;                  // 未知子 token
                }
            }
            continue;
        }

        // ======================= [difficulty] =======================
        if (token == "[difficulty]")
        {
            const int rows = ScanInt(&ok);
            if (rows > 6)
            {
                return 0x1b1a;
            }
            if (!ok)
            {
                return 0x1b1d;
            }
            for (int i = 1; i <= rows; ++i)
            {
                std::string rowName;
                if (!ScanStr(&rowName))
                {
                    return 0x1b25;
                }
                // 行名后五值：tag 字节 / 计数 ushort / 三字节列。
                out.m_field6c[i] = static_cast<unsigned char>(ScanInt(&ok));
                out.m_field72[i] = static_cast<unsigned short>(ScanInt(&ok));
                out.m_field7e[i] = static_cast<unsigned char>(ScanInt(&ok));
                out.m_field84[i] = static_cast<unsigned char>(ScanInt(&ok));
                out.m_field8a[i] = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x1b30;
                }
            }
            continue;
        }

        // 其他顶层 token：忽略并继续外层扫描
    }
}

// ============================================================================
// importInGameEventInfoList — ORIG 0x0897eb59（init.md +0x674d，
// "Event/InGameEventList.evt" → *m_liveServerInfo.m_eventScriptMng 基类）。
// ORIG 以静态 map+SpinLock 建 token→id 表（[/event_list]=0x24 … [/event]=0x37）
// 后 switch 分派；此处以等价常量表实现。[/event_list] 返回 true，其余正常
// 流结束返回 false。
// ============================================================================
bool importInGameEventInfoList(InGameEventInfoList* list, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;
    InGameEventInfo info;

    while (ScanType(token, true))
    {
        switch (lookupTokenId(token, kInGameEventTokenIds,
                              sizeof(kInGameEventTokenIds) / sizeof(kInGameEventTokenIds[0])))
        {
        case 0x24:                                    // [/event_list]
            return true;
        case 0x27:                                    // [idx]
            ScanInt(&info.m_eventId);
            ScanStr(&info.m_name);
            break;
        case 0x2b:                                    // [record type]
            ScanInt(&info.m_field4);
            break;
        case 0x2e:                                    // [start]
            ScanInt(&info.m_startTime);
            break;
        case 0x31:                                    // [end]
            ScanInt(&info.m_endTime);
            break;
        case 0x34:                                    // [state]
            // ORIG ScanInt 写 +0x14 起 4 字节（uchar 标志 + 3 pad）。
            ScanInt(reinterpret_cast<int*>(&info.m_isEventing));
            break;
        case 0x37:                                    // [/event]
            list->insertEventInfo(info.m_field4, info);
            break;
        default:                                      // 未识别 token 跳过
            break;
        }
    }
    return false;
}

// ============================================================================
// 三个子类型构造器为头文件内联（CDataManager.h，ORIG 具名 C1 0x837c6be/6e6/70e）。
// ORIG 0x891b4a8 W：临时元素 {kind,a,b} 后 push_back。
void stLimitItemUsageInfo::Add(char kind, int a, int b)
{
    LimitInfo entry;
    entry.m_kind = kind;
    entry.m_a = a;
    entry.m_b = b;
    m_list.push_back(entry);
}

// ORIG 0x891b4dc W。
void stResetItemInfo::Add(int space, long resetSec)
{
    ResetItem entry;
    entry.m_space = space;
    entry.m_resetSec = resetSec;
    m_list.push_back(entry);
}

// ORIG 0x891b502 W。
void stRefillItemInfo::Add(int itemId, int count, long endSec)
{
    RefillItem entry;
    entry.m_itemId = itemId;
    entry.m_count = count;
    entry.m_endSec = endSec;
    m_list.push_back(entry);
}

// ORIG 0x837109a W：三 int 逆序清零（全局类型，mangled 无 stQuestShop 前缀）。
stQuestShoptInitCost::stQuestShoptInitCost()
{
    m_cost2 = 0;
    m_cost1 = m_cost2;
    m_cost0 = m_cost1;
}

// ORIG stNPCRelationEventInfo C1 0x891c946 W：两个 FavorAndReward 子对象 +
// vector<int> @+0x3c 默认构造。
stNPCRelationEventInfo::stNPCRelationEventInfo() {}

// ORIG stMercenaryRewardGroup C1 0x89f7a9c W：int 清零 + string/vector 构造，
// 尾部两 dword 清零（对齐 pad）。
stMercenaryRewardGroup::stMercenaryRewardGroup()
    : m_groupId(0), m_name(), m_mobList(), m_itemList(),
      m_field20(0), m_field24(0)
{
}

// ORIG stMercenaryCompetitionArea C1 0x89f7b28 W：ushort 清零 + vector 构造
// + int@0x10 清零 + ushort@0x14 清零。
stMercenaryCompetitionArea::stMercenaryCompetitionArea()
    : m_worldMapIdx(0), m_groups(), m_groupIdSum(0), m_areaLevel(0)
{
}

// ORIG stHellPartyMonsterGroupList C1 0x891b1d6 W：memset(-1) 两段 + 三标量清零。
stHellPartyMonsterGroupList::stHellPartyMonsterGroupList()
{
    memset(m_monsterIdx, -1, sizeof(m_monsterIdx));
    memset(m_rate, -1, sizeof(m_rate));
    m_field50 = 0;
    m_field52 = 0;
    m_field54 = 0;
}

// ORIG stHellParty_t C1 0x891b242 W：map@0x54 构造；标量清零；三段 memset；
// 难度表五组循环清零；+0x90=0。
stHellParty_t::stHellParty_t()
    : m_groupMap(),
      m_maxBaseValue(0), m_minBaseValue(0), m_baseValue(0), m_baseRisingValue(0),
      m_subBaseRates(0.f), m_passSubBaseRates(0.f),
      m_difficultyRates(), m_partyRates(), m_ancientDungeonAddRate(0.f),
      m_groupIndex(0), m_guideLineValue(0)
{
    memset(m_levelDiffRates, 0, sizeof(m_levelDiffRates));
    memset(m_difficultyRates, 0, sizeof(m_difficultyRates));
    memset(m_partyRates, 0, sizeof(m_partyRates));
    for (int i = 0; i <= 5; ++i)
    {
        m_field6c[i] = 0;
        m_field72[i] = 0;
        m_field7e[i] = 0;
        m_field84[i] = 0;
        m_field8a[i] = 0;
    }
}

// ORIG D1 0x891c7e2 W：仅析构 map 成员（其余 POD 无动作）。
stHellParty_t::~stHellParty_t() {}
