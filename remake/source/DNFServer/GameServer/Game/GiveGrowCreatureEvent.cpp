// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GiveGrowCreatureEvent/<Func>.inc（不参与编译、内部不含 #include）。
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

// ORIG W 0x081b763e _Z9ss_strcpyILj30EEiRAT__cPKc：头内联模板版（同
// OnlyOrigBatch12.cpp 口径），调用点以数组引用选择本重载。
template <size_t N>
#include "GiveGrowCreatureEvent/ss_strcpy.inc"

// ============================ GiveGrowCreatureEvent ============================
#include "GiveGrowCreatureEvent/GiveGrowCreatureEvent.inc"
#include "GiveGrowCreatureEvent/dtor_GiveGrowCreatureEvent.inc"

#include "GiveGrowCreatureEvent/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x18, 0);
    trace("GiveGrowCreatureEvent Event start!");
}

#include "GiveGrowCreatureEvent/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x24, 0);
    trace("GiveGrowCreatureEvent Event end!");
}

#include "GiveGrowCreatureEvent/StartEvent_1.inc"
{
    SetEventFlag(true);
    cMyTrace trace(
        __PRETTY_FUNCTION__, 0x1e, 0);
    trace("GiveGrowCreatureEvent Event start! [monitor request]");
}

#include "GiveGrowCreatureEvent/StartAction.inc"
{
    // 遍历全服在线用户，state>2（地下城中在线态）广播事件信息。
    // [b28d-28] P183/P13/P28：ORIG 每轮重调 G_GameWorld()（begin/end 各自
    // 重物化，无 world/users 缓存局部）+ 增量段 `ppEi` 后置 ⇒ 源形如下。
    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            SendGrowthCreatureEventInfo(*user);
        }
    }
}

// [b28d-solo-W8LT_EventTail] DIFF→N 封顶（215=215，仅装载序/对齐 nop） | ORIG 0x081b6b70
// | P167 CLEAR（40/40）。
// 修法①（真语义 bug）：内层循环 ORIG 仅在 nameLen!=0 时 break（nameLen==0 继续），
// 原码无条件 break；②P171 删 `mine == 0`；③P167/P96：map 迭代器非 const + `->`
// （ptEv）、vector 迭代器 `(*it).`（deEv）；④P13/P28 缓存 end + 后置 `it++`；
// ⑤P198/P0 具名 jobCount（-0x10 槽往返）；⑥nameLen 分支改
// `if(nameLen==0){put_byte(1);} else{put_byte(2);put_int;put_str;break;}`（P47/P191
// 块序：ORIG then（==0 块）直落 kt++、else 后置跳外层）⇒ 块序与 p167 差 1 全消。
// 残余（N 封顶）：①`mine->begin()/end()` 调用点 ORIG this-first（`mov -0xc,%edx` 先）
// vs 4.4.6 恒 sret-first（`lea -0x3c,%eax` 先）——探针 t1..t4（初始化式/(*p)./引用
// 接收/直接初始化括号）全 sret-first；②else 块尾 ORIG 的 1 条对齐 nop（§6.1 豁免）
// 与本函数尾 4.4 对齐 nop 的位置差（两侧各 1 条，数量相等）。
#include "GiveGrowCreatureEvent/SendGrowthCreatureEventInfo.inc"
{
    PacketGuard pkt;
    pkt.put_header(0, 0x1c0);

    int now = GlobalData::s_systemTime_.getCurSec();
    int job = user.get_charac_job();
    std::vector<GrowthCreatureEvent::eventCharacInfo>* mine =
        static_cast<std::vector<GrowthCreatureEvent::eventCharacInfo>*>(
            user.getGrowthCreatureEventdata());

    // P198/P0：ORIG -0x10 有槽往返 ⇒ 具名 jobCount（声明+赋值分离）。
    int jobCount;
    jobCount = G_CDataManager()->getGiveGrowCreatureEventJobCount(now);
    pkt.put_byte(jobCount);

    // P13/P28：ORIG begin/end 各求值一次存具名局部、增量段后置 `jt++`（ppEi）；
    // 无 script 具名局部（直用 G_CDataManager()->m_createCreatureGift）；无 mapKey
    // 缓存局部（每用点重读 `jt->first`，P157/P183）。
    // P167/P96：ORIG map 迭代器用非 const `_Rb_tree_iterator` + `->`（ptEv），
    // vector 迭代器用 `(*it).`（deEv）——两者实例化不同成员。
    std::map<int, CharacterCreateCreatureGiftScript::eventJob>::iterator jt =
        G_CDataManager()->m_createCreatureGift.m_eventJobMap.begin();
    std::map<int, CharacterCreateCreatureGiftScript::eventJob>::iterator jtEnd =
        G_CDataManager()->m_createCreatureGift.m_eventJobMap.end();
    while (jt != jtEnd)
    {
        if (jt->second.m_minLevel < now && now < jt->second.m_maxLevel)
        {
            pkt.put_byte(jt->first);
            // ORIG：!empty() 走内层循环、else 空分支。
            if (!mine->empty())
            {
                std::vector<GrowthCreatureEvent::eventCharacInfo>::iterator kt =
                    mine->begin();
                std::vector<GrowthCreatureEvent::eventCharacInfo>::iterator ktEnd =
                    mine->end();
                while (kt != ktEnd)
                {
                    if ((*kt).m_job == jt->first)
                    {
                        int nameLen = static_cast<int>(strlen((*kt).m_name));
                        // ORIG：then=nameLen==0（put_byte(1) 直落 kt++）、else=nameLen!=0
                        // （put_byte(2)/put_int/put_str 后 break 跳外层）⇒ if/else 块序。
                        if (nameLen == 0)
                        {
                            pkt.put_byte(1);                // nameLen==0：继续内层循环
                        }
                        else
                        {
                            pkt.put_byte(2);
                            pkt.put_int(nameLen);
                            pkt.put_str((*kt).m_name, nameLen);
                            break;                      // ORIG：nameLen!=0 跳出内层
                        }
                    }
                    kt++;
                }
            }
            else
            {
                if (jt->first == job)
                    pkt.put_byte(1);
                else
                    pkt.put_byte(0);
            }
        }
        jt++;
    }

    pkt.finalize(true);
    user.Send(pkt);
}

// [b28d-solo-W8LT_EventTail] DIFF→N 封顶 | ORIG 0x081b6e7a | P167 CLEAR（11/11）。
// 修法：①P171 删 `mine == 0` 守卫（ORIG 只判 empty）；②P13 缓存 end() 具名局部；
// ③P28 增量段后置 `it++`（ppEi）；④P167 判据：`it->m_job` → `(*it).m_job`
// （ORIG 发 `__normal_iterator::deEv`；`->` 会误选 `ptEv`，p167 差集 MISSING deEv /
// EXTRA ptEv 一击命中）⇒ 73=73、槽集/块序全等。
// 残余 2 条（N 封顶）：`begin()/end()` 调用点 ORIG this-first（`mov -0xc,%edx` 先）
// vs 4.4.6 恒 sret-first（`lea -0x1c,%eax` 先）——P75 跨版本实参装载时刻；已穷举
// 5 形（初始化式/(*p)./引用接收/逗号声明/直接初始化括号）全部 sret-first。
#include "GiveGrowCreatureEvent/CheckEventInfo.inc"
{
    int job = user.get_charac_job();
    int now = GlobalData::s_systemTime_.getCurSec();
    if (!G_CDataManager()->isAvailableGiveGrowCreatureEventJob(job, now))
        return 3;

    std::vector<GrowthCreatureEvent::eventCharacInfo>* mine =
        static_cast<std::vector<GrowthCreatureEvent::eventCharacInfo>*>(
            user.getGrowthCreatureEventdata());
    // P171：ORIG 无 `mine == 0` 守卫（只判 empty）。
    if (mine->empty())
        return 0;

    // P13/P28：ORIG 循环前一次求值 end() 存具名局部、增量段用后置 `it++`（ppEi）。
    std::vector<GrowthCreatureEvent::eventCharacInfo>::iterator it = mine->begin();
    std::vector<GrowthCreatureEvent::eventCharacInfo>::iterator itEnd = mine->end();
    while (it != itEnd)
    {
        if ((*it).m_job == job)
            return 3;                                   // 该职业已领过
        it++;
    }
    return 0;
}

// [b28d-solo-W8LT_EventTail] AE | ORIG 0x081b6f76 | P167 CLEAR（30/30）。
// 修法①（真语义 bug）：ORIG 0x081b6fb9 第二查表实参为 infinityIdx（-0x18），
// 原码误传 creatureItemId ⇒ 已改正。修法②（P0/P183）：删 mine 具名指针局部
// （结果直传 push_back）⇒ 全槽回移 4、135=135 逐条全等。
#include "GiveGrowCreatureEvent/processEvent.inc"
{
    // ORIG：链式直调（无具名中间局部，P0 右值内联）；G_CDataManager() 每用
    // 点重取（×4，P48 无 CSE）；sigInfo/info 两段各自重取 serverGroup/no/job。
    int creatureItemId = user.getCurCharacInvenR()->GetCreatureMgrR()->GetCreatureItemId();

    int infinityIdx = G_CDataManager()->getInfinityChangeCreatureIndex(creatureItemId);
    // ORIG 0x081b6fb9：第二查表实参取 infinityIdx（-0x18），非 creatureItemId（真语义 bug）。
    int clientIdx = G_CDataManager()->getClientChangeCreatureIndex(infinityIdx);
    if (infinityIdx == -1 || clientIdx == -1)
        return 0x16;

    CItem* item = G_CDataManager()->find_item(infinityIdx);
    if (item == 0)
        return 0x16;

    int changeAbleLev = G_CDataManager()->getChangeAbleCreatureLev();
    if (item->GetItemType() == 0x16)
    {
        // 可成长生物：进化到客户端版本后继续走登记流程。
        if (!user.getCurCharacInvenW()->GetCreatureMgrW()->EventEvoluteCreature(
                infinityIdx, changeAbleLev, clientIdx))
            return 0x16;
    }

    // DB 登记当前角色（SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo 0x2c）。
    SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo sigInfo;
    sigInfo.m_serverGroup = static_cast<unsigned short>(user.GetServerGroupToPvP());
    sigInfo.m_characNo = user.getCurCharacNo();
    sigInfo.m_job = user.get_charac_job();
    ss_strcpy(sigInfo.m_name, user.getCurCharacName());
    DB_EventSaveGiveGrowCreature::makeRequest(user.get_acc_id(), sigInfo);

    // 内存登记（GrowthCreatureEvent::eventCharacInfo 同布局 0x2c）。
    GrowthCreatureEvent::eventCharacInfo info;
    info.m_serverGroup = static_cast<unsigned short>(user.GetServerGroupToPvP());
    info.m_characNo = user.getCurCharacNo();
    info.m_job = user.get_charac_job();
    ss_strcpy(info.m_name, user.getCurCharacName());

    // P0/P183：ORIG 无 mine 具名局部（getGrowthCreatureEventdata 结果直传
    // push_back 实参），具名指针多 2 条 mov 且全槽深 4。
    static_cast<std::vector<GrowthCreatureEvent::eventCharacInfo>*>(
        user.getGrowthCreatureEventdata())->push_back(info);
    return 0;
}

#include "GiveGrowCreatureEvent/isGrowCreatureMaxLevel.inc"
{
    // ORIG: 三连 call 结果直传 (%esp)，无具名局部（P0：无栈槽→无具名局部）。
    return user.getCurCharacInvenR()->GetCreatureMgrR()->isGrowCreatureMaxLevel();
}

// [b28d-solo-W8LT_EventTail] DIFF→NEAR(N7) | ORIG 0x081b717a | P167 CLEAR（18/18）。
// 修法①（真语义 bug）：ORIG 命中后置 found/foundIdx 并 break、循环后统一发 DB 请求
// （只处理首个命中项），原码在循环体内直接发请求；②比较处显式 `(short)` cast 复现
// ORIG `movzwl+cwtl` 两条；③P158 反向：found 声明 uchar + `==0` 复现 `cmpb $0; je`
// （bool 恒 xor 物化）；④P287 `size() > i` + unsigned i 复现 mem 直比 seta；
// ⑤P223 负例边界：found 守卫改正向包裹消第 3 个尾岛（ORIG 仅 2 岛）⇒ 130=130。
// 残余 24 行 = 6 处 `(*mine)[i]` 的**实参装载序**（N 封顶）：ORIG 4.1 恒
// `mov this,%eax; mov idx,%edx`（this-first，idx 走 edx 中转），4.4.6 恒 idx-first
// （`mov idx,%eax; mov %eax,0x4(%esp); mov this,%eax`）。探针 /tmp/probe_cn（路由同款
// 4.4.6 后端）穷举 8 形：uint/int/size_t/ushort 下标、`(*p)[i]`/`p->operator[](i)`/
// 显式 cast/局部引用 receiver 全部 idx-first ⇒ 源形不可择，记 N（P75 跨版本实参装载
// 时刻，T5：装载序非类型旋钮）。
#include "GiveGrowCreatureEvent/changeCharacName.inc"
    CUser* user, int key, char* old_, int new_)     // 0x081b717a
{
    // ORIG 汇编语义：user!=0 时 key=m_characNo、new_=新名字指针；
    // serverGroup 取 target 用户 GetServerGroupToPvP。元素命中条件为
    // m_serverGroup 相同 && m_characNo==key；若现名与新名 strcmp 不同，
    // strncpy(m_name,new_,0x1d) 并发 DB_EventUpdateGiveGrowCreature。
    // [b28d-28] ORIG 0x081b7180 只判 `user != 0`（无 new_ 判空）；serverGroup
    // 为 int 槽（`mov %eax,-0x1c`）；mine 只判 `empty()`（无 `mine == 0`，P171）。
    if (user == 0)
        return;

    int serverGroup = user->GetServerGroupToPvP();
    std::vector<GrowthCreatureEvent::eventCharacInfo>* mine =
        static_cast<std::vector<GrowthCreatureEvent::eventCharacInfo>*>(
            user->getGrowthCreatureEventdata());
    if (mine->empty())
        return;

    // ORIG 槽序 -0x14/-0xd/-0xc（foundIdx/found/i）：命中即置 found+foundIdx 并
    // break，循环后统一发 DB 请求（只处理首个命中项）。
    int foundIdx = 0;
    // P158 反向：ORIG `cmpb $0,-0xd(%ebp); je`（无 xor 物化）⇒ found 是 uchar。
    unsigned char found = 0;
    // P287：正写 `size() > idx` 优于 `idx < size()`（后者引 %ebx 缓存 +2 条）。
    for (unsigned int i = 0; mine->size() > i; ++i)
    {
        // P157/P183：ORIG 每个字段读取都重新 `operator[](i)`，禁缓存引用 e。
        // ORIG `movzwl + cwtl`（两条）⇒ 比较处显式 (short) cast（字段为 unsigned short）。
        if (static_cast<short>((*mine)[i].m_serverGroup) == serverGroup &&
            (*mine)[i].m_characNo == key &&
            strcmp((*mine)[i].m_name, old_ /*new 名*/) != 0)
        {
            strncpy((*mine)[i].m_name, old_, 0x1d);
            found = 1;
            foundIdx = i;
            break;
        }
    }
    // P223 负例边界：found 守卫若写早返会多产第 3 个尾岛（ORIG 仅 2 岛）⇒ 正向包裹。
    if (found != 0)
    {
        SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo sigInfo;
        sigInfo.m_serverGroup = (*mine)[foundIdx].m_serverGroup;
        sigInfo.m_characNo = (*mine)[foundIdx].m_characNo;
        sigInfo.m_job = (*mine)[foundIdx].m_job;
        strncpy(sigInfo.m_name, (*mine)[foundIdx].m_name, 0x1d);
        DB_EventUpdateGiveGrowCreature::makeRequest(user->get_acc_id(), sigInfo);
    }
}

#include "GiveGrowCreatureEvent/IsEventing.inc"
{
    return CEventBase::IsEventing(user);
}

// [b28d-solo-W8LT_EventTail] AE | ORIG 0x08449b32 | P167 CLEAR（16/16）。
// 修法①：P159 返回类型 bool→void（ORIG 正常路径尾块无 mov $1）；②P259 删
// stream 具名局部（Acquire 直传 guard ctor）。
#include "GiveGrowCreatureEvent/makeRequest.inc"
    unsigned int accId, SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo& info)
{
    // ORIG T 0x08449b32：opcode 0x317 / -1 / accId（无符号 lsEj）/ eventCharacInfo(0x2c)。
    // ORIG 用 operator*()/operator->() 链式（de/pt 调用）；memset 仅清 4 字节。
    // P259：ORIG 无 stream 具名局部（Acquire 结果直传 guard ctor 实参）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb983), true);
    guard.operator*()->operator<<(0x317);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);
    SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo* out =
        guard.operator->()->GetInBuffer<SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo>();
    memset(out, 0, 4);
    memcpy(out, &info, sizeof(info));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// [b28d-solo-W8LT_EventTail] AE | ORIG 0x08449d1c | P167 CLEAR（16/16）。
// 修法同 DB_EventSave：P159 返回类型 bool→void + P259 删 stream 具名局部。
#include "GiveGrowCreatureEvent/makeRequest_1.inc"
    unsigned int accId, SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo& info)
{
    // ORIG T 0x08449d1c：与 save 同构（同一 opcode 0x317 组）。
    // P259：同 save（无 stream 具名局部）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb9aa), true);
    guard.operator*()->operator<<(0x317);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);
    SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo* out =
        guard.operator->()->GetInBuffer<SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo>();
    memset(out, 0, 4);          // ORIG 0x08449d?? `movl $0x4,0x8(%esp)`（照抄，非 sizeof）
    memcpy(out, &info, sizeof(info));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
