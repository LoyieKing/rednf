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
// class SpinLock / struct less_TCHAR（[random message balloon] 静态查找表用）。
#include "CEnvironment.h"

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
    // b28c-L8 代落 C28L35（Main R1 裁决 NPCScript.cpp 归本 lane）：ORIG 0x089fbf22
    // 的两个 +0xf8/+0xfc assign 实际排在 +0x698（m_clickMarks）之后、+0x6b8（m_hide）
    // 之前（Ghidra C 段逐条同序）；旧形挂在 +0x110 之后 ⇒ 10 条指令整体错位（ndiff 20）。
    m_moveTownLimitStr = "";
    m_moveTownConfirmStr = "";
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
    // b28c-L8（P212 全头部声明复现块级槽序）：ORIG 有两个循环变量槽
    // （map-clear 环 i @-0x10、favorRate 环 j @-0xc）；本工具头实测为
    // 「后声明者更深」⇒ 按 j、i 序声明使 i 落 -0x10。
    int j;
    int i;
    m_enabled = false;
    m_defaultFavor = 0;
    m_maxGiftPerDay = 0;
    m_dialogDayGap1 = 0;
    m_dialogDayGap2 = 0;
    m_fav.m_moodChanges.clear();
    m_fav.m_keyItems.clear();
    // b28c-L8：ORIG 0x089fb5b9 只有**一个**循环变量槽（-0x10）、循环体单轮发
    // 4 次 map clear（基址 +0x28/+0x118/+0xa0/+0x190，步长 i*0x18），共 5 轮。
    // 旧双层形多做 20 倍 clear（幂等故行为不显，但多 4 条外层循环控制且 ndiff
    // 无法收敛）⇒ 收敛为单层。
    for (int i = 0; i < 5; ++i)
    {
        m_preferItems[i].clear();
        m_unpreferItems[i].clear();
        m_preferItemGroups[i].clear();
        m_unpreferItemGroups[i].clear();
    }
    // ORIG 0x089fb649：三档 favorRate 以 `for (j=0;j<3;++j)` 循环写常量
    // 1.0f（`mov $0x3f800000,%edx` 内联立即数、非内存读、非清零）。
    for (j = 0; j < 3; ++j)
    {
        m_favorRate[j] = 1.0f;
    }
    m_rewardItems.clear();
}

namespace {

// 解析失败统一出口：ORIG 在各 scan 失败处 return 0。
const int IMPORT_OK    = 1;
const int IMPORT_FAIL  = 0;

}  // namespace

// ----------------------------------------------------------------------------
// importNpcScript —— ORIG 0x089fc264 主分支逐关键字转写。
// 深水区（长尾 keyword 的次要字段、静态 lookup 表优化）以语义等价形式实现，
// 已在对应分支标注「推断」。
// ----------------------------------------------------------------------------
bool importNpcScript(NpcScript* npc, char const* path)  // ORIG load 调用侧 xor $1 取反 ⇒ bool（P22）
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
            if (sub == "[common]")
            {
                slot = 0xb;
            }
            else
            {
                slot = getCharacterJob(sub.c_str());
            }
            std::vector<int>& list = npc->m_skills[slot];
            list.clear();
            bool ok = false;
            int v = 0;
            while ((v = ScanInt(&ok), ok))
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
            int type = 0;
            while ((type = ScanInt(&ok), ok))
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
            int v = 0;
            while ((v = ScanInt(&ok), ok))
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
            // ORIG 0x089fcb8e：PresentResponce 是函数局部静态（__cxa_guard + atexit，
            // 无每次调用析构）。逐字段 ScanInt/ScanStr 写入静态对象后 push_back。
            static PresentResponceScript PresentResponce;
            PresentResponce.m_a = ScanInt(static_cast<bool*>(0));
            PresentResponce.m_b = ScanInt(static_cast<bool*>(0));
            PresentResponce.m_c = ScanInt(static_cast<bool*>(0));
            ScanStr(&PresentResponce.m_str);
            npc->m_presentResponces.push_back(PresentResponce);
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
            // ORIG 0x089fce08：声明局部 string + 一块清零缓冲（rep stos 0x100 dword），
            // 循环 ScanStr 到同一 string，取 c_str→strchr('>')，命中则 p+1 strcpy 进缓冲、
            // 再对该 string 赋 const char*（operator=(char const*)），最后 push_back。
            std::string one;
            char gtbuf[0x400] = { '\0' };
            while (ScanStr(&one))
            {
                char* pc = const_cast<char*>(one.c_str());
                char* pg = strchr(pc, '>');
                if (pg != 0)
                {
                    pg = pg + 1;
                    strcpy(gtbuf, pg);
                    one = pg;
                    npc->m_firstFavorDialogs.push_back(one);
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
            int v = 0;
            while ((v = ScanInt(&ok), ok))
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
                // ORIG 0x089fd096：ScanInt 结果先存命名 int，再 push_back(ERKi)。
                int tick = ScanInt(static_cast<bool*>(0));
                npc->m_balloonTicks.push_back(tick);
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
                                // ORIG make_pair<ENUM_NPC_MOOD&, unsigned int>：首参为
                                // ENUM_NPC_MOOD 具名左值，次参 unsigned int 右值临时。
                                ENUM_NPC_MOOD mood = static_cast<ENUM_NPC_MOOD>(3);
                                if (!ScanStr(&sub)) { break; }
                                mood = static_cast<ENUM_NPC_MOOD>(getNpcMoodFromString(sub.c_str()));
                                if (mood == 3) { return IMPORT_FAIL; }
                                bool ok = false;
                                int value = ScanInt(&ok);
                                if (!ok) { return IMPORT_FAIL; }
                                fav.m_fav.m_moodChanges.push_back(
                                    std::make_pair(mood,
                                                   static_cast<unsigned int>(value)));
                            }
                        }
                        else if (token == "[key item]")
                        {
                            // ORIG make_pair<unsigned long&, unsigned int&>：两参均具名左值。
                            for (;;)
                            {
                                bool ok = false;
                                unsigned long key = 0xffffffff;
                                unsigned int id = 0;
                                key = ScanInt(&ok);
                                if (!ok) { break; }
                                id = ScanInt(&ok);
                                if (!ok) { return IMPORT_FAIL; }
                                fav.m_fav.m_keyItems.push_back(std::make_pair(key, id));
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
                            fav.m_favorRate[2] =
                                static_cast<float>(ScanInt(&ok)) / NPC_FAVOR_RATE_DIVISOR;
                            if (!ok) { return IMPORT_FAIL; }
                            fav.m_favorRate[1] =
                                static_cast<float>(ScanInt(&ok)) / NPC_FAVOR_RATE_DIVISOR;
                            if (!ok) { return IMPORT_FAIL; }
                            fav.m_favorRate[0] =
                                static_cast<float>(ScanInt(&ok)) / NPC_FAVOR_RATE_DIVISOR;
                            if (!ok) { return IMPORT_FAIL; }
                            continue;
                        }
                        // ORIG 四个独立 gift 块（各自 make_pair<unsigned long&, GiftInfo&>
                        // + map::insert，键 = info.m_a 首字），勿合并（合并=少一半调用符号）。
                        // 组名块按 P48 各自调 getItemGroupNameTable()（find/end 各一次，不缓存）。
                        if (token == "[prefer item]")
                        {
                            bool ok = false;
                            int slot = 0;
                            while ((slot = ScanInt(&ok), ok))
                            {
                                if (slot > 4) { return IMPORT_FAIL; }
                                stFavorableRelationShip_t::stGiftItemInfo_t info;
                                info.m_a = ScanInt(&ok);
                                if (!ok) { return IMPORT_FAIL; }
                                info.m_b = static_cast<short>(ScanInt(&ok));
                                if (!ok) { return IMPORT_FAIL; }
                                info.m_c = static_cast<short>(ScanInt(&ok));
                                if (!ok) { return IMPORT_FAIL; }
                                fav.m_preferItems[slot].insert(
                                    std::make_pair(info.m_a, info));
                            }
                            continue;
                        }
                        if (token == "[prefer item group]")
                        {
                            bool ok = false;
                            int slot = 0;
                            while ((slot = ScanInt(&ok), ok))
                            {
                                if (slot > 4) { return IMPORT_FAIL; }
                                if (!ScanStr(&sub)) { return IMPORT_FAIL; }
                                std::map<std::string, int>::const_iterator it =
                                    getItemGroupNameTable().find(sub);
                                if (it == getItemGroupNameTable().end()) { return IMPORT_FAIL; }
                                stFavorableRelationShip_t::stGiftItemInfo_t info;
                                info.m_a = static_cast<unsigned long>(it->second);
                                info.m_b = static_cast<short>(ScanInt(&ok));
                                if (!ok) { return IMPORT_FAIL; }
                                info.m_c = static_cast<short>(ScanInt(&ok));
                                if (!ok) { return IMPORT_FAIL; }
                                fav.m_preferItemGroups[slot].insert(
                                    std::make_pair(info.m_a, info));
                            }
                            continue;
                        }
                        if (token == "[unprefer item]")
                        {
                            bool ok = false;
                            int slot = 0;
                            while ((slot = ScanInt(&ok), ok))
                            {
                                if (slot > 4) { return IMPORT_FAIL; }
                                stFavorableRelationShip_t::stGiftItemInfo_t info;
                                info.m_a = ScanInt(&ok);
                                if (!ok) { return IMPORT_FAIL; }
                                info.m_c = static_cast<short>(ScanInt(&ok));
                                if (!ok) { return IMPORT_FAIL; }
                                info.m_b = 1;
                                info.m_c = -info.m_c;
                                fav.m_unpreferItems[slot].insert(
                                    std::make_pair(info.m_a, info));
                            }
                            continue;
                        }
                        if (token == "[unprefer item group]")
                        {
                            bool ok = false;
                            int slot = 0;
                            while ((slot = ScanInt(&ok), ok))
                            {
                                if (slot > 4) { return IMPORT_FAIL; }
                                if (!ScanStr(&sub)) { return IMPORT_FAIL; }
                                std::map<std::string, int>::const_iterator it =
                                    getItemGroupNameTable().find(sub);
                                if (it == getItemGroupNameTable().end()) { return IMPORT_FAIL; }
                                stFavorableRelationShip_t::stGiftItemInfo_t info;
                                info.m_a = static_cast<unsigned long>(it->second);
                                info.m_c = static_cast<short>(ScanInt(&ok));
                                if (!ok) { return IMPORT_FAIL; }
                                info.m_b = 1;
                                info.m_c = -info.m_c;
                                fav.m_unpreferItemGroups[slot].insert(
                                    std::make_pair(info.m_a, info));
                            }
                            continue;
                        }
                        break;  // 推断：未知子 keyword 结束本段处理
                    }
                    continue;
                }
                if (token == "[reward item]")
                {
                    // ORIG 三层嵌套 ScanType 循环（0x89fdc81 外层 [reward item]、
                    // 0x89fdcf0 中层 [item info]、0x89fddeb 内层 [mood]），逐层退出。
                    for (;;)                       // 外层
                    {
                        if (!ScanType(token, true) || token == "[/reward item]")
                        {
                            break;                  // → 回 favorable 子块
                        }
                        if (token == "[item info]")
                        {
                            int level = 0;
                            int mood = 3;
                            for (;;)                 // 中层 [item info]
                            {
                                if (!ScanType(token, true) || token == "[/item info]")
                                {
                                    break;          // → 回外层
                                }
                                if (token == "[favor level]")
                                {
                                    bool ok = false;
                                    level = ScanInt(&ok);
                                    if (!ok) { return IMPORT_FAIL; }
                                }
                                if (token == "[mood]")
                                {
                                    if (!ScanStr(&sub)) { return IMPORT_FAIL; }
                                    mood = getNpcMoodFromString(sub.c_str());
                                    if (mood == 3) { return IMPORT_FAIL; }
                                    if (level == 0) { return IMPORT_FAIL; }
                                    bool stop = false;
                                    while (!stop)   // 内层 [mood]
                                    {
                                        bool haveFavor = false;
                                        for (;;)
                                        {
                                            if (!ScanType(token, true) || token == "[/mood]")
                                            {
                                                stop = true;   // → 回中层
                                                break;
                                            }
                                            if (token == "[favor]")
                                            {
                                                haveFavor = true;
                                                break;
                                            }
                                        }
                                        if (stop || !haveFavor) { break; }
                                        if (!ScanStr(&sub)) { break; }
                                        int dir = 3;
                                        if (sub == "up")        { dir = 1; }
                                        else if (sub == "down") { dir = 2; }
                                        else if (sub == "keep") { dir = 0; }
                                        if (dir == 3) { return IMPORT_FAIL; }
                                        bool ok = false;
                                        int a = 0;
                                        while ((a = ScanInt(&ok), ok))
                                        {
                                            stFavorableRelationShip_t::stRewardItemElement_t elem;
                                            elem.m_a = a;
                                            elem.m_b = ScanInt(&ok);
                                            if (!ok) { return IMPORT_FAIL; }
                                            elem.m_w = ScanInt(&ok);
                                            if (!ok) { return IMPORT_FAIL; }
                                            stFavorableRelationShip_t::stRewardItemInfo_t& rs =
                                                fav.m_rewardItems.m_slots[level][mood];
                                            elem.m_w += rs.m_weight[dir];
                                            rs.m_byDir[dir].push_back(elem);
                                            rs.m_weight[dir] = elem.m_w;
                                        }
                                    }
                                }
                            }
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
                if (token == "[favor_duration_reward]")
                {
                    // ORIG 0x089fe277：周期奖励直接写入内嵌 stPeriodRewardInfo
                    // （param+0x66c..0x680）：m_type(ushort)/m_name/m_a(int)/m_b(int)/
                    // m_desc/m_param。三临时 string（C1Ev×3，含异常清理 D1Ev），
                    // 成功后 aSERKSs×3 拷入。任一 scan 失败 → 退出 favorable 子块回主循环。
                    bool ok = false;
                    unsigned short type = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok) { break; }
                    std::string name;
                    if (!ScanStr(&name)) { break; }
                    unsigned int a = ScanInt(&ok);
                    if (!ok) { break; }
                    unsigned int b = ScanInt(&ok);
                    if (!ok) { break; }
                    std::string desc;
                    std::string param;
                    if (!ScanStr(&desc)) { break; }
                    if (!ScanStr(&param)) { break; }
                    fav.m_durationReward.m_type = type;
                    fav.m_durationReward.m_name = name;
                    fav.m_durationReward.m_a = a;
                    fav.m_durationReward.m_b = b;
                    fav.m_durationReward.m_desc = desc;
                    fav.m_durationReward.m_param = param;
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
            // ORIG 0x089fe66f：子 keyword 分发用函数局部静态查找表 __lookup
            // (map<char const*,int,less_TCHAR>) + SpinLock __init_lock + bool __inited
            // 双检锁惰性注册（三键 0x6ac/0x6b7/0x6c2）。注册链一次性插入全部三键后置位，
            // 再重查同一 token 走真实解析。
            typedef std::map<char const*, int, less_TCHAR> LookupMap;
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            enum
            {
                TAG_ANIMATION   = 0x6ac,
                TAG_PROBABILITY = 0x6b7,
                TAG_LIST        = 0x6c2
            };

            randomMessageBalloonInfo info;
            std::string sub;
            bool ok = true;
            while (ScanStr(&sub))
            {
                int id;
            recheck:
                if (!__inited)
                {
                    __init_lock.enter();
                    if (__inited)
                    {
                        __init_lock.leave();
                        goto recheck;
                    }
                    id = 0;   // 未就绪 → 走注册链
                }
                else
                {
                    LookupMap::const_iterator it = __lookup.find(sub.c_str());
                    if (it != __lookup.end())
                        id = it->second;
                    else
                        id = -1;
                }

                switch (id)
                {
                case 0:
                case TAG_ANIMATION:
                    if (__inited)
                    {
                        if (!ScanStr(&info.m_randomMessageAnimation)) { ok = false; break; }
                        if (!info.m_randomMessageAnimation.empty())
                            npc->m_balloonInfo.m_randomMessageAnimation =
                                info.m_randomMessageAnimation;
                    }
                    else
                    {
                        __lookup.insert(
                            LookupMap::value_type("[random message animation]",
                                                  static_cast<int>(TAG_ANIMATION)));
                        __lookup.insert(
                            LookupMap::value_type("[random message probability]",
                                                  static_cast<int>(TAG_PROBABILITY)));
                        __lookup.insert(
                            LookupMap::value_type("[random message list]",
                                                  static_cast<int>(TAG_LIST)));
                        __init_lock.leave();
                        __inited = true;
                        goto recheck;
                    }
                    break;
                case TAG_PROBABILITY:
                    if (!ScanInt(&info.m_randomMessageProbability)) { ok = false; break; }
                    if (info.m_randomMessageProbability != 0)
                        npc->m_balloonInfo.m_randomMessageProbability =
                            info.m_randomMessageProbability;
                    break;
                case TAG_LIST:
                {
                    if (!ScanInt(&id)) { ok = false; break; }   // id 复用为 count
                    info.m_randomMessageList.reserve(id);
                    std::string msg;
                    bool bad = false;
                    for (int i = 0; i < id; ++i)
                    {
                        if (!ScanStr(&msg)) { bad = true; break; }
                        info.m_randomMessageList.push_back(msg);
                    }
                    if (bad) { ok = false; break; }
                    if (!info.m_randomMessageList.empty())
                        npc->m_balloonInfo.m_randomMessageList = info.m_randomMessageList;
                    break;
                }
                default:
                    break;
                }
            }
            if (!ok) { return IMPORT_FAIL; }
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
    // ORIG：importNpcScript 结果寄存器直测（xor $1/test，无 ret 槽）；
    // 尾 mov $0x1 常量返回（bool 导入成功即 1，P56 形）。
    if (!importNpcScript(&m_npcScript, path))
    {
        return false;
    }
    m_resultCopy = m_npcScript.m_scriptFileIndex;  // ORIG mov 0x4(%eax) disp 直读首 int
    return true;
}

// ORIG 0x86d2736: m_roleMap.find(role) != end()（CNPCScript+0x50 = NpcScript+0x4c）。
bool CNPCScript::isExistRole(ENUM_NPC_ROLE role) const
{
    return m_npcScript.m_roleMap.find(role) != m_npcScript.m_roleMap.end();
}

// ORIG 0x8580fd0: 遍历奖励等级表（CNPCScript+0x658 = NpcScript+0x654）。
bool CNPCScript::isRewardLevel(unsigned short level) const
{
    // [b28c-21] 残差：ORIG `de()` 结果落 -0xc 槽后 cmpl 判空再 jne 直判命中；
    // 4.4 无指针槽且 sete 物化（迭代器槽序/条件极性跨版本），保持可读源码。
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
    // P48/P183：无 name 指针槽，每分支完整成员访问重取（ORIG add $0x674 重算）。
    if (m_npcScript.m_favorableRelationShip.m_durationReward.m_name == "item")
    {
        return 1;
    }
    if (m_npcScript.m_favorableRelationShip.m_durationReward.m_name == "quest")
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
    it = m_npcScript.m_favorableRelationShip.m_preferItems[mood].find(key);
    if (it != m_npcScript.m_favorableRelationShip.m_preferItems[mood].end())
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
        it = m_npcScript.m_favorableRelationShip.m_preferItemGroups[mood].find(
            static_cast<unsigned long>(groupName));
        if (it != m_npcScript.m_favorableRelationShip.m_preferItemGroups[mood].end())
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
    it = m_npcScript.m_favorableRelationShip.m_unpreferItems[mood].find(key);
    if (it != m_npcScript.m_favorableRelationShip.m_unpreferItems[mood].end())
    {
        give = it->second.m_c;
    }
    int groupName2 = item->getItemGroupName();
    if (groupName2 != 0)
    {
        it = m_npcScript.m_favorableRelationShip.m_unpreferItemGroups[mood].find(
            static_cast<unsigned long>(groupName2));
        if (it != m_npcScript.m_favorableRelationShip.m_unpreferItemGroups[mood].end())
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
            // P167：ORIG 在 create_item 前重新 at() 取值（第二次 at 调用）
            const stFavorableRelationShip_t::stRewardItemElement_t& e2 =
                slot.m_byDir[change].at(i);
            result.m_addInfo = e2.m_a;
            G_CDataManager()->m_itemList->create_item(
                ENUM_ITEM_CREATE_TYPE_0, result, e2.m_b);
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
