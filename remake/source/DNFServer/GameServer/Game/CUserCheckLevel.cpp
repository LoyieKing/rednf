// ============================================================================
// CUser::_check_level_up — 独占还原 TU（未登记 CMake，由宿主统一登记）
//
// ORIG: _ZN5CUser15_check_level_upEiRiS0_13eExpAddReasoni @ 0x08662aea
//       [0x08662aea, 0x08663119)
// 报告: docs/class_func_reports/CUser/_check_level_up.md（## 汇编 + ## 反编译 C
//       已完整核对；本文件逐段映射见 /tmp/df_batches/CheckLevelUp_only.md）
//
// 依赖符号均经 ORIG nm 实证存在：
//   _ZNK15CUserCharacInfo16get_charac_levelEv(080da2b8) getCurCharacR(08120432)
//   _ZNK12CDataManager13get_level_expEi(08360442) 14GetSpAtLevelUpEi(08360cb8)
//   _ZN15CUserCharacInfo15setCurCharacExpEi(0819a87c) addCurCharacExp(086967be)
//   incCurCharacLevel(08696762) getCurCharacInvenW(080da28e)
//   _ZNK15CUserCharacInfo15getCurCharacNameEv(08101028) getCurCharacExp(08151abe)
//   _ZN5CUser19GetTutorialSkipableEv(084ecad4) UpdateTutorialSkipable(08697318)
//   _ZN10HistoryLog12WriteLevelUpEP8_IO_FILEi(084b9e5e)
//   _ZNK8WongWork12CUserPremium12GetGoldBonusEi(08694a64)
//   RecalcAdditionalInfoEPK5CUser(086ae8c6)
//   _ZN10CInventory10gain_moneyEi15eMoneyAddReasonbi(084ff29c)
//   SendUpdateItemListENS_11eSendTargetE14ENUM_ITEMSPACEi(0867c65a)
//   _ZN8XNuclear6CHades5ExpUpEj(084b953e) CUser8getHadesEv(08230800)
//   _ZN15cUserHistoryLog7LevelUpEi14eLevelUpReason(086845b2)
//   DimensionInoutUpdateEbb(08656c12) CGMAccounts4isGMEj(08109346)
//   Packet_Monitor_Max_Level_BroadCastC1Ev(08694560)
//   CMonitorServerProxy10SendPacketEPci(08470df4)
//   GameWorld30getDungeonMinimumRequiredLevelEi(086c9076)
//   send_user_dungeon_inout_messageEP5CUserib(086c8fc8)
//   ServerParameterScript13isDungeonOpenEi(082687fc)
//   makeGuildLevelUpMessageEi(08679754) _update_charac_statEb(086646c8)
//   _ZNK5CUser14GetServerGroupEv(080cbc90) get_acc_id(080da36e)
//   G_CDataManager(080cc19b) G_GameWorld(080da3a7)
// ============================================================================
#include "CUser.h"
#include "CDataManager.h"
#include "GlobalData.h"                          // s_pGMAccounts_ / s_monitor_proxy_mgr
#include "CGuildServerProxy.h"                   // WongWork::CGMAccounts::isGM 权威声明
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"                     // GetServerProxy 模板完整定义
#include "CInventory.h"                         // gain_money（getCurCharacInvenW 完整类型）
#include "GameWorld.h"
#include "HistoryLog.h"                          // HistoryLog::WriteLevelUp(FILE*, int)

#include <stdio.h>
#include "ReverseEngineerLib.h"                  // TEST_CLASS_SIZE 宏
#include "Packet_Monitor_Max_Level_BroadCast.h"  // 0x177e/0x28 包（ORIG W ctor 0x8694560）

#include <string.h>

// bool CUser::_check_level_up(int exp, int& sp, int& sfp, eExpAddReason, int)
//
// ORIG 参数语义（报告反编译 param_2..param_5）：
//   expAdd    — 本次待结算经验增量；
//   sp/sfp    — 升级时按 GetSpAtLevelUp(next)/>49 返还的输出引用；
//   reason    — eExpAddReason，函数体未读取（调用约定占位）；
//   hadesFlag — 非 0 时向 XNuclear::CHades::ExpUp 上报经验。
// [b28d-54 N] ORIG 0x08662aea：大件帧/槽平移（N82）+ 跨版本短整扩展/寄存器角色
//   （N97/N7）；控制流与调用序列全等（406 vs 408）。
bool CUser::_check_level_up(int expAdd, int& sp, int& sfp,
                            CUser::eExpAddReason reason, int hadesFlag)
{
    (void)reason;   // ORIG 未读取该参数

    bool leveledUp = false;                                   // -0x21 标志位
    int nextLevel = get_charac_level() + 1;                   // -0x30 目标级

    if (getCurCharacR() == 0)                                 // +0x02c 空角色守卫
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x401b, 5);
        trace("CUser::_check_level_up m_selected is NULL");
        return false;
    }
    if (get_charac_level() == 0)                              // +0x08d 零级守卫
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x4022, 5);
        trace("CUser::_check_level_up m_selected->m_level is 0");
        return false;
    }

    for (;;)                                                  // 循环头 +0x0cd
    {
        // 经验区间：bottom=当前级经验下界，top=目标级经验上界。
        int bottom = G_CDataManager()->get_level_exp(get_charac_level());  // -0x2c
        int top = G_CDataManager()->get_level_exp(nextLevel);              // -0x28

        // ORIG：区间不一致条件对 get_charac_exp 各重调一次，trace 内再次重调
        // （共 3 次，缓存违反 P183/P48）。
        if (get_charac_exp() < bottom || get_charac_exp() > top)   // +0x110 区间不一致
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x403b, 5);
            trace("%s is Level(%d) and Exp(%d) Inconsistency(bottom(%d), top(%d))",
                  getCurCharacName(), get_charac_level(), get_charac_exp(), bottom, top);
            setCurCharacExp(bottom);                          // +0x1bd 钳回下界
        }

        if (top - bottom <= 0)                                // +0x1ca 表异常出口
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x4046, 5);
            trace("CUser::_check_level_up, LEVEL:%d TOP:%d BOTTOM:%d",
                  nextLevel, top, bottom);
            return false;
        }

        if (get_charac_exp() + expAdd < top)                  // +0x232 未达升级线
        {
            addCurCharacExp(expAdd);                          // +0x5d4
            if (hadesFlag != 0)                               // +0x5dd
                ((XNuclear::CHades*)getHades())->ExpUp((unsigned int)expAdd);
            expAdd = 0;                                       // +0x5f9
        }
        else                                                  // 升级路径 +0x243
        {
            sp += (int)G_CDataManager()->GetSpAtLevelUp(nextLevel);   // SP 返还
            if (nextLevel > 0x31)                             // >49 级 SFP+1
                sfp += 1;

            int overflow = top - get_charac_exp();            // -0x20 溢出经验
            ++nextLevel;                                      // -0x30++
            incCurCharacLevel();                              // +0x298
            expAdd += get_charac_exp() - top;                 // 剩余增量回写

            // >=10 级且未跳过新手引导 → 置跳过标记。
            if (get_charac_level() >= 10 && GetTutorialSkipable() == 0)
                UpdateTutorialSkipable();                     // +0x3b6d

            setCurCharacExp(top);                             // +0x2f5 经验写回 top
            _update_charac_stat(true);                        // 重算角色属性
            leveledUp = true;                                 // +0x30d

            // 升级日志（FILE* 句柄存于 m_field796f8，int 位宽与 ORIG 一致）。
            HistoryLog::WriteLevelUp(reinterpret_cast<FILE*>(m_field796f8),
                                     getCurCharacExp());

            // 金币加成：GetGoldBonus(level)!=0 时入账并刷新物品列表。
            // （get_charac_level 在条件与入账各重调一次，禁缓存）
            if (m_premium.GetGoldBonus(get_charac_level()) != 0)   // +0x358 判定
            {
                CInventory* inven = getCurCharacInvenW();     // +0x382
                inven->gain_money(m_premium.GetGoldBonus(get_charac_level()),  // +0x3a6
                                  static_cast<eMoneyAddReason>(0xd), true, 0);
                SendUpdateItemList(eSendTarget_1, ENUM_ITEMSPACE_0, 0);
            }
            m_premium.RecalcAdditionalInfo(this);             // +0x3e1

            if (hadesFlag != 0)                               // +0x3e6（上报溢出量）
                ((XNuclear::CHades*)getHades())->ExpUp((unsigned int)overflow);

            m_historyLog.LevelUp(get_charac_level(), eLevelUpReason_0);

            if (get_charac_level() > 0x3b)                    // >59 级维度进出刷新
                DimensionInoutUpdate(true, true);

            // 满 85 级且非 GM → 向 monitor 广播满级公告（包长 0x28）。
            if (get_charac_level() == 0x55 &&
                !GlobalData::s_pGMAccounts_->isGM(get_acc_id()))
            {
                Packet_Monitor_Max_Level_BroadCast pkt;       // +0x4ab ctor
                strncpy(pkt.m_name, getCurCharacName(), 0x1e);  // 名字 @+0xa
                CMonitorServerProxy* proxy =
                    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                        GetServerGroup());
                proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0x28);
            }

            // 达到地下城最低进图等级 → 按 ServerParameter 开关广播进出消息。
            if (get_charac_level() ==
                G_GameWorld()->getDungeonMinimumRequiredLevel(0x2aff))
            {
                bool open =
                    G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff);
                // ORIG：open 真假两分支各自重调 G_GameWorld 并独立
                // send_user_dungeon_inout_message（禁缓存，P183/P48）。
                if (open)
                    G_GameWorld()->send_user_dungeon_inout_message(this, 0x2aff,
                                                                   true);
                else
                    G_GameWorld()->send_user_dungeon_inout_message(this, 0x2aff,
                                                                   false);
            }

            makeGuildLevelUpMessage(get_charac_level());      // 公会升级公告
            _onLevelUp();                                     // ORIG T 0x866311a
        }

        // 循环尾判定（+0x600）：仍有剩余经验且下一级 <=85 才继续。
        if (!(expAdd > 0 && nextLevel <= 0x55))
            break;
    }
    return leveledUp;
}
