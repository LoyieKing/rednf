// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 SeriaRoomAniDecoEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <boost/foreach.hpp>       // ORIG：questClearLog 用 BOOST_FOREACH
#include <boost/lexical_cast.hpp>  // ORIG：buyItemLog/questClearLog 用 lexical_cast
#include "GlobalData.h"
#include "W8LT_EventTail.h"
#include "GameTypes.h"            // cMyTrace
#include "GlobalData.h"
#include "CSystemTime.h"
#include "StreamPool.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"
#include "MoreTypes.h"            // SIG_GIVE_GROWTH_CREATURE_EVENT
#include "CUser.h"
#include "../../../Library/Include/Core/SecureStdio.h"   // ss_strcpy                // GrowthCreatureEvent / getGrowthCreatureEventdata
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "CDataManager.h"
#include "MySQL.h"            // MySQL::{set_query,exec,getAffectedRowCount}
#include "CEnvironment.h"     // DB_HANDLE_9 / E_SERVER_GROUP_NONE
#include "CItem.h"
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GameWorld.h"
#include "RDARScriptStringManager.h"
#include "CMailBoxHelper.h"

// ============================ SeriaRoomAniDecoEvent ============================

#include "SeriaRoomAniDecoEvent/SeriaRoomAniDecoEvent.inc"
#include "SeriaRoomAniDecoEvent/dtor_SeriaRoomAniDecoEvent.inc"

#include "SeriaRoomAniDecoEvent/IsEventing.inc"
{
    return CEventBase::IsEventing(user);
}

#include "SeriaRoomAniDecoEvent/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x18, 0);
    trace("SeriaRoomAniDecoEvent Event start!");
}

#include "SeriaRoomAniDecoEvent/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x24, 0);
    trace("SeriaRoomAniDecoEvent Event end!");
}

#include "SeriaRoomAniDecoEvent/StartEvent_1.inc"
{
    SetEventFlag(true);
    cMyTrace trace(
        __PRETTY_FUNCTION__, 0x1e, 0);
    trace("SeriaRoomAniDecoEvent Event start! [monitor request]");
}

#include "SeriaRoomAniDecoEvent/StartAction.inc"
{
    // 全服 state>2 用户广播 SendEventInfo。
    // [b28d-28] 同 GiveGrowCreatureEvent::StartAction（P183 去缓存 + P28 后置）。
    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            SendEventInfo(*user);
        }
    }
}

#include "SeriaRoomAniDecoEvent/dailyresetData.inc"
{
    // 全服 state<=2 用户跳过；state>2 用户尝试发放当日摆件奖励（ORIG 顺序）。
    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() <= 2)
            continue;
        SendAniDecoItem(*user);
    }
}

// [b28d-solo-W8LT_EventTail] DIFF→NEAR(N7) | ORIG 0x0827b75a | P167 CLEAR（23/23）。
// 修法①：`int index` 改直接写 `item.m_addInfo`（ORIG 0x0827b80b `mov %eax,-0x63`
// = item@-0x65 + 0x02）⇒ 槽集/帧 0x1ac→0x19c 全对齐；②P223 多岛：守卫2/守卫4
// 改负条件 goto + 尾标签串 `L_itemIdx:; L_index:;` ⇒ 复现 ORIG +0x231/+0x234
// 两 nop 中继岛，148=148。残余（N 封顶）：ReqDBSendNewSystemMail 实参中
// `GetServerGroupToPvP()`↔`strlen()` 的 callee-saved 角色互换——ORIG 4.1 分配序
// esi(serverGroup)→edi(strlen)→ebx(characNo)，4.4.6 为 edi→esi→ebx（N109：裸 register
// 与 asm 绑定两形均不翻；两值均无槽往返，P0 禁具名缓存，且 T5 禁为装载序改类型）。
#include "SeriaRoomAniDecoEvent/SendAniDecoItem.inc"
{
    // [b23S3C] ORIG 0x0827b75a 形核（P48/每用点重取）：①单例 G_CDataManager() 在
    // maxAniDecoItem/getSeriaRoomAniDecoItem/find_item 三用点各重取（无具名缓存槽）；
    // ②getSeriaRoomAniDecoInfo() ORIG call 两次（sret @-0x24 供条件比较、@-0x28 供
    // itemIdx 入参与尾段 ++/set/makeRequest），故写两次独立取用而非复用同一局部；
    // ③sg 去具名局部直落实参（ORIG 直落 0x1c(%esp)，无 -off 往返）；
    // ④strncpy 后 ORIG 无补 NUL 的 movb ⇒ 删 title[255]。
    // 残余：ORIG `movzwl+setl` vs 4.4 `movzwl+setge/jne` 极性与 +2 字段的 cwtl/movswl
    // 单段 sext 选择 = 负例表"跨版本 sext/zext"/setcc-fold；帧 0x1ac vs 0x19c = N82。
    seriaRoom_AniDeco deco = user.getSeriaRoomAniDecoInfo();
    if (deco.m_field0 < static_cast<short>(G_CDataManager()->maxAniDecoItem()))
    {
        seriaRoom_AniDeco deco2 = user.getSeriaRoomAniDecoInfo();
        int itemIdx = G_CDataManager()->getSeriaRoomAniDecoItem(deco2.m_field0);
        // P223 多岛细则：ORIG 守卫2/守卫4 各落独立 nop 中继岛（+0x231/+0x234）
        // ⇒ 负条件 goto + 尾标签串（正向嵌套 if 被 4.4 jump-thread 成 0 岛）。
        if (itemIdx == 0)
            goto L_itemIdx;
        {
            Inven_Item item;
            CItem* src = G_CDataManager()->find_item(itemIdx);
            if (src != 0)
            {
                // ORIG 0x0827b80b `mov %eax,-0x63` = item(@-0x65)+0x02 ⇒ 写字段。
                item.m_addInfo = src->get_index();
                src->make_item(item);
                item.set_add_info(1);                   // 绑定发放

                // 标题取 game_server_msg_290、正文 289（RDAR 表 4）。
                char title[256];
                strncpy(
                    title,
                    g_scriptStringManager_.findString(4, "game_server_msg_290", 0),
                    0xff);
                if (item.m_addInfo == 0)
                    goto L_index;
                {
                    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                        g_scriptStringManager_.findString(
                            4, "game_server_msg_289", 0),
                        item, 0, (unsigned int)user.getCurCharacNo(),
                        title, (int)strlen(title), 0,
                        static_cast<ENUM_SERVER_GROUP>(
                            user.GetServerGroupToPvP()),
                        false, false);

                    deco2.m_field0 = deco2.m_field0 + 1;
                    user.setSeriaRoomAniDecoInfo(deco2);
                    DB_UpdateSeriaRoomAnimationDeco::makeRequest(
                        user.get_acc_id(),
                        (short)user.GetServerGroupToPvP(),
                        user.getCurCharacNo(),
                        deco2.m_field0, deco2.m_field2);
                }
            }
        }
    }
L_itemIdx:;
L_index:;
}

#include "SeriaRoomAniDecoEvent/SendEventInfo.inc"
{
    PacketGuard pkt;
    pkt.put_header(0, 0x1c1);
    // [b28c-30] P0/限定①：ORIG 0x0827b9c8 段 deco 为 sret 匿名临时槽（-0xc）——
    // `call getSeriaRoomAniDecoInfo; movzwl -0xa(%ebp),%eax; call put_short` 单语句；
    // 具名 deco 局部改槽序（4.4 pkt -0x14/deco -0x18 反向）。
    // [b23S3C] ORIG movzwl base+2（= deco_flow）⇒ 取 m_field2
    pkt.put_short(user.getSeriaRoomAniDecoInfo().m_field2);
    pkt.finalize(true);
    user.Send(pkt);
}

#include "SeriaRoomAniDecoEvent/CheckEventInfo.inc"
{
    return 0;
}

// [b23S3C] ORIG 0x0827ba52 `movzwl -0x22(%ebp)` = deco(sret @-0x24)+2 ⇒ 奖励索引用
// deco_flow(m_field2)，原码误取 m_field0（+0 是 receive_flow 计数）⇒ 真 bug 修正；
// strncpy 后 ORIG 无 `movb $0x0,title+255` ⇒ 删补 NUL 行（与 SendAniDecoItem 同）
// 残余 28 全为帧槽号平移（4.1 把 index 挤在 item 之后 -0x5f、4.4 独立高槽 = N82）、
// N7 虚调用 %edx/%eax 归属、以及 `mov MEM,%eax; test` vs 4.4 `cmpl $0,MEM`（N-88 候选）
// ⇒ 语义已核，调用点级无剩余缺口。
// [b28d-solo-W8LT_EventTail] DIFF→NEAR(N7) | ORIG 0x0827ba52 | P167 CLEAR（15/15）。
// 修法：`int index = src->get_index()` 改为直接写 `item.m_addInfo`（ORIG
// 0x0827bad5 `mov %eax,-0x5f` = item@-0x61 + 0x02）⇒ 槽集/帧全对齐、102=102；
// 亦解释 ORIG 的 unsigned 字段 test 形（P209）。残余（N 封顶，与 SendAniDecoItem
// 同族）：ReqDBSendNewSystemMail 实参中 `GetServerGroupToPvP()`↔`strlen()` 的
// callee-saved 角色互换——ORIG 4.1 esi→edi→ebx，4.4.6 edi→esi→ebx（N109；本 TU
// 已路由 4.4.6 后端，P230 亦不翻；两值无槽往返，P0 禁具名缓存）。
#include "SeriaRoomAniDecoEvent/processEvent.inc"
{
    seriaRoom_AniDeco deco = user.getSeriaRoomAniDecoInfo();
    int itemIdx =
        G_CDataManager()->getSeriaRoomAniDecoRewardItem(deco.m_field2);
    if (itemIdx == 0)
        return 0;

    Inven_Item item;
    CItem* src = G_CDataManager()->find_item(itemIdx);
    if (src != 0)
    {
        // ORIG 0x0827bad5 `mov %eax,-0x5f`：item(@-0x61)+0x02 ⇒ 原码直接写
        // Inven_Item::m_addInfo 字段（非独立 int 局部；make_item 不写 +0x02，
        // CEquipItem/CStackableItem::make_item 写点已核）。
        item.m_addInfo = src->get_index();
        src->make_item(item);
        item.set_add_info(1);

        char title[256];
        strncpy(title,
                g_scriptStringManager_.findString(4, "game_server_msg_291", 0),
                0xff);
        if (item.m_addInfo != 0)
        {
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, "game_server_msg_289", 0),
                item, 0, (unsigned int)user.getCurCharacNo(),
                title, (int)strlen(title), 0,
                static_cast<ENUM_SERVER_GROUP>(user.GetServerGroupToPvP()),
                false, false);
        }
    }
    return 0;
}

#include "SeriaRoomAniDecoEvent/makeRequest.inc"
    unsigned int accId, short serverGroup, int characNo, short field1, short field2)
{
    // ORIG T 0x0844af74：opcode 0x31e / -1 / 五参顺序写流。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbb98), true);
    guard.operator*()->operator<<(0x31e);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);
    guard.operator*()->operator<<(serverGroup);
    guard.operator*()->operator<<(characNo);
    guard.operator*()->operator<<(field1);
    guard.operator*()->operator<<(field2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_UpdateSeriaRoomAnimationDeco::dispatch —— ORIG T 0x0844adc0（报告
// docs/class_func_reports/DB_UpdateSeriaRoomAnimationDeco/dispatch.md）。
// GetDBHandle(DB_HANDLE_9,0) → >>accId(u32) >>serverGroup(s16) >>characNo(int)
// >>field1(s16) >>field2(s16) → UPDATE receive_flow=field1, deco_flow=field2；
// affected==0 → INSERT（occ_date=now()）；exec 失败返 0。
// ============================================================================
#include "SeriaRoomAniDecoEvent/dispatch.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int accId = 0;
    short serverGroup = 0;
    int characNo = 0;
    short field1 = 0;
    short field2 = 0;
    *stream >> accId;
    *stream >> serverGroup;
    *stream >> characNo;
    *stream >> field1;
    *stream >> field2;
    db->set_query(
        "upDate event_1208_seriaroom_anideco set receive_flow = %d, deco_flow = %d where m_id = %s and server_id = %d and charac_no = %d",
        field1, field2, NumberToString(accId, 0), (int)serverGroup, characNo);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_1208_seriaroom_anideco(m_id ,server_id, charac_no, receive_flow, deco_flow, occ_date) values(%s, %d, %d, %d, %d, now())",
            NumberToString(accId, 0), (int)serverGroup, characNo, field1,
            field2);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// [L32 T3] ORIG `correctQuestIds` D 0x09370168 size 0xc：TU 级 file-scope 全局
//   （非 const ⇒ .data；值 {0x1567,0x1568,0x1569}），由 OneADayItemShopEvent::isQuestId
//   0x081ba1e2 线性查表消费。原先误做成函数内 `static const int`（局部符号 + .rodata）
//   ⇒ 提为 TU 级定义。位置在首个强符号（SeriaRoomAniDecoEvent::C2Ev）之后，GI 键名不变。
int correctQuestIds[3] = {0x1567, 0x1568, 0x1569};

// ============================ OneADayItemShopEvent ============================
#include "SeriaRoomAniDecoEvent/OneADayItemShopEvent.inc"
#include "SeriaRoomAniDecoEvent/dtor_OneADayItemShopEvent.inc"

#include "SeriaRoomAniDecoEvent/StartEvent_2.inc"
{
    SetEventFlag(true);
}

#include "SeriaRoomAniDecoEvent/EndEvent_1.inc"
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x2d, 0);
    trace("OneADayItemShopEvent Event End!");
}

// [b28d-solo-W8LT_EventTail] AE | ORIG 0x081ba03a | P167 CLEAR（2/2）。
// 修法：P227 全表达式临时链（具名 trace 改 `cMyTrace(...)(fmt, a, b)`）⇒
// ORIG 的 `push esi/ebx` + ctor 前两条 movswl 跨 ctor 保活形逐条复现（35=35）。
#include "SeriaRoomAniDecoEvent/StartEvent_3.inc"
{
    // ORIG：先虚调 vptr+0x10 槽（即 this->StartEvent()），再取 this+0x04/+0x06
    // （stADDINFO 两 ushort）符号扩展打印 AddInfo:%d,%d（0xffff → -1）。
    this->StartEvent();
    // P227 全表达式临时链：ORIG 在 ctor 前把两 vararg 求值到 %esi/%ebx 跨 ctor 保活。
    cMyTrace(__PRETTY_FUNCTION__, 0x20, 0)(
        "OneADayItemShopEvent(AddInfo:%d,%d) Event Start!",
        static_cast<short>(m_addInfo.m_info0),
        static_cast<short>(m_addInfo.m_info1));
}

// ORIG 0x081ba100：G_GameWorld()->ResetOneADayItemShop()。
#include "SeriaRoomAniDecoEvent/StartAction_1.inc"
{
    G_GameWorld()->ResetOneADayItemShop();
}

#include "SeriaRoomAniDecoEvent/deliveryStart.inc"
{
    // b28c-L19：ORIG 早返形（cmpl/je→尾 nop 岛 + body 尾 jmp 跳过），非正向 if 包裹。
    if (d == 0)
        return;
    d->attach(1, this);                                // type1: questClear
    d->attach(2, this);                                // type2: buyItem
}

#include "SeriaRoomAniDecoEvent/deliveryEnd.inc"
{
    // b28c-L19：同 deliveryStart，ORIG 早返形。
    if (d == 0)
        return;
    d->detach(1, this);
    d->detach(2, this);
}

#include "SeriaRoomAniDecoEvent/delivery.inc"
{
    // P17/ORIG：线性 cmp/je 链（单读双判、body 后置）= 两 case switch 形。
    switch (msg.m_type)
    {
        case 1:
            questClearLog(msg);
            break;
        case 2:
            buyItemLog(msg);
            break;
    }
}

#include "SeriaRoomAniDecoEvent/isQuestId.inc"
{
    // ORIG：TU 级表 correctQuestIds（D 0x09370168, size 0xc）=
    // {0x1567, 0x1568, 0x1569}；命中返回 1。定义见文件前部（file-scope，非 const）。
    for (int i = 0; i < 3; ++i)
    {
        if (correctQuestIds[i] == questId)
            return true;
    }
    return false;
}

// [b28d-solo-W8LT_EventTail] AE | ORIG 0x081ba21e | P167 CLEAR（38/38）。
// 修法：①字段经 m 视图读（非 msg.）；②删 progress/p 具名局部（P0/P183）；
// ③条件 `empty()`→`size() != 0`（p167 MISSING size）；④循环体由 `text = X + sep`
// 改左结合累加 `text = text + X + sep`——补回 p167 MISSING 的 1×`operator+` 与
// 2×`~string`（ORIG 两次 op+ 的临时析构），AST 左结合形与 ORIG 逐条同构；
// ⑤memcpy dest 用具名 `sig.m_data`：4.4 复现 ORIG「先压 size/src、后 lea+add 算
// dest」的装载序（原 `(unsigned char*)&sig + 0x14` 使 4.4 提前算到 %edx）；
// ⑥`sig` 局部加 `__attribute__((aligned(4)))`：ORIG 4.1 给 pack(1) 载荷 4 字节
// 对齐槽（-0xbc），4.4 默认 1 字节对齐（-0xb9）⇒ 补属性后槽基与全帧逐条对齐。
// ⑦memcpy 无补 NUL（P171 照抄）。
#include "SeriaRoomAniDecoEvent/questClearLog.inc"
{
    EventStageLogDeliveryMsg& m = reinterpret_cast<EventStageLogDeliveryMsg&>(msg);  // ORIG：无 stage_view 调用

    if (!isQuestId(m.m_logType))
        return;

    // ORIG 4.1 给 pack(1) 载荷 4 字节对齐槽（-0xbc）；4.4 默认 1 字节对齐（-0xb9）
    // ⇒ 局部加 aligned(4) 复现槽基（不改类型 sizeof/布局，仅本函数栈槽）。
    SIG_EVENT_STAGE_LOG sig __attribute__((aligned(4)));
    // ORIG：经 m 视图（-0x1c 槽）读三字段，写入顺序 [1][2][3][0][4]
    sig.m_field0[1] = m.m_accId;
    sig.m_field0[2] = m.m_characNo;
    sig.m_field0[3] = m.m_serverGroup;
    sig.m_field0[0] = 0x6d;
    sig.m_field0[4] = 1;

    std::string text;                                  // 进度 first 值 '/' 连接
    std::string sep("/");
    // ORIG：无 progress 具名局部（每用点重读 m_progressList，P157/P183）；
    // 条件用 `size() != 0`（p167 MISSING size / EXTRA empty）；循环内用
    // `operator=`（p167 MISSING aSERKSs / EXTRA pLERKSs）。
    if (m.m_progressList != 0 && m.m_progressList->size() != 0)
    {
        typedef const std::pair<int, int> ProgressPair;
        BOOST_FOREACH(ProgressPair& pr, *m.m_progressList)
        {
            // ORIG：两次 operator+（text+lex 后接 +sep）⇒ 原码为左结合累加式。
            text = text + boost::lexical_cast<std::string>(pr.first) + sep;
        }
    }
    if (text.size() < 100)
    {
        memcpy(sig.m_data, text.c_str(), text.size());  // SIG 内嵌文本区（≤100）
    }

    DB_InsertEventStageLog::makeRequest(sig);
}

// [b28d-solo-W8LT_EventTail] AE | ORIG 0x081ba5e8 | P167 CLEAR（25/25）。
// 修法：①字段经 m 视图读；②删 p 具名局部（P0）；③`text += X + "/"` →
// `text = X + sep`（sep=string("/")，P13/P0 构造序在条件前）；④lexical_cast
// 实参直传 payload+2 地址（const&，无具名 value）；⑤删补 NUL（P171）；
// ⑥memcpy dest 改具名 `sig.m_data` ⇒ 复现 ORIG「先压 size/src、后 lea+add 算 dest」
// （原 `(unsigned char*)&sig + 0x14` 使 4.4 提前算到 %edx，P75 残差清零）。
// 依赖：CItemShop::IsOneADayShop char→bool（Main 代落）。
#include "SeriaRoomAniDecoEvent/buyItemLog.inc"
{
    // ORIG 汇编：shopId 实参取自 msg+0x10（m_logType），
    // CDataManager::m_itemShop(+0x8cbc)->IsOneADayShop(shopId) 为 0 则直接返回。
    EventStageLogDeliveryMsg& m = reinterpret_cast<EventStageLogDeliveryMsg&>(msg);  // ORIG：无 stage_view 调用
    if (G_CDataManager()->m_itemShop->IsOneADayShop(m.m_logType) != 1)
        return;

    SIG_EVENT_STAGE_LOG sig;    // 本函数 ORIG 槽基 -0x98 已 4 对齐，无需 aligned 属性
    // ORIG：经 m 视图（-0xc 槽）读三字段，写入顺序 [1][2][3][0][4]。
    sig.m_field0[1] = m.m_accId;
    sig.m_field0[2] = m.m_characNo;
    sig.m_field0[3] = m.m_serverGroup;
    sig.m_field0[0] = 0x6d;
    sig.m_field0[4] = 9;

    // ORIG：text 默认构造 + sep=string("/") 在 payloadPtr 检查前构造；赋值走
    // operator=（非 +=）；lexical_cast 实参直接传 payload+2 地址（const& 形参，
    // 无具名 value 局部，P0）；memcpy 无补 NUL（P171 照抄不写）。
    std::string text;
    std::string sep("/");
    if (m.m_payloadPtr != 0)
    {
        text = boost::lexical_cast<std::string>(
                   *reinterpret_cast<const unsigned long*>(m.m_payloadPtr + 2)) +
               sep;
        if (text.size() < 100)
        {
            memcpy(sig.m_data, text.c_str(), text.size());
        }
    }

    DB_InsertEventStageLog::makeRequest(sig);
}

// ==================== DB request 类（W8LT_EventTail.h 声明点） ====================
#include "SeriaRoomAniDecoEvent/makeRequest_1.inc"
{
    // ORIG T 0x0844920e：opcode 0x333 / -1 / GetInBuffer<SIG> 后 memcpy 0x78。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb7df),
                       true);
    guard.operator*()->operator<<(0x333);      // op*(guard)→op<<(int)
    guard.operator*()->operator<<(-1);         // op*(guard)→op<<(int)
    SIG_EVENT_STAGE_LOG* out =
        guard.operator->()->GetInBuffer<SIG_EVENT_STAGE_LOG>();
    memcpy(out, &sig, sizeof(SIG_EVENT_STAGE_LOG));     // ORIG: 0x78
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
