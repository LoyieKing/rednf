#include "NPCScript.h"

#include <algorithm>
#include <cstring>
#include <string>

#include "NpcScript.h"
#include "CCubeStatistic.h"   // getItemGroupNameTable()（ORIG 0x088c0409）
#include "CDataManager.h"     // G_CDataManager()/getItemList()（giveGiftItem 族）
#include "CItem.h"            // CItem::get_index/getItemGroupName
#include "DNFLexWrapper.h"    // ScanType/ScanInt/ScanStr
#include "DNFFunctionLib.h"   // get_rand_int(int)

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

// ORIG W 0x08582338：仅构造 this+0x04 NpcScript；+0x00 不初始化。
CNPCScript::CNPCScript()
{
}

// ORIG W 0x0858234e：仅析构 this+0x04 NpcScript。
CNPCScript::~CNPCScript()
{
}


// ----------------------------------------------------------------------------
// PresentResponceScript 族 —— ORIG [favor_dialog] 三结构生命周期：
//   Script    C1 0x8a00aba / clear 0x8a00a86（m_a/m_b/m_c=0 + m_str.clear()）
//   Exception C1 0x8a00b22 / clear 0x8a00b02（m_a=0 + m_str.clear()）
//   Event     C1 0x8a00b8a / clear 0x8a00b6a（m_a=0 + m_str.clear()）
// ----------------------------------------------------------------------------
PresentResponceScript::PresentResponceScript() : m_str()
{
    clear();
}

void PresentResponceScript::clear()
{
    m_a = 0;
    m_b = 0;
    m_c = 0;
    m_str.clear();
}

PresentResponceScriptException::PresentResponceScriptException() : m_str()
{
    clear();
}

void PresentResponceScriptException::clear()
{
    m_a = 0;
    m_str.clear();
}

// InteractiveDialogScript：ORIG C1 0x08a009f2（成员 C1 + clear）/
// clear T 0x08a0098a（+0/+4/+8 int=0、+0xc vector clear、+0x18 byte=0、
// +0x1c int=0、+0x20 byte=0、+0x24 string clear、+0x28 vector clear）。
InteractiveDialogScript::InteractiveDialogScript() : m_str()
{
    clear();
}

void InteractiveDialogScript::clear()
{
    m_a = 0;
    m_b = 0;
    m_c = 0;
    m_conditions.clear();
    m_flag0 = 0;
    m_d = 0;
    m_flag = false;
    m_str.clear();
    m_list2.clear();
}

PresentResponceScriptEvent::PresentResponceScriptEvent() : m_str()
{
    clear();
}

void PresentResponceScriptEvent::clear()
{
    m_a = 0;
    m_str.clear();
}

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
    m_hide = false;
    // ORIG clear 未显式清 +0x6a4..+0x6e4：randomMessageBalloonInfo / NpcMovieData
    // 由其自身 clear 保证空态，这里不重复清（与 ORIG 字节一致）。
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
    m_fav.m_moodChanges.clear();
    m_fav.m_keyItems.clear();
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
                    npc->m_roleMap[static_cast<ENUM_NPC_ROLE>(role)] =
                        ScanInt(static_cast<bool*>(0));
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
                npc->m_movieData.m_movies.push_back(sub);
            }
            npc->m_movieData.m_movieIdx = 0;
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
                                fav.m_fav.m_moodChanges.push_back(
                                    std::make_pair(static_cast<ENUM_NPC_MOOD>(mood),
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
                                fav.m_fav.m_keyItems.push_back(
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
                            stFavorableRelationShip_t::stGiftTable_t& table = (token == "[prefer item]")
                                                       ? fav.m_preferItems
                                                       : fav.m_unpreferItems;
                            const bool unprefer = (token == "[unprefer item]");
                            bool ok = false;
                            for (int slot = ScanInt(&ok); ok; slot = ScanInt(&ok))
                            {
                                if (slot > 4) { return IMPORT_FAIL; }
                                stFavorableRelationShip_t::stGiftItemInfo_t info;
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
                            stFavorableRelationShip_t::stGiftTable_t& table = (token == "[prefer item group]")
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
                                stFavorableRelationShip_t::stGiftItemInfo_t info;
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
                npc->m_balloonInfo.m_randomMessageAnimation = anim;
            }
            int prob = 0;
            if (ScanInt(&prob) && prob != 0)
            {
                npc->m_balloonInfo.m_randomMessageProbability = prob;
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
                    npc->m_balloonInfo.m_randomMessageList = list;
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

// ORIG W 0x08582374/0x085823a4：isKeyItem 的具名 functor（nm 三符号
//   isSameItem C1/C2/cl，W 弱符号；ctor 以 pair<unsigned long,unsigned int>
//   转发构造 m_pair，operator() 非 const 且语义为
//   item.first==m_pair.first && m_pair.second>=item.second）。
struct isSameItem
{
    std::pair<unsigned long, unsigned int> m_pair;

    isSameItem(unsigned long m, unsigned int j)
        : m_pair(m, j) {}

    bool operator()(const std::pair<unsigned long, unsigned int>& item)
    {
        if (m_pair.first == item.first && m_pair.second >= item.second)
        {
            return true;
        }
        return false;
    }
};

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

// 具名局部 comp 先构造（ORIG ctor 调用居函数首），比较用 eq + je 分支形态。
bool CNPCScript::isKeyItem(unsigned long key, unsigned int id) const
{
    isSameItem comp(key, id);
    std::vector<std::pair<unsigned long, unsigned int> >::const_iterator result =
        std::find_if(m_npcScript.m_favorableRelationShip.m_fav.m_keyItems.begin(),
                     m_npcScript.m_favorableRelationShip.m_fav.m_keyItems.end(), comp);
    if (result == m_npcScript.m_favorableRelationShip.m_fav.m_keyItems.end())
    {
        return false;
    }
    return true;
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

// ============================================================================
// CNPCScript 礼物/心情/奖励 6 函数 —— ORIG 0x85809e4 / 0x8580d30 / 0x8580f28 /
// 0x8581076 / 0x85811f8（T）；getFavorRatePerMood 为头内联（W 0x085819c6）。
// ============================================================================

// ORIG 0x85809e4：按 mood 槽依次查四张礼物表（m_maps[mood]），命中后比较
//   it->second.m_b（ushort）与 count 决定采纳 m_c 还是写 result=FAIL；
//   give!=0 时提前返回。m_c 为有符号 short（setg 16 位比较 / std::max<short>）。
unsigned short CNPCScript::giveGiftItem(unsigned int mood, CItem const* item, unsigned int count,
                                        ENUM_NPC_FAVOR_GIFT_RESULT_CONST& result) const
{
    short give = 0;
    std::map<unsigned long, stFavorableRelationShip_t::stGiftItemInfo_t>::const_iterator it;
    unsigned long key = item->get_index();
    it = m_npcScript.m_favorableRelationShip.m_preferItems.m_maps[mood].find(key);
    if (it != m_npcScript.m_favorableRelationShip.m_preferItems.m_maps[mood].end())
    {
        if (static_cast<unsigned short>(it->second.m_b) <= count)
        {
            give = it->second.m_c;
        }
        else
        {
            result = ENUM_NPC_FAVOR_GIFT_RESULT_FAIL;
        }
    }
    int groupName = item->getItemGroupName();
    if (groupName != 0)
    {
        it = m_npcScript.m_favorableRelationShip.m_preferItemGroups.m_maps[mood].find(
            static_cast<unsigned long>(groupName));
        if (it != m_npcScript.m_favorableRelationShip.m_preferItemGroups.m_maps[mood].end())
        {
            if (static_cast<unsigned short>(it->second.m_b) <= count)
            {
                if (it->second.m_c > give)
                {
                    give = it->second.m_c;
                }
            }
            else
            {
                result = ENUM_NPC_FAVOR_GIFT_RESULT_FAIL;
            }
        }
    }
    if (give != 0)
    {
        return give;
    }
    key = item->get_index();
    it = m_npcScript.m_favorableRelationShip.m_unpreferItems.m_maps[mood].find(key);
    if (it != m_npcScript.m_favorableRelationShip.m_unpreferItems.m_maps[mood].end())
    {
        give = it->second.m_c;
    }
    int groupName2 = item->getItemGroupName();
    if (groupName2 != 0)
    {
        it = m_npcScript.m_favorableRelationShip.m_unpreferItemGroups.m_maps[mood].find(
            static_cast<unsigned long>(groupName2));
        if (it != m_npcScript.m_favorableRelationShip.m_unpreferItemGroups.m_maps[mood].end())
        {
            if (give != 0)
            {
                give = std::max<short>(give, it->second.m_c);
            }
            else
            {
                give = it->second.m_c;
            }
        }
    }
    return give;
}

// ORIG 0x8580d30：按 m_rewardItems.m_slots[index][mood].m_byDir[change] 权重表
//   随机抽取（get_rand_int(weight)）；权重 0 条目（m_a==0 && m_b==0）直接 break。
Inven_Item CNPCScript::getGiftRewardItem(ENUM_NPC_MOOD mood,
                                         ENUM_NPC_FAVOR_CHANGE_CONST change,
                                         unsigned int index) const
{
    Inven_Item result;
    const stFavorableRelationShip_t::stRewardItemInfo_t& slot =
        m_npcScript.m_favorableRelationShip.m_rewardItems.m_slots[index][mood];
    int r = get_rand_int(static_cast<int>(slot.m_weight[change]));
    for (unsigned int i = 0; i < slot.m_byDir[change].size(); ++i)
    {
        const stFavorableRelationShip_t::stRewardItemElement_t& e =
            slot.m_byDir[change].at(i);
        if (static_cast<unsigned int>(e.m_w) > static_cast<unsigned int>(r))
        {
            if (e.m_a == 0 && e.m_b == 0)
            {
                break;
            }
            result.m_addInfo = e.m_a;
            G_CDataManager()->getItemList()->create_item(
                ENUM_ITEM_CREATE_TYPE_0, result, e.m_b);
            break;
        }
    }
    return result;
}

// ORIG 0x8580f28：m_moodChanges（NpcScript+0x180 处 pair<MOOD,uint> vector）
//   轮转：空表时 mood=1/count=0 返回 0；否则 ++count 后越界回绕，
//   输出 at(count) 的 first/second。
int CNPCScript::getNextMood(ENUM_NPC_MOOD& mood, unsigned int& count) const
{
    if (m_npcScript.m_favorableRelationShip.m_fav.m_moodChanges.empty())
    {
        mood = ENUM_NPC_MOOD_NORMAL;
        count = 0;
        return 0;
    }
    ++count;
    if (m_npcScript.m_favorableRelationShip.m_fav.m_moodChanges.size() <= count)
    {
        count = 0;
    }
    mood = m_npcScript.m_favorableRelationShip.m_fav.m_moodChanges.at(count).first;
    return m_npcScript.m_favorableRelationShip.m_fav.m_moodChanges.at(count).second;
}

// ORIG 0x8581076：遍历 m_levelRewards，匹配 level 后拷贝五字段（string 用
//   operator=）；循环条件每轮重求 end()。
stFavorableRelationShip_t::stLevelRewardInfo CNPCScript::getLevelRewardInfo(
    unsigned short level) const
{
    stFavorableRelationShip_t::stLevelRewardInfo result;
    std::vector<stFavorableRelationShip_t::stLevelRewardInfo>::const_iterator it =
        m_npcScript.m_favorableRelationShip.m_levelRewards.begin();
    for (; it != m_npcScript.m_favorableRelationShip.m_levelRewards.end(); it++)
    {
        const stFavorableRelationShip_t::stLevelRewardInfo* p = &*it;
        if (p != 0)
        {
            if (p->m_level == level)
            {
                result.m_level = p->m_level;
                result.m_flag0 = p->m_flag0;
                result.m_flag1 = p->m_flag1;
                result.m_name = p->m_name;
                result.m_desc = p->m_desc;
                break;
            }
        }
    }
    return result;
}

// ORIG 0x85811f8：m_buffRewards 权重随机（get_rand_int(1000)），条件
//   it->m_a < level；命中后拷贝 m_b/m_a/m_c 并 break。
stFavorableRelationShip_t::stBuffRewardInfo CNPCScript::getBuffRewardInfo(int level) const
{
    std::vector<stFavorableRelationShip_t::stBuffRewardInfo>::const_iterator it =
        m_npcScript.m_favorableRelationShip.m_buffRewards.begin();
    std::vector<stFavorableRelationShip_t::stBuffRewardInfo>::const_iterator end =
        m_npcScript.m_favorableRelationShip.m_buffRewards.end();
    stFavorableRelationShip_t::stBuffRewardInfo info;
    int r = get_rand_int(1000);
    int total = 0;
    info.m_b = 0;
    for (; it != end; it++)
    {
        if (it->m_a < level)
        {
            total += it->m_c;
            if (total > r)
            {
                info.m_b = it->m_b;
                info.m_a = it->m_a;
                info.m_c = it->m_c;
                break;
            }
        }
    }
    return info;
}
