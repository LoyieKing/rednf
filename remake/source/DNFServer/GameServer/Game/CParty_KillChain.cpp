// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CParty_KillChain/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

// calDeadUserDungeonPenalty 死亡计数表具名视图（ORIG 0x085ad9df 寻址实证：
// `mov slot,%edx; mov this,%eax; add $0xe0,%edx; mov 0x10(%eax,%edx,4),%eax`
// —— 表基址 this+0x10，索引 slot+0xe0；越界索引照抄 ORIG 无值域检查语义）。
struct KcDeadCountView
{
    char m_pad[0x10];
    int m_deadCounts[1];
};

// _makeRewardItemInfo 的尾随 Inven_Item 视图：ORIG 0x085ad0cc 每次字段访问都
// 重发 `mov 0x10(%ebp),%eax; add $0x3d,%eax`（无指针槽，P0）⇒ 用宏内联展开，
// 不落具名指针局部。+0x2 复用 Inven_Item::m_addInfo 作调用方扩展标志。
#define kcTailItem(p) \
    (reinterpret_cast<Inven_Item*>(reinterpret_cast<char*>(p) + 0x3d))

// addDungeonClear 的每日入场次数记录视图（ORIG 0x085bde31 寻址实证：
// `mov -0xc(%ebp),%ecx; add $0x300,%ecx; movzwl 0x8(%edx,%ecx,8),%edx` ——
// 记录基址 this+(i+0x300)*8+8，步长 8：short uniqueId @+0x0 / int 计数 @+0x4）。
struct KcClearRec
{
    short m_uniqueId;
    short m_pad02;
    int m_count;
};

struct KcClearRecView
{
    char m_pad[0x1808];
    KcClearRec m_recs[1];
};

// CDataManager 每日入场门槛（ORIG 0x085bde7e/0x085bdf11 实证 +0x512c/0x512e
// 两个 ushort；CDataManager.h 未建模该段，按 §4.3 具名视图访问）。
struct KcDataMgrDailyLimitView
{
    char m_pad[0x512c];
    unsigned short m_limitKind0;    // +0x512c
    unsigned short m_limitKind12;   // +0x512e
    unsigned short m_limitDiff1;    // +0x5130（DungeonPermission diff==1 门槛）
};

}  // namespace

// ============================================================================
// hasPCRoomPlayer —— ORIG T 0x085b4a70。
// 任一有效成员持有 0xc 号 premium → true。
// ============================================================================
#include "CParty_KillChain/hasPCRoomPlayer.inc"
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
#include "CParty_KillChain/GetPartyMemberForceDropFlag.inc"
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
#include "CParty_KillChain/existSamePCRoomUser.inc"
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
// [b28d-solo-CParty_KillChain N85+N94] ORIG 0x085ad9b4：本波已把槽布局修到与 ORIG 全等
// （rate -0x8 / memberCount -0x4 / fildl 临时 -0x14 / 返回槽 -0x18，帧 $0x18；修法 =
// P0 去 deadCnt 具名局部 + memberCount 前置到 slot 判定之前）。残余两条结构性差：
//   ① `rate == 0.3f` 的 x87 比较 lowering：ORIG 4.1 `fucompp; fnstsw %ax; test $0x45,%ah;
//      sete %al; test %al,%al; je`，4.4 恒 `fxch; fucomip; fstp %st(0); jne; jp`（N85；
//      -march=i486 探针亦不翻，属跨版本 lowering）。
//   ② 死亡计数表寻址：ORIG `add $0xe0,%edx; mov 0x10(%eax,%edx,4),%eax`（disp 保留），
//      4.4 恒折 `add $0xe0,%edx; add $0x4,%edx; mov (%eax,%edx,4),%eax`（N94/N4：7 形探针
//      ——成员数组视图/int* 加法/char* 加法/具名指针——全部折叠，源形不可择）。
// P167 CLEAR（本函数无 call 点）。语义等价，封顶。
#include "CParty_KillChain/calDeadUserDungeonPenalty.inc"
{
    float rate = 0.0f;
    // P0：ORIG 0x085ad9c2 在 slot 判定前即读入 memberCount 并占槽 -0x4。
    const int memberCount = m_battleData.m_field68c;
    if (slot < 0)
        return rate;

    if (memberCount > 1)
    {
        // P0/P183：ORIG 无 deadCnt 具名槽（0x085ad9eb 直接 fildl 临时槽）——
        // 表达式内联进 float 转换，禁具名 int 局部。
        rate = static_cast<float>(
                   reinterpret_cast<const KcDeadCountView*>(this)
                       ->m_deadCounts[slot + 0xe0])
             / static_cast<float>(memberCount - 1);
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
// [b28d-solo-CParty_KillChain AE+真 bug] ORIG 0x085ad0be：本波修法——①去 itemBlob/
// extFlag 引用/tailItem 三个具名槽，改 kcTailItem 宏内联（P0：ORIG 每次重发
// `mov 0x10(%ebp),%eax; add $0x3d,%eax`）；②slot 函数头预清 -1（P198①，ORIG 0x085ad0c5
// `movl $0xffffffff,-0xc(%ebp)`）；③getCurCharacInvenR receiver 链式内联（P0/P52，消 inven
// 槽、帧由 $0x78 回到 ORIG 的 push %ebx+sub $0x64）；④if/else 极性反写（ORIG put_byte(1)
// 是跳转块，P59/P194）。**真 bug**：tryInsertItemIntoInventory 原传 `*item`，ORIG 0x085ad111
// `lea 0x3d(%eax),%ebx` + 0x3d 字节按值拷贝 ⇒ 入参是 item+0x3d 的尾随 Inven_Item 视图对象。
// 123=123 AE，P167 CLEAR。
#include "CParty_KillChain/_makeRewardItemInfo.inc"
                                 PacketGuard& packet)
{
    // P198①：ORIG 0x085ad0c5 函数头 `movl $0xffffffff,-0xc(%ebp)` ⇒ 具名 int
    // slot 预清 -1（else 分支才回写 tryInsert 结果）。
    int slot = -1;

    if (kcTailItem(item)->m_addInfo == 0xffffffffu)
    {
        kcTailItem(item)->m_addInfo = 0;
        kcTailItem(item)->set_add_info(0);
    }

    // ORIG 0x085ad106 `test; je <put_byte(1) 块>`：extFlag==0 走**跳转**块，
    // 非 0 直落 ⇒ 源码须写 `!= 0` 正向臂 + `== 0` 落尾（P59/P194 极性判据）。
    if (kcTailItem(item)->m_addInfo != 0)
    {
        // ORIG 0x085ad111 `lea 0x3d(%eax),%ebx` + 0x3d 字节按值拷贝 ⇒ 入参是
        // 尾随 Inven_Item 视图对象（不是 item 本体）。
        // P0/P52：ORIG 0x085ad114 无 inven 具名槽（getCurCharacInvenR 结果在
        // %eax 跨按值拷贝段保活）⇒ receiver 链式内联，禁中间指针局部。
        slot = const_cast<CUser*>(user)
                   ->getCurCharacInvenR()
                   ->tryInsertItemIntoInventory(*kcTailItem(item));
        // ORIG 0x085ad19e `js <put_byte(1) 块>`：slot<0 走跳转块，slot>=0 直落。
        if (slot >= 0)
        {
            packet.put_byte(2);
        }
        else
        {
            packet.put_byte(1);
            kcTailItem(item)->m_addInfo = 0;
            kcTailItem(item)->set_add_info(0);
        }
    }
    else
    {
        packet.put_byte(1);
    }

    packet.put_int(0);
    packet.put_int(item->get_add_info());
    if (kcTailItem(item)->m_addInfo != 0)
    {
        packet.put_int(kcTailItem(item)->m_addInfo);
        packet.put_int(kcTailItem(item)->get_add_info());
    }
}

// ============================================================================
// increase_result_card_gold —— ORIG T 0x0859ba98。
// 遍历 result.m_dropItems 与成员槽双游标：checkValidUser(i) 且迭代器未尽时，
// 以 RewardCardGold(type=0xb) 结算该成员金卡加成并回写 item.m_addInfo2(+0x07)；
// 每次消费一个结果项（迭代器独立于成员循环推进）。
// ============================================================================
// [b28d-solo-CParty_KillChain AE] ORIG 0x0859ba98：77/77 助记符/槽位逐条全等（本波
// fresh .o 复验，routed 口径 identical/ae），vector begin/end/operator* 链与 ORIG 一致，
// P167 CLEAR。
#include "CParty_KillChain/increase_result_card_gold.inc"
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
            // P167 判据：ORIG 0x0859bb46/0x0859bb7f 调 `operator*`(deEv) 而非
            // `operator->`(ptEv) ⇒ 源形必须写 (*it).m（it-> 会多 2 个 ptEv 调用点）。
            res.m_value00 = (*it).m_addInfo2;   // item+0x07 金卡金额
            set_basic_reward(QuickParty::BASIC_REWARD_CARD_GOLD, param, res);
            (*it).m_addInfo2 = res.m_value00;
            ++it;
        }
    }
}

// ============================================================================
// MemberRegisterKillMonster —— ORIG T 0x085bae6e。
// 组队首杀同步：roleType==3（Boss）/8（APC Boss）/命名怪三分支，各遍历成员
// 1..3（跳过队长），对每成员 CKillMonsterInfo 先 Check 后 Register。
// ============================================================================
// [b28d-solo-CParty_KillChain AE] ORIG 0x085bae6e：本波修法——①删 mobIdx 缓存槽（P0：
// ORIG 0x085baee0 每处重读 monster.m_mobIndex）；②中间局部改 user(-0x28)+kmi(-0x24) 双
// 具名槽（ORIG 0x085baec0/0x085baed6 实证；原 void* data 槽语义错位）；③GetCharacExpandData
// 返回 CExpandEquipslot*，用 reinterpret_cast。160=160 AE，P167 CLEAR。
#include "CParty_KillChain/MemberRegisterKillMonster.inc"
{
    if (monster.m_roleType == 3)
    {
        for (int i = 1; i <= 3; ++i)
        {
            if (!checkValidUser(i))
                continue;
            // P0：ORIG 0x085baec0 有 user(-0x28)/kmi(-0x24) 两具名槽，且
            // m_mobIndex 每处重读（0x085baee0，无缓存局部）。
            CUser* user = get_user(i);
            CKillMonsterInfo* kmi = reinterpret_cast<CKillMonsterInfo*>(
                user->GetCharacExpandData(
                    static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xc)));
            if (!kmi->CheckBossKill(
                    static_cast<unsigned int>(monster.m_mobIndex), flag))
                kmi->RegisterBossKill(
                    static_cast<unsigned int>(monster.m_mobIndex), flag);
        }
    }
    else if (monster.m_roleType == 8)
    {
        for (int i = 1; i <= 3; ++i)
        {
            if (!checkValidUser(i))
                continue;
            CUser* user = get_user(i);
            CKillMonsterInfo* kmi = reinterpret_cast<CKillMonsterInfo*>(
                user->GetCharacExpandData(
                    static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xc)));
            if (!kmi->CheckAPCBossKill(
                    static_cast<unsigned int>(monster.m_mobIndex), flag))
                kmi->RegisterAPCBossKill(
                    static_cast<unsigned int>(monster.m_mobIndex), flag);
        }
    }
    else if (monster.isNamedMonster())
    {
        for (int i = 1; i <= 3; ++i)
        {
            if (!checkValidUser(i))
                continue;
            CUser* user = get_user(i);
            CKillMonsterInfo* kmi = reinterpret_cast<CKillMonsterInfo*>(
                user->GetCharacExpandData(
                    static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xc)));
            if (!kmi->CheckNamedKill(
                    static_cast<unsigned int>(monster.m_mobIndex), flag))
                kmi->RegisterNamedKill(
                    static_cast<unsigned int>(monster.m_mobIndex), flag);
        }
    }
}

// ============================================================================
// getDungeonTypeKind —— ORIG T 0x085bdd2a。
// 空=8；脚本 boss 图(dungeon->m_89c)!=0 → 1；维度可入 → 2；kind==1 → 3；
// 血轮 type1/type2 → 4/5；绝望塔 → 6；锦标赛 → 7；其余 0。
// ============================================================================
#include "CParty_KillChain/getDungeonTypeKind.inc"
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
// [b28d-solo-CParty_KillChain 修法+P167] ORIG 0x085bddfc：本波修法——①kind 具名槽
// 删除、改 switch(getDungeonTypeKind(dungeon))（P251：ORIG 0x085bde5f 无槽 cmp 链）；
// ②记录槽/门槛改具名视图（KcClearRecView/KcDataMgrDailyLimitView）消 char* 的 shl+add
// 与 add $0x512e；③比较序改 get_unique_id 在左（ORIG 0x085bde2d call 在前）；
// ④三条件改 P226 语句表达式复现 mov $1/jmp/mov $0/test 物化岛。
// 残余（N 封顶）：①ORIG 0x085bde2d 后 4.4 多一条 `mov %eax,%edx` 保存 uid（4.4 恒用 %eax
// 作记录基址、uid 被挤到 %edx；两操作数换位则记录读提前到 call 前且落 %ebx，180=180 仍
// DIFF）⇒ N18a；②三处 `mov 0x8(%ebp),%eax` 先于 `mov -0xc(%ebp),%edx`（ORIG 相反）⇒
// N7/P75 装载序。P167 CLEAR（8=8）。
#include "CParty_KillChain/addDungeonClear.inc"
{
    for (int i = 0; i <= 3; ++i)
    {
        // ORIG 0x085bde2d 先 call get_unique_id、0x085bde3a 后读记录（cmp %dx,%ax）
        // ⇒ 左操作数写 get_unique_id（求值序对齐；记录在左会把记录读提前到 call 前）。
        if (static_cast<short>(m_member[slot].m_pUser->get_unique_id()) !=
            reinterpret_cast<const KcClearRecView*>(this)
                ->m_recs[i].m_uniqueId)
            continue;

        // P251：ORIG 0x085bde5f `cmp $1,%eax; je; cmp $2,%eax; je;
        // test %eax,%eax; jne`（kind 无槽往返）⇒ 源形 switch(call())。
        switch (getDungeonTypeKind(dungeon))
        {
        case 0:
            // P226 扩展：ORIG 0x085bdebf `mov $1/jmp; mov $0; test; je` 短路
            // 物化岛 ⇒ 语句表达式包整个条件（内不含具名变量）。
            if (({ (param >= static_cast<int>(
                            reinterpret_cast<const KcDataMgrDailyLimitView*>(
                                G_CDataManager())->m_limitKind0) &&
                        reinterpret_cast<const KcClearRecView*>(this)
                                ->m_recs[i].m_count <= 10 &&
                        m_member[slot].m_field14 == 0); }))
            {
                return m_member[slot].m_pUser->AddDungeonClear(
                    m_battleField.m_dungeonIndex, 3);
            }
            continue;
        case 1:
            if (({ (param >= static_cast<int>(
                            reinterpret_cast<const KcDataMgrDailyLimitView*>(
                                G_CDataManager())->m_limitKind12) &&
                        reinterpret_cast<const KcClearRecView*>(this)
                                ->m_recs[i].m_count <= 0x1e &&
                        m_member[slot].m_field14 == 0); }))
            {
                return m_member[slot].m_pUser->AddDungeonClear(
                    m_battleField.m_dungeonIndex, 3);
            }
            continue;
        case 2:
            if (({ (param >= static_cast<int>(
                            reinterpret_cast<const KcDataMgrDailyLimitView*>(
                                G_CDataManager())->m_limitKind12) &&
                        reinterpret_cast<const KcClearRecView*>(this)
                                ->m_recs[i].m_count <= 0x32 &&
                        m_member[slot].m_field14 == 0); }))
            {
                return m_member[slot].m_pUser->AddDungeonClear(
                    m_battleField.m_dungeonIndex, 3);
            }
            continue;
        default:
            return false;
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
// [b28d-solo-CParty_KillChain 修法+P167] ORIG 0x085b12f8：本波修法——①删
// dungeonIdx/user/limit/diff 四个具名槽（ORIG 全部表达式直读：0x085b1300/0x085b1371/
// 0x085b13b5/0x085b139f 实证），diff 改 switch(call())（P251）；②门槛改
// KcDataMgrDailyLimitView 具名视图；③diff==1 比较改 P226 语句表达式复现 setle 物化。
// 残余（N 封顶）：帧同 $0x40、指令数 209=209；残差为 i/granted 槽（-0x10/-0x9 vs
// ORIG -0xc/-0xd）与 set find/end/key 槽组整体浅 4（difflib 因重复 call 行对齐放大）；
// 全头部声明（P212）反多一条 movb（210）⇒ N82/N98 槽分配族。P167 CLEAR（29=29）。
#include "CParty_KillChain/DungeonPermission.inc"
{
    // P52：ORIG 0x085b1300 先把 m_dungeonIndex 物化 %ebx 跨 G_CDataManager call。
    CDungeon* dungeon = G_CDataManager()->find_dungeon(
        m_battleField.m_dungeonIndex);
    if (dungeon == 0)
        return;

    const int minLevel = dungeon->get_min_level();
    for (int i = 0; i <= 3; ++i)
    {
        bool granted = false;
        if (!_checkValidUser(i))
            continue;
        // P0：ORIG 0x085b1371 无 user 具名槽（直接 this 载入 → call get_charac_level）。
        if (minLevel > static_cast<int>(
                           m_member[i].m_pUser->get_charac_level()))
            continue;

        // P251：ORIG 0x085b139f `cmp $1,%eax; je; cmp $2,%eax; je;
        // test %eax,%eax; jne`（diff 无槽往返）⇒ 源形 switch(call())。
        switch (kcbf(this).get_dungeon_diff())
        {
        case 0:
            granted = m_member[i].m_pUser->AddDungeonClear(
                m_battleField.m_dungeonIndex, 1);
            break;
        case 1:
            // P226：ORIG 0x085b1403 `cmp; setle %al; test %al,%al; je 岛` 物化。
            if (({ (static_cast<int>(
                        reinterpret_cast<const KcDataMgrDailyLimitView*>(
                            G_CDataManager())->m_limitDiff1) <= param); }))
                granted = m_member[i].m_pUser->AddDungeonClear(
                    m_battleField.m_dungeonIndex, 2);
            break;
        case 2:
            // P167/P183：ORIG 0x085b147e/0x085b1486 两次完整重读
            // getUltimateDungeonList（无指针缓存），0x085b14aa 用 operator==
            // 比较后 jne 跳过授予；ORIG 0x085b1476 调 const 版 set::find。
            if (static_cast<const std::set<int>*>(
                    G_CDataManager()->getUltimateDungeonList())
                    ->find(dungeon->get_index()) ==
                static_cast<const std::set<int>*>(
                    G_CDataManager()->getUltimateDungeonList())->end())
                goto DUNGEON_PERM_NEXT;
            granted = addDungeonClear(dungeon, param, i);
        DUNGEON_PERM_NEXT:
            break;
        default:
            break;
        }

        if (granted)
        {
            PacketGuard packet;
            packet.put_header(0, 5);
            packet.put_short(1);
            packet.put_short(m_battleField.m_dungeonIndex);
            packet.put_short(kcbf(this).get_dungeon_diff() + 1);
            packet.finalize(true);
            m_member[i].m_pUser->Send(packet);
        }
    }
}
