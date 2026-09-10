// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BlueMarble/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r BlueMarble（G2-3 PvP 链，大富翁房间，池化 300）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/BlueMarble.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 依赖子对象（BlueMarbleScriptManager/BlueMarbleUserInfo/CMTRand/...）为
// PvP_deps 桩；本 TU 以 asm-label extern 发跨类调用。只写本文件。
#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "BlueMarble.h"
#include "CDataManager.h"
#include "CInventory.h"          // 权威 CInventory（原本地最小声明与 CInventory.h:418 重定义冲突）
#include "CEnvironment.h"
#include "LogManager.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "PacketGuard.h"   // 权威 PacketGuard（0xc）
#include "GameWorld.h"            // G_GameWorld / GameWorld::gotoBlueMarble/outFromBlueMarble（§9）
#include "CPrivateStoreMgr.h"     // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore（§9）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString（§9）
#include "CGameManager.h"         // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail + G_CGameManager（§9）
#include "PacketTypes.h"          // 权威 MSG_MAILBOX_SEND（+0x11/+0x39/+0x3d 具名字段）

// 2026-09-05 裸偏移收尾：原本地 shadow struct BM_ScriptFields（把 m_pScript 当
// 未建模字节区直读 +0x30..+0x4c）删除 —— 该区间即权威
// BlueMarbleScriptManager::m_info（+0x30，BlueMarbleInfoScript），逐槽改用
// m_pScript->m_info.<字段>（证据：getRandomItem 0x080d7690 `mov 0x4c(%eax)`、
// setDiceNumber 0x080d6f88 `mov 0x48/0x44(%eax)`、checkUserConditionEnterBlueMarble
// 0x080d8d02 `mov 0x30/0x34/0x38/0x3c(%eax)`，均 4 字节读）；private 槽位经
// BlueMarbleInfoScript 的 `friend class BlueMarble` 授权（ORIG 无 getter 符号）。

// ---- CGameManager / G_CGameManager：经 #include "CGameManager.h"（§9） ----

// ============================================================================
// BlueMarble 实现
// ============================================================================

// [b28d-solo-BlueMarble AE] seed 循环修复：`volatile unsigned long seedValue`
// 强制 4.4 先求值实参再算 receiver，逐条复现 ORIG 0x080d8b22 段
// `mov -0x20(%ebp),%eax; …; lea (%ecx,%edx,1),%edx`（P316 volatile 求值序；
// 非 volatile 恒 receiver 先 + `add`）。旧 N(P75/N4) 判词（P226 语句表达式/
// 指针形/具名实参无效）作废；104=104 gap 0。ORIG 0x080d8a5c
#include "BlueMarble/BlueMarble.inc"
    : m_memoryPoolIndex(-1), m_state(BlueMarbleState::T_0), m_diceNumber(0), m_blueMarbleMap(0),
      m_pScript(0), m_maxBoardSize(0)
{
    m_pRands = new CMTRand[4];
    volatile unsigned long seedValue = (unsigned long)time((time_t*)0);
    for (int i = 0; i <= 3; ++i)
    {
        m_pRands[i].seed(seedValue);
    }
    m_users.clear();
    m_set.clear();
}

#include "BlueMarble/dtor_BlueMarble.inc"
{
    if (m_pRands)
    {
        delete[] m_pRands;
    }
    m_pRands = 0;
}

#include "BlueMarble/getMemoryPoolIndex.inc"
{
    return m_memoryPoolIndex;
}

#include "BlueMarble/setMemoryPoolIndex.inc"
{
    m_memoryPoolIndex = idx;
}

#include "BlueMarble/getState.inc"
{
    return m_state;
}

#include "BlueMarble/changeState.inc"
// 原 int；ORIG 0x080d9202 尾 pop;ret 无 %eax 置值 ⇒ void（mangled 无返回类型）
{
    m_state = state;
}

#include "BlueMarble/getWinner.inc"
{
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            continue;
        }
        if (m_users[i].getGrade() == 0)
        {
            return i;
        }
    }
    return -1;
}

#include "BlueMarble/getWaitUserCount.inc"
{
    int count = 0;
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() != 0)
        {
            ++count;
        }
    }
    return count;
}

#include "BlueMarble/getUserSlot.inc"
{
    if (user == 0)
    {
        return -1;
    }
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == user)
        {
            return i;
        }
    }
    return -1;
}

#include "BlueMarble/getUserState.inc"
{
    return m_users[seat].getUserState();
}

#include "BlueMarble/setUserState.inc"
{
    m_users[seat].setUserState(state);
}

// [b28d-solo-BlueMarble AE] 关键修复：`int zone` 具名局部改内联右值实参
// `mm.insert(std::make_pair(m_users[i].getBoardZone(), i))`——ORIG zone 槽
// -0x1c 是 make_pair<int,int&> 右值形参的匿名临时物化（非具名局部），4.4 据此
// 把临时区整体对齐 ORIG（zone -0x1c / sret -0x24 / pair -0x2c / insert 临时
// -0x30；179=179 gap 0）；并删除原 P190 float framePad（内联后自然对齐，
// pad 反致整体深移 4）。具名 zone 三形（块内/头部/先声明后赋值）恒落 -0x54
// ⇒ 原 N93/N82附 判词作废。P234 去 `std::move(zone)`（ORIG 无 out-of-line
// move call）保留。ORIG 0x080d7afc
#include "BlueMarble/setUserGrade.inc"
{
    std::multimap<int, int, std::greater<int> > mm;
    m_users[grade].setGrade(BlueMarbleUserGrade::T_0);
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        // ORIG 0x080d7b5d/0x080d7b87：==0 物化 sete %al 后 test;jne 出口（body fall，
        // 范式23 goto）。b28c-1：P0/N96 清绑，P168a② 具名 bool + 正向 goto；
        // 声明前置（P198① 两句式）避免 goto 跨初始化。
        if (m_users[i].getUser() == 0)
            goto NEXT0;
        if (m_users[i].getGrade() == 0)
            goto NEXT0;
        // ORIG 0x080d7bb0..：zone 槽 -0x1c；Ghidra `make_pair<int,int&>` +
        // 无 out-of-line call ⇒ 原码是左值 make_pair（P61/P234），
        // `std::move(zone)` 会多发一次 `call std::move<int>`（+5 条）。
        mm.insert(std::make_pair(m_users[i].getBoardZone(), i));
    NEXT0:
        ;
    }
    std::multimap<int, int, std::greater<int> >::iterator it = mm.begin();
    int curGrade = 0;
    int nextGrade = 1;
    int lastZone = 0;
    while (it != mm.end())
    {
        if (it->first == lastZone)
        {
            lastZone = it->first;
            m_users[it->second].setGrade((BlueMarbleUserGrade::T)curGrade);
        }
        else
        {
            lastZone = it->first;
            m_users[it->second].setGrade((BlueMarbleUserGrade::T)nextGrade);
            curGrade = nextGrade;
            ++nextGrade;
        }
        ++it;
    }
}

#include "BlueMarble/getUserDeadCount.inc"
{
    return m_users[seat].getUser()->getBlueMarbleUserDeadCount();
}

#include "BlueMarble/setUserDeadCount.inc"
{
    m_users[count].getUser()->increaseBlueMarbleUserDeadCount();
}

#include "BlueMarble/getMaxBoardSize.inc"
{
    return m_maxBoardSize;
}

// b28c-1 AE: P223 尾标签后 0 差。
#include "BlueMarble/setMaxBoardSize.inc"
{
    BlueMarbleMapScript* map = m_pScript->getMap(getBlueMarbleMap());
    // ORIG 0x080d6e88：以 getMap() 返回值为 this 直调 vector::size（m_tiles 在
    // BlueMarbleMapScript +0x00，地址等价）⇒ 具名 map->m_tiles.size()。
    // b28c-1：P223 空块 nop 中继岛（ORIG je 跳过 body 落 nop、body 尾 jmp 汇合）
    //   ⇒ goto 尾空标签形，等价语义。
    if (map == 0) goto done;
    m_maxBoardSize = (int)map->m_tiles.size();
done:;
}

#include "BlueMarble/getDiceNumber.inc"
{
    return m_diceNumber;
}

#include "BlueMarble/setDiceNumber.inc"
{
    m_diceNumber = (unsigned int)m_pRands[0].randInt() %
                    (unsigned int)m_pScript->m_info.m_diceMax +
                m_pScript->m_info.m_diceMin;
}

#include "BlueMarble/getBlueMarbleMap.inc"
{
    return m_blueMarbleMap;
}

#include "BlueMarble/setBlueMarbleMap.inc"
{
    m_blueMarbleMap = (unsigned int)m_pRands[1].randInt() %
                    (unsigned int)m_pScript->getMapTotalCount() +
                1;
}

#include "BlueMarble/getMovePosition.inc"
{
    return m_users[seat].getBoardZone();
}

#include "BlueMarble/setMovePosition.inc"
{
    m_users[seat].setBoardZone(pos, getMaxBoardSize());
}

#include "BlueMarble/getVisitZone.inc"
{
    std::set<int>::const_iterator it = m_set.find(seat);
    if (it == m_set.end())
    {
        return 0;
    }
    return 1;
}

#include "BlueMarble/setVisitZone.inc"
{
    std::set<int>::const_iterator it = m_set.find(zone);
    if (it == m_set.end())
    {
        m_set.insert(zone);
    }
}

#include "BlueMarble/getDungeonIndex.inc"
{
    return m_users[seat].getDungeon();
}

#include "BlueMarble/setDungeonIndex.inc"
{
    m_users[seat].setDungeon(dungeon);
}

#include "BlueMarble/getItemIndex.inc"
{
    return m_users[seat].getItem();
}

#include "BlueMarble/setItemIndex.inc"
{
    m_users[seat].setItem((unsigned int)itemIdx);
}

#include "BlueMarble/getExpBuff.inc"
{
    return m_users[seat].getExp();
}

#include "BlueMarble/setExpBuff.inc"
{
    m_users[seat].setExp(exp);
}

#include "BlueMarble/getGoldBuff.inc"
{
    return m_users[seat].getGold();
}

#include "BlueMarble/setGoldBuff.inc"
{
    m_users[seat].setGold(gold);
}

#include "BlueMarble/getDropBuff.inc"
{
    return m_users[seat].getDrop();
}

#include "BlueMarble/setDropBuff.inc"
{
    m_users[seat].setDrop(drop);
}

// [b28d-solo-BlueMarble AE] P251/P261+P305：原 if/goto 链使 4.4 每次重载形参槽
// （多 2 载入 + 3 nop 岛，53/58）；改 `switch((int)type)`（窄枚举必须 (int) 化，
// 否则 case 1/2 被 -fstrict-enums 静默删）后 type 单次载入复用 eax，53=53 ndiff 0。
// ORIG 0x080d990e
#include "BlueMarble/getBuff.inc"
{
    if (user == 0)
    {
        return 0;
    }
    int seat = getUserSlot(user);
    if (seat == -1)
    {
        LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__,
                              0x4c7, "BlueMarble::getBuff slot(%d)", seat);
        return 0;
    }
    // ORIG 0x080d9979 `mov 0x10(%ebp),%eax` 只载入一次并全程复用 eax：
    // 0x080d997c `cmp$1; je→getGoldBuff@999e`、0x080d9984 `cmp$2;
    // je→getDropBuff@99b2`、0x080d9988 `test; jne→mov$0@99c6`，fall
    // type==0→getExpBuff@9997（P251/P261 无槽 cmp 链 ⇒ 原码是 switch）。
    // P305：BlueMarbleBuffType 是窄枚举（仅 T_0=0），必须 `switch((int)type)`
    // 否则越界 case 1/2 被 -fstrict-enums 静默删除。
    switch ((int)type)
    {
    case 0:
        return getExpBuff(seat);
    case 1:
        return getGoldBuff(seat);
    case 2:
        return getDropBuff(seat);
    default:
        return 0;
    }
}

// b28c-1 AE: P218 下标 int 化（unsigned i→int i）后 0 差。
#include "BlueMarble/getRandomItem.inc"
{
    unsigned long range = (unsigned long)m_pScript->m_info.m_itemWeightTotal;
    int r = m_pRands[3].randInt(range);
    // P0：ORIG 无 items 局部槽——每次使用重算 this（0x80d87xx `mov 0x8,%edx;
    // mov 0x38,%edx; add $0x98,%edx`，循环内两处）。
    int count = (int)m_pScript->m_info.m_randomItemList.size();
    int acc = 0;
    // b28c-1：P218 下标 int 声明触发 RHS 值先（ORIG 0x080d7690 先载 i 再算
    //  this 链；unsigned i 恒 this 先）。
    int i = 0;
    while (i < count)
    {
        acc += m_pScript->m_info.m_randomItemList[i].m_weight;
        if (acc >= r)
        {
            return m_pScript->m_info.m_randomItemList[i].m_itemIndex;
        }
        ++i;
    }
    return 0;
}

// [b28d-solo-BlueMarble AE] 三处修复：①m_char4 char→bool（BlueMarbleTileScript.h，
// 已 R2 广播）+ 裸 `if(!tile->m_char4)` 复现 ORIG 0x080d74a5 `movzbl; xor $1;
// test; je`（P292/P300）；②type 分派 if 链改 `switch((int)type)`（窄枚举必须
// (int) 化，P251/P261/P305）复现单次载入 + cmp$7/$8/$2 链；③去掉 `int r` 局部
// （ORIG randInt 结果直入 %ebx、无槽，P0）后帧顶槽基同时对齐。214=214 ndiff 0。
// ORIG 0x080d73dc
#include "BlueMarble/getRandomDungeon.inc"
{
    std::vector<int> local;
    int level = m_users[seat].getUser()->get_charac_level();
    // ORIG 0x080d741d-0x080d748a：getBlueMarbleMap()+getMap 链在判空与
    // getTile 两处**各取一次**（无 map 局部缓存，P160/P172）；判空为
    // 4.1 sete 双物化（test;je 直连不可复现，残差记录）。
    if (m_pScript->getMap(getBlueMarbleMap()) == 0)
    {
        return 0;
    }
    BlueMarbleTileScript* tile = m_pScript->getMap(getBlueMarbleMap())->getTile(
        (unsigned int)m_users[seat].getBoardZone());
    if (tile == 0)
    {
        return 0;
    }
    // ORIG 0x080d74a5 `movzbl 0x4(%eax),%eax; xor $0x1,%eax; test %al,%al; je`
    // ⇒ m_char4 已 bool 化（BlueMarbleTileScript.h）+ 裸 `if(!field)`（P292/P300）。
    if (!tile->m_char4)
    {
        return 0;
    }
    // ORIG 0x080d74ba-：type 三分支各自 sret 临时槽（-0x38/-0x2c/-0x20）
    // 直写 `local = getXXXIndex(level)`（每分支一对 op=+D1），无跨分支
    // 共享 tmp——旧 tmp 块形与 move 中转已废。
    // ORIG 0x080d74ba `mov 0x10(%ebp),%eax` 单次载入 + cmp$7/cmp$8/cmp$2 链
    // ⇒ switch（P251/P261）；Type 是窄枚举（仅 Type_0）故必须 (int) 化（P305）。
    switch ((int)type)
    {
    case 7:
        local = m_pScript->m_info.getUniqueDungeonIndex(level);
        break;
    case 8:
    case 2:
        local = m_pScript->m_info.getBossDungeonIndex(level);
        break;
    default:
        local = m_pScript->m_info.getRandomDungeonIndex(level);
        break;
    }
    if (local.size() == 0)
    {
        return 0;
    }
    // ORIG 0x080d7611 `mov %eax,%ebx`（randInt 结果直入 callee-saved、无 r 槽，
    // P0：原码无具名 r 局部）⇒ 调用内联进取模表达式。
    unsigned int idx =
        (unsigned int)m_pRands[2].randInt() % (unsigned int)local.size();
    if (idx >= local.size())
    {
        return 0;
    }
    return local[idx];
}

#include "BlueMarble/checkJoin.inc"
{
    if (getState() != 0)
    {
        return 0;
    }
    if (getWaitUserCount() >= m_pScript->m_info.m_playerMax)
    {
        return 0;
    }
    return 1;
}

#include "BlueMarble/join.inc"
{
    if (user == 0)
    {
        return 0x17;
    }
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            m_users[i].setUser(user);
            m_users[i].getUser()->setBlueMarbleIndex((short)getMemoryPoolIndex());
            m_users[i].getUser()->setMoveSpace((UserSpace::T)1);
            break;
        }
    }
    G_GameWorld()->gotoBlueMarble(user);
    sendBlueMarbleRoomInfo();
    return 0;
}

#include "BlueMarble/leaveUser.inc"
{
    if (user == 0)
    {
        LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x3f9,
                              "BlueMarble::leaveUser user is NULL");
    }
    else
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x400,
                                  "BlueMarble::leaveUser slot(%d)", seat);
        }
        else
        {
            G_GameWorld()->outFromBlueMarble(user);
            setLeaveUserSlot(seat);
            setLeaveUserBlueMarbleState();
            sendBlueMarbleRoomInfo();
        }
    }
}

#include "BlueMarble/setScript.inc"
{
    m_pScript = script;
}

#include "BlueMarble/setUserInfo.inc"
{
    BlueMarbleUserInfo info;
    info.setScript(script);
    m_users.push_back(info);
}

#include "BlueMarble/resetBlueMarble.inc"
{
    m_diceNumber = 0;
    m_blueMarbleMap = 0;
    m_set.clear();
    m_maxBoardSize = 0;
}

#include "BlueMarble/resetBuffInfo.inc"
{
    m_users[seat].resetBuff();
}

#include "BlueMarble/resetIndexInfo.inc"
{
    m_users[seat].resetIndex();
}

// [b28d-solo-BlueMarble AE + 真语义 bug] moveSpace 守卫原写反（==1 入主流程，
// ORIG 是 ==1 返回 0x17）；改 `(getMoveSpace()-1)==0` early-return 同时复现
// ORIG `sete; test; je <主流程>` 落点，175=175 ndiff 0。ORIG 0x080d8d02
#include "BlueMarble/checkUserConditionEnterBlueMarble.inc"
{
    if (user == 0)
    {
        return 0x17;
    }
    int count = 0;
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == user)
        {
            ++count;
        }
    }
    if (count > 0)
    {
        leaveUser(user);
        return 0x12;
    }
    // ORIG +0x95 `cmp $3; setne %al; test; je <体>` / +0xcd `cmp $1; sete %al;
    // test; je <体>`：guard 物化形（P194 反形），真→fall 0x17 岛，假→je 入体。
    // b28c-1：P0/N96 清绑（P168a② 具名 bool）。
    if (user->get_state() != 3)
    {
        return 0x17;
    }
    if (user->checkInBlueMarble())
    {
        return 0x17;
    }
    // ORIG +0xcd `cmp $1,%eax; sete %al; test; je <主流程体@0x80d8de3>`：
    // 物化 (moveSpace==1)，**al==0（moveSpace!=1）跳主流程**，fall 返回 0x17。
    // [b28d-solo-BlueMarble 真语义 bug] 旧 goto 形条件写反（==1 入主流程）；
    // 4.4 对 `!(x==1)` 折成 setne，唯 `(x-1)==0` 形复现 sete+je 落点
    // （探针 /tmp/p_bm3.cpp c20/c22）。early-return 使 return 块 fall-through。
    if ((user->getMoveSpace() - 1) == 0)
    {
        return 0x17;
    }
    // P48 去缓存：ORIG +0x0e7/+0x0ff 对上下界各独立重调 get_charac_level
    //（两次调用，不缓存进 level 局部）；第二条件 call 在前（P134）。
    if (user->get_charac_level() < m_pScript->m_info.m_enterLevel ||
        user->get_charac_level() > m_pScript->m_info.m_enterMaxLevel)
    {
        return 0x17;
    }
    if ((int)user->getBlueMarbleEnterCount() >=
        m_pScript->m_info.m_enterCount)
    {
        return 0x17;
    }
    if (user->getCurCharacVill() == 7)
    {
        return 0x17;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        return 0x17;
    }
    if (user->IsBusyDisjointStore())
    {
        return 0x17;
    }
    if (user->IsCurCharacGhost())
    {
        return 0x17;
    }
    if (user->CheckInTrade())
    {
        return 0x17;
    }
    if (user->isCompetitionMercenary())
    {
        return 0x17;
    }
    if (user->CheckInParty())
    {
        CParty* party = (CParty*)user->GetParty();
        if (party != 0)
        {
            party->send_invite_bluemarble(user->get_unique_id() & 0xffff);
            G_CGameManager()->CheckOutParty(user, false);
        }
    }
    return 0;
}

// [b28d-solo-BlueMarble NEAR(26)/N82附] 三处结构修复：①1024B 数组声明移入 if 块
// （ORIG rep stos 在 user==0 判空之后，0x080d9c28-3e）；②memoryPoolIndex/
// characNo/m_id 加纯 register 提示复现 ORIG edi/esi/ebx 跨调用保活
// （P231/P248 三条件；push edi/esi/ebx 齐备，无 asm 绑定）；③空 else{return;}
// 复现尾 `jmp <epilogue>; nop` 岛（P285）。137=137 gap 0。
// 余 26 条 = 4.4 标量/聚合分区（N82附）：buf -0x430 vs ORIG -0x424、三标量
// -0x28/-0x24/-0x20 vs ORIG -0x434/-0x430/-0x42c、log -0x30 vs -0x24。
// 本轮再实测两形均不翻：标量移入 if 块（buf 之后）、三标量包局部聚合 struct
// （劣化 DIFF，深槽多 -0x43c/-0x44c）；连同旧「块内提头/标量 register/全头部
// 声明」共六形 ⇒ N82附/P112 封顶。
// ORIG 0x080d9c12
#include "BlueMarble/checkBlueMarbleUserConfirm.inc"
{
    int i;
    if (user != 0)
    {
        // ORIG 0x080d9c28-0x080d9c3e：1024 字节 rep stos 清零位于 user==0
        // 判空之后 ⇒ 数组声明在 if 块内（函数头声明会在入口即清零）。
        char local_428[1024] = {0};
        sprintf(local_428, "./log/%s/BlueMarbleConfirmInfo",
                G_CEnvironment()->get_file_name());
        for (i = 0; i < m_pScript->m_info.m_playerMax; ++i)
        {
            if (m_users[i].getUser() == user)
            {
                // [M3S4 本轮实测-3形 · 138=138 与 ORIG 等长] 三标量声明位置三形
                // （函数头 / 外层 if 块 buf 之前 / 最内层 if 块使用处）产物**逐行相同**
                // ⇒ GCC -O0 槽分配与块层级无关；ORIG 槽分区（-0x434/-0x430/-0x42c
                // 三标量在 buf -0x424 **之下**，log -0x24、i -0x1c）不可复现
                // ⇒ N82附 封顶（四数 AE 化不可达）；本形使 our 139→138。
                int userBlueMarbleIndex = (int)(short)m_users[i].getUser()->getBlueMarbleIndex();
                // ORIG 0x080d9cd3 `mov %eax,%edi` / 0x080d9d58 `mov %eax,%esi` /
                // 0x080d9d8f `mov %eax,%ebx`：三值跨后续 call 保活于 callee-saved
                // 且无栈槽（P231/P248 三条件齐备）⇒ 纯 register 提示（非 asm 绑定）。
                register int memoryPoolIndex = getMemoryPoolIndex();
                UserSpace::T moveSpace = m_users[i].getUser()->getMoveSpace();
                int userState = m_users[i].getUser()->get_state();
                register int characNo = m_users[i].getUser()->get_charac_no(-1);
                register char* m_id = NumberToString(m_users[i].getUser()->get_acc_id(), 0);
                CMyFileLog log(__PRETTY_FUNCTION__,
                               0x514);
                log(local_428,
                    "checkBlueMarbleUserConfirm() m_id(%s) charac_no(%u)userState(%d) userSpace(%d) blueMarble_Index(%d) user_bluemarble_index(%d) slot_index(%d)",
                    m_id, characNo, userState, moveSpace, memoryPoolIndex,
                    userBlueMarbleIndex, i);
            }
        }
    }
    else
    {
        // P285：空 else{return;} 保留 if 体尾 `jmp <epilogue>; nop` 岛
        // （ORIG 0x080d9e15/0x080d9e17）。
        return;
    }
}

#include "BlueMarble/enterBlueMarble.inc"
{
    int result = 0;
    if (user == 0)
    {
        return 0x17;
    }
    if (!checkJoin())
    {
        return 0x17;
    }
    result = checkUserConditionEnterBlueMarble(user);
    if (result > 0)
    {
        return result;
    }
    result = join(user);
    if (result > 0)
    {
        return result;
    }
    if (getWaitUserCount() == m_pScript->m_info.m_playerMax)
    {
        changeState((BlueMarbleState::T)1);
    }
    return 0;
}

#include "BlueMarble/checkStartBlueMarble.inc"
{
    if (getState() != 1)
    {
        return 0;
    }
    if (getWaitUserCount() < m_pScript->m_info.m_playerMax)
    {
        changeState(BlueMarbleState::T_0);
        return 0;
    }
    return 1;
}

#include "BlueMarble/startBlueMarble.inc"
{
    if (checkStartBlueMarble())
    {
        resetBlueMarble();
        changeState((BlueMarbleState::T)2);
        setBlueMarbleMap();
        setMaxBoardSize();
        for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
        {
            if (m_users[i].getUser() == 0)
            {
                continue;
            }
            m_users[i].setUserState((BlueMarbleUserState::T)1);
            m_users[i].getUser()->increaseBlueMarbleEnterCount();
            m_users[i].getUser()->sendBlueMarbleEnterCount();
        }
        sendStartBlueMarble();
    }
}

// b28c-1 N: 第三条件 je/jne 极性（4.4 块序折叠；三目/表达式/具名 bool 三形实测均不可翻）。
// [b28d-32 AE + 真语义 bug: 旧实现两 return 值写反（全真返回 false）；取反条件形复现 ORIG 三条件 jle/jne/je + bVar1 双岛 | ORIG 0x080d6ed8]
#include "BlueMarble/checkThrowDice.inc"
{
    // [b28d-32] ORIG 0x080d6ed8：三条件全真 → bVar1=0 → `test al; je; mov$1`
    // ⇒ 返回 true（0x080d6f46 mov$0 / 0x080d6f56 mov$1）。旧实现两个 return 值
    // 写反（全真返回 false）= 真语义 bug，已对调；形态与 ORIG 逐块同构
    // （bVar1 物化 + `test al; je; mov$0/$1` 双岛）。
    if (!(m_users[seat].getBoardZone() < getMaxBoardSize() &&
          m_users[seat].getUserState() == (BlueMarbleUserState::T)1 &&
          getState() == 2))
    {
        return false;
    }
    return true;
}

#include "BlueMarble/throwDice.inc"
{
    setDiceNumber();
    sendBlueMarbleDiceInfo(seat);
}

#include "BlueMarble/userThrowDice.inc"
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x446,
                                  "BlueMarble::userThrowDice slot(%d)", seat);
        }
        else if (checkThrowDice(seat))
        {
            resetIndexInfo(seat);
            throwDice(seat);
            setMovePosition(seat, getDiceNumber());
            intoBoardZone(seat);
            sendBoardZoneActionInfo(seat, getDungeonIndex(seat),
                                    (unsigned int)getItemIndex(seat));
        }
    }
done:
    ;
}

// [b28d-32 AE] ORIG 0x080d9c08 尾岛 `jmp +0x1bf; nop`：P223 fC 条件 goto 前置
// （`if(!checkThrowDice) goto DONE; body; DONE:;`）复现 128=128 逐条全等；
// 旧 body 尾 goto 恒被 4.4 jump-thread（少 jmp+nop 2 条）。
#include "BlueMarble/movePosition.inc"
{
    int seat = getUserSlot(user);
    if (seat == -1)
    {
        LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x4f4,
                              "BlueMarble::movePosition slot(%d)", seat);
    }
    // ORIG 0x80d9ab9 xor $0x1,%eax; test; jne 出口（=false 走体）——用已验证的
    // `(int)!boolcall()` 物化形（handleStageCommand 0x84661a3 同构）。
    // [b28d-32] P223 fC 形：条件 goto 置于 body 之前，复现 ORIG 0x080d9bfd
    // 尾 `jmp +0x1bf; nop` 岛（body 尾 goto 恒被 4.4 jump-thread）。
    else
    {
        if ((int)!checkThrowDice(seat))
            goto DONE;
        PacketGuard packet;
        packet.put_header(0, 0x1af);
        packet.put_byte(seat);
        packet.put_byte(pos - getMovePosition(seat));
        packet.finalize(true);
        sendToBlueMarble(packet);
        resetIndexInfo(seat);
        setMovePosition(seat, pos - getMovePosition(seat));
        intoBoardZone(seat);
        sendBoardZoneActionInfo(seat, getDungeonIndex(seat),
                                (unsigned int)getItemIndex(seat));
    }
DONE:
    ;
}

#include "BlueMarble/checkMoveFirstZone.inc"
{
    switch (type)
    {
    case (BlueMarbleTileScript::Type)11:
    case (BlueMarbleTileScript::Type)12:
    case (BlueMarbleTileScript::Type)13:
        return 0;
    case (BlueMarbleTileScript::Type)9:
    case (BlueMarbleTileScript::Type)10:
        if (getMovePosition(seat) != 1)
        {
            return 0;
        }
        break;
    default:
        break;
    }
    return 1;
}

// b28c-1 AE: P0/N96 清绑 + P223 双岛（0x080d737c/0x080d737f）后 0 差。
#include "BlueMarble/tileType.inc"
{
    int zone = m_users[seat].getBoardZone();
    // P0+范式22：ORIG 0x080d70f2 `test;sete %al` 物化 + `test al; jne` 出口
    //（body fall）。zone 槽 -0x10（0x080d70d2）。
    // b28c-1：P0/N96 清绑（P168a② 具名 bool + 正向 goto）。
    // ORIG 0x080d70f7：test al; jne 出口 —— 正向 goto（范式23）阻 4.4 xor 取反。
    if (m_pScript->getMap(getBlueMarbleMap()) == 0)
        goto T_OUT;
    {
        BlueMarbleTileScript* tile = m_pScript->getMap(getBlueMarbleMap())->getTile(
            (unsigned int)zone);
        // b28c-1：ORIG 双岛（0x080d737c mapNull 岛 / 0x080d737f tile==0 岛）
        //   ⇒ P223 多岛形：第二个条件也用 goto 到相邻标签。
        if (tile == 0)
            goto T_OUT2;
        switch (tile->m_type)   // ORIG 0x080d713b `mov (%eax),%eax`（int 读）
        {
            case 2:
            case 8:
                setDungeonIndex(seat,
                                getRandomDungeon(seat, (BlueMarbleTileScript::Type)2));
                break;
            case 3:
                setGoldBuff(seat, tile->m_value);
                break;
            case 4:
                setExpBuff(seat, tile->m_value);
                break;
            case 5:
                setDropBuff(seat, tile->m_value);
                break;
            case 6:
                setItemIndex(seat, getRandomItem());
                break;
            case 7:
                if (!getVisitZone(zone))
                {
                    setDungeonIndex(seat,
                                    getRandomDungeon(seat,
                                                     (BlueMarbleTileScript::Type)7));
                }
                break;
            case 9:
                setVisitZone(zone);
                setMovePosition(seat, tile->m_value);
                if (checkMoveFirstZone(seat, (BlueMarbleTileScript::Type)9))
                {
                    resetBuffInfo(seat);
                }
                intoBoardZone(seat);
                break;
            case 10:
                setVisitZone(zone);
                setMovePosition(seat, tile->m_value - getMovePosition(seat));
                if (checkMoveFirstZone(seat, (BlueMarbleTileScript::Type)10))
                {
                    resetBuffInfo(seat);
                }
                intoBoardZone(seat);
                break;
            case 11:
            case 12:
            case 13:
                setDungeonIndex(seat, tile->m_value);
                break;
        default:
            break;
        }
    }
T_OUT2:
    ;
T_OUT:
    ;
}

#include "BlueMarble/intoBoardZone.inc"
{
    setDungeonIndex(zone, getRandomDungeon(zone, (BlueMarbleTileScript::Type)14));
    tileType(zone);
    setVisitZone(m_users[zone].getBoardZone());
}

#include "BlueMarble/checkLastZoneDungeonClear.inc"
{
    if (getState() == 2)
    {
        if (m_users[seat].getBoardZone() == getMaxBoardSize())
        {
            return 1;
        }
    }
    return 0;
}

#include "BlueMarble/enterDungeon.inc"
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x45c,
                                  "BlueMarble::enterDungeon slot(%d)", seat);
        }
        else
        {
            setUserState(seat, (BlueMarbleUserState::T)2);
        }
    }
done:
    ;
}

#include "BlueMarble/exitDungeon.inc"
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x49d,
                                  "BlueMarble::exitDungeon slot(%d)", seat);
        }
        else
        {
            setUserState(seat, (BlueMarbleUserState::T)1);
        }
    }
done:
    ;
}

#include "BlueMarble/clearDungeon.inc"
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x48a,
                                  "BlueMarble::clearDungeon slot(%d)", seat);
        }
        else if (checkLastZoneDungeonClear(seat))
        {
            finishBlueMarble(seat);
            resultReward();
        }
    }
done:
    ;
}

// b28c-1 AE: P0/N96 清绑 + P223 三岛（0x080d96e7/ea/ed）后 0 差。
#include "BlueMarble/giveupDungeon.inc"
{
    // b28c-1：P223 多岛形（ORIG 三岛 0x080d96e7/ea/ed：user==0、mapNull、tile==0
    //  各 goto 到相邻尾标签），P0/N96 清绑后以裸条件 + goto 复现。
    if (user == 0) goto DONE1;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x46b,
                                  "BlueMarble::giveupDungeon slot(%d)", seat);
        }
        else
        {
            if (m_pScript->getMap(getBlueMarbleMap()) == 0)
                goto DONE2;
            {
                BlueMarbleTileScript* tile = m_pScript->getMap(getBlueMarbleMap())->getTile(
                    (unsigned int)m_users[seat].getBoardZone());
                if (tile == 0)
                    goto DONE3;
                {
                    if (checkMoveFirstZone(seat,
                                           (BlueMarbleTileScript::Type)tile->m_type))
                    {
                        setMovePosition(seat, 1 - getMovePosition(seat));
                        resetBuffInfo(seat);
                        sendTileGiveUpInfo(seat);
                    }
                    setUserState(seat, (BlueMarbleUserState::T)1);
                }
            }
        }
    }
DONE1:
    ;
DONE2:
    ;
DONE3:
    ;
}

// b28c-1 AE: P223 三岛 + P194 反形裸短路后 0 差。
#include "BlueMarble/giveBonusItem.inc"
{
    // b28c-1：P223 多岛形（ORIG 三岛 0x080d9905/908/90b：user==0、state 条件假、
    //  m_addInfo==0 各落相邻尾标签）。
    if (user == 0) goto DONE1;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x4ac,
                                  "BlueMarble::giveBonusItem slot(%d)", seat);
        }
        else
        {
            // ORIG 0x080d9876..9900：bad=(state!=2||userstate!=1) 物化 mov$1/$0 后
            // test al; jne 岛2（P194 X1 反形，裸短路形）。
            if (getState() != 2 || getUserState(seat) != 1)
                goto DONE2;
            Inven_Item item;
            item.m_addInfo = getItemIndex(seat);
            item.m_addInfo2 = 1;
            if (item.m_addInfo == 0)
                goto DONE3;
            {
                insertItem(seat, item, (eItemAddReason)0x41);
                resetIndexInfo(seat);
            }
            goto DONE3;
        }
    }
DONE1:
    ;
DONE2:
    ;
DONE3:
    ;
}

// [b28d-solo-BlueMarble AE] P223 尾岛：NEXT0 岛尾必须写 `continue;`——`goto NEXT1`
// 会被 4.4 jump-thread（NEXT0 fall-through 即 NEXT2→NEXT1），少 ORIG 0x080d7e6d
// 第二条 `jmp +0x92`；continue 形保留该 jmp，双 nop 岛齐全，53=53 ndiff 0。
// ORIG 0x080d7dde
#include "BlueMarble/resultReward.inc"
{
    Inven_Item item;
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            // b28c-1：ORIG continue 目标 = 岛1（0x080d7e6c nop+jmp），P223 多岛形。
            goto NEXT0;
        }
        item.reset();
        setResultItem(i, item);
        if (item.m_addInfo != 0)
        {
            insertItem(i, item, (eItemAddReason)0x42);
            // ORIG 0x80d7e6a：insertItem 后显式 jmp 循环尾块@80d7e70（4.1 岛形，
            // 范式24 goto 落点岛阻 4.4 fall-through 合并）。
            goto NEXT1;
        }
        // b28c-1：P223 多岛形（ORIG 0x80d7e6c nop+jmp 岛 + 0x80d7e6f nop 岛）
        //  ⇒ 条件假分支同样 goto 到相邻标签；NEXT0 岛自带 goto NEXT1 以复现
        //  ORIG 0x080d7e6d 第二条 `jmp +0x92`（标签共址会被 4.4 合并少 1 条）。
        goto NEXT2;
    NEXT0:
        ;
        continue;
    NEXT2:
        ;
    NEXT1:
        ;
    }
}

#include "BlueMarble/finishBlueMarble.inc"
{
    setUserGrade(seat);
    changeState((BlueMarbleState::T)3);
    sendFinishBlueMarble();
}

// [b28d-solo-BlueMarble AE] P251/P261：if-else 链（带 -0xc grade 槽 + 逐次重载
// 比较）改 `switch (getGrade())` 后 4.4 复现 ORIG 决策树（cmp1/je、cmp1/jg、
// test/je、cmp2、cmp3）与 case 块序 0,1,2,3，104=104 ndiff 0。ORIG 0x080d7e8e
#include "BlueMarble/setResultItem.inc"
{
    // ORIG 0x080d7eb5-：每处 `(*rewards)[g]` 均从 this+0x38+0xa4 重新算
    // 基址再 call vector::operator[]（4 组 5 条装载链，无 rewards 指针
    // 局部缓存——旧缓存形每位省 2 条即 we-short 主因，P172 去缓存）。
    // ORIG 决策树 `cmp$1; je; cmp$1; jg; test; je; jmp` + case 块序 0,1,2,3
    // 且 getGrade() 结果直用 %eax（无 grade 局部槽）⇒ 原码是 switch
    // （P251/P261；if-else 链会带 -0xc 状态槽并逐次重载比较）。
    switch (m_users[seat].getGrade())
    {
    case 0:
        item.m_addInfo = m_pScript->m_info.m_rewardList[0].m_a;
        item.m_addInfo2 = m_pScript->m_info.m_rewardList[0].m_b;
        break;
    case 1:
        item.m_addInfo = m_pScript->m_info.m_rewardList[1].m_a;
        item.m_addInfo2 = m_pScript->m_info.m_rewardList[1].m_b;
        break;
    case 2:
        item.m_addInfo = m_pScript->m_info.m_rewardList[2].m_a;
        item.m_addInfo2 = m_pScript->m_info.m_rewardList[2].m_b;
        break;
    case 3:
        item.m_addInfo = m_pScript->m_info.m_rewardList[3].m_a;
        item.m_addInfo2 = m_pScript->m_info.m_rewardList[3].m_b;
        break;
    }
}

// [b28d-solo-BlueMarble NEAR(N7/N109)] P0 去缓存/register 化 + P235 union 视角后
// 177=177 gap 0。修法：addInfo/msg268/msg269/mailRet 四个具名局部内联
// （ORIG 分别直入 %ebx、直作 strncpy 实参、`test;sete;test;je` 无槽比较）、
// serverGroup/len 纯 register 复现 callee-saved 保活（P231/P248）、characNo
// 内联实参、MSG 载荷改 char[0x145]+union{raw,MSG_MAILBOX_SEND msg} 视角
// （P235；完整类型成员直访问使三字段 store 折叠为 -0x236/-0x232 绝对 disp）。
// 本轮修复 ②：strncpy dst 改 `local_273.raw + 0x11`（数组退化 + 常量偏移）
// 逐条复现 ORIG 0x080d8273 `lea -0x26f; add $0x11`；具名结构成员
// `msg.m_name11` 恒折 `lea 0x11(%eax)` ⇒ 原 N102/N4 判词作废（DIFF→NEAR）。
// 余 N7/N109：%esi/%ebx 角色互换（ORIG serverGroup→ebx、len→esi；4.4 按首次
// 使用恒 serverGroup→esi；声明序交换、声明/赋值分离两形实测均不翻）。
// ORIG 0x080d800e
#include "BlueMarble/insertItem.inc"
{
    // ORIG 0x080d801c `mov 0x7(%eax),%ebx`（Inven_Item::m_addInfo2）直入
    // callee-saved、无 addInfo 槽（P0）⇒ 内联实参。
    G_CDataManager()->m_itemList->create_item(ENUM_ITEM_CREATE_TYPE_0, item,
                                              item.m_addInfo2);
    // ORIG 0x080d80xx：链式直调（getCurCharacInvenW 返回值 %edx 直接作
    // this 压栈，无 inven 局部槽——旧缓存形多 1 条 reload）。
    int ret = ((CInventory*)m_users[seat].getUser()->getCurCharacInvenW())
                  ->insertItemIntoInventory(item, reason, true, false);
    if (ret < 0)
    {
        char local_2e[30];
        char local_12e[256];
        // ORIG 0x080d8127/0x080d8162：findString 返回值直作 strncpy 第 2 参
        // （无 msg268/msg269 槽，P0）。
        strncpy(local_2e, g_scriptStringManager_.findString(
                    4, "game_server_msg_268", 0), 0x1d);
        strncpy(local_12e, g_scriptStringManager_.findString(
                    4, "game_server_msg_269", 0), 0xff);
        // ORIG 0x080d819e `mov %eax,%ebx` / 0x080d81b6 `mov %eax,%esi`：
        // serverGroup/len 跨后续调用保活于 callee-saved 且无栈槽
        // （P231/P248 三条件齐备）⇒ 纯 register 提示。
        // [M3S4 实测-3形 · 177=177 NEAR 封顶] 残差仅 %ebx/%esi 角色互换（ORIG
        // serverGroup→%ebx、len→%esi；我方反向）：①len 内联实参（只留 serverGroup
        // register）同互换；②连同旧判词的声明序交换/声明赋值分离共 3 形 ⇒ 4.4
        // callee-saved 分配序差，无源码形可翻。
        register ENUM_SERVER_GROUP serverGroup =
            m_users[seat].getUser()->GetServerGroup();
        register size_t len = strlen(local_12e);
        // ORIG 0x080d822d `test %eax,%eax; sete %al; test %al,%al; je`：
        // mailRet 无槽（P0），比较内联进 if。
        if (WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                local_2e, item, 0,
                (unsigned)m_users[seat].getUser()->get_charac_no(-1),
                local_12e, (int)len, 0xf, serverGroup, false, false) == 0)
        {
            // ORIG 栈槽 local_273（0x145）= MSG_MAILBOX_SEND 载荷（权威布局
            // PacketTypes.h：m_name11@+0x11 / m_itemId39@+0x39 / m_extra3d@+0x3d；
            // 写指令 `mov %eax,-0x236/-0x232(%ebp)` 均 4 字节 ⇒ 字段宽一致）。
            // 载体用 char[0x145]（ORIG 该槽 1 字节对齐：-0x26f(%ebp)）+ 匿名
            // union 视角：完整类型成员直访问让 4.4 折叠 disp（P235），
            // 避免逐处 reinterpret_cast 的 base lea；MSG_MAILBOX_SEND 为
            // pack(1) POD（无 ctor/dtor）⇒ union 平凡、sizeof 0x145/align 1 不变。
            union MailPayload {
                char raw[0x145];
                MSG_MAILBOX_SEND msg;
            };
            MailPayload local_273;
            memset(local_273.raw, 0, 0x145);
            // ORIG 0x080d8273/0x080d8278 `lea -0x26f; add $0x11`：数组退化 + 常量偏移
            // 形（P214①/N102 反例解；`msg.m_name11` 结构成员恒折 disp 成 lea 0x11(%eax)）。
            // raw 为 union 字节视角（+0x11 即 m_name11 起始），偏移由字段自身证明。
            strncpy(local_273.raw + 0x11, local_2e, 0x1d);
            local_273.msg.m_itemId39 =
                static_cast<unsigned long>(item.m_addInfo);  // +0x39 附件物品 id
            local_273.msg.m_extra3d = item.m_addInfo2;       // +0x3d 附件 add_info
            m_users[seat].getUser()->m_historyLog.SendMail(&local_273.msg, 0);
        }
    }
    else
    {
        sendUpdateItemInfo(seat, ret);
    }
}

#include "BlueMarble/setLeaveUserBlueMarbleState.inc"
{
    if (getWaitUserCount() == 0)
    {
        for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
        {
            m_users[i].resetBlueMarbleUserInfo();
        }
        changeState(BlueMarbleState::T_0);
        resetBlueMarble();
    }
    else
    {
        if (getState() == 1)
        {
            changeState(BlueMarbleState::T_0);
        }
    }
}

#include "BlueMarble/setLeaveUserSlot.inc"
{
    if (m_users[seat].getUser() == 0)
    {
        m_users[seat].resetBlueMarbleUserInfo();
        LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x2c4,
                              "BlueMarble::setLeaveUserSlot slot(%d)", seat);
    }
    else
    {
        m_users[seat].getUser()->setBlueMarbleIndex(-1);
        m_users[seat].getUser()->setMoveSpace((UserSpace::T)0);
        m_users[seat].getUser()->resetBlueMarbleUserDeadCount();
        m_users[seat].resetBlueMarbleUserInfo();
    }
}

#include "BlueMarble/sendBlueMarbleDiceInfo.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x1af);
    packet.put_byte(seat);
    packet.put_byte((int)(char)getDiceNumber());
    packet.finalize(true);
    sendToBlueMarble(packet);
}

#include "BlueMarble/sendBlueMarbleRoomInfo.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x1ae);
    packet.put_short(getMemoryPoolIndex());
    packet.put_byte(getWaitUserCount());
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            continue;
        }
        packet.put_byte(i);
        packet.put_short((unsigned short)m_users[i].getUser()->get_unique_id());
    }
    packet.finalize(true);
    sendToBlueMarble(packet);
}

#include "BlueMarble/sendBoardZoneActionInfo.inc"
                                        unsigned int pos) const
{
    PacketGuard packet;
    packet.put_header(0, 0x1b2);
    packet.put_short(zone);
    packet.put_int((int)pos);
    packet.finalize(true);
    m_users[seat].getUser()->Send(packet);
}

// b28c-1 AE: P223 尾空标签岛（0x080d87f7 jmp+nop）后 0 差。
#include "BlueMarble/sendFinishBlueMarble.inc"
{
    int winner = getWinner();
    // b28c-1：P223 尾空标签岛（ORIG 0x080d87f7 `jmp +0xa8; nop` 汇合形）
    //   ⇒ 条件反极性 goto + 内层复合块（避免 goto 跨 packet 初始化声明）。
    if (winner == -1) goto done;
    {
        PacketGuard packet;
        packet.put_header(0, 0x1b1);
        packet.put_byte(winner);
        packet.finalize(true);
        sendToBlueMarble(packet);
    }
done:;
}

#include "BlueMarble/sendStartBlueMarble.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x1b0);
    packet.put_byte((int)(char)getBlueMarbleMap());
    packet.finalize(true);
    sendToBlueMarble(packet);
}

#include "BlueMarble/sendTileGiveUpInfo.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x1b3);
    packet.put_byte(seat);
    packet.finalize(true);
    sendToBlueMarble(packet);
}

#include "BlueMarble/sendUpdateItemInfo.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    // ORIG 0x080d7exx：getCurCharacInvenR 结果免槽直入 this（%eax 跨 3 条 mov 保活，
    // P0 右值内联 + 范式37）。
    ((CInventory*)m_users[seat].getUser()->getCurCharacInvenR())
        ->MakeItemPacket(INVEN_TYPE_EQUIP, idx, packet);
    packet.finalize(true);
    m_users[seat].getUser()->Send(packet);
}

#include "BlueMarble/sendToBlueMarble.inc"
{
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            continue;
        }
        m_users[i].getUser()->Send(guard);
    }
}
