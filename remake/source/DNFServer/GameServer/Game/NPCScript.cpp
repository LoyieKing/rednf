#include "NPCScript.h"

#include <algorithm>
#include <cstring>
#include <string>

#include "NpcScript.h"
#include "CCubeStatistic.h"   // getItemGroupNameTable()（ORIG 0x088c0409）
#include "DNFLexWrapper.h"    // ScanType/ScanInt/ScanStr

// ============================================================================
// CNPCScript / NpcScript / importNpcScript
//   NpcScript ctor/clear：ORIG 0x089fb942 / 0x089fbf22（NpcScript/ 报告逐字段）
//   importNpcScript：     ORIG 0x089fc264（_lt_global_gt_/importNpcScript.md
//                         反编译 C 逐分支转写，§9 主分支级还原）
//   CNPCScript 访问器：    ORIG 0x85809a6 / 0x86d2736 / 0x8580fd0 /
//                         0x8580e6a / 0x85811a4
// ============================================================================

// ---- 外部符号（ORIG 真实 T/B；定义在各自 TU，链接期解析）----
// getNpcRoleByString/getNpcMoodFromString/getDirectoryPath/g_npcScript*/
// STScriptFileList::FindIndexByFullScan 声明见 NpcScript.h。

// [favor rate per mood] 的除数（ORIG _DAT_08e1dbf4 = 100.0f）。
static const float NPC_FAVOR_RATE_DIVISOR = 100.0f;

// ----------------------------------------------------------------------------
// NpcScript::clear —— ORIG 0x089fbf22 逐字段。
// ----------------------------------------------------------------------------
void NpcScript::clear()
{
    m_scriptFileIndex = 0;
    m_name = "";
    m_fieldName = "";
    m_smallFace = "";
    m_smallFaceId = 0;
    m_bigFace = "";
    m_bigFaceId = 0;
    m_popupFace = "";
    m_popupFaceId = 0;
    m_fieldAnimation = "";
    m_fieldWav = "";
    for (int i = 0; i < 2; ++i)
    {
        m_popupWav[i] = "";
    }
    m_lookAround = true;
    m_roleMap.clear();
    m_questSpeech = "";
    for (int i = 0; i < 12; ++i)
    {
        m_skills[i].clear();
    }
    m_directoryPath = "";
    m_unknown38 = -1;
    m_stringData.clear();
    m_intData.clear();
    m_moveTownLimitStr = "";
    m_moveTownConfirmStr = "";
    m_balloonMessages.clear();
    m_balloonTicks.clear();
    m_dialogs.clear();
    m_presentResponces.clear();
    m_presentResponceExceptions.clear();
    m_presentResponceEvents.clear();
    m_favorableRelationShip.clear();
    m_firstFavorDialogs.clear();
    m_visibleOnDungeonClear = false;
    m_webPages.clear();
    m_clickMarks.clear();
    m_randomMessageAnimation = "";   // 推断：ORIG clear 未显式清 +0x6a4..+0x6b4，
                                     // 由各自容器构造保证空态；字符串赋值等价。
    m_randomMessageProbability = 0;  // 推断：同上
    m_randomMessageList.clear();
    m_hide = false;
    m_moviePaths.clear();            // 推断：内嵌 NpcMovieData 首成员队列
}

// ----------------------------------------------------------------------------
// NpcScript 构造/析构：ORIG ctor 为各子对象默认构造 + clear()；成员自带构造，
// 此处仅补齐与 ORIG 一致的 clear 调用。析构由成员自动完成 —「推断等价」。
// ----------------------------------------------------------------------------
NpcScript::NpcScript()
{
    clear();
}
NpcScript::~NpcScript()
{
}

// stFavorableRelationShip_t::clear —— ORIG 0x089fb56c T（忠实转写）：
//   标量区清零（word@0 / dword@4 / word@8,0xa,0xc）
//   → moodChanges(+0x10)/keyItems(+0x1c) 两个 vector clear
//   → 四张礼物表 ×5 个 map（stride 0x18）clear
//   → favorRate×3 置 1.0f（0x3f800000，+0x4d8/4dc/4e0）
//   → m_rewardItems(+0x208).clear()。
void stFavorableRelationShip_t::clear()
{
    m_enabled = false;
    m_defaultFavor = 0;
    m_maxGiftPerDay = 0;
    m_dialogDayGap1 = 0;
    m_dialogDayGap2 = 0;
    m_moodChanges.clear();
    m_keyItems.clear();
    for (int l = 0; l < 5; ++l)
    {
        for (int i = 0; i < 5; ++i)
        {
            m_preferItems.m_maps[l].clear();
            m_unpreferItems.m_maps[l].clear();
            m_preferItemGroups.m_maps[l].clear();
            m_unpreferItemGroups.m_maps[l].clear();
        }
    }
    // ORIG：三档 favorRate 写入常量 1.0f（非清零）。
    static const float one = 1.0f;
    m_favorRate0 = one;
    m_favorRate1 = one;
    m_favorRate2 = one;
    m_rewardItems.clear();
}

namespace {

// 解析失败统一出口：ORIG 在各 scan 失败处 return 0。
const int IMPORT_OK    = 1;
const int IMPORT_FAIL  = 0;

// [1st favor dialog]：取 '>' 之后的子串（strchr(s,'>')+1）。
bool takeAfterGt(const std::string& s, std::string& out)
{
    std::string::size_type pos = s.find('>');
    if (pos == std::string::npos)
    {
        return false;
    }
    out = s.substr(pos + 1);
    return true;
}

}  // namespace

// ----------------------------------------------------------------------------
// importNpcScript —— ORIG 0x089fc264 主分支逐关键字转写。
// 深水区（长尾 keyword 的次要字段、静态 lookup 表优化）以语义等价形式实现，
// 已在对应分支标注「推断」。
// ----------------------------------------------------------------------------
int importNpcScript(NpcScript* npc, char const* path)
{
    if (!loadRDARScriptFile(g_npcScriptBaseDirectory, path))
    {
        return IMPORT_FAIL;
    }

    npc->clear();
    // ORIG 0x088bbc80：getDirectoryPath(npc->m_directoryPath(+0x100), "%s%s", base, path)
    getDirectoryPath(npc->m_directoryPath, "%s%s", g_npcScriptBaseDirectory, path);

    std::string token;
    std::string sub;

    for (;;)
    {
        if (!ScanType(token, true))
        {
            // EOF：回写脚本索引并成功返回。
            npc->m_scriptFileIndex = g_npcScriptFileList.FindIndexByFullScan(path);
            return IMPORT_OK;
        }

        if (token == "[name]")
        {
            ScanStr(&npc->m_name);
            continue;
        }
        if (token == "[field name]")
        {
            ScanStr(&npc->m_fieldName);
            continue;
        }
        if (token == "[small face]")
        {
            ScanStr(&npc->m_smallFace);
            npc->m_smallFaceId = ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (token == "[big face]")
        {
            ScanStr(&npc->m_bigFace);
            npc->m_bigFaceId = ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (token == "[popup face]")
        {
            ScanStr(&npc->m_popupFace);
            npc->m_popupFaceId = ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (token == "[field animation]")
        {
            ScanStr(&npc->m_fieldAnimation);
            continue;
        }
        if (token == "[field wav]")
        {
            ScanStr(&npc->m_fieldWav);
            continue;
        }
        if (token == "[popup wav]")
        {
            ScanStr(&npc->m_popupWav[0]);
            ScanStr(&npc->m_popupWav[1]);
            continue;
        }
        if (token == "[look around]")
        {
            npc->m_lookAround = ScanInt(static_cast<bool*>(0)) != 0;
            continue;
        }
        if (token == "[role]")
        {
            while (ScanStr(&sub))
            {
                int role = getNpcRoleByString(sub);
                if (role == -1)
                {
                    ScanInt(static_cast<bool*>(0));  // 丢弃数值
                }
                else
                {
                    npc->m_roleMap[role] = ScanInt(static_cast<bool*>(0));
                }
            }
            continue;
        }
        if (token == "[quest speech]")
        {
            ScanStr(&npc->m_questSpeech);
            continue;
        }
        if (token == "[skill]")
        {
            int slot = 0xb;  // [common]
            ScanStr(&sub);
            if (sub != "[common]")
            {
                slot = getCharacterJob(sub.c_str());
            }
            std::vector<int>& list = npc->m_skills[slot];
            list.clear();
            bool ok = false;
            for (int v = ScanInt(&ok); ok; v = ScanInt(&ok))
            {
                list.push_back(v);
            }
            continue;
        }
        if (token == "[web page]")
        {
            int count = ScanInt(static_cast<bool*>(0));
            for (int i = 0; i < count; ++i)
            {
                if (!ScanStr(&sub))
                {
                    break;  // 推断：ORIG 未检查返回值，此处防御性终止
                }
                npc->m_webPages.push_back(sub);
            }
            continue;
        }
        if (token == "[/web page]")
        {
            continue;
        }
        if (token == "[movie path]")
        {
            while (ScanStr(&sub))
            {
                npc->m_moviePaths.push_back(sub);
            }
            npc->m_moviePathIndex = 0;
            continue;
        }
        if (token == "[/movie path]")
        {
            continue;
        }
        if (token == "[ingame advertisement movie path]")
        {
            while (ScanStr(&sub))
            {
                // ORIG 同样只消费不存储。
            }
            continue;
        }
        if (token == "[speech]")
        {
            // ORIG 用临时 string 消费全部字段，不写回结构。
            std::string tmp;
            ScanInt(static_cast<bool*>(0));
            ScanInt(static_cast<bool*>(0));
            ScanInt(static_cast<bool*>(0));
            ScanStr(&tmp);
            ScanStr(&tmp);
            bool ok = false;
            while (ScanInt(&ok), ok)
            {
            }
            continue;
        }
        if (token == "[click mark]")
        {
            bool ok = false;
            for (int type = ScanInt(&ok); ok; type = ScanInt(&ok))
            {
                ClickMarkInfo info;
                info.m_type = type;
                info.m_param = (type == 0) ? ScanInt(static_cast<bool*>(0)) : 0;
                npc->m_clickMarks.push_back(info);
            }
            continue;
        }
        if (token == "[dialog]")
        {
            InteractiveDialogScript dlg;
            dlg.m_a = ScanInt(static_cast<bool*>(0));
            dlg.m_b = ScanInt(static_cast<bool*>(0));
            dlg.m_c = ScanInt(static_cast<bool*>(0));
            int condCount = ScanInt(static_cast<bool*>(0));
            if (condCount != -1)
            {
                for (int i = 0; i < condCount * 2; ++i)
                {
                    dlg.m_conditions.push_back(ScanInt(static_cast<bool*>(0)));
                }
            }
            dlg.m_d = ScanInt(static_cast<bool*>(0));
            dlg.m_flag = ScanInt(static_cast<bool*>(0)) != 0;
            ScanStr(&dlg.m_str);
            bool ok = false;
            for (int v = ScanInt(&ok); ok; v = ScanInt(&ok))
            {
                dlg.m_list2.push_back(v);
            }
            npc->m_dialogs.push_back(dlg);
            continue;
        }
        if (token == "[/dialog]")
        {
            continue;
        }
        if (token == "[favor_dialog]")
        {
            PresentResponceScript pr;
            pr.m_a = ScanInt(static_cast<bool*>(0));
            pr.m_b = ScanInt(static_cast<bool*>(0));
            pr.m_c = ScanInt(static_cast<bool*>(0));
            ScanStr(&pr.m_str);
            npc->m_presentResponces.push_back(pr);
            continue;
        }
        if (token == "[/favor_dialog]")
        {
            continue;
        }
        if (token == "[favor_dialog_exception]")
        {
            PresentResponceScriptException ex;
            ex.m_a = ScanInt(static_cast<bool*>(0));
            ScanStr(&ex.m_str);
            npc->m_presentResponceExceptions.push_back(ex);
            continue;
        }
        if (token == "[/favor_dialog_exception]")
        {
            continue;
        }
        if (token == "[event_favor_dialog]")
        {
            PresentResponceScriptEvent ev;
            ev.m_a = ScanInt(static_cast<bool*>(0));
            ScanStr(&ev.m_str);
            npc->m_presentResponceEvents.push_back(ev);
            continue;
        }
        if (token == "[/event_favor_dialog]")
        {
            continue;
        }
        if (token == "[1st favor dialog]")
        {
            while (ScanStr(&sub))
            {
                std::string after;
                if (takeAfterGt(sub, after))
                {
                    npc->m_firstFavorDialogs.push_back(after);
                }
            }
            continue;
        }
        if (token == "[/1st favor dialog]")
        {
            continue;
        }
        if (token == "[favor_duration_reward_buff_dialog]")
        {
            ScanStr(&npc->m_favorDurationBuffDialog);
            ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (token == "[string data]")
        {
            npc->m_stringData.clear();
            while (ScanStr(&sub))
            {
                npc->m_stringData.push_back(sub);
            }
            continue;
        }
        if (token == "[int data]")
        {
            npc->m_intData.clear();
            bool ok = false;
            for (int v = ScanInt(&ok); ok; v = ScanInt(&ok))
            {
                npc->m_intData.push_back(v);
            }
            continue;
        }
        if (token == "[balloon message]")
        {
            npc->m_balloonMessages.clear();
            npc->m_balloonTicks.clear();
            while (ScanStr(&sub))
            {
                npc->m_balloonMessages.push_back(sub);
                npc->m_balloonTicks.push_back(ScanInt(static_cast<bool*>(0)));
            }
            continue;
        }

        if (token == "[favorable relationship]")
        {
            stFavorableRelationShip_t& fav = npc->m_favorableRelationShip;
            fav.m_enabled = true;

            for (;;)
            {
                if (!ScanType(token, true) || token == "[/favorable relationship]")
                {
                    break;
                }
                if (token == "[default favor]")
                {
                    bool ok = false;
                    fav.m_defaultFavor = ScanInt(&ok);
                    if (!ok) { return IMPORT_FAIL; }
                    continue;
                }
                if (token == "[max gift per day]")
                {
                    bool ok = false;
                    fav.m_maxGiftPerDay = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok) { return IMPORT_FAIL; }
                    continue;
                }
                if (token == "[dialog day not continuously]")
                {
                    bool ok = false;
                    fav.m_dialogDayGap1 = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok) { return IMPORT_FAIL; }
                    fav.m_dialogDayGap2 = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok) { return IMPORT_FAIL; }
                    continue;
                }
                if (token == "[mood]")
                {
                    for (;;)
                    {
                        if (!ScanType(token, true) || token == "[/mood]")
                        {
                            break;
                        }
                        if (token == "[change]")
                        {
                            for (;;)
                            {
                                int mood = 3;
                                if (!ScanStr(&sub)) { break; }
                                mood = getNpcMoodFromString(sub.c_str());
                                if (mood == 3) { return IMPORT_FAIL; }
                                bool ok = false;
                                int value = ScanInt(&ok);
                                if (!ok) { return IMPORT_FAIL; }
                                fav.m_moodChanges.push_back(
                                    std::make_pair(mood,
                                                   static_cast<unsigned int>(value)));
                            }
                        }
                        else if (token == "[key item]")
                        {
                            bool ok = false;
                            for (int key = ScanInt(&ok); ok; key = ScanInt(&ok))
                            {
                                unsigned int id = ScanInt(&ok);
                                if (!ok) { return IMPORT_FAIL; }
                                fav.m_keyItems.push_back(
                                    std::make_pair(
                                        static_cast<unsigned long>(
                                            static_cast<unsigned int>(key)),
                                        id));
                            }
                        }
                    }
                    continue;
                }
                if (token == "[gift item]")
                {
                    for (;;)
                    {
                        if (!ScanType(token, true) || token == "[/gift item]")
                        {
                            break;
                        }
                        if (token == "[favor rate per mood]")
                        {
                            bool ok = false;
                            fav.m_favorRate2 =
                                static_cast<float>(ScanInt(&ok)) / NPC_FAVOR_RATE_DIVISOR;
                            if (!ok) { return IMPORT_FAIL; }
                            fav.m_favorRate1 =
                                static_cast<float>(ScanInt(&ok)) / NPC_FAVOR_RATE_DIVISOR;
                            if (!ok) { return IMPORT_FAIL; }
                            fav.m_favorRate0 =
                                static_cast<float>(ScanInt(&ok)) / NPC_FAVOR_RATE_DIVISOR;
                            if (!ok) { return IMPORT_FAIL; }
                            continue;
                        }
                        if (token == "[prefer item]" || token == "[unprefer item]")
                        {
                            stGiftTable_t& table = (token == "[prefer item]")
                                                       ? fav.m_preferItems
                                                       : fav.m_unpreferItems;
                            const bool unprefer = (token == "[unprefer item]");
                            bool ok = false;
                            for (int slot = ScanInt(&ok); ok; slot = ScanInt(&ok))
                            {
                                if (slot > 4) { return IMPORT_FAIL; }
                                stGiftItemInfo_t info;
                                info.m_a = ScanInt(&ok);
                                if (!ok) { return IMPORT_FAIL; }
                                info.m_b = ScanInt(&ok);
                                if (!ok) { return IMPORT_FAIL; }
                                info.m_c = static_cast<short>(ScanInt(&ok));
                                if (!ok) { return IMPORT_FAIL; }
                                if (unprefer)
                                {
                                    info.m_b = -info.m_b;   // ORIG: 取负
                                    info.m_c = 1;
                                    // 推断：unprefer 分支第三值固定 1（ORIG
                                    // local_1f4=1），info.m_c 存放该标志位。
                                }
                                // 键构造：ORIG make_pair(ulong, GiftInfo)，ulong 键
                                // 组合方式未完全还原 —「推断」：(slot<<32)|itemId。
                                unsigned long key =
                                    (static_cast<unsigned long>(slot) << 32) |
                                    static_cast<unsigned long>(info.m_a);
                                table.m_maps[slot].insert(
                                    std::make_pair(key, info));
                            }
                            continue;
                        }
                        if (token == "[prefer item group]" ||
                            token == "[unprefer item group]")
                        {
                            stGiftTable_t& table = (token == "[prefer item group]")
                                                       ? fav.m_preferItemGroups
                                                       : fav.m_unpreferItemGroups;
                            const bool unprefer =
                                (token == "[unprefer item group]");
                            bool ok = false;
                            for (int slot = ScanInt(&ok); ok; slot = ScanInt(&ok))
                            {
                                if (slot > 4) { return IMPORT_FAIL; }
                                if (!ScanStr(&sub)) { return IMPORT_FAIL; }
                                std::map<std::string, int>& names =
                                    getItemGroupNameTable();
                                std::map<std::string, int>::const_iterator it =
                                    names.find(sub);
                                if (it == names.end()) { return IMPORT_FAIL; }
                                int groupIdx = it->second;
                                stGiftItemInfo_t info;
                                info.m_a = groupIdx;
                                info.m_b = ScanInt(&ok);
                                if (!ok) { return IMPORT_FAIL; }
                                info.m_c = static_cast<short>(ScanInt(&ok));
                                if (!ok) { return IMPORT_FAIL; }
                                if (unprefer)
                                {
                                    info.m_b = -info.m_b;
                                    info.m_c = 1;  // 推断：同 unprefer item
                                }
                                unsigned long key =
                                    (static_cast<unsigned long>(slot) << 32) |
                                    static_cast<unsigned long>(groupIdx);
                                table.m_maps[slot].insert(std::make_pair(key, info));
                            }
                            continue;
                        }
                        break;  // 推断：未知子 keyword 结束本段处理
                    }
                    continue;
                }
                if (token == "[reward item]")
                {
                    // ORIG 嵌套：[reward item]{ [item info]{ [favor level]/[mood]{
                    //   [favor] ... } } } ；[/item info]/[/reward item] 逐层退出。
                    int level = 0;
                    int mood = 3;
                    bool inItemInfo = false;
                    for (;;)
                    {
                        if (!ScanType(token, true))
                        {
                            break;  // 推断：ORIG 以 ScanType 失败逐层退出
                        }
                        if (token == "[/reward item]")
                        {
                            break;
                        }
                        if (token == "[item info]")
                        {
                            level = 0;
                            mood = 3;
                            inItemInfo = true;
                            continue;
                        }
                        if (token == "[/item info]")
                        {
                            inItemInfo = false;
                            continue;
                        }
                        if (!inItemInfo)
                        {
                            continue;  // 推断：[item info] 外的 token 跳过
                        }
                        if (token == "[favor level]")
                        {
                            bool ok = false;
                            level = ScanInt(&ok);
                            if (!ok) { return IMPORT_FAIL; }
                            continue;
                        }
                        if (token == "[mood]")
                        {
                            if (!ScanStr(&sub)) { return IMPORT_FAIL; }
                            mood = getNpcMoodFromString(sub.c_str());
                            if (mood == 3) { return IMPORT_FAIL; }
                            if (level == 0) { return IMPORT_FAIL; }
                            for (;;)
                            {
                                if (!ScanType(token, true) || token == "[/mood]")
                                {
                                    break;
                                }
                                if (token != "[favor]")
                                {
                                    continue;  // 推断：[/mood] 外未知 token 跳过
                                }
                                if (!ScanStr(&sub)) { break; }
                                int dir = 3;
                                if (sub == "up")        { dir = 1; }
                                else if (sub == "down") { dir = 2; }
                                else if (sub == "keep") { dir = 0; }
                                if (dir == 3) { return IMPORT_FAIL; }
                                bool ok = false;
                                for (int a = ScanInt(&ok); ok; a = ScanInt(&ok))
                                {
                                    stFavorableRelationShip_t::stRewardItemElement_t elem;
                                    elem.m_a = a;
                                    elem.m_b = ScanInt(&ok);
                                    if (!ok) { return IMPORT_FAIL; }
                                    elem.m_w = ScanInt(&ok);
                                    if (!ok) { return IMPORT_FAIL; }
                                    stFavorableRelationShip_t::stRewardItemInfo_t& rs =
                                        fav.m_rewardItems.m_slots[level][mood];
                                    elem.m_w += rs.m_weight[dir];  // ORIG: 第三字段=旧计数+delta
                                    rs.m_byDir[dir].push_back(elem);
                                    rs.m_weight[dir] = elem.m_w;
                                }
                            }
                            continue;
                        }
                    }
                    continue;
                }
                if (token == "[favor level reward mail]")
                {
                    for (;;)
                    {
                        bool ok = false;
                        stFavorableRelationShip_t::stLevelRewardInfo info;
                        info.m_level = static_cast<unsigned short>(ScanInt(&ok));
                        if (!ok) { break; }
                        info.m_flag0 = static_cast<unsigned int>(ScanInt(&ok));
                        if (!ok) { break; }
                        info.m_flag1 = static_cast<unsigned int>(ScanInt(&ok));
                        if (!ok) { break; }
                        std::string name;
                        std::string desc;
                        if (!ScanStr(&name)) { break; }
                        if (!ScanStr(&desc)) { break; }
                        info.m_name = name;
                        info.m_desc = desc;
                        fav.m_levelRewards.push_back(info);
                    }
                    continue;
                }
                if (token == "[favor_duration_reward_buff]")
                {
                    bool ok = false;
                    unsigned short a = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok) { continue; }  // ORIG: 失败回到主循环
                    stFavorableRelationShip_t::stBuffRewardInfo info;
                    info.m_a = a;
                    info.m_b = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok) { continue; }
                    info.m_c = static_cast<unsigned int>(ScanInt(&ok));
                    if (!ok) { continue; }
                    fav.m_buffRewards.push_back(info);
                    continue;
                }
                break;  // 推断：未知子段结束 favorable relationship 块
            }
            continue;
        }

        if (token == "[visible on dungeon clear]")
        {
            npc->m_visibleOnDungeonClear =
                ScanInt(static_cast<bool*>(0)) == 1;
            continue;
        }
        if (token == "[move town limit str]")
        {
            ScanStr(&npc->m_moveTownLimitStr);   // +0xf8
            continue;
        }
        if (token == "[move town confirm str]")
        {
            ScanStr(&npc->m_moveTownConfirmStr); // +0xfc
            continue;
        }
        if (token == "[random message balloon]")
        {
            // ORIG 经静态 __lookup map 分发到三个子 keyword；此处直接顺序解析，
            // 语义等价 —「推断」。
            std::string anim;
            if (ScanStr(&anim) && !anim.empty())
            {
                npc->m_randomMessageAnimation = anim;
            }
            int prob = 0;
            if (ScanInt(&prob) && prob != 0)
            {
                npc->m_randomMessageProbability = prob;
            }
            bool ok = false;
            int count = ScanInt(&ok);
            if (ok)
            {
                std::vector<std::string> list;
                list.reserve(count);
                std::string msg;
                for (int i = 0; i < count && ScanStr(&msg); ++i)
                {
                    list.push_back(msg);
                }
                if (!list.empty())
                {
                    npc->m_randomMessageList = list;
                }
            }
            continue;
        }
        if (token == "[hide]")
        {
            npc->m_hide = ScanInt(static_cast<bool*>(0)) == 1;
            continue;
        }
        // 推断：未知 keyword 直接跳过（ORIG 无 default 动作）。
    }
}

namespace {

// isKeyItem 使用的比较器（ORIG 反编译中称 isSameItem）。
struct NPC_IsSameItem
{
    const unsigned long m_key;
    const unsigned int  m_itemId;

    NPC_IsSameItem(unsigned long key, unsigned int id)
        : m_key(key), m_itemId(id) {}

    bool operator()(const std::pair<unsigned long, unsigned int>& item) const
    {
        return item.first == m_key && item.second == m_itemId;
    }
};

}  // namespace

// ORIG 0x85809a6:
//   ret = importNpcScript((NpcScript*)(this+4), path);
//   if (ret == 1) *(int*)this = *(int*)(this+4);
//   return ret == 1;
bool CNPCScript::load(char const* path)
{
    int ret = importNpcScript(&m_npcScript, path);
    if (ret == 1)
    {
        m_resultCopy = *reinterpret_cast<unsigned int*>(&m_npcScript);
    }
    return ret == 1;
}

// ORIG 0x86d2736: m_roleMap.find(role) != end()（CNPCScript+0x50 = NpcScript+0x4c）。
bool CNPCScript::isExistRole(ENUM_NPC_ROLE role) const
{
    return m_npcScript.m_roleMap.find(role) != m_npcScript.m_roleMap.end();
}

// ORIG 0x8580fd0: 遍历奖励等级表（CNPCScript+0x658 = NpcScript+0x654）。
bool CNPCScript::isRewardLevel(unsigned short level) const
{
    for (std::vector<stFavorableRelationShip_t::stLevelRewardInfo>::const_iterator it =
             m_npcScript.m_favorableRelationShip.m_levelRewards.begin();
         it != m_npcScript.m_favorableRelationShip.m_levelRewards.end(); ++it)
    {
        if (it->m_level == level)
        {
            return true;
        }
    }
    return false;
}

// ORIG 0x8580e6a: find_if(keyItems, isSameItem) != end()（+0x18c）。
bool CNPCScript::isKeyItem(unsigned long key, unsigned int id) const
{
    return std::find_if(m_npcScript.m_favorableRelationShip.m_keyItems.begin(),
                        m_npcScript.m_favorableRelationShip.m_keyItems.end(),
                        NPC_IsSameItem(key, id)) !=
           m_npcScript.m_favorableRelationShip.m_keyItems.end();
}

// ORIG 0x85811a4: +0x670 字符串 == "item" -> 1；=="quest" -> 2；否则 0。
int CNPCScript::getPeriodRewardType() const
{
    const std::string& t = m_npcScript.m_favorableRelationShip.m_durationRewardName;
    if (t == "item")
    {
        return 1;
    }
    if (t == "quest")
    {
        return 2;
    }
    return 0;
}

// 构造/析构：成员均为 std 容器（自带默认构造/析构）。
CNPCScript::CNPCScript() {}
CNPCScript::~CNPCScript() {}
