// CPartyDepsB2.cpp —— CParty 第二组（独占 TU，2026-08-24）。
// 逐函数依据 docs/class_func_reports/CParty/ 完整报告（## 汇编 + ## 反编译 C）：
//   _Send_TOD_APCInfo.md            ORIG T 0x085bac26
//   _onGetItem.md                   ORIG T 0x085b949c
//   checkDungeonDifficulty.md       ORIG T 0x085be932
//   checkInoutConditionDungeon.md   ORIG T 0x085abc80（三参重载；无参 0x85ac59c 在 CParty.cpp）
//   choose_random_buff.md           ORIG T 0x0859b814
//   die_user.md                     ORIG T 0x085a7828
//   finish_loading.md               ORIG T 0x085b15e0
//   getMemberMaxLevelBeforeDungeon.md ORIG T 0x085b613e
//   get_party_seatno.md             ORIG T 0x085a277a
//   giveup_game.md                  ORIG T 0x085b2baa
// 字段全部使用 CParty.h 具名成员；控制流按 ORIG 汇编形态逐段转写：
// 顺序守卫代替 && 链、单出口析构点、比较树/短路次序一一对应。
// 依赖的跨域叶子实现随本批次落地：
//   QuickParty::CQuickPartyRewardManager::get_random_buff_prob (0x0826d860)
//   EventClassify::CEventScriptMng::process_add_item_count_db  (0x0810bea8)
//   CSpecialItemRoutingManager::ProcessGiveUp                  (0x0860b2c2)
//   TOD_Layer::TOD_Layer / TowerOfDespairMgr::SendAPCInfo      (TowerOfDespairMgr.cpp)

#include <string.h>

#include <algorithm>

#include "GameRand.h"                   // get_rand_int
#include "QuickParty.h"                 // 需先于 CParty.h（守卫次序，同 CParty.cpp）
#include "CParty.h"
#include "CDungeon.h"
#include "BattleData.h"   // finish_loading 装载标志 m_arr7e4（CParty +0xb10）
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "LogManager.h"                 // logFormat
#include "HistoryLog.h"                 // WriteDie / WriteGiveUp
#include "CSystemTime.h"                // GlobalData::s_systemTime_ / getCurSec
#include "Secu_HackLogCheckByParty.h"   // dungeonFail
#include "CDataManager.h"               // G_CDataManager / find_item / get_event_script_mng
#include "CGameManager.h"               // G_CGameManager / GetQuickPartyRewardManager /
                                        // GetSpecialItemRoutingManager
#include "CBattle_Field.h"              // GetDungeon / IsFirstEnterDungeon / check_start_point /
                                        // get_dungeon_diff / m_villageMonsterId / m_battleField.m_dungeonClearTracer
#include "GameWorld.h"                  // G_GameWorld / out_from_dungeon / send_all
#include "CUser.h"                      // Send/make_basic_info/getHackAnalyzer/getMoveSpace/...
#include "CUserCharacInfo.h"            // getCurCharacStamina/getCurCharacNo/... （经 CUser 继承）
#include "CDungeonEntranceLog.h"        // GetInstanceDungeonEntranceLog
#include "GlobalData.h"                 // s_pTowerOfDespairMgr / s_systemTime_
#include "CEventScriptMng.h"            // EventClassify::CEventScriptMng
#include "Statistics.h"                 // Statistics::DungeonFail（static ABI）
#include "CSpecialItemRoutingManager.h" // ProcessGiveUp
#include "TowerOfDespairMgr.h"          // TOD_Layer / TowerOfDespairMgr

// ============================================================================
// CParty::get_party_seatno —— ORIG 0x085a277a
// ============================================================================
int CParty::get_party_seatno(CUser* user)
{
    if (user == 0)
        return 0;

    for (int i = 0; i <= 3; ++i)                        // -0xc
    {
        if (!_checkValidUser(i))
            continue;
        if (m_member[i].m_pUser == user)
            return i;
    }
    return 0;
}

// ============================================================================
// CParty::getMemberMaxLevelBeforeDungeon —— ORIG 0x085b613e
// ============================================================================
int CParty::getMemberMaxLevelBeforeDungeon()
{
    int level;                                          // -0x10（槽位对齐 ORIG）
    int maxLevel = 0;                                   // -0x14
    for (int i = 0; i <= 3; ++i)                        // -0xc
    {
        if (!_checkValidUser(i))
            continue;
        level = m_member[i].m_pUser->get_charac_level_before_dungeon();
        maxLevel = std::max(maxLevel, level);
    }
    return maxLevel;
}

// ============================================================================
// CParty::die_user —— ORIG 0x085a7828
// ============================================================================
void CParty::die_user(CUser* user)
{
    int slot = -1;                                      // -0x10
    for (int i = 0; i <= 3; ++i)                        // -0xc
    {
        if (_checkValidUser(i))
        {
            if (m_member[i].m_pUser == user)
            {
                ++m_member[i].m_pUser->m_character.m_f04;   // +0x8e038 死亡计数
                slot = i;
                HistoryLog::WriteDie(
                    reinterpret_cast<FILE*>(user->m_field796f8));
                break;
            }
        }
    }

    PacketGuard packet;                                 // -0x1c
    packet.put_header(1, 0x2b);
    packet.put_byte(0);
    if (slot == -1)
    {
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        // X2 形（判据①-③：ORIG 0x085a794a `movzbl 0x380(%edx,%eax,1); xor $1,%eax;
        // test %al,%al; je big` 有 je 消费者）；块布局按 ORIG：live 非零（arr==0）
        // fall 走 0x12 区、je 后跳到 big 区（技巧 17 fall-through 块在前）——
        // `if(!live){big}else{0x12}` 写法 4.4 反产 jne+fall=big 极性镜像。
        register unsigned char live asm("eax") = m_battleData.m_arr54[slot];
        live ^= 1;
        if (live)                       // +0x380[slot]==0 → 0x12 回执
        {
            packet.put_byte(0x12);
            packet.finalize(true);
            user->Send(packet);
        }
        else                            // +0x380[slot]==1 → 死亡清算
        {
            set_charac_live(user, (ENUM_USER_DUNGEON_STATE)0);
            if (user->GetRevivalTime() >
                GlobalData::s_systemTime_.getCurSec())
            {
                set_charac_live(user, (ENUM_USER_DUNGEON_STATE)2);
                user->SetRevivalTime(0);
            }
            user->SetUseAPCPotionTime(0);

            if (check_allmember_die())
            {
                if (!m_tournamentVictory)
                {
                    if (m_battleField.m_bloodState != 1)
                        send_dungeon_fail_msg();
                }
            }
        }
    }
}

 // ============================================================================
// CParty::finish_loading —— ORIG 0x085b15e0
// ============================================================================
void CParty::finish_loading(CUser* user)
{
    for (int i = 0; i <= 3; ++i)                        // -0xc
    {
        if (m_member[i].m_pUser != user)
            continue;
        if (m_battleData.m_arr50[i] != 0)        // ORIG: movzbl 0x37c; test; jne 循环尾
            continue;
        m_battleData.m_arr50[i] = 1;
        // ORIG 0x085b15e0 域：`movzbl 0xb10(%edx,%eax,1); xor $0x1,%eax; test; je`
        // 具名 m_arr7e4 消裸偏移后 scale 单段 ✓；裸 `!field` 4.4 产 test;jne 缺
        // xor ⇒ X2 判据③有消费者，register uchar 绑 %eax + `^=1` 形。
        register unsigned char fv asm("eax") = m_battleData.m_arr7e4[i];
        fv ^= 1;
        // 【二次核验】goto 展平 probe 反产 3 jmp；嵌套形 5 行残差为 4.4 CFG
        // 块布局（双 jmp 汇聚 + bool setne 归一）产品，维持最优 5 行嵌套。
        if (fv)
        {
            if (OnStartMapFinishLoading(user))
                m_battleData.m_arr7e4[i] = 1;
        }
    }

    // ORIG 两次独立 this+0xb24 计算（GetDungeon / IsFirstEnterDungeon），
    // 不保存 CBattle_Field 引用。
    CDungeon* dungeon =
        reinterpret_cast<CBattle_Field*>(&m_battleField)->GetDungeon();  // -0x10
    // ORIG 0x085b16??：&& 短路 flag-island 双层物化（`mov $1,%eax; jmp; mov $0,%eax;
    // test %al; je`，技巧 40/22 形）——独立 enterEachMap 槽会反产 movl 存回。
    if (dungeon != 0 &&
        dungeon->IsEnterEachMap())
    {
        if (reinterpret_cast<CBattle_Field*>(&m_battleField)->IsFirstEnterDungeon())
            check_allmember_loading(false);
        else
            check_allmember_loading(true);
    }
    else
    {
        check_allmember_loading(false);
    }
}

// ============================================================================
// CParty::checkDungeonDifficulty —— ORIG 0x085be932
// ============================================================================
bool CParty::checkDungeonDifficulty(char difficulty, CDungeon const* dungeon)
{
    if (dungeon == 0)
        return false;
    if (difficulty == 0)
        return false;
    if (m_manager->IsGameMasterMode())                  // +0x74（ORIG 直取不判空）
        return false;
    if (dungeon->isTournamentDungeon())
        return false;
    if (getManager()->getMoveSpace() == (UserSpace::T)1)
        return false;
    if (dungeon->isRiskDungeon())
        return false;
    return true;
}

// ============================================================================
// CParty::_Send_TOD_APCInfo —— ORIG 0x085bac26（恒返回 1，单出口）
// ============================================================================
int CParty::_Send_TOD_APCInfo(int todLayerIdx)
{
    if (m_battleField.m_pDungeon->isTowerOfDespairDungeon())           // +0xcac（ORIG 直取）
    {
        TowerOfDespairMgr* todMgr =
            static_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr);

        TOD_Layer layerZero(0);                         // -0xe
        todMgr->SendAPCInfo(layerZero, getManager());
        if (todLayerIdx != -1)
        {
            TOD_Layer layerParam((unsigned short)todLayerIdx);  // -0x10
            todMgr->SendAPCInfo(layerParam, getManager());
        }
    }
    return 1;
}

// ============================================================================
// CParty::_onGetItem —— ORIG 0x085b949c
// ============================================================================
void CParty::_onGetItem(CUser* user, unsigned int itemIdx, unsigned int count)
{
    CItem* item = G_CDataManager()->find_item((int)itemIdx);   // -0x14
    if (item == 0)
        return;

    if (((EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng())
            ->is_eventing(0x193))
        ((EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng())
            ->process_add_item_count_db(user, (int)itemIdx, (int)count);

    if (item->is_stackable())
    {
        // ORIG 0x085b951f：`mov -0x14,%eax; mov %eax,-0x10(%ebp)` 第二具名指针
        // 槽（-0x10），后续 vtable+0xc 虚调用经 p（call *%edx 槽形，N7 残差另计）；
        // boxKind 判定为 switch 线性降序树（0x289749→2 / 0x28982d→1 / 0x380→0，
        // 入口 `movl $0,-0xc` 单初始化 + 各 case 单 mov+jmp 合并，0x085b9541..）。
        CItem* p = item;
        if (p->GetItemType() == 0x1a)                // vtable 槽 +0xc
        {
            int boxKind = 0;                            // -0xc
            switch (itemIdx)
            {
            // ORIG body 内存序 0x380@0x85b9560 → 0x28982d@0x85b9569 →
            // 0x289749@0x85b9572（值升序、cmp 降序线性探）⇒ case 按升序书写。
            case 0x380:    boxKind = 0; break;
            case 0x28982d: boxKind = 1; break;
            case 0x289749: boxKind = 2; break;
            default: break;
            }
            CUser::SendRandomBoxStatistic(boxKind, RANDOMBOX_ACTION_CREATE);
        }
    }
}

// ============================================================================
// CParty::choose_random_buff —— ORIG 0x0859b814（返回选中类型或 11）
// ============================================================================
int CParty::choose_random_buff(QuickParty::RandomBuffDungeon dungeon)
{
    if (!is_quick_party())
        return 11;
    if ((int)dungeon > 6)
        return 11;

    int totalProb =
        G_CGameManager()->GetQuickPartyRewardManager()
            ->get_random_buff_prob((QuickParty::RandomBuffType)11,
                                   dungeon);            // 行 11=汇总行
    if (totalProb == 0)
        return 11;

    int roll = get_rand_int(totalProb);                 // -0x14
    int acc = 0;                                        // -0x10
    for (int type = 0; type <= 10; ++type)              // -0xc
    {
        // ORIG 循环体内每次重新取管理器（无中间存储的链式调用）
        acc += G_CGameManager()->GetQuickPartyRewardManager()
                   ->get_random_buff_prob((QuickParty::RandomBuffType)type,
                                          dungeon);
        if (roll <= acc)
        {
            // 【二次核验】ORIG 0x0859b814 域首存基址先载(mov 0x8,%eax;-0xc,%edx)，
            // 4.4 调度恒 RHS 先——this->显式/指针中转两 probe 均不改或毁槽序，
            // 维持直存（1 行 NEAR 封顶）。
            m_field1ae8 = type;                         // +0x1ae8
            return m_field1ae8;
        }
    }
    return 11;
}

// ============================================================================
// CParty::checkInoutConditionDungeon —— ORIG 0x085abc80（三参重载，单出口）
// 返回错误码（0=通过）；错误路径同步向全队发包。
// ============================================================================
int CParty::checkInoutConditionDungeon(CDungeon const* dungeon,
                                       ENUM_DUNGEON_MODE mode,
                                       ENUM_CMDPACKET cmd)
{
    int ret;                                            // ebx（单出口寄存）
    if (dungeon == 0)
        return 1;

    PacketGuard packet;                                 // -0x28
    unsigned char ancientFlag = 0;                      // -0x29
    Inven_Item ancientItems[4];                         // -0x140（元素步长 0x3d）
    memset(ancientItems, 0, 0xf4);                      // ORIG ctor 后整体清零
    int itemParams[4];                                  // -0x3c
    memset(itemParams, 0, sizeof(itemParams));

    if (cmd == (ENUM_CMDPACKET)0x1bc)
    {
        int discard;                                    // 结果弃置（ORIG 同形死码测试）
        for (int i = 0; i <= 3; ++i)                    // -0x18
            discard = !checkValidUser(i);
        (void)discard;

        for (int j = 0; j <= 3; ++j)                    // -0x14
        {
            if (!checkValidUser(j))
                continue;
            // 疲劳 != 'd'(100) → 广播 0xea 错误
            if (m_member[j].m_pUser->getCurCharacStamina() != 100)
            {
                packet.clear();
                packet.put_header(1, 0x1bc);
                packet.put_byte(0);
                packet.put_byte(0xea);
                packet.put_byte((int)ancientFlag);
                packet.finalize(true);
                send_to_party(packet);
                ret = 0xea;
                goto DONE;
            }
        }
    }

    if (!CheckEnterAncientDungeon(dungeon, ancientFlag, ancientItems, itemParams))
    {
        packet.clear();
        packet.put_header(1, (int)cmd);
        packet.put_byte(0);
        packet.put_byte(0x11);
        packet.put_byte((int)ancientFlag);
        packet.finalize(true);
        send_to_party(packet);
        ret = 0x11;
        goto DONE;
    }

    if (dungeon->get_index() != 0)
    {
        if (!UseAncientDungeonItems(dungeon, ancientItems, itemParams))
        {
            PacketGuard failPacket;                     // -0x4c
            failPacket.put_header(1, 0x10);
            failPacket.put_byte(0);
            failPacket.put_int(1);
            failPacket.finalize(true);
            send_to_party(failPacket);
            ret = 0x11;
            goto DONE;
        }
    }

    if ((signed char)dungeon->get_dimension_possible() > 0)
    {
        ancientFlag = 0;
        int dimErr = CheckEnterDimensionDungeon(dungeon, ancientFlag, true);  // -0x10
        if (dimErr == 0xac)
        {
            packet.clear();
            packet.put_header(1, (int)cmd);
            packet.put_byte(0);
            packet.put_byte(0xac);
            packet.put_byte((int)ancientFlag);
            packet.finalize(true);
            send_to_party(packet);
            ret = 0xac;
            goto DONE;
        }
        if (dimErr == 0xad)
        {
            packet.clear();
            packet.put_header(1, (int)cmd);
            packet.put_byte(0);
            packet.put_byte(0xad);
            packet.put_byte((int)ancientFlag);
            packet.finalize(true);
            send_to_party(packet);
            ret = 0xad;
            goto DONE;
        }
        if (dimErr == 0xab)
        {
            packet.clear();
            packet.put_header(1, (int)cmd);
            packet.put_byte(0);
            packet.put_byte(0xab);
            packet.finalize(true);                      // 不带 flag 字节
            send_to_party(packet);
            ret = 0xab;
            goto DONE;
        }
        if (!setStandardDimensionLevel(dungeon))
        {
            packet.clear();
            packet.put_header(1, (int)cmd);
            packet.put_byte(0);
            packet.put_byte(0xab);
            packet.finalize(true);
            send_to_party(packet);
            ret = 0xab;
            goto DONE;
        }
    }

    {
        int startErr = 0;                               // -0x1c
        int startSlot = 0;                              // -0x40
        if (mode != (ENUM_DUNGEON_MODE)1)
        {
            startErr = check_dungeon_start(dungeon, startSlot);
            if (startErr != 0)
            {
                // 错误码分流（ORIG 比较树）：带槽 {10,0x5f,0xf5,0xf8}；
                // 不带槽 {0xf3..0xf4,0xf6..0xf7}；其余落入锦标赛兜底。
                if (startErr > 0xf4)
                {
                    if (startErr > 0xf7)
                    {
                        if (startErr == 0xf8)
                            goto EMIT_SLOT_ERR;
                        goto TOURNAMENT_CHECK;
                    }
                    if (startErr >= 0xf6)
                        goto EMIT_PLAIN_ERR;
                    goto EMIT_SLOT_ERR;                 // ==0xf5
                }
                if (startErr >= 0xf3)
                    goto EMIT_PLAIN_ERR;                // 0xf3/0xf4
                if (startErr == 10 || startErr == 0x5f)
                    goto EMIT_SLOT_ERR;
                goto TOURNAMENT_CHECK;

EMIT_SLOT_ERR:
                packet.clear();
                packet.put_header(1, (int)cmd);
                packet.put_byte(0);
                packet.put_byte(startErr);
                packet.put_byte(startSlot);
                packet.finalize(true);
                send_to_party(packet);
                ret = startErr;
                goto DONE;

EMIT_PLAIN_ERR:
                packet.clear();
                packet.put_header(1, (int)cmd);
                packet.put_byte(0);
                packet.put_byte(startErr);
                packet.finalize(true);
                send_to_party(packet);
                ret = startErr;
                goto DONE;
            }
        }
    }

TOURNAMENT_CHECK:
    // 锦标赛图（0x78/0x79）：人数≥2 或单参模式时报错日志并广播 0xf3
    if (dungeon->get_index() == 0x78 || dungeon->get_index() == 0x79)
    {
        if (get_member_count() >= 2 || mode == (ENUM_DUNGEON_MODE)1)
        {
            for (int t = 0; t <= 3; ++t)                // -0xc
            {
                if (!_checkValidUser(t))
                    continue;
                LogManager::logFormat(
                    1, "party.cpp", __PRETTY_FUNCTION__, 0x25eb,
                    "TOURNAMENT_DUNGEON_ERROR_LOG : CHARAC_NO (%d) MODE (%d)",
                    m_member[t].m_pUser->getCurCharacNo(), (int)mode);
            }
            packet.clear();
            packet.put_header(1, (int)cmd);
            packet.put_byte(0);
            packet.put_byte(0xf3);
            packet.finalize(true);
            send_to_party(packet);
            ret = 1;
            goto DONE;
        }
    }
    ret = 0;

DONE:
    return ret;
}

// ============================================================================
// CParty::giveup_game —— ORIG 0x085b2baa（单出口）
// 参数：user=放弃者 a2=允许惩罚 a3=按成功结算 a4=强制整队分支。
// ============================================================================
void CParty::giveup_game(CUser* user, bool a2, bool a3, bool a4)
{
    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_battleField);
    Secu_HackLogCheckByParty& secu =
        *reinterpret_cast<Secu_HackLogCheckByParty*>(&m_padSecu);

    bool specialDungeon = false;                        // -0x3d local_41
    CDungeon* dungeon = m_battleField.m_pDungeon;                      // -0x3c (+0xcac)
    if (dungeon != 0)
    {
        if ((char)dungeon->m_bloodDungeonType > 0)
            specialDungeon = true;
        if (dungeon->isTowerOfDespairDungeon())
            specialDungeon = true;
    }

    bool allowPenalty = a2;                             // -0x15c local_160
    if (dungeon != 0)
    {
        if ((signed char)dungeon->get_dimension_possible() > 0)
            allowPenalty = false;
        else if (dungeon->m_89c != 0)
            allowPenalty = false;
        else if (dungeon->isTournamentDungeon())
            allowPenalty = false;
    }
    if (dungeon != 0)
    {
        if (dungeon->m_entryRestrictFlag != 0)
            allowPenalty = false;
    }
    if (dungeon != 0)
    {
        if (dungeon->m_villageAttackEventFlag != 0)
            allowPenalty = false;
    }

    bool pvpMode = (m_battleField.m_bloodState == 1);                  // -0x35 (+0xcd8)

    user->reset_level_before_dungeon();

    bf.m_dungeonClearTracer.Trace(__PRETTY_FUNCTION__); // 战场 +0xc7c 追踪器

    Reset_party_overlapped_drop_ratio();

    int memberCountSnapshot = get_member_count();       // -0x34（ORIG 存后未读）
    (void)memberCountSnapshot;

    if (user == 0)
        goto AFTER_STATISTICS;
    if (user->getCurCharacR() == 0)
        goto AFTER_STATISTICS;
    if (pvpMode)
        goto AFTER_STATISTICS;
    Statistics::DungeonFail((int)user->getCurCharacR()->m_level);  // characR+0x27
AFTER_STATISTICS:

    for (int i = 0; i <= 3; ++i)                        // -0x24
    {
        if (!_checkValidUser(i))
            continue;
        HistoryLog::WriteGiveUp(
            reinterpret_cast<FILE*>(m_member[i].m_pUser->m_field796f8),
            user->getCurCharacName());
    }

    // 村庄怪物战进行中（bf+0x128≠0）或特殊图 idx==10000 → 取消惩罚资格
    if (bf.m_villageMonsterId != 0)
        allowPenalty = false;
    else if (dungeon != 0)
    {
        if (dungeon->get_index() == 10000)
            allowPenalty = false;
    }

    if (a3)
    {
        OnSuccessedDungeonClear(user);
    }
    else
    {
        if (get_member_count() == 1)
        {
            CheckHackAverageHitCount();
            secu.dungeonFail();                         // +0x1abc
        }
        OnFailedDungeonClear(user);
    }

    if (user != 0)
    {
        if (bf.check_start_point())
        {
            WongWork::CHackAnalyzer* hackAnalyzer = user->getHackAnalyzer();
            hackAnalyzer->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x325,
                                           1, 0, 0);
        }
    }

    SaveCoin((eCoinSubReason)3);

    char eplpState = GetEPLPState();                    // -0x2d local_31
    if (eplpState == 1)
    {
        CUser* manager = getManager();
        if (manager->getMoveSpace() != (UserSpace::T)1)
        {
            int dungeonIdx = dungeon->get_index();
            GetInstanceDungeonEntranceLog()->DecrementDungeonEntrance(dungeonIdx,
                                                                      false);
        }
    }

    PacketGuard packet;                                 // -0x4c
    if (!m_autoCreated)                                 // +0x13c
    {
        if (!a3)
            set_charac_live(user, (ENUM_USER_DUNGEON_STATE)0);

        char memberNames[0x100];                        // -0x14c
        memset(memberNames, 0, sizeof(memberNames));
        _getMemberNames(memberNames);

        bool groupBranch;                               // (EPLP命令!=3 && 全灭)||成功||a4
        if (GetSelectedEPLPCmd() != 3)
        {
            if (check_allmember_die())
                groupBranch = true;
            else if (a3)
                groupBranch = true;
            else if (a4)
                groupBranch = true;
            else
                groupBranch = false;
        }
        else if (a3)
            groupBranch = true;
        else if (a4)
            groupBranch = true;
        else
            groupBranch = false;

        if (groupBranch)
        {
            for (int k = 0; k <= 3; ++k)                // -0x20
            {
                if (!_checkValidUser(k))
                    continue;
                if (dungeon != 0)
                {
                    int partyType = m_battleField.m_bloodState;
                    char* names = _getMemberNames(memberNames);
                    int diff = bf.get_dungeon_diff();
                    const char* dunName = dungeon->GetDungeonName();
                    m_member[k].m_pUser->m_historyLog.LeaveDungeon(
                        dunName, diff, names, partyType);
                }
                if (bf.m_villageMonsterId != 0)
                {
                    if (m_member[k].m_pUser != user)
                    {
                        if (a3)
                            OnSuccessedDungeonClear(m_member[k].m_pUser);
                        else
                            OnFailedDungeonClear(m_member[k].m_pUser);
                    }
                }
                GameWorld* gameWorld = G_GameWorld();
                gameWorld->out_from_dungeon(m_member[k].m_pUser);
                if (!pvpMode)
                {
                    if (!specialDungeon)
                    {
                        if (eplpState == 1)
                        {
                            if (allowPenalty)
                                m_member[k].m_pUser->giveup_panalty();
                        }
                    }
                }
            }

            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(0);
            packet.put_short(get_member_count());
            for (int n = 0; n <= 3; ++n)                // -0x1c
            {
                if (!_checkValidUser(n))
                    continue;
                m_member[n].m_pUser->make_basic_info((char*)&packet, 0);
            }
            packet.finalize(true);
            if (!IsExistInvisible())
                G_GameWorld()->send_all(packet);
            else
                send_to_party(packet);

            set_state(1);
            battle_data_reset();
        }
        else
        {
            int leaveResult = leave_user(user, (ENUM_PARTY_INFO_TYPE)2);  // -0x28
            if (leaveResult != 1)
            {
                G_GameWorld()->out_from_dungeon(user);
                if (!pvpMode)
                {
                    if (!specialDungeon)
                    {
                        if (eplpState == 1)
                        {
                            if (allowPenalty)
                                user->giveup_panalty();
                        }
                    }
                }

                packet.clear();
                packet.put_header(0, 2);
                packet.put_byte(0);
                packet.put_short(1);
                user->make_basic_info((char*)&packet, 0);
                packet.finalize(true);
                G_GameWorld()->send_all(packet, user);  // 排除放弃者的全服广播
            }
            sendInoutConditionDungeon(
                (RetryFailReason::T)checkInoutConditionDungeon());
        }
    }
    else                                                // 自动创建队伍：单人收尾
    {
        leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
        G_GameWorld()->out_from_dungeon(user);
        if (!pvpMode)
        {
            if (!specialDungeon)
            {
                if (eplpState == 1)
                {
                    if (allowPenalty)
                        user->giveup_panalty();
                }
            }
        }

        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(0);
        packet.put_short(1);
        user->make_basic_info((char*)&packet, 0);
        packet.finalize(true);
        if (getManager()->getMoveSpace() == (UserSpace::T)1)
            send_to_party(packet);
        else
            G_GameWorld()->send_all(packet, user);
    }

    CSpecialItemRoutingManager* routingMgr =
        G_CGameManager()->GetSpecialItemRoutingManager();
    if (routingMgr != 0)
        routingMgr->ProcessGiveUp(this);
}
