// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 HeroMissionLink4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "HeroMissionLink4/getItemIndex.inc"

// ---------------------------------------------------------------------------
// ORIG _ZN16HeroMissionValue5clearEv T 0x08164b80：全字段清零（setEventData 依赖）。
// ---------------------------------------------------------------------------
#include "HeroMissionLink4/clear.inc"
{
    m_mission = 0;
    m_field4 = 0;
    m_field6 = 0;
    m_field8 = 0;
    m_fieldA = 0;
}

// ---------------------------------------------------------------------------
// ORIG _ZN16HeroMissionValue5resetEv T 0x08164bb2（0x17，LinkFinal2 核销）：
// 仅 +0x04/+0x06 两个 ushort 清零（objdump 逐指令转写）。
// ---------------------------------------------------------------------------
#include "HeroMissionLink4/reset.inc"
{
    m_field4 = 0;   // ORIG movw $0x0,+0x04
    m_field6 = 0;   // ORIG movw $0x0,+0x06
}

// ---------------------------------------------------------------------------
// HeroMissionEvent::sendMissionData @ 0x0816770e
// ---------------------------------------------------------------------------
#include "HeroMissionLink4/sendMissionData.inc"
{
    // [b28c-L7] 判词：ORIG 0x0816770e 段为 `mov (%eax);add $0x34;mov (%eax);
    // call *%edx`（vptr+0x34 虚调 IsEventing(0)）⇒ 必须虚调用形；直读
    // m_isEventing 产 movzbl 0x8(%eax) 伪差。
    if (!IsEventing(0))
        return;


    if (user == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x307,
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
            __PRETTY_FUNCTION__, 0x30e,
            "[HeroMission] Not exist hero mission value. (charac:%u)",
            (unsigned int)user->get_charac_no(-1));
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
// EventClassify::CEventScriptMng::test_process_change_day @ 0x0810cc70
// ---------------------------------------------------------------------------
#include "HeroMissionLink4/test_process_change_day.inc"
{
    if (timeVal <= 0x132daaf)
    {
        return;
    }

    m_todayTime = timeVal;                          // +0x48
    m_todayTimeDay = (unsigned char)(timeVal % 100);  // +0x4c

    std::map<unsigned short, CUser*>::iterator it =
        G_GameWorld()->m_UserInWorld.begin();       // +0x134 m_UserInWorld
    while (it != G_GameWorld()->m_UserInWorld.end())
    {
        CUser* user = it->second;
        if (user != 0 && user->get_state() >= 3)
        {
            user->RecoverFatigue(0);
            user->SendFatigue();
            static_cast<EventClassify::CEventScriptMng*>(
                G_CDataManager()->get_event_script_mng())
                ->send_event_init_data(user, true);
        }
        it++;
    }
}

// ---------------------------------------------------------------------------
// WongWork::CCompoundAvatar::ProcTest @ 0x08333fda
// ---------------------------------------------------------------------------
// [b28d-D33 P28/P61 封顶] ORIG 0x08333fda：142 vs 143 条；p167 差集 = ne/make_pair
// 非 const(MISSING) vs eq/make_pair const(EXTRA)（P61/P81 左值性 + P220 极性）。
// [F2d-C05② 甄别] 无命中形：gap=+1（show_func_diff orig=143 our=142，我们**少**）
//   ⇒ F2 减指令方向相反（残余 ORIG-only={mov×2} / OURS-only={add}）。维持封顶。
#include "HeroMissionLink4/ProcTest.inc"
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
// [laneC] ORIG 0x08332e4e：头部 begin→-0xc / end→-0x10 各一次，循环条件与循环后
// 比较复用缓存的 end 槽（P13 end 迭代器缓存；ORIG 仅 1 处 `call vector::end`，
// 0x8332e61 begin / 0x8332e76 end；我方原形两处 `items.end()` 发 2 次 call ⇒
// p167 EXTRA vector::end）。改为具名 itEnd 缓存后与 ORIG 调用点集一致。
#include "HeroMissionLink4/getItemIndex_1.inc"
{
    std::vector<std::pair<int, int> >::const_iterator it = items.begin();
    std::vector<std::pair<int, int> >::const_iterator itEnd = items.end();
    for (; it != itEnd; ++it)
    {
        if (it->second > rand)
            break;
    }
    if (it == itEnd)
        return 0;
    return it->first;
}

// ---------------------------------------------------------------------------
// ORIG _ZN16HeroMissionValue14setStringTokenESs T 0x08164d68 sz 0x1aa：
//   clear 后按空格分隔（char_separator(" ", NULL, drop)）逐 token atoi 写入：
//   tok0→m_mission(int)、tok1→+0x04、tok2→+0x06、tok3→+0x0a、tok4→+0x08。
//   注意第 4/5 个 token 写入偏移与字段声明序相反（ORIG 如此，忠实保留）。
// ---------------------------------------------------------------------------
#include "HeroMissionLink4/setStringToken.inc"
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

    // P23：ORIG 每步以 ++ 返回值直传下一调用（`mov %eax,(%esp); call ptEv`），
    // 拆语句 `++tit; tit->...` 会重复 lea 迭代器地址（每处 +2 条）。
    m_mission = atoi(tit->c_str());                     // +0x00
    m_field4 = (unsigned short)atoi((++tit)->c_str());  // +0x04
    m_field6 = (unsigned short)atoi((++tit)->c_str());  // +0x06
    m_fieldA = (unsigned short)atoi((++tit)->c_str());  // +0x0a
    m_field8 = (unsigned short)atoi((++tit)->c_str());  // +0x08
}

// ---------------------------------------------------------------------------
// ~HeroMissionEvent（ORIG D1/D2 @0x081665a2 同体、D0 @0x0816662c）。
// 类现为真实继承（Singleton+CEventBase），虚析构由编译器自动发射
// D1/D2（同体）+ D0（call D1 + operator delete），与 ORIG 三形态对应。
// ORIG 析构顺序：m_values(+0x24) → m_conditions(+0x0c)，与本类成员声明逆序
// 一致（编译器按声明逆序自动析构）。
// ---------------------------------------------------------------------------
#include "HeroMissionLink4/dtor_HeroMissionEvent.inc"
{
    // 成员（std::map/vector）由编译器按逆序自动析构。
}
