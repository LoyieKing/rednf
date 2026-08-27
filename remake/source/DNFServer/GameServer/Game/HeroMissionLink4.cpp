// ============================================================================
// HeroMissionLink4.cpp — LinkMisc4 批次：事件/头像域 4 个 ORIG 符号。
//
// 逐报告忠实转写（docs/class_func_reports/）：
//   HeroMissionEvent::sendMissionData(CUser*)            T 0x0816770e sz 0x29d
//   HeroMissionEvent::setEventData(CUser*, char const*)  T 0x08166752 sz 0x5bc
//   EventClassify::CEventScriptMng::test_process_change_day(int)
//                                                        T 0x0810cc70 sz 0x13f
//   WongWork::CCompoundAvatar::ProcTest(ENUM_CHARACTERJOB, ENUM_EQUIPMENTTYPE,
//                                       int, int, std::map<int,int>&)
//                                                        T 0x08333fda sz 0x1e8
//
// HeroMissionEvent 为布局镜像类（HeroMissionEvent.h）：ORIG 开头的虚调用
// vptr+0x34 = CEventBase::IsEventing(this,0)（movzbl +0x08），此处按头内
// m_isEventing 标志等价判活。
// ============================================================================
#include "HeroMissionEvent.h"
#include "CCompoundAvatar.h"
#include "CDataManager.h"
#include "CEventScriptMng.h"
#include "CUser.h"
#include "GameWorld.h"
#include "GameRand.h"       // get_rand_int
#include "InterfacePacketBuf.h"
#include "LogManager.h"
#include "PacketGuard.h"
#include "PvPTypes.h"       // CMTRand 完整类型

#include <boost/tokenizer.hpp>

#include <map>
#include <string>
#include <utility>
#include <vector>

// ORIG _Z12getItemIndexRKSt6vectorISt4pairIiiESaIS1_EEi T 0x08332e4e sz 0x9a：
// 按 rand 值在 {itemId,count} 权重表中选 itemId。定义缺失（blocked，见交付），
// 此处仅声明以完成本批可编译。
int getItemIndex(const std::vector<std::pair<int, int> >& items, int rand);

// ---------------------------------------------------------------------------
// ORIG _ZN16HeroMissionValue5clearEv T 0x08164b80：全字段清零（setEventData 依赖）。
// ---------------------------------------------------------------------------
void HeroMissionValue::clear()
{
    m_mission = 0;
    m_field4 = 0;
    m_field6 = 0;
    m_field8 = 0;
    m_fieldA = 0;
}

// ---------------------------------------------------------------------------
// HeroMissionEvent::sendMissionData @ 0x0816770e
// ---------------------------------------------------------------------------
void HeroMissionEvent::sendMissionData(CUser* user)
{
    if (m_isEventing == 0)      // ORIG: 虚 IsEventing(0)==1 才继续
    {
        return;
    }


    if (user == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            "void HeroMissionEvent::sendMissionData(CUser*)", 0x307,
            "[HeroMission] Invalid user pointer. (processMission)");
        return;
    }

    unsigned int characNo = (unsigned int)user->get_charac_no(-1);
    std::map<unsigned int, std::vector<HeroMissionValue> >::iterator it =
        m_values.find(characNo);
    if (it == m_values.end())
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            "void HeroMissionEvent::sendMissionData(CUser*)", 0x30e,
            "[HeroMission] Not exist hero mission value. (charac:%u)", characNo);
        return;
    }

    // 包 0x22f：short count × {mission(+0), +4, +8, +0xa}
    PacketGuard packet;
    packet.put_header(0, 0x22f);
    packet.put_short((int)it->second.size());
    for (std::vector<HeroMissionValue>::iterator vit = it->second.begin();
         vit != it->second.end(); ++vit)
    {
        packet.put_short(vit->m_mission);   // +0x00
        packet.put_short(vit->m_field4);    // +0x04
        packet.put_short(vit->m_field8);    // +0x08
        packet.put_short(vit->m_fieldA);    // +0x0a
    }
    packet.finalize(true);
    user->Send(packet);
}

// ---------------------------------------------------------------------------
// HeroMissionEvent::setEventData @ 0x08166752
// ---------------------------------------------------------------------------
void HeroMissionEvent::setEventData(CUser* user, char const* data)
{
    if (m_isEventing == 0)      // ORIG: 虚 IsEventing(0)==1 才继续
    {
        return;
    }
    if (user->get_charac_no(-1) == 0)
    {
        return;
    }

    std::vector<HeroMissionValue> values;
    std::string scriptData(data);

    if (scriptData.length() == 0)
    {
        // 空数据：按条件表生成初始值单（满级 70 时任务 4/6 直接置完成态）
        for (std::map<HeroMissionCondition::MissionNo::T,
                      BaseHeroMissionCondition*>::iterator cit =
                 m_conditions.begin();
             cit != m_conditions.end(); ++cit)
        {
            HeroMissionValue value;
            value.clear();
            value.m_mission = cit->first;
            if (cit->first == HeroMissionCondition::MissionNo::T_4 &&
                user->get_charac_level() == 70)
            {
                value.m_field4 = 1;
                value.m_field6 = 1;
                value.m_field8 = 70;
                value.m_fieldA = 1;
            }
            if (cit->first == HeroMissionCondition::MissionNo::T_6 &&
                user->get_charac_level() == 70)
            {
                value.m_field4 = 1;
            }
            values.push_back(value);
        }
    }
    else
    {
        // ORIG: boost::char_separator(",") 切分，逐 token setStringToken
        boost::char_separator<char> separator(",");
        boost::tokenizer<boost::char_separator<char> > tokens(scriptData,
                                                              separator);
        for (boost::tokenizer<boost::char_separator<char> >::iterator tit =
                 tokens.begin();
             tit != tokens.end(); ++tit)
        {
            HeroMissionValue value;     // ORIG 不 clear，由 setStringToken 填充
            value.setStringToken(*tit);
            values.push_back(value);
        }
    }

    unsigned int characNo = (unsigned int)user->get_charac_no(-1);
    std::map<unsigned int, std::vector<HeroMissionValue> >::iterator it =
        m_values.find(characNo);
    if (it != m_values.end())
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            "void HeroMissionEvent::setEventData(CUser*, const char*)", 0x24b,
            "[HeroMission] Already exist character data. (charac:%u)",
            characNo);
        m_values.erase(characNo);
    }
    m_values.insert(std::make_pair(characNo, values));
}

// ---------------------------------------------------------------------------
// EventClassify::CEventScriptMng::test_process_change_day @ 0x0810cc70
// ---------------------------------------------------------------------------
void EventClassify::CEventScriptMng::test_process_change_day(int timeVal)
{
    if (timeVal <= 0x132daaf)
    {
        return;
    }

    m_todayTime = timeVal;                          // +0x48
    m_todayTimeDay = (unsigned char)(timeVal % 100);  // +0x4c

    GameWorld* world = G_GameWorld();               // +0x134 m_UserInWorld
    std::map<unsigned short, CUser*>::iterator it = world->m_UserInWorld.begin();
    while (it != world->m_UserInWorld.end())
    {
        CUser* user = it->second;
        if (user != 0 && user->get_state() >= 3)
        {
            user->RecoverFatigue(0);
            user->SendFatigue();
            EventClassify::CEventScriptMng* scriptMng =
                static_cast<EventClassify::CEventScriptMng*>(
                    G_CDataManager()->get_event_script_mng());
            scriptMng->send_event_init_data(user, true);
        }
        ++it;
    }
}

// ---------------------------------------------------------------------------
// WongWork::CCompoundAvatar::ProcTest @ 0x08333fda
// ---------------------------------------------------------------------------
void WongWork::CCompoundAvatar::ProcTest(ENUM_CHARACTERJOB job,
                                         ENUM_EQUIPMENTTYPE equipType,
                                         int addRate, int bonusFlag,
                                         std::map<int, int>& result)
{
    tagCompoundAvatar* info = m_tables[job].getCompoundAvatarInfo(1);

    for (int loop = 0; loop < 10000; ++loop)
    {
        int itemIdx = 0;
        // 命中权重 = m_a[equipType] + addRate * m_e[equipType]
        int rate = info->m_a[equipType] + addRate * info->m_e[equipType];
        if (bonusFlag == 1)
        {
            rate += *(int*)((char*)G_CDataManager() +
                            0x4a54);   // ORIG 直访（字段未建模）
        }

        unsigned long range = 10000;
        if ((unsigned)m_rand->randInt(range) < (unsigned)rate)
        {
            itemIdx = getItemIndex(info->m_optsA[equipType],
                                   get_rand_int(info->m_b[equipType]));
        }
        if (itemIdx == 0)
        {
            itemIdx = getItemIndex(info->m_optsB[equipType],
                                   get_rand_int(info->m_c[equipType]));
        }

        std::map<int, int>::iterator rit = result.find(itemIdx);
        if (rit == result.end())
        {
            result.insert(std::make_pair(itemIdx, 1));
        }
        else
        {
            ++rit->second;
        }
    }
}
// ---------------------------------------------------------------------------
// ORIG _Z12getItemIndexRKSt6vectorISt4pairIiiESaIS1_EEi T 0x08332e4e sz 0x9a：
// 顺序找首个 second 高于 rand 的表项返回其 first（itemId）；未命中返 0。
// ORIG 循环内 __gnu_cxxne 判续 + 循环后 __gnu_cxxeq 判 ==end 双实例化，
// 对应普通 for 迭代器循环。
// ---------------------------------------------------------------------------
int getItemIndex(const std::vector<std::pair<int, int> >& items, int rand)
{
    std::vector<std::pair<int, int> >::const_iterator it = items.begin();
    for (; it != items.end(); ++it)
    {
        if (it->second > rand)
            break;
    }
    if (it == items.end())
        return 0;
    return it->first;
}

// ---------------------------------------------------------------------------
// ORIG _ZN16HeroMissionValue14setStringTokenESs T 0x08164d68 sz 0x1aa：
//   clear 后按空格分隔（char_separator(" ", NULL, drop)）逐 token atoi 写入：
//   tok0→m_mission(int)、tok1→+0x04、tok2→+0x06、tok3→+0x0a、tok4→+0x08。
//   注意第 4/5 个 token 写入偏移与字段声明序相反（ORIG 如此，忠实保留）。
// ---------------------------------------------------------------------------
void HeroMissionValue::setStringToken(std::string token)
{
    clear();

    typedef boost::token_iterator<
        boost::char_separator<char>,
        __gnu_cxx::__normal_iterator<const char*, std::string>,
        std::string> TokenIterator;

    boost::char_separator<char> separator(" ", 0,
                                          boost::drop_empty_tokens);
    boost::tokenizer<boost::char_separator<char>,
                     __gnu_cxx::__normal_iterator<const char*, std::string>,
                     std::string>
        tokens(token, separator);

    TokenIterator tit = tokens.begin();

    m_mission = atoi((*tit).c_str());                   // +0x00
    ++tit;
    m_field4 = (unsigned short)atoi((*tit).c_str());    // +0x04
    ++tit;
    m_field6 = (unsigned short)atoi((*tit).c_str());    // +0x06
    ++tit;
    m_fieldA = (unsigned short)atoi((*tit).c_str());    // +0x0a
    ++tit;
    m_field8 = (unsigned short)atoi((*tit).c_str());    // +0x08
}

// ---------------------------------------------------------------------------
// ~HeroMissionEvent 三形态（ORIG D1/D2 @0x081665a2 同体、D0 @0x0816662c）。
// ORIG 析构顺序：m_values(+0x24) → m_conditions(+0x0c)，与本类成员声明逆序
// 一致；vptr 镜像字段无需处理（ORIG 仅重写 vptr，镜像方案无虚表）。
// 类为非虚布局镜像，GCC 正常析构定义即产出 D1/D2 两符号（同体，与 ORIG
// 别名一致）；D0 删除型析构单独以 asm 标签桥出，语义为 D1 + operator delete，
// 与 ORIG D0 体逐条对应（call D1Ev; call _ZdlPv）。
// ---------------------------------------------------------------------------
HeroMissionEvent::~HeroMissionEvent()
{
    // 成员（std::map/vector）由编译器按逆序自动析构。
}

// ORIG _ZN16HeroMissionEventD0Ev T 0x0816662c：先析构再 operator delete。
void HeroMissionEvent_deleting_dtor(HeroMissionEvent* self)
    __asm__("_ZN16HeroMissionEventD0Ev");
void HeroMissionEvent_deleting_dtor(HeroMissionEvent* self)
{
    delete self;
}
