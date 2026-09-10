// ============================================================================
// CPartyDepsC2.cpp —— CParty 第三组独占恢复（2026-08-24，PartyDepsC2）
//
// 覆盖 11 个 ORIG 符号（全部经 docs/class_func_reports/CParty/ 对应报告
// ## 汇编 + ## 反编译 C 完整读取后转写；未命名 thunk 均以 nm 定位为真实 W 符号）：
//   increase_ancient_dungeon_ticket  T 0x0859bc9a（increase_ancient_dungeon_ticket.md）
//   isDungeonDifficultyInfromNotice  T 0x085be8a0（isDungeonDifficultyInfromNotice.md）
//   isGuildParty                     T 0x085b49d0（isGuildParty.md）
//   isPremiumGoldCardReward          T 0x085b11ee（isPremiumGoldCardReward.md）
//   make_blood_dungeon_info          T 0x0859e0d2（make_blood_dungeon_info.md）
//   make_blood_dungeon_info_blank    T 0x0859e480（make_blood_dungeon_info_blank.md）
//   move_map                         T 0x085a8f4e（move_map.md）
//   send_loadingcheck_msg            T 0x085b1fe0（send_loadingcheck_msg.md）
//   set_random_reward                T 0x0859b8fa（set_random_reward.md）
//   useItem                          T 0x085a7800（useItem.md）
//   use_skill                        T 0x085a77d8（use_skill.md）
// ============================================================================

#include <string.h>
#include <vector>

#include "CParty.h"
                 // CGameManager.h 先于 CDungeon.h：其 QuickParty.h 完整定义
                 // 需先于 CDungeon.h 的 CQuickPartySystemManager 兜底声明
#include "CGameManager.h"
#include "PacketGuard.h"
#include "TimerQueue.h"
#include "Secu_HackLogCheckByParty.h"
#include "PacketTypes.h"         // MSG_MOVE_MAP 完整类型
#include "CCommonStatisticsMgr.h" // GetInstanceCommonStatisticsMgr/IncreaseQuantity
#include "CDungeon.h"
#include "CDataManager.h"
#include "CInventory.h"         // insertItemIntoInventory 完整类型
#include "CWorldMap.h"
#include "GameWorld.h"
#include "CBloodDungeonRewardFirstEvent.h"   // GetGold W 0x085bff1a（真实类，本批次闭环）
#include "CBloodDungeonRewardSecondEvent.h"  // GetGold W 0x085bff26
 #include "CGuildServerProxy.h"   // CEventManager / CEventBase（IsEventing vptr+0x34 槽13）
 #include "GlobalData.h"
 #include "CMailBoxHelper.h"
 #include "RDARScriptStringManager.h"
 #include "CBattle_Field.h"
#include "W9_EventSmall.h"       // CCharacBloodDungeon（GetEnterCount/GetWeeklyEnterCount）

// [b28d-59] ORIG 0x085b2002/0x085b2009 以内存读引用 _ZL37CONST_SINGLE_MAP_LOADING_
// WAIT_TIMEOUT / _ZL36CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT（非立即数）⇒
// static volatile const int 阻止 4.4 常量折叠（同 CDeathTower.cpp 0x8467122 先例）。
static volatile const int CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT = 0x78;
static volatile const int CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT = 0x5a;

// ---------------------------------------------------------------------------
// increase_ancient_dungeon_ticket —— ORIG T 0x0859bc9a
// 报告：docs/class_func_reports/CParty/increase_ancient_dungeon_ticket.md
// ---------------------------------------------------------------------------
void CParty::increase_ancient_dungeon_ticket()
{
    // [b28d-solo-CPartyDepsC2 判词] 封 N-lowering（ORIG 0x0859bc9a）：p167 19=19
    // CLEAR；槽位（-0x1c/-0x18/-0x14/-0x10/-0xc/-0x6e/-0x31/-0x16e）、find_item
    // ebx 装载时刻（`*&itemId`）、尾岛数（2 个）已逐条对齐；残差仅 3 条比较
    // lowering（探针 /tmp/b28d_c2_probe/p7.cpp 穷举）：
    //   ① m_89c（组合位 uchar，全仓 10+ 消费点用 !=0，不可 bool 化）`!=1`：
    //      ORIG `xor$1;test%al` vs 4.4 恒 `cmp$1,%al`（g0..g4 五形全 cmp）；
    //   ② slot<0：ORIG `mov;shr$0x1f;test%al;je` vs 4.4 恒 `cmpl$0;jns`
    //      （f0..f5 六形全 cmpl）。
    // 尾部 nop 对齐填充豁免（N112）。语义等价。
    if (m_battleField.m_pDungeon == 0)
        return;
    // movzbl 0x89c：要求副本 +0x89c 字节 == 1 才继续。ORIG 0x0859bcc6 为
    // `xor $0x1,%eax; test %al,%al; jne`（4.1 对 `x != 1` 的取反物化；
    // m_89c 为组合位 uchar 不可 bool 化）⇒ 显式 xor 形（语义等价）。
    if ((m_battleField.m_pDungeon->m_89c ^ 1) != 0)
        return;

    // dungeon+0x7f4 = RequiredItem.m_0（道具 idx）；+0x7f8 = RequiredItem.m_4
    const int itemId = m_battleField.m_pDungeon->m_entryRequiredItem.m_itemIdx;
    const int createParam = m_battleField.m_pDungeon->m_entryRequiredItem.m_itemCount;

    // find_item 结果存槽 local，比较用 cmpl $0（ORIG -0x14）；G_CDataManager()
    // 每次内联重取（P0：ORIG 无 dataMgr 局部，寄存器直传）。
    // ORIG 0x0859bcf5 `mov -0x1c(%ebp),%ebx` 先装载实参再 call G_CDataManager
    // ⇒ `*&` 形（P226 姊妹，探针 p8 p4 逐条全等）令 4.4 提前升格 callee-saved。
    CItem* found = G_CDataManager()->find_item(*&itemId);  // 0x0835fa32
    // ORIG find_item null 分支 `je 0x0859bfd8` 直落主出口（无 nop 中继岛）
    // ⇒ 正向包裹（P285）。
    if (found != 0)
    {
        Inven_Item item;                    // Inven_Item::Inven_Item W 0x080cb854
        item.m_addInfo = itemId;            // uStack_70/uStack_6e 两 short 写 = +0x02 int
        G_CDataManager()->m_itemList->create_item(  // CItemList::create_item T 0x08511fb8
            (ENUM_ITEM_CREATE_TYPE)0, item, createParam);

        // create 后有效性判定：m_addInfo(+0x02) 与 get_add_info()(+0x07) 任一非零
        // ORIG：bVar1 在寄存器物化（mov$1/jmp/mov$0 后 test%al），条件为
        // (m_addInfo != 0) || (get_add_info() != 0)
        if (item.m_addInfo != 0 || item.get_add_info() != 0)
        {
            for (int i = 0; i < 4; ++i)     // cmpl $3/setle：i<=3 共 4 槽
            {
                if (!checkValidUser(i))     // checkValidUser(int) W 0x08145868
                    continue;
                int slot = -1;              // movl $0xffffffff,-0xc
                // insertItemIntoInventory(Inven_Item, eItemAddReason, bool, bool)
                // 实参 (item, 0x38, 1, 1)；返回槽号符号扩展判负
                slot = m_member[i].m_pUser->getCurCharacInvenW()
                           ->insertItemIntoInventory(
                               item, (eItemAddReason)0x38, true, true);
                // ORIG 0x0859be5a `mov -0xc(%ebp),%eax; shr $0x1f,%eax; test
                // %al,%al; je 成功`（4.4 无此 lowering，探针 p7 六形全败）。
                if (((unsigned int)slot >> 31) != 0)
                {
                    // 入包失败 → 系统邮件补发门票后重置栈上物品
                    char title[0x15];       // local_35[21]
                    strncpy(title,
                            g_scriptStringManager_.findString(
                                4, "game_server_msg_53", 0),
                            0x14);
                    char body[0x100];       // local_172[256]
                    strncpy(body,
                            g_scriptStringManager_.findString(
                                4, "game_server_msg_154", 0),
                            0xff);
                    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                        title, item, 0,
                        (unsigned int)m_member[i].m_pUser->getCurCharacNo(),
                        body, (int)strlen(body), 0,
                        m_member[i].m_pUser->GetServerGroup(), false, false);
                    item.reset();           // Inven_Item::reset W 0x080cb7d8
                }
                else
                {
                    m_member[i].m_pUser->SendUpdateItem((CUser::eSendTarget)1,
                                                        (ENUM_ITEMSPACE)0, slot);
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// isDungeonDifficultyInfromNotice —— ORIG T 0x085be8a0
// 报告：docs/class_func_reports/CParty/isDungeonDifficultyInfromNotice.md
// ---------------------------------------------------------------------------
bool CParty::isDungeonDifficultyInfromNotice(bool flag)
{
    // [b28d-59 判词] 封 N（P292/P248档① 跨 TU bool 链，ORIG 0x085be8a0）：49/47 条，
    // 残差首条=ORIG `call getMemberDungeonFlag; test %al; je`（bool callee）vs 我方
    // `setne %al`（CUserCharacInfo.h:923 声明 unsigned char）。整链同改需动
    // CUserCharacInfo.h/.cpp（非本泳道 TU）⇒ 记 N 待属主同步；其余差异同源。
    // this+0x74 = m_manager（CUserCharacInfo 基类方法直呼）
    if (m_manager->getMemberDungeonFlag())           // W 0x085bfaec（bool 直判：ORIG test;je 单物化）
        return false;
    if (m_manager->getCurCharacTutorialFlag() == 0)  // W 0x085bfb44：新手教程
        return false;                                // 未完成（==0）不提示
    if (flag)                                // param_1 为真直接不提示
        return false;
    // this+0xc4c = CBattle_Field(+0xb24)+0x128 = m_villageMonsterId(short)。
    // 直接成员访问，编译器以 this 基址直寻 0xc4c（ORIG `movzwl 0xc4c(%eax)`）。
    if (m_battleField.m_villageMonsterId != 0)
        return false;
    if (getManager()->getMoveSpace() == (UserSpace::T)1)   // getMoveSpace 0x08690eca
        return false;
    return true;
}

// ---------------------------------------------------------------------------
// isGuildParty —— ORIG T 0x085b49d0
// 报告：docs/class_func_reports/CParty/isGuildParty.md
// ---------------------------------------------------------------------------
bool CParty::isGuildParty()
{
    // [b28d-59] AE（ORIG 0x085b49d0）：两臂 result=true 块布局（P47）修正后 52/52
    // 指令序列逐条一致（.o 仅 call 重定位显示差）。
    bool result = false;     // local_19
    // ORIG 槽序（-0x15 result / -0x14 key / -0x10 guildKey / -0xc i）⇒ 声明序
    // result→key→guildKey→i（P28），key 为循环外具名局部、循环体内赋值。
    int key;                 // -0x14
    int guildKey = -1;       // -0x10
    for (int i = 0; i <= 3; ++i)   // -0xc
    {
        if (!_checkValidUser(i))            // _ZN6CParty15_checkValidUserEi 0x85b4d12
            continue;
        key = m_member[i].m_pUser->get_charac_guildkey();  // W 0x0822f46c
        if (key == 0)
            return false;
        // [b28d-59] ORIG 0x085b4a47/0x085b4a4d 两处 movb $1,-0x15（P47 块布局）：
        // key!=guildKey 时先判 guildKey==-1，两臂各写 result=true。
        if (key != guildKey)
        {
            if (guildKey != -1)
                return false;
            result = true;
        }
        else
        {
            result = true;
        }
        guildKey = key;
    }
    return result;
}

// ---------------------------------------------------------------------------
// isPremiumGoldCardReward —— ORIG T 0x085b11ee
// 报告：docs/class_func_reports/CParty/isPremiumGoldCardReward.md
//   数据经 m_serverParameter(+0x68) m_premiumGoldCard4894(+0x4894) m_conds[i]
//   （+0x08 起，步长 0x10）：count=m_conditionCount（short @+0x04，0x4900），
//   元素 min=max=prob=reward 与原 ORIG 平铺 int 偏移 0x4904/+0x4910 一致。
// ---------------------------------------------------------------------------
bool CParty::isPremiumGoldCardReward(CDungeon const* dungeon, unsigned int& param)
{
    // [b28d-59 判词] 封 N82（ORIG 0x085b11ee）：83/80 条，残差=帧内槽序 +
    // 条件极性（jle/jg 落点相同）。语义等价。
    // ORIG 0x085b11ee：dungeon->get_standard_level() 结果存槽（local -0x28）；
    // G_CDataManager() 每次取字段时重新调用（-O0 无 CSE，ORIG 每轮 loop 4 次重取），
    // 无命名 mgr/ev 局部（P0 槽律）。字段经 m_serverParameter(+0x68)
    // m_premiumGoldCard4894(+0x4894) m_conds[i]（+0x08 起，步长 0x10）。
    const int standardLevel = dungeon->get_standard_level();   // W 0x080f9810

    // movzwl 0x4900 + cwtl：count = m_conditionCount（short @ +0x04）扩为 int 上限
    const int condCount =
        (int)G_CDataManager()->m_serverParameter.m_premiumGoldCard4894.m_conditionCount;

    for (int i = 0; i < condCount; ++i)
    {
        const int minLevel =
            G_CDataManager()->m_serverParameter.m_premiumGoldCard4894.m_conds[i].m_field0;
        const int maxLevel =
            G_CDataManager()->m_serverParameter.m_premiumGoldCard4894.m_conds[i].m_field4;
        const int prob =
            G_CDataManager()->m_serverParameter.m_premiumGoldCard4894.m_conds[i].m_field8;
        const unsigned int rewardItem = (unsigned int)
            G_CDataManager()->m_serverParameter.m_premiumGoldCard4894.m_conds[i].m_fieldc;

        // [b28d-solo-CPartyDepsC2] ORIG 0x085b128e 域：三 guard 各 `jle` 跳尾部
        // nop 中继岛（共享尾 return 0 块），循环正常结束再 `jmp` 尾块。早返形 4.4
        // 恒反转成 `jg 续; mov $0; jmp`（P222）⇒ 改负条件 goto 共享尾标签（P223 定案形）。
        if (minLevel < 1)
            goto FAIL;
        if (maxLevel < 1)
            goto FAIL;
        if (prob < 1)
            goto FAIL;
        if (standardLevel >= minLevel && standardLevel <= maxLevel)
        {
            const int dice = get_rand_int(1000);   // get_rand_int T 0x086b1b87
            if (dice <= prob)
            {
                param = rewardItem;
                return true;
            }
        }
    }
FAIL:
    return false;
}

// ---------------------------------------------------------------------------
// make_blood_dungeon_info —— ORIG T 0x0859e0d2
// 报告：docs/class_func_reports/CParty/make_blood_dungeon_info.md
// ---------------------------------------------------------------------------
bool CParty::make_blood_dungeon_info(CUser* user, PacketGuard& packet)
{
    // [b28d-solo-CPartyDepsC2] P167 修复（ORIG 0x0859e0d2）：原 5 处调用点缺口
    // （GetDungeonList×2 / GetRepeatEvent×2 / GetCharacExpandData×1 /
    //  GetEnterCount×1 / GetWeeklyEnterCount×1）系局部缓存与裸字段视图所致，
    // 按 P183 去缓存 + 真实 getter 调用（ORIG 逐点重调）。
    if (m_hellWorldMap == 0)
    {
        // G_GameWorld W 0x080da3a7；GetWorldMapIndex T 0x086cebe0
        int worldIdx = G_GameWorld()->GetWorldMapIndex(user);
        if (worldIdx > 0)
            m_hellWorldMap =   // CWorldMapList::find_world_map T 0x083647a2
                G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
    }
    if (m_hellWorldMap == 0)
        return false;
    if (!m_hellWorldMap->hasDeathTower())      // hasDeathTower W 0x0822b520
        return false;

    packet.put_byte(get_member_count());

    for (int i = 0; i <= 3; ++i)
    {
        // ORIG 0x0859e197 `test %al,%al; je ++i` = 正向包裹（P59 反形）；
        // P183：无 memberUser 局部，逐点重算 m_member[i].m_pUser。
        if (_checkValidUser(i))
        {
            // put_short W 0x080d9ea4；uid 取低 16 位
            packet.put_short(m_member[i].m_pUser->get_unique_id() & 0xffff);

            // 计数占位字节：get_index W 0x08110b4c / put_byte W 0x080cb920 /
            // put_byte(int&,int) W 0x08110b28；ORIG 0x0859e1e8 压的是
            // dungeonCount 槽（=0）而非立即数。
            int countIdx = packet.get_index();
            int dungeonCount = 0;
            packet.put_byte(dungeonCount);

            // CWorldMap::GetDungeonList W 0x085beda4 = &m_dungeonList(+0x10)：
            // ORIG 0x0859e1f7（begin 段）与 0x0859e344（每轮 end 段）各自独立调用。
            for (std::vector<int>::iterator it =
                     m_hellWorldMap->GetDungeonList().begin();
                 it != m_hellWorldMap->GetDungeonList().end(); ++it)
            {
                CDungeon* dungeon = G_CDataManager()->find_dungeon(*it);  // 0x0835f9f8
                // ORIG 0x0859e244/0x0859e256 两跳同落 ++it（正向包裹）。
                if (dungeon != 0 && (char)dungeon->m_bloodDungeonType == 1)
                {
                    packet.put_short(*it);
                    packet.put_int(get_blood_dungeon_admission_fee(
                        m_member[i].m_pUser));
                    // ORIG 0x0859e2c4/0x0859e302 两次独立 GetCharacExpandData(1) +
                    // 真实 getter（GetEnterCount 0x085bfe88 / Weekly 0x085bfe94）。
                    packet.put_int(reinterpret_cast<CCharacBloodDungeon*>(
                                       m_member[i].m_pUser->GetCharacExpandData(
                                           ENUM_CHARAC_EXPAND_TYPE_1))
                                       ->GetEnterCount());
                    packet.put_int(reinterpret_cast<CCharacBloodDungeon*>(
                                       m_member[i].m_pUser->GetCharacExpandData(
                                           ENUM_CHARAC_EXPAND_TYPE_1))
                                       ->GetWeeklyEnterCount());
                    ++dungeonCount;
                }
            }
            packet.put_byte(countIdx, dungeonCount);
        }
    }

    // 金币奖励：repeat 0x2e(SecondEvent) → 0x2d(FirstEvent) → Jackpot
    // IsEventing 经 vptr+0x34（槽13）虚调用，实参 0；ORIG 每次使用点独立重调
    // GetRepeatEvent（0x0859e3b1/0x0859e3de 与 0x0859e40c/0x0859e439）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x2e)->IsEventing(0))
    {
        packet.put_int(static_cast<CBloodDungeonRewardSecondEvent*>(
                           GlobalData::s_event_manager->GetRepeatEvent(0x2e))
                           ->GetGold());
    }
    else
    {
        if (GlobalData::s_event_manager->GetRepeatEvent(0x2d)->IsEventing(0))
        {
            packet.put_int(static_cast<CBloodDungeonRewardFirstEvent*>(
                               GlobalData::s_event_manager->GetRepeatEvent(0x2d))
                               ->GetGold());
        }
        else
        {
            packet.put_int(G_CDataManager()->m_serverParameter.GetJackpot());
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// make_blood_dungeon_info_blank —— ORIG T 0x0859e480
// 报告：docs/class_func_reports/CParty/make_blood_dungeon_info_blank.md
// ---------------------------------------------------------------------------
void CParty::make_blood_dungeon_info_blank(PacketGuard& packet)
{
    packet.put_byte(get_member_count());
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            packet.put_short(m_member[i].m_pUser->get_unique_id() & 0xffff);
            packet.put_byte(0);
        }
    }
    packet.put_int(0);
}

// ---------------------------------------------------------------------------
// move_map —— ORIG T 0x085a8f4e
// 报告：docs/class_func_reports/CParty/move_map.md
// ---------------------------------------------------------------------------
bool CParty::move_map(MSG_MOVE_MAP& info)
{
    // [b28d-solo-CPartyDepsC2 判词] 封 N7/N107（ORIG 0x085a8f4e）：p167 36=36
    // CLEAR；P183 去 CBattle_Field 引用缓存（逐点 reinterpret_cast 重算
    // this+0xb24）、P212 全头部声明（槽 -0x30/-0x24/-0x20/-0x1c/-0x18/-0x14/
    // -0x10/-0x31/-0xc 全等）、P47 极性（MakePacket >0 / levelGap >6 块序）、
    // P225 拆嵌套 if（drop-test 两条件免 bool 物化岛）已逐条归位；残差 =
    // ①CheckMoveMapFitInPathHackCnt 调用点实参寄存器轮换（ORIG ecx/edx/eax/
    //   esi/ebx ↔ 4.4 ebx/ecx/edx/eax/esi，N7 族，长度助记符全等仅寄存器名差）；
    // ②`&m_padSecu[0]` 取址 ORIG `lea 0x1abc(%ebx),%esi` vs 4.4 `add $0x1abc,%eax`
    //   （N107 六形探针不可翻）。语义等价。
    PacketGuard packet;   // ORIG 在状态检查前构造，所有出口析构
    int ret = 0;          // ORIG 0x085a8f61 `movl $0x0,-0x24(%ebp)`
    // [b28d-solo-CPartyDepsC2] P212 全头部声明：ORIG 槽深序
    //   dungeon -0x20 / curPosX -0x1c / curPosY -0x18 / fromMapIdx -0x14 /
    //   toMapIdx -0x10 / firstVisit -0x31 / levelGap -0xc。
    CDungeon* dungeon;
    int curPosX;
    int curPosY;
    int fromMapIdx;
    int toMapIdx;
    bool firstVisit;
    int levelGap;

    if (m_state != 2)
    {   // 非地图移动状态 → 0x30 号 cmd 包错误码 0x13
        send_to_party_cmd_error((ENUM_CMDPACKET)0x30, 0x13);
        return false;
    }

    dungeon = m_battleField.m_pDungeon;
    if (dungeon != 0 && dungeon->IsEnterEachMap())
    {   // 逐张进入型副本：直接改写当前座位号
        reinterpret_cast<CBattle_Field*>(&m_battleField)->SetCurSeatNo(
            (int)(char)info.m_39);                      // SetCurSeatNo W 0x082a43cc
    }
    else if (reinterpret_cast<CBattle_Field*>(&m_battleField)
                 ->IsClearDungeon())                    // W 0x0822d08c
    {
        send_to_party_cmd_error((ENUM_CMDPACKET)0x30, 0x13);
        return false;
    }

    curPosX =
        reinterpret_cast<CBattle_Field*>(&m_battleField)->GetCurPosX();  // W 0x085bf32c
    curPosY =
        reinterpret_cast<CBattle_Field*>(&m_battleField)->GetCurPosY();  // W 0x085bf382
    fromMapIdx = reinterpret_cast<CBattle_Field*>(&m_battleField)
                     ->get_map_index(curPosX, curPosY);                  // W 0x085bf260

    if (info.m_field17 == 1)
        reinterpret_cast<CBattle_Field*>(&m_battleField)
            ->checkLayeredMap((int)info.m_x, (int)info.m_y);   // T 0x0830e32e

    // ORIG 第二次 get_map_index 结果存栈后未再消费，保留调用形态
    toMapIdx = reinterpret_cast<CBattle_Field*>(&m_battleField)
                   ->get_map_index((int)info.m_x, (int)info.m_y);
    (void)toMapIdx;

    // 反作弊路径校验（[b28d-solo-CParty2] +0x1abc 已字段化为 m_secu；
    // 取址 4.4 恒 add vs ORIG lea 属 N107 封顶）
    m_secu.CheckMoveMapFitInPathHackCnt(m_host, this, info.m_fieldF,
                                        info.m_field13, fromMapIdx);

    firstVisit = false;
    ret = reinterpret_cast<CBattle_Field*>(&m_battleField)
              ->MoveMap((int)info.m_x, (int)info.m_y, firstVisit);  // T 0x08308ae6
    if (ret > 0)
    {   // MoveMap 失败码 > 0：错误码原样回发
        send_to_party_cmd_error((ENUM_CMDPACKET)0x30, (unsigned char)(ret & 0xff));
        return false;
    }

    checkMoveMapHackCnt(info);

    // ORIG 0x085a919c `jle 0x1d 块` ⇒ 条件写 >0（0xd3 块在前，P47 块序）。
    if ((char)m_battleField.m_pDungeon->m_bloodDungeonType > 0)
        reinterpret_cast<CBattle_Field*>(&m_battleField)->MakePacket(0xd3, &packet);
    else
        reinterpret_cast<CBattle_Field*>(&m_battleField)->MakePacket(0x1d, &packet);
    send_to_party(packet);

    check_passive_skill();
    send_loadingcheck_msg();
    resetFinishLoadingVar();
    changePartyMemberInfoOnMoveMap(info, firstVisit);

    // 队伍统计：人数>1 时按等级差分桶计数
    levelGap = _getMemberLevelGap();
    if (get_member_count() > 1)
    {
        // ORIG 0x085a9257 `jle 3 块` ⇒ 条件写 >6（GetPartyType 块在前，P47 块序）。
        if (levelGap > 6)
            GetInstanceCommonStatisticsMgr()->IncreaseQuantity(
                (CCommonStatisticsMgr::ENUM_STATISTIC_CONTAINER)0, GetPartyType());
        else
            GetInstanceCommonStatisticsMgr()->IncreaseQuantity(
                (CCommonStatisticsMgr::ENUM_STATISTIC_CONTAINER)0, 3);
    }

    changeBattleDataOnMoveMap(firstVisit);

    // GM 掉落测试：记录途经地图（+0x1af4 vector<MapInfo>，push_back W 0x084b472c）。
    // ORIG 两条件各自独立 test/je（P225 推广：拆嵌套 if 免 bool 物化岛）。
    if (getDungeonDropTestFlag())
    {
        if (!firstVisit)
        {
            // [b28d-solo-CParty2] +0x1af4 已字段化为 m_passedMap（vector<MapInfo>）
            m_passedMap.push_back(*reinterpret_cast<CBattle_Field*>(&m_battleField)
                                      ->GetCurrentMapInfo());  // W 0x0822cbcc
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// send_loadingcheck_msg —— ORIG T 0x085b1fe0
// 报告：docs/class_func_reports/CParty/send_loadingcheck_msg.md
// 常量沿用 ORIG 局部静态实证值（同 source/Community/CommonConstants.h）：
//   CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT=0x78、CONST_PARTY_...=0x5a
// ---------------------------------------------------------------------------
void CParty::send_loadingcheck_msg()
{
    // [b28d-59] AE（ORIG 0x085b1fe0）：三目 + static volatile const 全局常量读 +
    // key 内联实参（P24）修正后 38/38 逐条一致。
    m_pad119 = 0;   // this+0x119 字节清零（装载检查标志）

    // [b28d-59] ORIG 0x085b1ffd..0x085b200e：cmp $1; jne; mov A,%eax; jmp;
    // mov B,%eax; mov %eax,-0xc ⇒ 三目形 + 全局常量内存读（非 if 赋值形）。
    const int timeout =
        get_member_count() == 1 ? CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT
                                : CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT;

    // InsertTimer(objType=1, param1=m_partyIndex(+0x64), msg=0xb, sec=timeout,
    //             param2=key, param3=0)
    // [b28d-59] key 内联实参（P24：ORIG 0x085b201f gen_timer_key 先算入 %ebx、
    // m_partyIndex 入 %esi，G_TimerQueue() 置后；具名 key 局部会多栈槽）。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                (TIMER_MESSAGE)0xb, timeout,
                                gen_timer_key((TIMER_MESSAGE)0xb), 0);
}

// ---------------------------------------------------------------------------
// set_random_reward —— ORIG T 0x0859b8fa
// 报告：docs/class_func_reports/CParty/set_random_reward.md
// ---------------------------------------------------------------------------
void CParty::set_random_reward(QuickParty::RandomBuffType type,
                               QuickParty::RewardData_Param param,
                               QuickParty::RewardData_Result& result)
{
    // [b28d-solo-CPartyDepsC2] 修 N18b 变体（ORIG 0x0859b8fa）：`m_field1ae8`
    // （unsigned int）与 enum 形参直接比较时 4.4 恒两段寄存器装载（P209 实测：
    // 字段/形参异型），ORIG 0x0859b925 段为同型 mem 直比
    // `mov 0x1ae8(%eax),%eax; cmp 0xc(%ebp),%eax`。用 enum 同型引用视图读
    // （P235 同址双视角，探针 /tmp/b28d_c2_probe/p6.cpp n0/n1 逐条全等）⇒ 字段本体
    // 仍为 unsigned int（store 段 0x0859b814 P211 异型 RHS-first 不变）。
    if (!is_quick_party())                  // is_quick_party W 0x0822d952
        return;
    if (m_field1ae8 == 0xb)                 // BASIC_REWARD_CARD_GOLD 占用时跳过
        return;
    if (m_field1ae8Type != type)            // 同址 enum 视角（CParty.h union）
        return;

    QuickParty::IRewardType* obj =
        G_CGameManager()->GetQuickPartyRewardManager()->get_reward_type_object(
            (QuickParty::ENUM_REWARD_TYPE)1, (unsigned int)type);
    // ORIG 0x0859b95a `je 0x0859b98c` 直落主出口（无 nop 岛）⇒ 正向包裹（P257/P285：
    // 早返形恒多一岛）。vtable 槽1（vptr+0x04）。
    if (obj != 0)
        obj->process(param, result);
}

// ---------------------------------------------------------------------------
// useItem —— ORIG T 0x085a7800
// 报告：docs/class_func_reports/CParty/useItem.md
// ---------------------------------------------------------------------------
void CParty::useItem(CUser* user, Inven_Item const& item)
{
    reinterpret_cast<CBattle_Field*>(&m_battleField)->onUseItem(user, item);
}

// ---------------------------------------------------------------------------
// use_skill —— ORIG T 0x085a77d8
// 报告：docs/class_func_reports/CParty/use_skill.md
// ---------------------------------------------------------------------------
void CParty::use_skill(CUser* user, int skillIdx, int param)
{
    OnUsedSkill(user, skillIdx, param);     // OnUsedSkill T 0x085b5f8e
}
