// ============================================================================
// CParty_KillChain.cpp —— CParty 域击杀/奖励链第三阶依赖（LinkKillChain 批次）。
//
// 覆盖 10 个 ORIG 真实符号（全部经 docs/class_func_reports/CParty/ 对应报告
// ## 汇编 + ## 反编译 C 完整读取后忠实转写，无桩）：
//   MemberRegisterKillMonster      T 0x085bae6e（MemberRegisterKillMonster.md）
//   _makeRewardItemInfo            T 0x085ad0be（_makeRewardItemInfo.md）
//   DungeonPermission              T 0x085b12f8（DungeonPermission.md）
//   calDeadUserDungeonPenalty      T 0x085ad9b4（calDeadUserDungeonPenalty.md）
//   existSamePCRoomUser            T 0x085b67ca（existSamePCRoomUser.md）
//   increase_result_card_gold      T 0x0859ba98（increase_result_card_gold.md）
//   hasPCRoomPlayer                T 0x085b4a70（hasPCRoomPlayer.md；链上依赖）
//   GetPartyMemberForceDropFlag    T 0x085bdc00（GetPartyMemberForceDropFlag.md；依赖）
//   addDungeonClear                T 0x085bddfc（addDungeonClear.md；DungeonPermission 依赖）
//   getDungeonTypeKind             T 0x085bdd2a（getDungeonTypeKind.md；依赖）
//
// 成员槽布局（ORIG 实证）：m_member[i] @ this+i*0x18+0x78，m_pUser 在槽首；
// m_field14(+0x8c) 为成员副本次数槽。每日入场次数记录数组位于 this+0x1808
// （每成员 8 字节：short uniqueId / int 计数，addDungeonClear 实证），处于
// CParty.h 占位区，按裸偏移访问。m_dungeonIdx 为 this+0xcb8（同上）。
// ============================================================================

#include <string.h>

#include <set>
#include <vector>

#include "QuickParty.h"        // 必须先于 CDungeon.h/CParty.h（守卫惯例）
#include "CParty.h"
#include "CKillMonsterInfo.h"  // 击杀/奖励链 CKillMonsterInfo 六谓词（桥清零）
#include "CDataManager.h"      // G_CDataManager/find_dungeon/getUltimateDungeonList
#include "CDungeon.h"          // get_min_level/get_index/get_dimension_possible/…
#include "CBattle_Field.h"     // bf 子对象视图
#include "CUser.h"             // CUser/CUserCharacInfo
#include "CInventory.h"        // tryInsertItemIntoInventory
#include "WarField.h"          // map_monster 完整定义
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"
#include "LogManager.h"
#include "CQuickPartyRewardManager.h"  // QuickParty::RewardData_Param/Result

namespace
{

// P49：寻址助手用宏（inline 函数 -O0 产真 call；ORIG 0x0859ba98 域
// `mov this; add $0xb24,%eax; mov esp; call` 直取址形）。
#define kcbf(p) ((p)->m_battleField)

}  // namespace

// ============================================================================
// hasPCRoomPlayer —— ORIG T 0x085b4a70。
// 任一有效成员持有 0xc 号 premium → true。
// ============================================================================
bool CParty::hasPCRoomPlayer()
{
    for (int i = 0; i <= 3; ++i)
    {
        // ORIG 0x85b4a7f 域：两独立守卫（xor;test;jne continue → call;test;je
        // skip）——合并 && 违 P133（汇聚 setne 形）。
        if (!_checkValidUser(i))
            continue;
        if (m_member[i].m_pUser->isAffectedPremium(
                static_cast<ENUM_PREMIUM_TYPE>(0xc)))
            return true;
    }
    return false;
}

// ============================================================================
// GetPartyMemberForceDropFlag —— ORIG T 0x085bdc00。
// 任一成员 get_user(i)!=0 且 isGMUser 且 getForceDropFlag → 1。
// ============================================================================
int CParty::GetPartyMemberForceDropFlag()
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* user = get_user(i);
        if (user == 0)
            continue;
        if (user->isGMUser() && user->getForceDropFlag())
            return 1;
    }
    return 0;
}

// ============================================================================
// existSamePCRoomUser —— ORIG T 0x085b67ca。
// 统计 PC 房号==pcbangNo 且非 0 的有效成员数，>=2 → true。
// ============================================================================
bool CParty::existSamePCRoomUser(unsigned int pcbangNo)
{
    int count = 0;
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            // ORIG 两次调用 GetPCRoomNo：uVar3==param_1 且 iVar4!=0。
            if (static_cast<unsigned int>(
                    m_member[i].m_pUser->GetPCRoomNo()) == pcbangNo
                && static_cast<unsigned int>(
                    m_member[i].m_pUser->GetPCRoomNo()) != 0)
            {
                count += 1;
                if (count > 1)
                    return true;
            }
        }
    }
    return false;
}

// ============================================================================
// calDeadUserDungeonPenalty —— ORIG T 0x085ad9b4。
// slot<0 → 0；memberCount(m_battleData.m_field68c=CParty+0x9b8)>1 时
// rate = float(记录[slot])/float(count-1)，ORIG 语义：rate == 0.3f
// （.rodata 0x08cbbbe8 = 0.3f）→ 归零（fucompp+test $0x45+sete 严格相等比较，非 <）。
// 死亡计数表基址 this+0x10+(slot+0xe0)*4（ORIG 寻址实证），位于 m_battleData
// （+0x32c）内，超建模段按裸偏移访问。
// ============================================================================
// [b28d-7 N82] ORIG 0x085ad9b4：float 局部槽 -0x8/-0x4（帧 $0x18），4.4 -0xc/-0x8 + 块序不同；P167 CLEAR ⇒ 封顶。
float CParty::calDeadUserDungeonPenalty(int slot)
{
    float rate = 0.0f;
    if (slot < 0)
        return rate;

    const int memberCount = m_battleData.m_field68c;
    if (memberCount > 1)
    {
        const int deadCnt = *reinterpret_cast<int*>(
            reinterpret_cast<char*>(this) + 0x10 + (slot + 0xe0) * 4);
        rate =
            static_cast<float>(deadCnt) / static_cast<float>(memberCount - 1);
        if (rate == 0.3f)
            rate = 0.0f;
    }
    return rate;
}

// ============================================================================
// _makeRewardItemInfo —— ORIG T 0x085ad0be。
// item+0x3f（调用方结构扩展标志，越界字段按裸偏移访问）：-1 → 归零并清尾随
// addInfo；为 0 → 包内 put_byte(1)；否则尝试入包（tryInsertItemIntoInventory
// 槽位<0 视为失败回退 put_byte(1)，成功 put_byte(2)）。尾部统一
// put_int(0)/put_int(item.add_info())，标志非 0 时再补写标志与尾随 addInfo。
// 尾随 addInfo 宿主为 item+0x3d 处的 Inven_Item 视图（set_add_info 写其 +0x07）。
// ============================================================================
// [b28d-7 N82] ORIG 0x085ad0be：帧 $0x64 + 单一 push %ebx + 无具名 blob 局部，4.4 帧 $0x78 + 多级具名槽（-0x14/-0x18/-0x1c）；P167 CLEAR ⇒ 封顶。
void CParty::_makeRewardItemInfo(CUser const* user, Inven_Item* item,
                                 PacketGuard& packet)
{
    char* itemBlob = reinterpret_cast<char*>(item);
    int& extFlag = *reinterpret_cast<int*>(itemBlob + 0x3f);
    Inven_Item* tailItem = reinterpret_cast<Inven_Item*>(itemBlob + 0x3d);

    if (extFlag == -1)
    {
        extFlag = 0;
        tailItem->set_add_info(0);
    }

    if (extFlag == 0)
    {
        packet.put_byte(1);
    }
    else
    {
        const CInventory* inven =
            const_cast<CUser*>(user)->getCurCharacInvenR();
        const int slot = inven->tryInsertItemIntoInventory(*item);
        if (slot < 0)
        {
            packet.put_byte(1);
            extFlag = 0;
            tailItem->set_add_info(0);
        }
        else
        {
            packet.put_byte(2);
        }
    }

    packet.put_int(0);
    packet.put_int(item->get_add_info());
    if (extFlag != 0)
    {
        packet.put_int(extFlag);
        packet.put_int(tailItem->get_add_info());
    }
}

// ============================================================================
// increase_result_card_gold —— ORIG T 0x0859ba98。
// 遍历 result.m_dropItems 与成员槽双游标：checkValidUser(i) 且迭代器未尽时，
// 以 RewardCardGold(type=0xb) 结算该成员金卡加成并回写 item.m_addInfo2(+0x07)；
// 每次消费一个结果项（迭代器独立于成员循环推进）。
// ============================================================================
// [b28d-7 AE] ORIG 0x0859ba98：77/77 助记符/槽位逐条全等（446h46 路由口径），vector begin/end/operator* 链与 ORIG 一致，P167 CLEAR。
void CParty::increase_result_card_gold(WongWork::stGenerateResult_t& result)
{
    std::vector<Inven_Item>::iterator it = result.m_dropItems.begin();
    for (int i = 0; i <= 3; ++i)
    {
        if (!checkValidUser(i))
            continue;
        // P220 正极性包裹：ORIG `call ==; test; je`（==真跳主流程后）
        if (it != result.m_dropItems.end())
        {
            QuickParty::RewardData_Param param(this);
            param.m_param4 = kcbf(this).get_dungeon_index();
            QuickParty::RewardData_Result res;
            res.m_value00 = it->m_addInfo2;   // item+0x07 金卡金额
            set_basic_reward(QuickParty::BASIC_REWARD_CARD_GOLD, param, res);
            it->m_addInfo2 = res.m_value00;
            ++it;
        }
    }
}

// ============================================================================
// MemberRegisterKillMonster —— ORIG T 0x085bae6e。
// 组队首杀同步：roleType==3（Boss）/8（APC Boss）/命名怪三分支，各遍历成员
// 1..3（跳过队长），对每成员 CKillMonsterInfo 先 Check 后 Register。
// ============================================================================
// [b28d-7 N82] ORIG 0x085bae6e：m_mobIndex 直用无槽，4.4 缓存至 -0x30（帧同 0x58）；P167 CLEAR ⇒ 封顶。
void CParty::MemberRegisterKillMonster(map_monster& monster, unsigned char flag)
{
    const unsigned int mobIdx = static_cast<unsigned int>(monster.m_mobIndex);

    if (monster.m_roleType == 3)
    {
        for (int i = 1; i <= 3; ++i)
        {
            if (!checkValidUser(i))
                continue;
            void* data = get_user(i)->GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xc));
            CKillMonsterInfo* kmi = static_cast<CKillMonsterInfo*>(data);
            if (!kmi->CheckBossKill(mobIdx, flag))
                kmi->RegisterBossKill(mobIdx, flag);
        }
    }
    else if (monster.m_roleType == 8)
    {
        for (int i = 1; i <= 3; ++i)
        {
            if (!checkValidUser(i))
                continue;
            void* data = get_user(i)->GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xc));
            CKillMonsterInfo* kmi = static_cast<CKillMonsterInfo*>(data);
            if (!kmi->CheckAPCBossKill(mobIdx, flag))
                kmi->RegisterAPCBossKill(mobIdx, flag);
        }
    }
    else if (monster.isNamedMonster())
    {
        for (int i = 1; i <= 3; ++i)
        {
            if (!checkValidUser(i))
                continue;
            void* data = get_user(i)->GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xc));
            CKillMonsterInfo* kmi = static_cast<CKillMonsterInfo*>(data);
            if (!kmi->CheckNamedKill(mobIdx, flag))
                kmi->RegisterNamedKill(mobIdx, flag);
        }
    }
}

// ============================================================================
// getDungeonTypeKind —— ORIG T 0x085bdd2a。
// 空=8；脚本 boss 图(dungeon->m_89c)!=0 → 1；维度可入 → 2；kind==1 → 3；
// 血轮 type1/type2 → 4/5；绝望塔 → 6；锦标赛 → 7；其余 0。
// ============================================================================
int CParty::getDungeonTypeKind(CDungeon const* dungeon)
{
    if (dungeon == 0)
        return 8;
    // m_89c（CDungeon.h:777，unsigned char）：ORIG movzbl 0x89c(%eax);test %al
    if (dungeon->m_89c != 0)
        return 1;
    // ORIG 0x85bdd40 test %al;setg —— 有符号字节 >0 比较（(signed char) 先例同
    // CPartyDepsB2:550/CPartyOnlyOrig:881）；uchar 直 >=1 会折 setne。
    if ((signed char)dungeon->get_dimension_possible() > 0)
        return 2;
    if (dungeon->getDungeonKind() == 1)
        return 3;
    if (dungeon->get_blood_dungeon_type() == 1)
        return 4;
    if (dungeon->get_blood_dungeon_type() == 2)
        return 5;
    if (dungeon->isTowerOfDespairDungeon())
        return 6;
    if (dungeon->isTournamentDungeon())
        return 7;
    return 0;
}

// ============================================================================
// addDungeonClear —— ORIG T 0x085bddfc。
// 按 param(slot) 成员的 uniqueId 匹配记录槽（this+0x1808+i*8：short uniqueId /
// int 计数）；kind==1 上限 30、kind==2 上限 50、kind==0 上限 10（param 与
// dm+0x512e/0x512c 当日门槛双判定），且成员 m_field14(+0x8c)==0 时
// AddDungeonClear(m_battleField.m_dungeonIndex=CParty+0xcb8, 3)。其余 kind
// 或未命中返回 false。
// ============================================================================
// [b28d-7 修法+P167] ORIG 0x085bddfc：三分支各自 return AddDungeonClear（0x085bdf02/085bdf95/085be024，共 3 处）——旧版共享末尾 return 少 2 次调用，已按分支补回；残余为 N82 槽基（-0xc vs -0x10）与两步寻址（add/shl vs lea），P167 已 CLEAR。
bool CParty::addDungeonClear(const CDungeon* dungeon, int param, int slot)
{
    for (int i = 0; i <= 3; ++i)
    {
        // 记录槽基址 this + (i+0x300)*8（= this+i*8+0x1800），uniqueId 位于 +8，
        // 计数位于 +0xc（ORIG addDungeonClear 寻址实证）。
        if (*reinterpret_cast<short*>(reinterpret_cast<char*>(this)
                + (i + 0x300) * 8 + 8) !=
            static_cast<short>(m_member[slot].m_pUser->get_unique_id()))
            continue;

        const int kind = getDungeonTypeKind(dungeon);
        if (kind == 1)
        {
            if (param < static_cast<int>(*reinterpret_cast<
                    unsigned short*>(reinterpret_cast<char*>(G_CDataManager())
                    + 0x512e))
                || *reinterpret_cast<int*>(
                    reinterpret_cast<char*>(this) + (i + 0x300) * 8 + 0xc)
                    > 0x1e
                || m_member[slot].m_field14 != 0)
                continue;
            // [b28d-7 真缺口] P167：ORIG 0x85bdf02 每分支独立 return
            // AddDungeonClear（共 3 处 0x85bdf02/0x85bdf95/0x85be024），
            // 旧版共享末尾 return 少 2 次调用。
            return m_member[slot].m_pUser->AddDungeonClear(
                m_battleField.m_dungeonIndex, 3);
        }
        else if (kind == 2)
        {
            if (param < static_cast<int>(*reinterpret_cast<
                    unsigned short*>(reinterpret_cast<char*>(G_CDataManager())
                    + 0x512e))
                || *reinterpret_cast<int*>(
                    reinterpret_cast<char*>(this) + (i + 0x300) * 8 + 0xc)
                    > 0x32
                || m_member[slot].m_field14 != 0)
                continue;
            return m_member[slot].m_pUser->AddDungeonClear(
                m_battleField.m_dungeonIndex, 3);
        }
        else
        {
            if (kind != 0)
                return false;
            if (param < static_cast<int>(*reinterpret_cast<
                    unsigned short*>(reinterpret_cast<char*>(G_CDataManager())
                    + 0x512c))
                || *reinterpret_cast<int*>(
                    reinterpret_cast<char*>(this) + (i + 0x300) * 8 + 0xc)
                    > 10
                || m_member[slot].m_field14 != 0)
                continue;
            return m_member[slot].m_pUser->AddDungeonClear(
                m_battleField.m_dungeonIndex, 3);
        }
    }
    return false;
}

// ============================================================================
// DungeonPermission —— ORIG T 0x085b12f8。
// 副本准入结算：逐成员校验等级下限后按当前难度分支记账（diff0→普通 1 次/
// diff1→dm+0x5130 门槛下 2 次/diff2→ultimate 清单内 addDungeonClear），
// 记账成功即发 0x05 准入通知包（short: 1 / dungeonIdx / diff+1）。
// ============================================================================
// [b28d-7 修法+P167] ORIG 0x085b12f8：diff==2 分支两次完整重读 getUltimateDungeonList（0x085b147e/085b1486）+ const 版 set::find（0x085b1476）+ operator==（0x085b14aa）——旧版缓存指针 + operator!=，已按 ORIG 形重写；残余 N82 槽基/帧（0x40 vs 0x50），P167 已 CLEAR（29=29）。
void CParty::DungeonPermission(int param)
{
    const int dungeonIdx = m_battleField.m_dungeonIndex;
    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (dungeon == 0)
        return;

    const int minLevel = dungeon->get_min_level();
    for (int i = 0; i <= 3; ++i)
    {
        bool granted = false;
        if (!_checkValidUser(i))
            continue;
        CUser* user = m_member[i].m_pUser;
        if (minLevel > static_cast<int>(user->get_charac_level()))
            continue;

        const int diff = kcbf(this).get_dungeon_diff();
        if (diff == 1)
        {
            const unsigned short limit = *reinterpret_cast<unsigned short*>(
                reinterpret_cast<char*>(G_CDataManager()) + 0x5130);
            if (static_cast<int>(limit) <= param)
                granted = user->AddDungeonClear(dungeonIdx, 2);
        }
        else if (diff == 2)
        {
            // [b28d-7 真缺口] P167/P183：ORIG 0x85b147e/0x85b1486 两次完整重读
            // getUltimateDungeonList（无指针缓存），0x85b14aa 用 operator==
            // 比较后 jne 跳过授予；旧版缓存指针 + operator!= 造成调用点差集。
            // ORIG 0x85b1476 调 const 版 set::find ⇒ 视图侧 const 化（零 ABI 影响）。
            if (static_cast<const std::set<int>*>(
                    G_CDataManager()->getUltimateDungeonList())
                    ->find(dungeon->get_index()) ==
                static_cast<const std::set<int>*>(
                    G_CDataManager()->getUltimateDungeonList())->end())
                goto DUNGEON_PERM_NEXT;
            granted = addDungeonClear(dungeon, param, i);
        DUNGEON_PERM_NEXT:
            ;
        }
        else if (diff == 0)
        {
            granted = user->AddDungeonClear(dungeonIdx, 1);
        }

        if (granted)
        {
            PacketGuard packet;
            packet.put_header(0, 5);
            packet.put_short(1);
            packet.put_short(dungeonIdx);
            packet.put_short(kcbf(this).get_dungeon_diff() + 1);
            packet.finalize(true);
            user->Send(packet);
        }
    }
}
