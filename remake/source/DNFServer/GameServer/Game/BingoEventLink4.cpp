// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BingoEventLink4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "BingoEvent.h"
#include "CBingoTest.h"
#include "CEnvironment.h"
#include "CSystemTime.h"
#include "CGameManager.h"
#include "Action.h"

#include "GlobalData.h"
#include "CGuildServerProxy.h"
#include "CDataManager.h"
#include "CUser.h"
#include "Inven_Item.h"
#include "CItem.h"
#include "CStackableItem.h"   // m_vec3a0（+0x3a0 标记号 vector 具名起点）
#include "CMailBoxHelper.h"
#include "RDARScriptStringManager.h"
#include "LogManager.h"
#include "PacketGuard.h"

#include <bitset>
#include <boost/foreach.hpp>   // ORIG：StartAction/dailyresetData 用 BOOST_FOREACH 遍历 +0x22c map
#include <map>
#include <vector>
#include <time.h>
#include <string.h>

// C1 @ 0x080c9984：CEventBase() + vptr + new CBingo(0x1c) 包 shared_ptr<CBingo>
// 挂到 this+0xc。初始化列表 m_bingo(new CBingo()) 顺序与 ORIG 一致。
#include "BingoEventLink4/BingoEvent.inc"
    : m_bingo(new CBingo())
{
}

// ---------------------------------------------------------------------------
// ORIG 全局位工具（source 无既有 producer，objdump 直转）：
//   _Z13isSetBit_Uintji T 0x0889232e：(value & (1<<bit)) != 0
//   _Z11setBit_UintRjbi T 0x088922d7：set ? |= : &=~
#include "BingoEventLink4/isSetBit_Uint.inc"
{
    // ORIG: 恒寄存器 `1<<bit`（int 移位）并内联，无具名局部槽；`1u` 会差成 `shr`+分开。
    return (value & (1 << bit)) != 0;
}

#include "BingoEventLink4/setBit_Uint.inc"
{
    if (set)
    {
        value |= (1u << bit);
    }
    else
    {
        value &= ~(1u << bit);
    }
}

// ---------------------------------------------------------------------------
// BingoEvent 方法（docs/class_func_reports/BingoEvent/ 13 报告全文已读）。
// ---------------------------------------------------------------------------

// D1 @ 0x080c9a06：vptr + shared_ptr<CBingo> 析构 + CEventBase 析构（编译器按
// 声明逆序生成：m_bingo 先于 CEventBase）。D0 @ 0x080c9a68 由编译器生成。
#include "BingoEventLink4/dtor_BingoEvent.inc"
{
}

// getBingoSystem @ 0x080c9fea：返回 this+0xc —— 成员 boost::shared_ptr<CBingo>
// 实例地址（ORIG ctor 将 new CBingo(0x1c) 挂到该槽；调用方经 operator-> 解引用）。
#include "BingoEventLink4/getBingoSystem.inc"
{
    return &m_bingo;   // +0x0c boost::shared_ptr<CBingo> 实例地址
}

// StartEvent @ 0x080c9a86：置事件标志；按当前秒与疲劳小时槽（G_CEnvironment
// +0x37c）取日号，1..31 内重置棋盘与 quiz，打日志。
#include "BingoEventLink4/StartEvent.inc"
{
    SetEventFlag(true);
    int hourLimit = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // env+0x37c
    long curSec = GlobalData::s_systemTime_.getCurSec();
    int day = m_bingo->getToday(curSec, hourLimit);
    if (day > 0 && day < 0x20)
    {
        m_bingo->resetBoard(day);
        m_bingo->resetQuiz(day);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x2a, 0);
        trace("BingoEvent Day(%d)", day);
    }
}

// StartEvent(Word_Param) @ 0x080c9b68：经 vptr+0x8（slot 2）调无参 StartEvent()，
// 打日志。param 未参与计算（ORIG 如此）。
#include "BingoEventLink4/StartEvent_1.inc"
{
    // b26 N 封顶（N7）：残差仅虚调用 callee 寄存器 edx↔eax 互换
    // （ORIG mov (%eax),%edx; call *%edx vs 我们 mov (%eax),%eax; call *%eax）。
    StartEvent();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x30, 0);
    trace("BingoEvent Event Start!");
}

// StartAction @ 0x080c9bb8：遍历 CGameManager+0x22c（accId→CUser* map），
// 对每个在线用户发 bingo 数据包。
// [b28d-D9] 修复/N89：BOOST_FOREACH 复现（102→5）。原「残 = getUserByAccIdMap
// inline getter out-of-line call」已消除：ORIG 无该 getter 符号（直读 +0x22c），
// 调用点改 friend 直访 m_userByAccId。
#include "BingoEventLink4/StartAction.inc"
{
    // [b28d-D9] ORIG 0x080c9bb8 用 BOOST_FOREACH 展开（contain/begin/end/deref/next/
    // done 全序列），非手写迭代器循环；容器为 G_CGameManager()+0x22c map。
    typedef std::map<unsigned int, CUser*>::value_type UserPair;
    BOOST_FOREACH(UserPair& pr, G_CGameManager()->m_userByAccId)
    {
        CUser* user = pr.second;
        if (user != 0)
        {
            user->sendBingoData();
        }
    }
}

// EndEvent @ 0x080c9d30：置事件标志 + 日志。
#include "BingoEventLink4/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x3f, 0);
    trace("BingoEvent Event End!");
}

// dailyresetData @ 0x080c9d82：按日重置棋盘/quiz；随后遍历在线用户，清
// BingoData、重发数据包并刷新装饰物计数。
// [b28d-D9] 修复/N89：BOOST_FOREACH 复现（143→5）。原「残 = getUserByAccIdMap
// inline getter out-of-line call」已消除（同上：friend 直访 m_userByAccId）。
#include "BingoEventLink4/dailyresetData.inc"
{
    int hourLimit = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // env+0x37c
    long curSec = GlobalData::s_systemTime_.getCurSec();
    int day = m_bingo->getToday(curSec, hourLimit);
    if (day > 0 && day < 0x20)
    {
        m_bingo->resetBoard(day);
        m_bingo->resetQuiz(day);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x50, 0);
        trace("BingoEvent Day(%d)", day);
    }

    // [b28d-D9] ORIG 0x080c9d82 同样为 BOOST_FOREACH 展开（deref 后取 pair.second
    // 落 -0xc 槽，判空后三步操作）。
    typedef std::map<unsigned int, CUser*>::value_type UserPair;
    BOOST_FOREACH(UserPair& pr, G_CGameManager()->m_userByAccId)
    {
        CUser* user = pr.second;
        if (user != 0)
        {
            static_cast<BingoData*>(user->getBingoData())->clear();
            user->sendBingoData();
            user->sendDecorationObjectCnt(0);
        }
    }
}

// reqBingoMark @ 0x080c9ff6（docs/class_func_reports/BingoEvent/reqBingoMark.md）：
// user/item 判空 → 0x13；已标记 3+ 次 → 0x7；物品请求号越界 → 0x17；随机值
// 无效 → 0x13；标记成功且新增连线时逐线发奖励，回 0x1c3 包，返回 0。
// [b28d-D9] P209/P238：unsigned 局部 for 尾测 4.4 恒 mem 直比 + 常量 store 中转形（ORIG 0x080c9ff6 sz 0x0000026c）
#include "BingoEventLink4/reqBingoMark.inc"
{
    if (param.m_user == 0)
    {
        return 0x13;
    }
    if (param.m_item == 0)
    {
        return 0x13;
    }
    // [b23S3C] ORIG 0x080c9ff6 `call getBingoData` 共 8 次（+0x39/+0xc4/+0xd7/+0x10b/
    // +0x129/+0x13b/+0x172/+0x1ee），无 `data` 具名指针槽 ⇒ P48 每用点重取；
    // 字段访问一律经 BingoData* 出单条 disp 形（`movb $0x1,0x6(%eax)`/`movzbl 0x4(%eax)`/
    // `mov %dl,0x4(%eax)`）。calNewValue 的 &m_eventId ORIG 落 %ebx 跨 operator-> 存活
    // （P197②），残余记 N。
    // +0x04（连线数，signed char）> 2 即已达 3 次标记上限。
    if (static_cast<signed char>(
            static_cast<BingoData*>(param.m_user->getBingoData())->m_pad4) > 2)
    {
        return 0x7;
    }
    // CStackableItem +0x3a0 = m_vec3a0（vector<int>），第 0 项为请求标记号（推断）。
    // [b23S3C] ORIG `add $0x3a0,%eax; mov %eax,(%esp)` 直落 operator[] 的 this 实参，
    // 无 itemVec 具名槽（P0/P48）⇒ 索引表达式内联；同时少一槽使 PacketGuard 帧基
    // 由 -0x30 回到 ORIG 的 -0x2c。
    int markNo = reinterpret_cast<CStackableItem*>(param.m_item)->m_vec3a0[0];
    if (markNo < 1 || markNo > 0x32)
    {
        return 0x17;
    }
    int randValue = m_bingo->getBingoRandomVaule(markNo);
    if (randValue == 0)
    {
        return 0x13;
    }
    unsigned int oldBoard =
        static_cast<BingoData*>(param.m_user->getBingoData())->m_eventId;
    int newLines = m_bingo->calNewValue(
        static_cast<BingoData*>(param.m_user->getBingoData())->m_eventId, randValue);
    if (static_cast<BingoData*>(param.m_user->getBingoData())->m_eventId != oldBoard)
    {
        static_cast<BingoData*>(param.m_user->getBingoData())->m_needSave = true;
        int diff = newLines - static_cast<signed char>(
            static_cast<BingoData*>(param.m_user->getBingoData())->m_pad4);
        if (diff > 0)
        {
            // b28c-L10 判词（N，P209 族）：ORIG 0x080ca37c clamp 段为
            // `mov -0x10(%ebp),%eax; cmp $0x3,%eax; jle` + `mov $0x3,%eax;
            // mov %eax,-0x10(%ebp)`（载寄存器）；4.4 恒 mem 直比
            // `cmpl $0x3,-0x10(%ebp); jle` + `movl $0x3,-0x10(%ebp)`。
            // 探针 /tmp/l10_probe10/11/12.cpp 十二形全败：仅「unsigned 变量 +
            // (int) cast」可载寄存器，但外层条件码变 je/jbe（ORIG 为 jle）。
            // [SweepBingoEventLink4] 追加探针 /tmp/probe_clamp{,2,3}.cpp 三十余形复核：
            // 指针形 `int* p=&diff; if(*p>3)`（fI）可出载寄存器比较，但 store 恒折
            // `movl $0x3,(%eax)`；三目 `diff = diff>3?3:diff;` 产 `cmovle`（本 TU
            // 未路由 -march=i486）；ORIG 的 `mov $0x3,%eax; mov %eax,-0x10(%ebp)`
            // 共享 store 属 N76/N97 跨版本形 ⇒ N 成立。
            // 语义等价（diff>0 已保证非负）。
            // [P6S6 2026-09-11 复核·仍 N] 本波按 ORIG 0x080ca14f
            //   `mov -0x10(%ebp),%eax; cmp $0x3,%eax; jle 0x080ca15c; mov $0x3,%eax;
            //    (0x080ca15c) mov %eax,-0x10(%ebp)`（load + 共享 store）反推试三目：
            //   `diff = diff > 3 ? 3 : diff;` 实测得 4.4 if-conversion 形
            //   `cmpl $0x3,-0x10(%ebp); cmovle -0x10(%ebp),%eax`（182/183，且删 jle 岛）；
            //   `if/else diff=diff` 自赋值被前端消去 → 回到 181/183 直比直存原形。
            //   ⇒ 4.4 -O0 对 clamp 走 cmov/mem 直比两条路，均非 4.1 的 load+共享 store，
            //     源码不可择 ⇒ 维持 N。
            if (diff > 3)
            {
                diff = 3;
            }
            static_cast<BingoData*>(param.m_user->getBingoData())->m_pad4 =
                static_cast<char>(newLines);
            for (int i = 0; i < diff; ++i)
            {
                sendOneMatchLineReward(*param.m_user);
            }
        }
    }
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x1c3);
    packet.put_byte(randValue);
    packet.put_int(static_cast<int>(
        static_cast<BingoData*>(param.m_user->getBingoData())->m_eventId));
    packet.finalize(true);
    param.m_user->Send(packet);
    return 0;
}

// sendOneMatchLineReward @ 0x080ca3d4：读 m_bingo.m_map30[CDataManager+0xb568+0x30
// =+0xb598] 的 key=1（单线奖励配置 <type, value>），按 type 构造 Inven_Item 后
// 以系统邮件发放。
// [SweepBingoEventLink4] 已闭合（AE 143/143，原 DIFF 144/145）：count 由
// unsigned int 改 int ⇒ 复现 ORIG 0x080ca5cd `mov -0x10(%ebp),%edx`（压栈前
// 提前装载 count，与形参 unsigned int 异型触发 4.4 同形）；unsigned 版 4.4
// 恒延迟到 `mov %eax,0x8(%esp)` 前装载（P211 档③/P75 反例，探针实测）。
#include "BingoEventLink4/sendOneMatchLineReward.inc"
{
    // [b28d-D9] P167 差集（缺 G_CDataManager ×1）：ORIG 0x080ca3d4 无 rewardMap
    // 引用缓存槽，find 与 end 各自重新内联 `call G_CDataManager; lea 0xb598(%eax)`
    // （P0/P183）；具名引用使第二次 call 被槽缓存吞掉。
    std::map<int, std::pair<int, int> >::iterator it =
        G_CDataManager()->m_bingo.m_map30.find(1);
    if (it == G_CDataManager()->m_bingo.m_map30.end())
    {
        LogManager::logFormat(1, "BingoEvent.cpp",
                              __PRETTY_FUNCTION__, 0xc0,
                              "bingo script error");
        return;
    }

    const int type = it->second.first;
    const int value = it->second.second;

    Inven_Item item;
    // int（非 unsigned）：与形参 unsigned int 异型 ⇒ 4.4 提前装载（见函数头）。
    int count = 0;
    if (type == 0)
    {
        count = value;
    }
    else if (type == 1)
    {
        item.m_addInfo = 1;                                  // +0x02 itemIdx=1
        item.m_field1 = 2;                                   // +0x01 ITEM_TYPE=2
        item.m_addInfo2 = value;                             // +0x07 add_info
        item.m_fieldb = 1;                                   // +0x0b endurance
        item.ResetItemAttr();                                // +0x06 attr=0
    }
    else
    {
        CItem* found = G_CDataManager()->find_item(type);
        if (found == 0)
        {
            return;
        }
        item.m_addInfo = type;                               // +0x02 itemIdx
        found->make_item(item);                              // vtbl slot +0x08
        item.set_add_info(value);                            // m_addInfo2 = value
    }

    char sender[21];
    strncpy(sender,
            g_scriptStringManager_.findString(4, "game_server_msg_292", 0), 0x14);
    char title[256];
    strncpy(title,
            g_scriptStringManager_.findString(4, "game_server_msg_293", 0), 0xff);

    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender, item, count,
        static_cast<unsigned int>(user.getCurCharacNo()),
        title, static_cast<int>(strlen(title)),
        0, user.GetServerGroup(), false, false);
}

// ---------------------------------------------------------------------------
// sendBingoData @ 0x0869134e：取 repeat 事件 0x6a（BingoEvent）当日棋盘号，
// 连同用户 bingo 数据（棋盘值 +0x00、连线数 +0x05）打包 0x1c2 发送。
#include "BingoEventLink4/sendBingoData.inc"
{
    int today = 0;
    BingoEvent* event = reinterpret_cast<BingoEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0x6a));
    if (event != 0)
    {
        // ORIG +0x03b：getBingoSystem → boost::shared_ptr<CBingo>::operator->()（内部
        // pt()）→ getToday 链式内联。用 shared_ptr 的 operator-> 复现 pt() 调用，
        // 不裸转换 CBingo** 绕过。
        today = reinterpret_cast<boost::shared_ptr<CBingo>*>(
                    event->getBingoSystem())
                    ->operator->()
                    ->getToday();
    }

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x1c2);
    packet.put_byte(static_cast<int>(static_cast<char>(today)));
    packet.put_int(static_cast<int>(
        static_cast<BingoData*>(getBingoData())->m_eventId));
    // ORIG (uint)*(byte*)(iVar2+5) movzbl 直读 +0x05；bool `?1:0` 会产 setcc 归一。
    packet.put_byte(static_cast<int>(
        static_cast<unsigned char>(
            static_cast<BingoData*>(getBingoData())->m_isWin)));
    packet.finalize(true);
    Send(packet);
}
