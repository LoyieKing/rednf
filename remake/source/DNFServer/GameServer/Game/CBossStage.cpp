// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBossStage/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CBossStage.h"
#include "WarField.h"   // MapInfo::Clear / map_item / map_monster
#include "GameRand.h"   // get_rand_int

#include <cstring>

#include "CUser.h"                  // CUser/CHackAnalyzer/cUserHistoryLog/ENUM_ERROR
#include "QuickParty.h"         // 先于 CDungeon.h：QUICKPARTY_H 守卫去重
#include "CDungeon.h"               // CDungeon/get_standard_level/RandomList
#include "CMap.h"                   // CMap/_mapMonster/_mapItem 完整类型
#include "CItemGenerator.h"         // WongWork::stGenerateRefData_t/stGenerateResult_t
#include "CGenUniqueNo.h"           // WongWork::CGenUniqueNo::genIPGNo
#include "CParty.h"                 // get_user/checkValidUser/getManager/m_battleField.m_bloodState
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "CDataManager.h"           // find_map/find_item/G_CDataManager
#include "CInventory.h"             // gain_money/delete_item/insertItemIntoInventory
#include "MonsterIndependentDrop.h" // generateIndependentItem/CAICharacter 权威镜像
#include "CMonster.h"               // CMonster::generateIndependentItem
#include "GlobalData.h"             // GlobalData::s_systemTime_
#include "CSystemTime.h"            // getCurTickCount
#include "CSpecialItemRoutingManager.h"  // StartRouting（RoutingPickupItem）
#include "CGameManager.h"               // G_CGameManager
#include "Secu_HackLogCheckByParty.h"   // MSG_MONSTER_DIE 完整定义

namespace WongWork
{

// Ctor（ORIG 0x0814bd0c）：CBossPlay 基类、置 vtable（编译器自动）、构造
// MapInfo、Clear、get_rand_int(60000) 写 m_assignItemIdx/m_dropItemCnt，
// 函数体内依次 m_stageCur=1、m_dungeon=0、m_difficulty=0。
#include "CBossStage/CBossStage.inc"
{
    m_stageCur = 1;
    m_dungeon = 0;
    m_difficulty = 0;
    m_stageData.Clear();
    m_stageData.m_assignItemIdx = get_rand_int(60000);   // ORIG +0xa30
    m_stageData.m_dropItemCnt = get_rand_int(60000);     // ORIG +0xa34
}

// Dtor（ORIG 0x0814bdd6）：置 vtable（编译器自动）、m_stageCur=1、
//   m_dungeon=0、m_difficulty=0、Clear、rand×2、析构 MapInfo、基类（编译器自动）。
#include "CBossStage/dtor_CBossStage.inc"
{
    m_stageCur = 1;
    m_dungeon = 0;
    m_difficulty = 0;
    m_stageData.Clear();
    m_stageData.m_assignItemIdx = get_rand_int(60000);
    m_stageData.m_dropItemCnt = get_rand_int(60000);
}

// ORIG 0x0814bea2：m_startTick=0、m_endTick=0、MapInfo::Clear、
//   memset(m_useItemCount, 0, 0x10)（CBossPlay::m_useItemCount[4]，ORIG this+0x9c8）。
#include "CBossStage/reset.inc"
{
    m_startTick = 0;
    m_endTick = 0;
    m_stageData.Clear();
    memset(m_useItemCount, 0, sizeof(m_useItemCount));
}


// ============================================================================
// 新增虚函数（虚表槽 [8]..[11]，紧随 CBossPlay 6 槽之后）
// ============================================================================


// ---- handleMoveMap（ORIG 0x0814f66a）----
// 换图入口：_checkMemberDie 短路 → 逐成员反外挂计数 + MapClear 日志 →
// moveNextStage → randomMapIndex →（GM 强制图/难度）→ find_map → reset →
// consistMap（失败 throw ENUM_ERROR(1)，catch 后广播 0x30 包并返回 0）→
// 记 startTick → SendMapInfoPacket。
// [b28d-62] N82：p167 CLEAR（0 missing/0 extra），273/280 助记符同序；残差为
//   帧/槽号（4.1 生命周期穿插 vs 4.4 分区复用），语义完成。ORIG 0x0814f66a
// [b28d-solo-CBossStage] N封顶：删 manager!=0 判空（ORIG 无，P171 照抄）
//   （ndiff 41→28，gap 0）。残差=N7 虚调用 vptr 寄存器 %eax↔%edx 3 条 +
//   N122 bool 取反 xor 物化 1 条。
#include "CBossStage/handleMoveMap.inc"
{
    if (_checkMemberDie())
        return 0;

    // 已进行时长（秒）：(m_endTick - m_startTick) / 1000
    // ORIG 0x0814f683 `movl $0x0,-0x20(%ebp)` 预清（P198① 两句式）
    int playSec = 0;
    playSec = (m_endTick - m_startTick) / 1000;

    for (int i = 0; i < 4; ++i)
    {
        CUser* user = m_party->get_user(i);
        if (user == 0 || !m_party->checkValidUser(i))
            continue;

        if (playSec <= 4)
            user->getHackAnalyzer()->addServerHackCnt(
                user, (ENUM_HACKTYPE)0x19f, 1, 0, 0);
        if (mapIndex[i] == 0)
            user->getHackAnalyzer()->addServerHackCnt(
                user, (ENUM_HACKTYPE)0x19b, 1, 0, 0);
        // ORIG this+0x9c8+i*4 逐 int 判零（CBossPlay::m_useItemCount[i]）
        if (m_useItemCount[i] == 0)
            user->getHackAnalyzer()->addServerHackCnt(
                user, (ENUM_HACKTYPE)0xdd, 1, 0, 0);

        // cUserHistoryLog 为 CUser 嵌套成员 m_historyLog（ORIG user+0x79700）
        user->m_historyLog.MapClear(mapIndex[i], playSec, 0);
    }

    // ORIG 0x0814f84e `call moveNextStage; test %al,%al; je <失败尾块>`（无 xor 物化）
    // ⇒ P292 正极性 if(bool)：写成 `if (f()) { … }` 而非 `if (!f()) return`。
    if (moveNextStage())
    {
        int mapIdx = 0;
        if (!randomMapIndex(mapIdx))
            return 0;

        try
        {
            CUser* manager = m_party->getManager();
            // ORIG 0x0814f891..89e 直接 call isGMUser（无 manager!=0 判空）⇒ P171 照抄
            if (manager->isGMUser())
            {
                // CUser GM 调试字段（"bossmap"/"bossdifficult" 指令写入，取走即清零）
                if (manager->m_gmBossMapIndex != 0)
                {
                    mapIdx = manager->m_gmBossMapIndex;
                    manager->m_gmBossMapIndex = 0;
                }
                if (manager->m_gmBossDifficulty != 0)
                {
                    m_difficulty = (int)(signed char)manager->m_gmBossDifficulty;
                    manager->m_gmBossDifficulty = 0;
                }
            }

            const CMap* map =
                (const CMap*)G_CDataManager()->find_map(mapIdx);
            if (map == 0)
                return 0;

            reset();
            if (!consistMap(map))
                throw (ENUM_ERROR)1;

            m_startTick = (int)GlobalData::s_systemTime_.getCurTickCount();
            SendMapInfoPacket();
            return 1;
        }
        // P229：ORIG 0x0814f9ac `mov (%eax),%eax; mov %eax,-0xc` ⇒ 按值 catch
        //   （引用 catch 会先落指针槽再回读）。
        catch (ENUM_ERROR e)
        {
            PacketGuard packet;
            packet.put_header(1, 0x30);
            packet.put_byte(0);
            packet.put_byte((int)e);
            packet.finalize(true);
            m_party->send_to_party(packet);
            return 0;
        }
    }
    return 0;
}

// ---- handlePickupItem（ORIG 0x0814fa76）----
// 拾取分发：stageData_.item（MapInfo@+0x9f4 的 m_itemMap@+0xa18）按 itemUID
// 查找；金币（m_item.m_addInfo==0）走 GoldPickupItem；否则 find_item 后按
// 队伍人数/存活数/rarity 分发 Routing/Random/Normal。
// [b28d-62] N89/P96：p167 显示 ORIG 用非 const _Rb_tree_iterator，我方
//   const_iterator（转换 ctor ×2 + const 版 operator 族）+ getParty 缓存差。
//   ORIG 0x0814fa76
// [b28d-solo-CBossStage] N82：去 MapInfo& stage 引用（直读 m_stageData）、
//   map_item item 默认构造 + item=it->second 结构赋值、迭代器 const_iterator
//   （ORIG 有转换构造）。残差=帧/槽分区（ORIG 迭代器槽 -0x64..-0x70）。
// [b28f-SweepCBossTower P248 档①] 返回类型 int→bool（CBossStage.h:54 同步）：
//   ORIG 调用点 0x08143dbb `call; mov %al,-0x9(%ebp)`（无 test/setne ⇒ 单字节
//   返回）；返回类型不入 mangled（_ZN8WongWork10CBossStage16handlePickupItemEP5CUseribRc
//   不变），vtable 槽序/sizeof 零影响；本 TU 四数改前/改后 identical=228
//   ae=368(596) near=2 diff=7 全等（零回归）。
// [b28f-SweepDeps 命中 · P0「ORIG 无槽」形] `bool canRoute` / `bool canRandom`
//   具名局部删除、条件内联进 `if`：ORIG 0x0814fbc0 段为**寄存器直测**
//   （`mov $0x1,%eax; jmp; mov $0x0,%eax; test %al,%al; je`，**无槽往返**）；
//   我方具名 bool 使 4.4 落槽（`mov %al,-0x1a; cmpb $0x0,-0x1a; je`，每处 +2 条）。
//   实测：immcmp 1/3→**1/1**、test 15/13→**15/15**、our 338→**336**、gap −5→**−3**
//   （ORIG 333）；TU 四数不变（228/369/2/6）⇒ 严格改善保留。
//   残余 = 迭代器/聚合槽分区（ORIG it -0x64..-0x70 / 我 -0x28..-0x34，
//   帧两侧同为 0xec）⇒ N82附 封顶。
#include "CBossStage/handlePickupItem.inc"
                                  char& outRarity)
{
    (void)flag;   // ORIG 未读取第 3 参（仅按值传递占位）
    // [R2PrettAClass 2026-09-11 · prett A 类落地] 原 `static const char kPretty[]` 手写串已删，
    //   调用点直接内联 __PRETTY_FUNCTION__（4.4 下 `static const char k[] = __PRETTY_FUNCTION__`
    //   数组初始化编译不过 ⇒ 必须内联）。ORIG .rodata @0x08b75e20 size 0x4e =
    //   "virtual bool WongWork::CBossStage::handlePickupItem(CUser*, int, bool, char&)"；
    //   类内声明为虚成员 ⇒ 宏自动带 `virtual `（探针 /tmp/r2/probe.cpp 实证：类外定义不写
    //   virtual 时宏仍打印 virtual），非 static 成员 ⇒ 无 `static ` 前缀（换宏三条件②通过）。

    // P0：ORIG 0x0814fa9e `mov 0x8(%ebp),%eax; lea 0xa18(%eax),%ecx` ⇒
    //   m_stageData.m_itemMap 直访问（无 MapInfo& 引用槽）。
    map_item item;                    // ORIG 0x0814fa8b map_item::C1Ev（-0xd0）
    std::map<int, map_item>::const_iterator it =
        m_stageData.m_itemMap.find(itemUID);
    // [b28d-solo] p167：ORIG 调 `operator!=`（ne）+ je 落失败块 ⇒ 正极翻转。
    if (it != m_stageData.m_itemMap.end())
    {
    item = it->second;                // ORIG 0x0814fb2f rep movsl 结构赋值
    if (item.m_item.m_addInfo == 0)
    {
        // 金币
        if (!GoldPickupItem(user, (unsigned int)itemUID, item))
            return 0;
        return 1;
    }

    CItem* script = G_CDataManager()->find_item(item.m_item.m_addInfo);
    if (script == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x57f, 5);
        trace("item not find - item_id : %d, itemUID : %d",
              item.m_item.m_addInfo, itemUID);
        return 0;
    }

    // ORIG 逐虚槽分类（结果仅副作用语义，后续未读取，忠实保留）：
    //   可堆叠 && GetItemType()==9 → 7；否则生物物品 → 7；头像物品 → 1。
    int routeKind = 0;
    if (script->is_stackable())
    {
        if (script->GetItemType() == 9)
            routeKind = 7;
    }
    else
    {
        if (script->IsCreatureItem())
            routeKind = 7;
        else if (script->IsAvatarItem())
            routeKind = 1;
    }
    // [b28d-solo] p167：ORIG 无 getParty 调用（0 处）⇒ m_party 直读（P0）。
    CParty* party = m_party;
    // [b28f 实验 A 轴/P0] ORIG 0x0814fbc0 段为寄存器直测（`mov $0x1,%eax; jmp;
    //   mov $0x0,%eax; test %al,%al; je`，无槽往返）⇒ 删具名 bool，条件内联。
    if (party->get_member_count() > 1 &&
        getLiveMemberCount() > 1 &&
        item.m_count != 0 &&
        (script->get_rarity() == 4 ||
         script->IsRoutingItem()))
    {
        if (!RoutingPickupItem(user, (unsigned int)itemUID, item))
            return 0;
        return 1;
    }

    outRarity = (char)script->get_rarity();
    // [b28f 实验 A 轴/P0] 同上（ORIG 同段形）。
    if (party->get_member_count() > 1 &&
        getLiveMemberCount() > 1 &&
        item.m_count != 0 &&
        (script->get_rarity() == 1 ||
         script->get_rarity() == 2 ||
         script->get_rarity() == 3))
    {
        if (!RandomPickupItem(user, (unsigned int)itemUID, item))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5ac, 5);
            trace("RandomPickupItem false");
            return 0;
        }
    }
    else
    {
        if (!NormalPickupItem(user, (unsigned int)itemUID, item))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5b4, 5);
            trace("NormalPickupItem false");
            return 0;
        }
    }
    return 1;
    }
    cMyTrace trace(__PRETTY_FUNCTION__, 0x56e, 5);
    trace("stageData_.item not find itemUID : %d", itemUID);
    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x2e);
    packet.put_byte(0);
    packet.put_byte(4);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

// ---- handleDropItem（ORIG 0x0814ff4a）----
// 丢弃：dropItem 成功 → 取 +0xa34 序号（-1 回执失败包）→ MapInfo::Add_Item →
// 0x28 广播包 → 0x32 成功回执给丢弃者；失败/越限 → 0x32 失败回执。
// [b28d-62] N82+P52：p167 EXTRA 1×getParty（ORIG 缓存 party 指针跨调用），
//   其余 0/0；280/280。ORIG 0x0814ff4a
// [b28d-solo-CBossStage] N82附封顶：m_party 直读 + 三目布局（ndiff 202→5）。
//   残差 5=ORIG -0x78 seq 具名副本槽（4.1 生命周期穿插 vs 4.4 分区，新增标量
//   即重排类对象槽，三形实测劣化 241）。
#include "CBossStage/handleDropItem.inc"
{
    if (e < 0)
        return 0;

    PacketGuard packet;
    packet.clear();
    map_item item;
    if (!dropItem(user, c, d, e, item))
    {
        packet.put_header(1, 0x32);
        packet.put_byte(0);
        packet.put_byte(0x11);
        packet.put_byte((int)(signed char)c);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    // [b28d-62 翻案] ORIG 0x0814ff5f..ff6e `mov 0xa34(%eax),%eax; mov %eax,[S];
    //   lea 0x1(%eax),%edx; mov %edx,0xa34(%eax)` = 后置 ++ 惯用形（拆两句
    //   多 1 条槽回读）⇒ 旧 N18a 封顶判词作废。
    int seq = m_stageData.m_dropItemCnt++;   // ORIG +0xa34（物品序号）
    if (seq == -1)
    {
        packet.put_header(1, 0x32);
        packet.put_byte(0);
        packet.put_byte(0x11);
        packet.put_byte((int)(signed char)c);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    // ORIG 0x0814ff4a+0x6b3 `mov -0x1c(%ebp),%eax; mov %eax,-0x78(%ebp)`：
    //   Add_Item 前把 seq 写入 item+0x04（m_itemIndex）⇒ 源码 `item.m_itemIndex = seq;`
    item.m_itemIndex = seq;
    m_stageData.Add_Item(item);

    packet.put_header(0, 0x28);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_short(a);
    packet.put_short(b);
    packet.put_short(item.m_itemIndex);   // ORIG 0x0815016e 读 item+0x04（-0x78）
    packet.put_int(item.m_item.m_addInfo);
    packet.put_byte((int)item.m_item.GetItemAttr());
    packet.put_int(item.m_item.get_add_info());
    packet.put_short((int)item.m_item.m_fieldb);
    packet.put_int(item.m_item.m_fieldd);
    packet.put_byte((int)item.m_item.m_amp.getAbilityType() & 0xff);
    packet.put_short((int)item.m_item.m_amp.getAbilityValue() & 0xffff);
    packet.put_packet(item.m_item);
    packet.put_byte(item.m_count ? 0 : 1);   // ORIG test/je 布局（==0→1 直落）
    packet.finalize(true);
    m_party->send_to_party(packet);          // ORIG 0x081502a1 m_party 直访问

    packet.clear();
    packet.put_header(1, 0x32);
    packet.put_byte(1);
    packet.put_byte((int)(signed char)c);
    packet.put_short(d);
    packet.put_int(e);
    packet.finalize(true);
    user->Send(packet);
    return 1;
}

// ---- handleKillMonster（ORIG 0x08150388）----
// 击杀：killMonsterErase → partyType==1（PowerWar）时 0x26 广播即返回 →
// KillMonsterGetExp → KillMonster 日志 → killMonsterDropItem。
// [b28d-62] N82：p167 EXTRA getParty/get_party_type 各 1（ORIG 缓存结果），
//   188/193。ORIG 0x08150388
// [b28d-solo-CBossStage] AE：m_party->m_battleField.m_bloodState 直读（消
//   getParty/get_party_type call）；内联 curExp/roleType（P0）；KillMonsterGetExp
//   返回类型 int→bool（P248 档①，.h 同步，mangled 不变）。ndiff 131→0。
#include "CBossStage/handleKillMonster.inc"
                                  unsigned short b, const MSG_MONSTER_DIE& die,
                                  int* out)
{
    (void)out;   // ORIG 未使用第 5 参
    map_monster mob;
    int eraseCnt = 0;
    if (!killMonsterErase(a, mob, eraseCnt))
        return 0;

    // P0：ORIG 0x081503f5 `mov 0x4(%eax),%eax; mov 0xcd8(%eax),%eax` ⇒ 原码为
    //   m_party 直访问（getParty/get_party_type 均未发射调用）。
    if (m_party->m_battleField.m_bloodState == 1)   // PowerWar
    {
        PacketGuard packet;
        packet.put_header(0, 0x26);
        packet.put_short((int)a);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.finalize(true);
        m_party->send_to_party(packet);
        return 1;
    }

    unsigned int exp = 0;
    if (!KillMonsterGetExp(mob, exp))
        return 0;

    // cUserHistoryLog 为 CUser 嵌套成员 m_historyLog（ORIG user+0x79700）
    user->m_historyLog.KillMonster(mob.m_mobIndex,
                      die.m_killValue1,   // +0xa1e（耗时类）
                      die.m_killValue2,   // +0xa1a（经验类）
                      die.m_killPairA24,
                      die.m_killPairA26,
                      die, mob.m_roleType, mob.m_roleType == 5,
                      user->getCurCharacExp(), (int)exp,
                      (eExpAddReason)0xb);

    if (!killMonsterDropItem(mob, eraseCnt, a, b))
        return 0;
    return 1;
}

// ============================================================================
// 非虚方法（逐函数按 docs/class_func_reports/WongWork__CBossStage/ 报告转写）
// 字段速查（均已具名，偏移仅作对照）：
//   m_dungeon(+0x9f0)  m_stageData(MapInfo@+0x9f4；m_mapIndex+0x9f8、
//     m_assignItemIdx+0xa30、m_dropItemCnt+0xa34、m_mapBasisLevel+0xa3c)
//   m_stageCur(+0xae0)  m_stageMax(+0xae4)  m_difficulty(+0xae8)
//   m_expParamA(+0xaec)  基类 CBossPlay::m_useItemCount[4](+0x9c8)
// ============================================================================

// ---- moveNextStage（ORIG 0x0814d1b6）----
// 怪物清空（monsterMap.empty()）且 m_stageCur < m_stageMax（无符号比较）
// 时 m_stageCur+=1 进入下一层。
// [N18a 封顶] 残差仅 0x814d1dd `mov 0xae0(%eax),%edx` 单步直落；4.4 恒折
// `mov %eax; mov %eax,%edx` 两步（N18a：单段 regmove 不可复现），其余 27 条逐条同形。
// [b28d-62] AE：删同型冗余 cast（P41 变体：cast 致 `mov %eax,%edx` 中转）后
//   28=28，p167 CLEAR。ORIG 0x0814d1b6
#include "CBossStage/moveNextStage.inc"
{
    if (!m_stageData.m_monsterMap.empty())
        return false;
    // ORIG 0x0814d1ee `jb` 跳增块、mov$0 fall-through：条件写作 >= 早退布局。
    // [b28d-62] 删同型冗余 cast：ORIG 0x814d1dd `mov 0xae0(%eax),%edx` 单段直落；
    //   `(unsigned int)` cast 使 4.4 多一条 `mov %eax,%edx` 中转（P41 变体）。
    if (m_stageCur >= m_stageMax)
        return false;
    m_stageCur += 1;
    return true;
}

// ---- randomMapIndex（ORIG 0x0814f4fc）----
// acc/roll 先置零；栈上 vector 拷贝后遍历；getRandomTowerMapIndexes 在
// find 与 end 处各调用一次（ORIG 两次外呼）。
// [b28d-62] N82：p167 CLEAR，109/112。ORIG 0x0814f4fc
// [b28d-solo-CBossStage] AE：P0 去 receiver 指针局部 + const 视图内联调 const
//   find（消转换构造 call）；stageCur 声明/赋值分离复现槽序。ndiff 58→0。
#include "CBossStage/randomMapIndex.inc"
{
    int acc = 0;
    int roll = 0;
    int stageCur;
    std::map<int, std::vector<RandomList> >::const_iterator it;
    std::vector<RandomList> vec;
    stageCur = m_stageCur;                            // +0xae0
    // P0：ORIG 无 receiver 指针槽往返 ⇒ getRandomTowerMapIndexes() 内联两次；
    // ORIG 调 const 版 find（_ZNKSt3map…4findERKi）⇒ receiver 按 const 视图内联。
    it = static_cast<const std::map<int, std::vector<RandomList> >*>(
             m_dungeon->getRandomTowerMapIndexes())->find(stageCur);
    if (it == static_cast<const std::map<int, std::vector<RandomList> >*>(
                  m_dungeon->getRandomTowerMapIndexes())->end())
        return false;
    vec = it->second;
    roll = get_rand_int(10000);
    for (unsigned int i = 0; i < vec.size(); ++i)
    {
        acc += vec[i].m_weight;
        if (roll < acc)
        {
            index = vec[i].m_mapIndex;
            break;
        }
    }
    return true;
}

// ---- killMonsterErase（ORIG 0x0814d214）----
// find(uid) → const_iterator 判 end() → mob=it->second、outCount=(uchar)+0x14、
// erase(by key) → true；未找到 false。
// [b28d-62] N82：erase key 改内联 `(int)uidParam`（新临时槽）后 68=68；残差仅
//   uid 与 find-sret 槽互换（P112 槽序不可推）。p167 CLEAR。ORIG 0x0814d214
// [b28d-solo-CBossStage] AE：found 具名 iterator 中转（P50 复活）消 uid/find-sret
//   槽互换；erase key 保持内联 (int)uidParam。ndiff 8→0。
#include "CBossStage/killMonsterErase.inc"
                                  int& outCount)
{
    int uid = uidParam;                           // ORIG -0x18 key 槽（int 承接）
    // [b28d-62] erase 的 key 用内联 `(int)uidParam`（ORIG 0x0814d2d3 从 -0x2c 形参
    //   截断槽重算并落**新槽** -0xc），非复用 uid 槽；find/end 保持直构形
    //   （先声明后赋值实测 68→78 劣化，P50 复活不适用本件）。
    std::map<int, map_monster>::iterator found =
        m_stageData.m_monsterMap.find(uid);
    std::map<int, map_monster>::const_iterator it = found;
    if (it != m_stageData.m_monsterMap.end())
    {
        mob = it->second;                             // map_monster::operator=
        outCount = (unsigned char)it->second.m_level;   // ORIG pair+0x14 = mob+0x10
        m_stageData.m_monsterMap.erase((int)uidParam); // by-key erase
        return true;
    }
    return false;
}

// ---- KillMonsterGetExp（ORIG 0x0814d300）----
// 对每名有效队员：get_mob_reward → getMonsterTotalExp → 死亡之塔两层费率 →
// sumGainedEachExp → calcHelpAbuseRatio → gain_exp_sp(+Noti) → CheckQuestMonster。
// [b28d-62] N82：p167 EXTRA getParty/get_party_type 各 1（ORIG 缓存结果）。
//   ORIG 0x0814d300
// [b28d-solo-CBossStage] N82封顶：exp 写回/getMonsterTotalExp + float 截断费率
//   （原 double+0.5 语义错）+ m_party/m_dungeon 直读 + m_expParamAE(P254)。
//   残差 44=4.4 float 浅槽 vs ORIG 深槽（标量分区，N82附）。
#include "CBossStage/KillMonsterGetExp.inc"
{
    // P0：ORIG 0x0814d308/0x0814d5da `mov 0x4(%eax),%eax` / `mov 0x9f0(%eax),%eax`
    //   ⇒ m_party/m_dungeon 直访问（无 getParty() 调用、无 party/dungeon 槽）；
    //   0x0814d3bf 起各段逐次重读 mob.m_roleType（无 roleType 具名局部）。
    // ORIG 0x0814d316 返回值落 -0x1c 死槽（P171 照抄，后续无读取）
    // [b28d-solo] ORIG float expF/expF2 在深槽 -0x50/-0x4c、块内 data 在浅槽
    //   -0x24；4.4 恒聚合对象占低槽、标量分区（N82附）。
    // [b28f-SweepCBossStage 三形实测复核] ①本形（expF/expF2 函数头）: AE 差 12 块
    //   （our 226）；②标量全头部声明（sp/sfp/reason 提到头）: our 226→229 条、
    //   槽阶梯全乱（ORIG `movl $0xb,-0x10` 被我方落 -0x1c）⇒ 劣化回滚；
    //   ③expF/expF2 移入 for 体内: AE 差 18 块（our -0x1c/-0x18）⇒ 劣化回滚。
    //   ⇒ 4.4 在本件按「先用者浅」分区，与 ORIG 4.1「先用者深」方向相反，
    //   声明位置不可翻（P212 反向 + N82附），封顶。p167 CLEAR（仅 C1/C2 别名）。
    // [K2ERandomBoss 2026-09-11 负结论] 前置 `char framePad[0x2c]`（P232 撑帧）
    // 试验：帧 sub 0x60→**0x90**（ORIG 0x70，方向相反）、差异块 38/差异行 75
    // **逐值不变** ⇒ 该旋钮不改 float 组槽序，已回滚（ORIG 0x0814d425 fstps
    // -0x50 / 0x0814d484 fstps -0x4c 为 4.1「先用者深」分区，4.4 反向）。
    float expF;
    float expF2;
    int memberLevelGap = m_party->getMemberLevelGap();
    (void)memberLevelGap;

    for (int i = 0; i < 4; ++i)
    {
        CUser* user = 0;             // ORIG 0x0814d325 预清（P198①）
        user = m_party->get_user(i);
        if (user == 0 || !m_party->checkValidUser(i))
            continue;
        if (m_party->m_battleField.m_bloodState == 1)   // PowerWar
            continue;

        G_CDataManager()->get_mob_reward(user->get_charac_level(), exp);
        // ORIG 0x0814d402 返回值直接写回 exp（无 total 具名局部）
        exp = m_party->getMonsterTotalExp(
            exp, mob.m_roleType, m_expParamAE, 1.0f, 1.0f, false);

        // 死亡之塔费率（ORIG 0x0814d422 fildll+fstps / 0x0814d44c flds+fmuls+
        //   fistpl：float 运算 + 截断，无 +0.5 舍入；+0x00=apcExpWeight、
        //   +0xc84=monsterExpWeight）。
        if (mob.m_roleType == 5)
        {
            expF = (float)exp;
            exp = (unsigned int)(int)(G_CDataManager()->GetTowerScript(
                m_dungeon->get_index())->m_apcExpWeight * expF);
        }
        expF2 = (float)exp;
        exp = (unsigned int)(int)(G_CDataManager()->GetTowerScript(
            m_dungeon->get_index())->m_monsterExpWeight * expF2);

        STGainedEachExpData data;                 // ORIG ctor W 0x08151a56
        data.m_exp = exp;
        exp = m_party->sumGainedEachExp(user, data);
        user->calcHelpAbuseRatio(exp, false);

        int sp = 0;
        int sfp = 0;
        int reason = 0xb;            // ORIG 0x0814d532/0x0814d544 if 形非三目
        if (mob.m_roleType == 5)
            reason = 0xc;
        if (user->gain_exp_sp((int)exp, sp, sfp, (CUser::eExpAddReason)reason,
                              mob.m_mobIndex, false))
            user->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);

        ENUM_QUEST_ENEMY_TYPE questType = (ENUM_QUEST_ENEMY_TYPE)1;
        if (mob.m_roleType > 4 && mob.m_roleType <= 8)
            questType = (ENUM_QUEST_ENEMY_TYPE)2;
        user->CheckQuestMonster(m_dungeon->get_index(), mob.m_mobIndex,
                                questType);
    }
    return true;
}

// ---- SendMapInfoPacket（ORIG 0x0814ce12）----
// [b28d-62] 修: P96 迭代器改非 const（消转换 ctor ×4 + const 版 operator 族，
//   263→243）；余 N82 帧/槽差（sub 0x3dc vs 0x380）+ getParty 缓存。ORIG 0x0814ce12
// [b28d-solo-CBossStage] N104：补 put_short(m_fieldb) + m_addInfo 改 put_int
//   （真 bug：原漏写包字段）+ map_item 栈拷贝 + m_stageData 直读。残差 6 条=
//   EH 尾块相对序（4.4 就地内联 vs 4.1 尾块）。
#include "CBossStage/SendMapInfoPacket.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x1e2);
    packet.put_short(m_stageCur);                          // 当前层
    packet.put_int(m_stageData.m_mapBasisLevel);           // seed 值（ORIG +0xa3c）
    packet.put_short(m_stageData.m_mapIndex);              // 当前图索引

    // P0：ORIG 0x0814ce5e `mov 0x8(%ebp),%eax; add $0xa00,%eax` ⇒ m_stageData
    //   直访问（无 MapInfo& 引用槽）。
    packet.put_byte((int)m_stageData.m_monsterMap.size());
    // [b28d-62] P96：ORIG 用非 const `_Rb_tree_iterator`（p167 MISSING ptEv/*++/!= 族）
    //   ⇒ 改 iterator，消 const_iterator 转换 ctor ×4 与 const 版 operator 族。
    // [b28d-solo] ORIG 0x0814cf8f..cf9a 的 `++it` 在循环体尾（回边前）、
    //   D1/end/ne 在条件块（0x0814cfb7..cfed）⇒ 源形 while + 体尾 ++it。
    std::map<int, map_monster>::iterator it = m_stageData.m_monsterMap.begin();
    while (it != m_stageData.m_monsterMap.end())
    {
        map_monster mob(it->second);               // ORIG 栈拷贝后逐字段写包
        packet.put_int(mob.m_mobId);               // +0x00
        packet.put_short((int)(short)mob.m_instId);       // +0x04
        packet.put_short(mob.m_mobIndex);                 // +0x0c（ORIG 无 cwtl）
        packet.put_byte((int)(unsigned char)mob.m_level); // +0x10
        packet.put_byte((int)(signed char)mob.m_roleType);// +0x08
        packet.put_byte((int)(unsigned char)mob.m_field13);
        packet.put_byte((int)(unsigned char)mob.m_field14);
        ++it;
    }

    packet.put_byte((int)m_stageData.m_itemMap.size());
    for (std::map<int, map_item>::iterator it = m_stageData.m_itemMap.begin();
         it != m_stageData.m_itemMap.end(); ++it)
    {
        // ORIG 0x0814d044..058 `rep movsl` 0x15 dword ⇒ 栈上整体拷贝非引用
        map_item item(it->second);
        // +0x01 等级字节（ORIG movzbl+movsbl 后 put_byte：0814d05a）
        packet.put_byte((int)(signed char)(unsigned char)item.m_standardLevel);
        // [b28f-SweepCBossStage] 真 bug：ORIG 0x0814d083 `mov -0x84(%ebp),%eax` →
        //   call 080d9ea4（put_short），非 put_int（080cb93c）。原写 put_int 令
        //   0x1e2 包 itemIndex 后所有字段错位 2 字节（p167: put_short −1 / put_int +1）。
        packet.put_short(item.m_itemIndex);                // +0x04
        packet.put_int(item.m_item.m_addInfo);             // ORIG 0x0814d088 32 位读 -0x76
        packet.put_int(item.m_item.get_add_info());        // Inven_Item+7
        packet.put_short((int)item.m_item.m_fieldb);       // ORIG 0x0814d0ba -0x6d（Inven_Item+0xb）
        packet.put_byte((int)item.m_item.m_amp.getAbilityType() & 0xff);
        packet.put_short((int)item.m_item.m_amp.getAbilityValue() & 0xffff);
    }
    packet.finalize(true);
    m_party->send_to_party(packet);
    return 1;
}


// ---- GoldPickupItem（ORIG 0x0814f0e6）----
// 金币按存活队员平分（拾取者拿余数），逐槽写 0x27 包并 erase 掉落物。
// [b28d-62] N82：p167 EXTRA isMemberAlive/getParty 各 1（ORIG 缓存结果）。
//   ORIG 0x0814f0e6
// [b28d-solo-CBossStage] AE：m_memberAlive 内联(P119) + m_party 直读 +
//   P226 语句表达式消 valid 槽 + baseShare 副本 + member=0 预清(P198①)。
#include "CBossStage/GoldPickupItem.inc"
                                const map_item& item)
{
    int liveCnt = 0;
    for (int i = 0; i < 4; ++i)
    {
        // P0/P119：ORIG 0x0814f0f6 `movzbl 0xd(%edx,%eax,1),%eax` 内联读
        //   m_memberAlive[i]（isMemberAlive 为头内 inline，ORIG 未发射调用）。
        if (m_memberAlive[i])
            ++liveCnt;
    }

    PacketGuard packet;
    packet.clear();
    if (liveCnt == 0)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(0x15);
        packet.finalize(true);
        user->Send(packet);
        return false;
    }

    int gold = item.m_item.get_add_info();        // map_item+0x10 Inven_Item
    int share = gold / liveCnt;
    int rem = gold % liveCnt;
    int bonus = share + rem;                      // 拾取者份额
    int baseShare = share;                        // ORIG 0x0814f1e5 -0x20 副本

    packet.put_header(0, 0x27);
    packet.put_short((int)uid);
    packet.put_short((int)(user->get_unique_id() & 0xffff));
    // P0：ORIG 0x0814f248/0x814f266 `mov 0x4(%eax),%eax` ⇒ m_party 直访问。
    for (int i = 0; i < 4; ++i)
    {
        CUser* member = 0;                     // ORIG 0x0814f241 预清（P198①）
        member = m_party->get_user(i);
        // P226：ORIG 0x0814f260..28d 短路 bool 物化无槽（mov$1/jmp/mov$0+统一
        //   test）⇒ 语句表达式包住条件，避免 4.4 把具名 bool 落槽。
        if (({ member != 0 && m_party->checkValidUser(i); }))
        {
            packet.put_byte(1);
            int amount = (member == user) ? bonus : baseShare;
            int gained = member->getCurCharacInvenW()->gain_money(
                amount, (eMoneyAddReason)4, true, 0);
            if (gained < amount)
            {
                if (gained < 0)
                    gained = 0;
                member->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0,
                                            (unsigned int)amount,
                                            (unsigned int)gained);
            }
            packet.put_int(gained);
            packet.put_byte(1);
            packet.put_int(0);
            packet.put_int(0);
        }
        else
        {
            packet.put_byte(0);
            packet.put_int(0);
        }
    }
    m_stageData.m_itemMap.erase((int)uid);
    packet.finalize(true);
    m_party->send_to_party(packet);
    return true;
}

// ---- RoutingPickupItem（ORIG 0x0814e57a）----
// [b28d-62] N82：p167 CLEAR。ORIG 0x0814e57a
// [b28d-solo-CBossStage] AE：删具名 bool failed（P226/P158）；`while (ret != 0)
//   {…return false;}` 复现 ORIG setne;test;je 物化（ndiff 33→0）。
#include "CBossStage/RoutingPickupItem.inc"
                                   map_item& item)
{
    CSpecialItemRoutingManager* mgr =
        G_CGameManager()->GetSpecialItemRoutingManager();

    int ret = 0;
    ret = mgr->StartRouting(user, item.m_item, uid);
    while (ret != 0)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.finalize(true);
        user->Send(packet);
        return false;
    }
    return true;
}


// ---- NormalPickupItem（ORIG 0x0814ecec）----
// [b28d-62] N82：p167 CLEAR。ORIG 0x0814ecec
// [b28d-solo-CBossStage] 封顶 N7+N82：user==0 正向包裹（P222，ORIG je 814f0d5）、
//   isAvatarItemType 正极+avatar 直落（P168a③b）后 98→37；残差=N7 虚调用
//   %edx↔%eax ×3（12 条）+ agency/PacketGuard 槽 -0x34/-0x40↔-0x40/-0x4c
//   （4.4 聚合优先低槽，N82附）。p167 CLEAR。
#include "CBossStage/NormalPickupItem.inc"
                                  const map_item& item)
{
    // [b28d-solo] ORIG 0x0814ecfb `cmpl $0,0xc(%ebp); je 814f0d5`（共享尾失败块
    //   直落 epilogue）⇒ P222 正向包裹（4.4 早返恒 `jne; mov $0,%ebx; jmp`）。
    if (user != 0)
    {
    const CItem* script = G_CDataManager()->find_item(item.m_item.m_addInfo);
    if (script == 0)
        return false;

    // ORIG 逐虚槽判定（is_stackable + GetItemType==9 / IsCreatureItem / IsAvatarItem）
    int routeKind = 0;
    if (script->is_stackable())
    {
        if (script->GetItemType() == 9)
            routeKind = 7;
    }
    else
    {
        if (script->IsCreatureItem())
            routeKind = 7;
        else if (script->IsAvatarItem())
            routeKind = 1;
    }
    int slot = -1;
    // [b28d-solo] ORIG 0x0814eeaf `call isAvatarItemType; test %al; je <insert>`
    //   正极形 + avatar 块直落（4.4 对 `!cond` 生 xor $1，P168a③b）。
    if (item.m_item.isAvatarItemType())
    {
        char agency[24];
        memset(agency, 0, sizeof(agency));
        WongWork::CGenUniqueNo::genIPGNo(
            (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5,
            (unsigned int)user->getCurCharacNo(), agency);
        // 第三参取 map_item+0x1b = m_item.m_fieldb 低字节（ORIG movzwl+movsbl 内联）
        slot = user->addAvatarItem((unsigned long)item.m_item.m_addInfo, 0,
                                   (char)(unsigned char)item.m_item.m_fieldb, agency,
                                   (eAvatarItemAddReason)0x10);
    }
    else
    {
        slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            item.m_item, (eItemAddReason)4, true, true);
    }
    if (slot == -1)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(4);
        packet.finalize(true);
        user->Send(packet);
        return false;
    }

    m_stageData.m_itemMap.erase((int)uid);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x27);
    packet.put_short((int)uid);
    packet.put_short((int)(user->get_unique_id() & 0xffff));
    for (int i = 0; i < 4; ++i)
        packet.put_byte(0);                        // 无掷点
    packet.put_short((int)(user->get_unique_id() & 0xffff));
    packet.put_short(slot);
    packet.put_byte(routeKind);
    packet.finalize(true);
    m_party->send_to_party(packet);
    return true;
    }
    return false;
}

// ---- RandomPickupItem（ORIG 0x0814e684）----
// 存活且背包可容纳的成员各掷 1..100，最高者获得物品；广播含四人掷点。
// [b28d-62] N82：p167 CLEAR。ORIG 0x0814e684
// [b28d-solo-CBossStage] 封顶 N7/N122/SAVE_EXPR/N82：bestRoll int→char（ORIG
//   -0x29 `cmp mem,%al; jle` 字节比较）、m_party 直读（消 getParty 局部槽）、
//   member 预清(P198①)、短路 `member==0||!checkValid`(P224)、winner!=0 正向
//   包裹(P222) 后 258→175；残差=N7×3(12 条)+N122 xor 1+4.4 SAVE_EXPR 复用
//   rolls[i]（ORIG 两次独立读）+槽/帧分区。p167 CLEAR。
#include "CBossStage/RandomPickupItem.inc"
                                  const map_item& item)
{
    // [b28d-solo] ORIG 0x0814e68f..e6a1 清零序：rolls(-0x44 movl)、bestRoll
    //   （-0x29 **char**：`cmp -0x29,%al; jle` 有符号字节比较）、winner(-0x28)、
    //   i(-0x24)；ORIG 全程 m_party 直读（无 party 局部槽，P0）。
    char rolls[4] = { 0 };
    char bestRoll = 0;
    CUser* winner = 0;
    for (int i = 0; i < 4; ++i)
    {
        CUser* member = 0;              // ORIG 0x0814e6ad 预清（P198①）
        member = m_party->get_user(i);
        // [b28d-solo] ORIG 0x0814e6b8..e6d0：member==0 与 !checkValidUser 各自
        //   跳 continue（P224 短路极性内联直测）。
        if (member == 0 || !m_party->checkValidUser(i))
            continue;
        int trySlot =
            member->getCurCharacInvenR()->tryInsertItemIntoInventory(item.m_item);
        if (trySlot != -1 && isMemberAlive(i))
        {
            rolls[i] = (char)(get_rand_int(100) + 1);
            // [b28d-solo] ORIG 0x0814e7d0..e7e5 两次独立读 rolls[i]（P157 去缓存）
            //   ⇒ 书写序 rolls[i] > bestRoll。
            if (rolls[i] > bestRoll)
            {
                bestRoll = rolls[i];
                winner = member;
            }
        }
        else
        {
            rolls[i] = 0;
        }
    }

    // [b28d-solo] ORIG 0x0814e80e `cmpl $0,-0x28; je <失败包 814ec36>`（失败包在
    //   主流程后）⇒ 正极包裹（P222）。
    if (winner != 0)
    {
    // [R2PrettAClass 2026-09-11 · prett A 类落地] 同上：手写串已删，内联 __PRETTY_FUNCTION__。
    //   ORIG .rodata @0x08b75e80 size 0x53 = "bool WongWork::CBossStage::RandomPickupItem("
    //   "CUser*, unsigned int, const map_item&)"（非虚、非 static ⇒ 宏无前缀）。
    const CItem* script = G_CDataManager()->find_item(item.m_item.m_addInfo);
    if (script == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x39c, 5);
        trace("item not find : %d", item.m_item.m_addInfo);
        return false;
    }

    // ORIG 逐虚槽判定（is_stackable + GetItemType==9 / IsCreatureItem / IsAvatarItem）
    int routeKind = 0;
    if (script->is_stackable())
    {
        if (script->GetItemType() == 9)
            routeKind = 7;
    }
    else
    {
        if (script->IsCreatureItem())
            routeKind = 7;
        else if (script->IsAvatarItem())
            routeKind = 1;
    }
    int slot = -1;
    if (!item.m_item.isAvatarItemType())
    {
        slot = winner->getCurCharacInvenW()->insertItemIntoInventory(
            item.m_item, (eItemAddReason)4, true, true);
    }
    else
    {
        char agency[24];
        memset(agency, 0, sizeof(agency));
        WongWork::CGenUniqueNo::genIPGNo(
            (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5,
            (unsigned int)winner->getCurCharacNo(), agency);
        char periodByte = (char)(unsigned char)item.m_item.m_fieldb;
        slot = winner->addAvatarItem((unsigned long)item.m_item.m_addInfo, 0,
                                     periodByte, agency,
                                     (eAvatarItemAddReason)0x10);
    }
    if (slot == -1)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(4);
        packet.finalize(true);
        winner->Send(packet);
        return false;
    }

    m_stageData.m_itemMap.erase((int)uid);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x27);
    packet.put_short((int)uid);
    packet.put_short((int)(user->get_unique_id() & 0xffff));
    for (int i = 0; i < 4; ++i)
        packet.put_byte((int)rolls[i]);
    packet.put_short((int)(winner->get_unique_id() & 0xffff));
    packet.put_short(slot);
    packet.put_byte(routeKind);
    packet.finalize(true);
    m_party->send_to_party(packet);
    return true;
    }
    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x2e);
    packet.put_byte(0);
    packet.put_byte(4);
    packet.finalize(true);
    user->Send(packet);
    return false;
}

// ---- dropItem（ORIG 0x0814de1a）----
// 按 invenType 取出物品构建掉落 map_item，再从原容器删除并记流通统计。
// type：0=背包/金币、2=仓库、3=装备栏、7=造物。
// [b28d-62] N82：p167 CLEAR。ORIG 0x0814de1a
// [b28d-solo-CBossStage] N82：p167 CLEAR。if 链改 switch(type)（P208 决策树）。
//   残差=槽/帧分区 + case 0 金币块内联结构（ORIG 金币输出在 case 0 内并 jmp 删除段）。
// [b28d-solo-CBossStage] 封顶 N-switch跳表+N76：case 序改 0,2,3,7（P46 叶块物理
//   序）、case 0 内嵌金币块+公共段只判 tmp.m_addInfo（P222）、attachType 改
//   switch（跳表 0x8b7511c）且 case3 用 `!tmp.m_field0`（P248 档②）、删除段改
//   switch+去三目+deleted bool（P158）、m_amp 按 guid/head 视图序、out+0x00 写
//   m_count 后 169→12；残差=4.4 对 6-case 用二分树 vs ORIG 跳表（跨版本 switch
//   展开，N）+m_amp0Int store 序（this-first vs value-first，P211 旋钮已穷举）。
//   p167 CLEAR。
#include "CBossStage/dropItem.inc"
                          map_item& out)
{
    Inven_Item tmp;                               // ORIG 栈 Inven_Item
    // P208：ORIG 0x0814de2? 决策树 `movsbl -0x6c; cmp $2; je; cmp $2; jg; test; je;
    //   jmp; cmp $3; je; cmp $7; je` ⇒ 源形 switch(type)（if 链恒产 cmpb 直比）。
    switch (type)
    {
    case 0:
        // [b28d-solo] ORIG 0x0814de67..deb0：case 0 内 `slot==0` 直落金币输出块
        //   （jmp 814e1c1 跳过 GetInvenSlot），else 才取槽（P222 正向包裹）。
        if (slot == 0)
        {
            out.m_count = 0;              // byte +0x00 = 0
            out.m_item.m_field1 = 2;              // byte +0x11 = 2
            out.m_item.m_addInfo = 0;             // dword +0x12 = 0
            out.m_item.m_fieldb = 0;              // word +0x1b = 0
            out.m_item.set_add_info(count);
            out.m_item.ResetItemAttr();
        }
        else
            tmp = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, slot);
        break;
    case 2:
        tmp = user->getCurCharacCargoR()->get_cargo_slot(slot);
        break;
    case 3:
        tmp = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)0, slot);
        break;
    case 7:
        tmp = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)3, slot);
        if ((unsigned char)tmp.m_field1 == 5)     // ORIG +1 字节==5 拒绝
            return false;
        break;
    default:
        return false;
    }

    int sellPrice = 0;                            // ORIG local_20
    // [b28d-solo] ORIG 0x0814e1c1..e1cd 公共段只判 tmp.m_addInfo==0（无 type/slot
    //   条件；case 0/slot==0 的金币输出已在 case 内完成）。
    // [b28d-solo] ORIG 0x0814e1c8 `mov -0x57(%ebp),%eax`（disp 直读 tmp+2）；
    //   指针 cast 解引用恒多 lea+add 两条。
    // [b28d-solo] ORIG 0x0814e1cd `je 814e360`（金币块在后）⇒ 主逻辑走 then、
    //   金币块落 else（P272 块序交换）。
    if (tmp.m_addInfo != 0)                       // dword@tmp+2 != 0 → 构建掉落物
    {
        const CItem* script =
            G_CDataManager()->find_item(tmp.m_addInfo);
        if (script == 0)
            return false;

        // ORIG 0x0814e212 跳表前为 `mov [S],%eax; cmp $0x5,%eax; ja; mov
        //   <A>(,%eax,4),%eax`（值始终在 %eax，无栈槽往返）⇒ P251/P261：
        //   原码是 `switch (script->GetAttachType())` 直切 call 结果；写成具名
        //   `int attachType=…; switch(attachType)` 在 4.4 必产 `cmpl $0x5,slot`
        //   + shl 索引（P209/N94 实测改类型无效）。
        // [b28d-solo] ORIG 0x0814e212 跳表（0x8b7511c：case1/2/5→814e21b 返 false；
        //   case3→814e225 查 tmp+0 字节；case0/4/default→814e23a 主流程）⇒ switch。
        switch (script->GetAttachType())
        {
        case 0:
            break;
        case 1:
        case 2:
            return false;
        case 3:
            if (!tmp.m_field0)
                return false;
            break;
        case 4:
            break;
        case 5:
            return false;
        default:
            break;
        }

        if (tmp.isEquipableItemType())
        {
            // ORIG 读装备对象 +0x234 int = CEquipItem::m_itemType（CEquipItem.h）
            const CEquipItem* equip =
                static_cast<const CEquipItem*>(script);
            if (equip->m_itemType == 0xb || equip->m_itemType < 10)
                return false;
        }

        // 构建 out（逐字段按 asm 宽度；无 memset，ORIG 逐字段写）
        out.m_count = 0;                          // byte +0x00 = 0
        out.m_item.m_field1 = tmp.m_field1;               // byte +0x11
        out.m_item.m_addInfo = tmp.m_addInfo;             // dword +0x12
        out.m_item.m_fieldd = tmp.m_fieldd;               // dword +0x1d
        // [b28d-solo] ORIG 0x0814e29a..e2b4 拷贝序 = amp+0x04/0x08/0x0c/0x00
        //   （guid 视图先、头部 int 后）⇒ 分两次具名视图赋值（P235/P247）。
        out.m_item.m_amp.m_guid = tmp.m_amp.m_guid;       // +0x04..0x0d
        out.m_item.m_amp.m_amp0Int = tmp.m_amp.m_amp0Int; // +0x00 四字节
        if (!tmp.isEquipableItemType())
            out.m_item.set_add_info(count);               // 非装备用丢弃数量
        else
            out.m_item.set_add_info(tmp.m_addInfo2);      // 装备沿用原 add_info2
        out.m_item.SetItemAttr(tmp.GetItemAttr());
        out.m_item.m_fieldb = tmp.m_fieldb;               // word +0x1b
        out.m_item.m_field0 = tmp.m_field0;               // byte +0x10
        G_Store()->GetSellItemPrice(
            const_cast<Inven_Item&>(tmp), script, (short)count, false,
            sellPrice);
    }
    else
    {
        out.m_count = 0;                  // byte +0x00 = 0
        out.m_item.m_field1 = 2;                  // byte +0x11 = 2
        out.m_item.m_addInfo = 0;                 // dword +0x12 = 0
        out.m_item.m_fieldb = 0;                  // word +0x1b = 0
        out.m_item.set_add_info(count);
        out.m_item.ResetItemAttr();
    }

    // 删除段
    // [b28d-solo] ORIG 0x0814e441..446 `movzbl -0x15; xor $1; test; je <成功>`
    //   ⇒ P158 bool 汇聚变量 + 取反早退。
    bool deleted = false;
    // [b28d-solo] ORIG 0x0814e3a7..e3d4 与入口同形决策树（cmp $2/$0/$3/$7）⇒
    //   删除段也是 switch(type)，case 书写序 0,2,3,7（叶块物理序）。
    switch (type)
    {
    case 0:
        if (slot == 0)
            deleted = (char)(user->getCurCharacInvenW()->use_money(
                count, (eMoneySubReason)6, true));
        else
            deleted = (char)(user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, slot, count, (eItemDelReason)6, true));
        break;
    case 2:
        deleted = (char)(user->getCurCharacCargoW()->delete_item(
            slot, count, (eItemDelReason)0x10));
        break;
    case 3:
        deleted = (char)(user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)0, slot, 1, (eItemDelReason)6, true));
        break;
    case 7:
        deleted = (char)(user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)3, slot, 1, (eItemDelReason)6, true));
        break;
    default:
        return false;
    }

    if (!deleted)
        return false;
    if (type == 0 && slot == 0)
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0x17, user, (unsigned int)count);
    else
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)sellPrice);
    return true;
}

// ---- killMonsterDropItem（ORIG 0x0814d628）----
// 怪物/APC 掉落生成 → 逐件构建 map_item（count=1、dropIndex=b、
// createTick=当前秒）入 MapInfo → 0x26 包广播。
// [b28d-62] N89+N84：const_iterator 族/put_binary(char*) 签名/memcpy + getParty×3
//   缓存差。ORIG 0x0814d628
// [b28d-solo-CBossStage] N82：p167 CLEAR（0/0）。补 newItems.push_back（真 bug：
//   原 newItems 恒空 ⇒ 0x26 包不广播物品）+ put_binary PKc 重载 + 结构赋值替
//   memcpy + P13 end 缓存（两个转换构造）+ G_CDataManager/m_party/m_dungeon 直读。
//   残差=帧/槽分区（527 条超大函数）。
// [b28d-solo-CBossStage] 封顶 N82/N7：AI 分支前置（ORIG 0x0814d699 `jle` AI 直落、
//   mon 跳转，P47）、ref.m_field3e_byte 字节视图（消 and/or 4 条）、ref 写序
//   rateSlot→field34、hasTowerItemDrop 调用点 2 条形（CDungeon.h bool，已 R2 广播）
//   后 327→224；残差=槽/帧分区（ORIG 帧 0x21c vs 0x20c）+ map_monster 构造次数
//   （ORIG 复用 -0xf50 单对象）+N7。p167 CLEAR。
// [b28d-62 本轮] 修 5 处真缺口：m_item 赋值先于 m_itemIndex、m_dropItemCnt 后置++
//   （0x0814d95b 复用 eax）、Add_Item 先于 push_back、countIdx/itemCount 预清 0
//   （P198①）、put_byte(itemCount) 非立即数、isAvatarItemType 两处正极性、
//   period/pEquip 具名局部（0x0814db51/db58）⇒ 527/527 slotnorm 0 块，DIFF→NEAR；
//   残差仅槽号分区（N82/N126）。
#include "CBossStage/killMonsterDropItem.inc"
                                     unsigned short a, unsigned short b)
{
    std::vector<unsigned long> specifyDrops;      // 塔指定掉落（ORIG 收集后未再读取）
    std::list<map_item> newItems;
    stGenerateResult_t result;

    // [b28f-SweepCBossStage] ORIG 0x0814d648..d68b 三段序：三个 C1（-0x50/-0x58/-0xc8）
    //   全部发完**之后**才是三个 clear ⇒ 原码是先声明三对象、再统一 clear；
    //   原「声明+clear 交错」形被 4.4 输出 ctorX,clearX,ctorY,… 与 ORIG 错位。
    specifyDrops.clear();
    newItems.clear();
    result.m_dropItems.clear();

    // P0/p167：ORIG 直调 G_CDataManager()（MISSING ×1）与 m_party（EXTRA getParty ×3）
    // [b28d-solo] ORIG 0x0814d699 `jle 814d77b`：AI 路径直落、mon 路径跳转
    //   ⇒ 源码分支序 AI 在前（P47/P222 块序）。
    if (mob.m_roleType > 4)
    {
        CAICharacter* ai =
            G_CDataManager()->m_aiCharacterList->get(mob.m_mobIndex);
        if (ai != 0)
        {
            if (m_dungeon->hasTowerItemDrop())
                ai->deathTowerSpecifyItemDrop(specifyDrops);
            stGenerateRefData_t ref;
            // [b28d-solo] ORIG 0x0814d6f2..d700 写序：m_rateSlot(+0x36) 先、
            //   m_field34(+0x34) 后（字节写物理序）。
            ref.m_rateSlot = (char)m_difficulty;
            ref.m_field34 = (char)eraseCnt;
            ref.m_field3e_byte = (unsigned char)m_party->get_member_count();  // +0x3e
            ai->generateIndependentItem(ref, result.m_dropItems);
        }
    }
    else
    {
        CMonster* mon =
            (CMonster*)G_CDataManager()->find_monster(mob.m_mobIndex);
        if (mon != 0)
        {
            stGenerateRefData_t ref;
            ref.m_rateSlot = (char)m_difficulty;
            ref.m_field34 = (char)eraseCnt;        // +0x34（DepB 具名化）
            // [b28d-solo] ORIG 0x0814d6f8 `mov %al,-0x11a(%ebp)` 单条字节写
            //   （复合赋值恒 and/or 4 条）⇒ 字节视图字段。
            ref.m_field3e_byte = (unsigned char)m_party->get_member_count();  // +0x3e
            mon->generateIndependentItem(ref, result.m_dropItems);
        }
    }

    // 生成结果逐件入图（ORIG：byte count=1、word +8=b、dword +0xc=当前秒）
    // p167：ORIG 有 list<map_item>::push_back + 转换构造 + ne<const Inven_Item*>
    //   ⇒ newItems 收集 + const 引用迭代器；memcpy 是我们的发明（ORIG 结构赋值）。
    // P13：ORIG end() 在循环初始化区求值一次（两个转换构造）⇒ 手动缓存 end
    for (std::vector<Inven_Item>::const_iterator it =
             result.m_dropItems.begin(),
         endIt = result.m_dropItems.end();
         it != endIt; ++it)
    {
        map_item mi;
        mi.m_count = 1;                    // ORIG 仅写低字节，ctor 已清零等价
        mi.m_dropIndex = b;
        mi.m_createTick =
            (int)GlobalData::s_systemTime_.getCurSec();
        mi.m_item = *it;                   // ORIG 结构赋值（无 memcpy call）
        // ORIG 0x0814d94c：m_item 赋值后才取 m_itemIndex/自增；
        // 0x0814d995 Add_Item 先于 0x0814d9aa list::push_back。
        // ORIG 0x0814d94c..d961：`mov 0xa34(%eax),%eax; mov %eax,[S];
        // lea 0x1(%eax),%edx; mov %edx,0xa34(%eax)` = 后置 ++ 惯用形
        // （拆两句会多 1 条槽回读 / 复合 += 多 2 条重读）。
        mi.m_itemIndex = m_stageData.m_dropItemCnt++;
        m_stageData.Add_Item(mi);
        newItems.push_back(mi);
    }

    // ORIG 0x0814d9d4/d9db：countIdx/itemCount 在 PacketGuard 构造前预清 0
    // （P198① 两句式），0x0814da29 再回填 get_index()。
    int countIdx = 0;
    int itemCount = 0;
    PacketGuard packet;
    packet.put_header(0, 0x26);
    packet.put_short((int)a);
    countIdx = packet.get_index();         // 物品计数回填位置
    packet.put_byte(itemCount);            // ORIG 从 -0x34 读，非立即数 0
    // ORIG 0x0814da3e..da87：begin→it→end→endIt 构造，之后才 emblem.init()
    // （0x0814da95）；end() 仅循环外求值一次（P13）。
    std::list<map_item>::const_iterator it = newItems.begin();
    std::list<map_item>::const_iterator endIt = newItems.end();
    stAvatarEmblemInfo_t emblem;
    emblem.init();
    for (; it != endIt; ++it)
    {
        const map_item& item = *it;
        packet.put_short(item.m_itemIndex);              // +0x04
        packet.put_int(item.m_item.m_addInfo);           // +0x12 dword
        packet.put_byte((int)item.m_item.GetItemAttr() & 0xff);
        // ORIG 0x0814db05 `test %al; je <get_add_info 块>`：正极性分支序
        if (item.m_item.isAvatarItemType())
        {
            CEquipItem* equip = (CEquipItem*)G_CDataManager()->find_item(
                item.m_item.m_addInfo);
            if (equip == 0)
            {
                packet.put_int(item.m_item.get_add_info());
            }
            else
            {
                // ORIG 0x0814db51/db58：period 预清 0 + 具名指针局部
                // （-0x20/-0x1c 双槽），getAvatarSocket 用该局部。
                int period = 0;
                CEquipItem* pEquip = equip;
                period = pEquip->getAvatarPeriod(
                    (unsigned char)item.m_item.m_fieldb);
                packet.put_int(period * 0x15180);
                emblem.init();
                pEquip->getAvatarSocket((int)item.m_item.m_fieldb, emblem);
            }
        }
        else
        {
            packet.put_int(item.m_item.get_add_info());
        }
        packet.put_short((int)item.m_item.m_fieldb);     // word @+0x1b
        packet.put_byte((int)item.m_item.m_amp.getAbilityType() & 0xff);
        packet.put_short((int)item.m_item.m_amp.getAbilityValue() & 0xffff);
        packet.put_packet(item.m_item);
        // ORIG 0x0814dc5e `test %al; je <put_byte(0)>`：正极性
        if (item.m_item.isAvatarItemType())
        {
            packet.put_byte(1);
            packet.put_int(0x1e);
            packet.put_binary(reinterpret_cast<const char*>(&emblem), 0x1e);
        }
        else
        {
            packet.put_byte(0);
        }
        packet.put_short((int)item.m_dropIndex);         // word @+0x08
        ++itemCount;
    }
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_byte(countIdx, itemCount);   // 回填物品数
    packet.finalize(true);
    m_party->send_to_party(packet);
    return true;
}

// ---- consistMap（ORIG 0x0814bef4）----
// 按源图布怪：AI 布点 → AP 随机怪（预算扣减）→ 脚本怪列表 → 特殊被动物品。
// 任一 AI 查询失败返回 false。map_monster 具名字段映射：
//   m_field11 半字 = ORIG +0x11/+0x12 两字节写、m_field13/m_field14 同名、
//   m_dropCnt(+0x2c) 收布点/脚本怪掉落数。
// [b28d-62] N89+N82：const/非 const 迭代器族 + map 拷贝 ctor + getParty。
//   ORIG 0x0814bef4
// [b28d-solo-CBossStage] 待办：p167 剩 8 missing（getIdx/getAppearancePoint/
//   get_standard_level 各 -2、G_CDataManager -1、_mapItem C1Ev -1、at -1、
//   getItemGenerator -1）。已修：G_CDataManager 直调 ×5、apStat 默认构造+赋值、
//   (*it).m_type（P167）、mobs/items 去 const。927 条超大函数，槽/帧分区为主。
// [b28d-solo-CBossStage] 封顶 N82：p167 修 getIdx×2/getAppearancePoint×2/
//   get_standard_level×2/G_CDataManager×1/getItemGenerator×1（全部改内联重读，
//   P157/P0）+ find 极性 `!=`（P220）+ upper_bound/begin 缓存（P13）+ at const 版
//   + memcpy→结构赋值（N84）；残差=_mapItem C1Ev 发射点（4.4 list move 赋值 vs
//   ORIG 元素默认构造，N86 族）+map_monster C1/D1 计数+槽/帧分区（0xfbc vs 0xf9c）。
//   927 条超大函数，语义完成（调用点差集仅剩 1 项结构性）。
// [b28d-62 本轮] 修 3 处真缺口：map_monster 提升到三循环外（ORIG 仅一枚
//   -0xf50 对象，P0 槽证据；帧 0x161c→0xfbc 对齐、-29 条）、第二段 AP 循环
//   改底部条件 while（ORIG 0x0814c240 `jmp cond`，P283）、等级夹取拆嵌套
//   if（ORIG 0x0814c704 双判）、refGen1/refGen4 构造提前到 objectDropItems/
//   item_list 之前 ⇒ 929/920（gap +9，其中 6 条为 list 拷贝构造的 4.4 move
//   折叠，N 类）⇒ 本件无我方缺失的语义段；p167 CLEAR。
#include "CBossStage/consistMap.inc"
{
    m_stageData.m_mapIndex = map->get_index();

    std::vector<unsigned int> usedAIs;
    // ORIG：std::map::operator= 从 CAICharacterList::getAPCount() 结果拷贝
    // P0：ORIG 每处直调 G_CDataManager()（p167 MISSING ×5）⇒ 禁 dm 缓存。
    // p167：ORIG 为 C1Ev + operator=（默认构造 + 拷贝赋值），非拷贝构造。
    std::map<int, int> apStat;
    apStat = G_CDataManager()->m_aiCharacterList->getAPCount();
    int counter = 0;
    // ORIG 0x0814bf55 后构造 map_monster（-0xf50）一次，三个布怪循环
    // 各自 memset 后复用（P0 槽证据：ORIG 仅一枚 0x34c 对象）。
    map_monster mob;

    // ---- 第一段：图上 AI 布点（STAICharacterMapArrangeData 向量）----
    const std::vector<STAICharacterMapArrangeData>* arranges =
        map->getAICharacters();
    if (!arranges->empty())
    {
        counter = 0;
        for (std::vector<STAICharacterMapArrangeData>::const_iterator it =
                 arranges->begin();
             it != arranges->end(); ++it)
        {
            // P167：ORIG 用 operator*（p167 MISSING de / EXTRA ptEv）⇒ (*it)
            unsigned int aiType = (unsigned int)(*it).m_type;
            CAICharacter* ai =
                G_CDataManager()->m_aiCharacterList->get(aiType);
            if (ai == 0)
                return false;

            usedAIs.push_back((unsigned int)ai->getIdx());
            // [b28d-solo] P157：ORIG 0x0814c03b/814c053 两次独立调用 getAppearancePoint
            //   （无 ap 具名局部）。
            if (ai->getAppearancePoint() != 0)
            {
                std::map<int, int>::iterator st =
                    apStat.find(ai->getAppearancePoint());
                if (st != apStat.end())
                    st->second = std::max(st->second - 1, 0);
            }

            memset(&mob, 0, sizeof(mob));
            mob.m_mobId = counter;                            // +0x00 序号
            mob.m_mobIndex = (int)aiType;                     // +0x0c
            mob.m_instId = m_stageData.m_assignItemIdx;       // +0x04 seq++
            m_stageData.m_assignItemIdx += 1;
            mob.m_roleType = 5;                               // APC
            mob.m_level = (char)(unsigned char)ai->getLevel();
            mob.m_dropCnt = (*it).m_camp;                     // +0x10 dword（STMapScript 批次命名 m_camp；
                                                              //   本调用面按等级消费，语义双证待裁决）
            m_stageData.Add_Mob(mob);                         // ctor 已清 +0x11/+0x12
            ++counter;
        }
    }

    // ---- 第二段：AP 随机怪（剩余点数预算扣减）----
    int remainAP = map->getAPCRandomPoint();
    if (remainAP != 0)
    {
        counter = 0x40;
        // ORIG 0x0814c240 `jmp 0814c5e5`：条件在循环尾（4.1 while 布局）；
        //   while(true)+头部 break 会被 4.4 条件置顶（P283）。
        while (remainAP >= G_CDataManager()->m_aiCharacterList->getMinAP())
        {
            CAICharacter* ai = G_CDataManager()->m_aiCharacterList->getByAP(
                (unsigned int)(get_rand_int(remainAP) + 1));
            if (ai == 0)
                return false;

            // [b28d-solo] P157/P220：ORIG 0x0814c326 用 `__gnu_cxx::ne`
            //   （find(...) != end）且 alreadyUsed 路径直落、新增路径跳转；
            //   getIdx×3/getAppearancePoint×2 均内联（无 aiIdx/ap 局部）。
            if (std::find(usedAIs.begin(), usedAIs.end(),
                          (unsigned int)ai->getIdx()) != usedAIs.end())
            {
                // 已用过的 AI：重算 ≤ 剩余点数的可用 AP 总量，为 0 则退出
                // [b28d-solo] ORIG 0x0814c252..c2dc begin/upper_bound 各求值一次
                //   （P13 缓存 end 迭代器）。
                std::map<int, int>::const_iterator st = apStat.begin();
                std::map<int, int>::const_iterator stEnd =
                    apStat.upper_bound(remainAP);
                int avail = 0;
                for (; st != stEnd; ++st)
                    avail += st->second;
                if (avail == 0)
                    break;
            }
            else
            {
                usedAIs.push_back((unsigned int)ai->getIdx());
                std::map<int, int>::iterator st =
                    apStat.find(ai->getAppearancePoint());
                if (st != apStat.end())
                    st->second = std::max(st->second - 1, 0);
                remainAP -= ai->getAppearancePoint();

                memset(&mob, 0, sizeof(mob));
                mob.m_mobId = counter;
                mob.m_mobIndex = ai->getIdx();
                mob.m_instId = m_stageData.m_assignItemIdx;
                m_stageData.m_assignItemIdx += 1;
                mob.m_roleType = 5;
                mob.m_level = (char)(unsigned char)ai->getLevel();
                mob.m_dropCnt = 100;                          // ORIG 立即数 100
                m_stageData.Add_Mob(mob);
                ++counter;
            }
        }
    }

    // seed 值（ORIG get_rand_int(0) 后清 bit 0x40000）
    m_stageData.m_mapBasisLevel = get_rand_int(0);
    *(unsigned int*)&m_stageData.m_mapBasisLevel &= 0xfffbffffu;

    std::vector<int> singleSpawnSeqs;   // 非 named 单人刷怪序号表

    // ---- 第三段：脚本怪列表 ----
    counter = 0;
    // p167：ORIG 有 _List_const_iterator<mapMonster> 转换构造 ×2 ⇒ 非 const list
    //   + const_iterator（转换形）。
    std::list<_mapMonster> mobs = map->get_monster_list();
    for (std::list<_mapMonster>::const_iterator it = mobs.begin();
         it != mobs.end(); ++it)
    {
        const _mapMonster& em = *it;
        memset(&mob, 0, sizeof(mob));
        mob.m_mobId = counter;

        // 等级：em.m_flag04==0 直接取 em.m_field08；否则 标准等级+m_field08，
        // 字节域夹取 [1..99]
        unsigned char lv;
        if ((char)em.m_flag04 == 0)
            lv = (unsigned char)(char)em.m_field08;
        else
        {
            lv = (unsigned char)(m_dungeon->get_standard_level() +
                 (int)(signed char)em.m_field08);
            // ORIG 0x0814c704..c72d 双判 `cmp $0x62; ja` + `test; jle`（Ghidra
            //   `(lv < 99) && (lv == 0)`）；4.4 前端逻辑折叠成单判 `lv == 0`
            //   （&&/嵌套 if 两形实测同码）⇒ 6 条按跨版本 N 封顶。
            if (lv < 99 && lv == 0)
                lv = 1;
            else if (lv > 99)
                lv = 99;
        }
        mob.m_level = (char)lv;

        if (em.m_mobId == -1)
            mob.m_mobIndex =
                G_CDataManager()->m_monsterManager->get_levelrandom_mob((int)lv);
        else
            mob.m_mobIndex = em.m_mobId;

        // ORIG 分别写 +0x11/+0x12 两字节 = m_field11 半字（小端拼装）
        mob.m_field11 = (short)(
            ((unsigned short)(unsigned char)(char)em.m_levelAdd) |
            ((unsigned short)(unsigned char)(char)em.m_field14 << 8));
        mob.m_roleType = (char)(unsigned char)em.m_roleType;
        mob.m_instId = m_stageData.m_assignItemIdx;
        m_stageData.m_assignItemIdx += 1;

        if ((signed char)mob.m_roleType == 0 && !mob.isNamedMonster())
            singleSpawnSeqs.push_back(mob.m_instId);

        mob.m_field13 = (char)(unsigned char)em.m_champion;   // em +0x1c
        // em +0x20 低字节（_mapMonster::m_posX 首字节；CBattle_Field 同型转写
        // BattleMonsterDepsC2.cpp mob.m_field14 ← ref.m_posX）
        mob.m_field14 = (char)(unsigned char)em.m_posX;
        mob.m_dropCnt = em.m_hpPct;                           // em +0x28
        m_stageData.Add_Mob(mob);
        ++counter;
    }

    // ---- 第四段：特殊被动物品生成 ----
    // [b28d-solo] p167：ORIG 用 const 版 vector::at ⇒ specialItems 为 const 指针。
    const std::vector<stDungeonAssignItem_t>* specialItems =
        m_dungeon->getSpecialPassiveObjectItems();
    // ORIG 0x0814c925/c933：refGen1/refGen4 构造先于 getObjectDropItems()
    //   与 get_item_list()（Ghidra C 序：specialItems→refGen1→refGen4→
    //   objectDropItems→items）。
    stGenerateRefData_t refGen1;
    stGenerateRefData_t refGen4;
    // ORIG 同时取出 getObjectDropItems() 但未使用（忠实保留调用）
    m_dungeon->getObjectDropItems();
    // [b28d-solo] 两句式（C1Ev+move 赋值）实测引入 list aSEOS2_ NEW 符号且
    //   _mapItem C1Ev 仍未发射 ⇒ 回退拷贝构造形。
    std::list<_mapItem> items = map->get_item_list();

    std::list<_mapItem>::iterator it = items.begin();
    while (it != items.end())
    {
        const _mapItem& mi = *it;
        const stDungeonAssignItem_t& assign =
            specialItems->at((unsigned int)mi.m_objectIdx);

        unsigned char genLevel;
        if (assign.m_assignItemIdx != -1)
        {
            genLevel = (unsigned char)(char)assign.m_assignItemIdx;
        }
        else
        {
            // [b28d-solo] P157：ORIG 0x0814c9ee/ca03/ca2f 三次独立调用
            //   get_standard_level（无 sl 具名局部）。
            if (m_dungeon->get_standard_level() <= 0 ||
                m_dungeon->get_standard_level() > 200)
            {
                // ORIG 该路径不推进迭代器直接回到循环判定——标准等级恒在域内
                // 时不可达；为防重建服务死循环改为跳过并注释差异。
                ++it;
                continue;
            }
            genLevel =
                (unsigned char)(char)m_dungeon->get_standard_level();
        }
        (void)genLevel;   // ORIG 计算后未再读取（忠实保留）

        stGenerateResult_t result;
        // ORIG consistMap（0x0814bef4 +0xb54..+0xb5a）此处把 mi.m_field04 低字节
        // 写入独立栈标量 -0xb2f(%ebp)——位于 -0xb20(%ebp) 的 result 对象【之外】，
        // 写后全函数无任何读取（Ghidra local_b33 死存储；stGenerateResult_t 布局
        // vector(0xc)+CLuckPoint(0x24)=0x30 已完整覆盖，不存在 +0x0f 成员；
        // +0x0f 落在 CLuckPoint::m_users._M_start 指针内部，不可字段化）。
        // 忠实转写 = 删除该死存储；不得向 result+0x0f 写入（会破坏 vector 头）。

        // [b28d-solo] P0：ORIG 0x0814ca60/ca9d 每处直调 G_CDataManager()->getItemGenerator()。
        G_CDataManager()->getItemGenerator()
            ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_1)
            ->generateSpecificItem(refGen1, result);
        G_CDataManager()->getItemGenerator()
            ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_4)
            ->generateSpecificItem(refGen4, result);

        for (std::vector<Inven_Item>::iterator d = result.m_dropItems.begin();
             d != result.m_dropItems.end(); ++d)
        {
            map_item drop;
            drop.m_count = 1;              // ORIG movb $1（0830b5ab 同型）
            // ORIG 写 map_item+0x01 等级字节：add_item_curr_map 08302f84 /
            // consistMap 0814c9da 同型（来源 _mapItem.m_field0c 或标准等级）
            drop.m_standardLevel = (char)mi.m_field0c;
            drop.m_itemIndex = m_stageData.m_dropItemCnt;
            m_stageData.m_dropItemCnt += 1;
            // [b28d-solo] N84：ORIG 为结构整体赋值（rep movsl），memcpy 是发明 call。
            drop.m_item = *d;
            m_stageData.Add_Item(drop);
        }
        ++it;
    }
    return true;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0814f426（docs/class_func_reports/WongWork__CBossStage/
// RandomConsistMap.md 全文，GlobalBossAndAltar 2026-08-24）：
// randomMapIndex 失败 → false；GM 且 m_gmBossMapIndex(+0x796d0) 非空时强制该图并清零；
// find_map 空/consistMap 失败 → false；成功记 m_startTick(+0x9e8) 并返回 true。
// ----------------------------------------------------------------------------
#include "CBossStage/RandomConsistMap.inc"
{
    int mapIdx = 0;
    if (randomMapIndex(mapIdx) != true)
        return false;


    // ORIG 0x0814f462/46f 两条件各自 test/je 直落 L66，无 choose 岛：嵌套 if。
    if (user.isGMUser())
        if (user.m_gmBossMapIndex != 0)
    {
        mapIdx = user.m_gmBossMapIndex;
        user.m_gmBossMapIndex = 0;
    }

    CMap* map = static_cast<CMap*>(G_CDataManager()->find_map(mapIdx));
    // ORIG 0x0814f4a3..b3 choose 岛无 bool 槽（mov$1/jmp/mov$0 + 统一 test）。
    if (map == 0 || !consistMap(map))
        return false;
    // ORIG this+0x9e8 = m_startTick
    m_startTick = (int)GlobalData::s_systemTime_.getCurTickCount();
    return true;
}

}  // namespace WongWork