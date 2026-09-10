// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W6_CeraShopDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CCeraShop.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "SpecialItemHandler.h"
#include "CStreamGuard.h"
#include "CCeraShopBonusItemEvent.h"   // 权威声明（P104 ODR 收敛，2026-08-29 EventManagerWave）
#include "Stream.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "InterfacePacketBuf.h"
#include "PacketTypes.h"
#include "CDataManager.h"
#include "CItemList.h"
#include "CItemLimitEdition.h"
#include "CEquipItem.h"
#include "CUser.h"
#include "DB_UpdateGiftCount.h"     // DB_UpdateGiftCount / DB_UpdateBreakAwayAccruedCera 权威声明
#include "CInventory.h"
#include "CMailBoxHelper.h"
#include "GameWorld.h"
#include "CGuildServerProxy.h"   // CEventManager / CEventBase 权威声明
#include "CEventMgr.h"
#include "CGenUniqueNo.h"
#include "LogManager.h"
#include "CCirculationStatistic.h"
#include "RDARScriptStringManager.h"
#include "TaiwanCash.h"          // WongWork::IPG::CIPGHelper 权威声明
#include "CSystemTime.h"
#include "CEnvironment.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <algorithm>

#include "BreakAwayPreventSystem.h"   // break_away_prevent::CBreakAwayPreventSystem 权威声明


// ORIG T @0x0831ce5e [updateUsedCera.md]：仅转发 DB_UpdateBreakAwayAccruedCera::
// makeRequest；不访问任何成员字段，调用视图本地布局即 ABI 安全。
#include "W6_CeraShopDeps/updateUsedCera.inc"
                                                                 unsigned int cera)
{
    DB_UpdateBreakAwayAccruedCera::makeRequest(accId, cera);
}

// DB_UpdateGiftCount/DB_UpdateBreakAwayAccruedCera 权威声明（含 makeRequest/dispatch
// 与生命周期实现）见 DB_UpdateGiftCount.h（本批次新建 TU，已登记 CMake note）。

// WongWork::CWebEvent 权威声明现位于 GlobalData.h（ReqSaveEventItem ORIG T 0x086c1910）。

namespace WongWork
{

namespace
{
const int kBuyEntryStride = 0x1cf;   // MSG 条目区步长（ProcessBuyRequest imul 实证）
}  // namespace

// SIPGData 为 char 镜像（TaiwanCash.h）；按偏移读形。P49：-O0 下 inline helper
// 会产真实 call 符号（仅NEW 发明符号违规），统一用宏直发 ORIG 的内联寻址形。
// AtoiAt/ReadIntAt 语义字段化由后续 IPG 批次收口【推断】。
#define AtoiAt(data, off) atoi(reinterpret_cast<const char*>(data) + (off))
// [b28d-S278 P319] 整型加法+回 cast：复现 4.1 `add $K,%eax; mov %eax,%reg`
// 两步形（指针算术 4.4 恒折单条 `lea K(%eax),%reg`；探针 /tmp/lg_probe4.cpp fA/fB）。
#define BufAt(data, off) ((char*)((unsigned int)(data) + (off)))

// ---------------------------------------------------------------------------
// CallSpecialItemHandler —— ORIG T 0x08325854 (0x2a)
// 薄转发：m_handler->execute(type,param,result)。
// ---------------------------------------------------------------------------
#include "W6_CeraShopDeps/CallSpecialItemHandler.inc"
                                       STSpecailItem_Result& result)
{
    return m_handler->execute(type, param, result);
}

// ---------------------------------------------------------------------------
// LogError —— ORIG T 0x0832033a (0x13f)
// StreamPool 取流 → guard.operator*() 两次分别 operator<<(0x32e)(int 形参) 与
// operator<<(input[0])(unsigned int 形参，ORIG 0x080e2292) → SIG ctor
// (0x08328110 清零) + 7 字段按 ORIG 写入序（+0/+4/+0x14/+0x10/+8/+0xc/+0x18）→
// guard.operator->()->put_binary(&sig,0x1c)（P103 链保留）→ put(DB_Q) → 返回 1
// （P159 尾部 mov $1 跨 D1 保活）。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_CeraShopDeps] verdict=AE（check_tu_routed DIFF→AE；修法=P248 档③
// 字段同型化：stErrorLogInput_t::m_field0 与 SIG_CERASHOP_ERROR_LOG::m_field0
// int→unsigned int，消 4.4 的 `mov (%eax),%eax; mov %eax,%ebx` 两段，复现 ORIG
// 0x08320394 单段直落 callee-saved。p167 CLEAR）。
#include "W6_CeraShopDeps/LogError.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("CeraShop.cpp", 0x7a),
                       true);

    (**guard).operator<<(0x32e);          // operator*() → operator<<(int)
    // [b28c-L7] 负例：P226 裸语句表达式形 `({ *(guard.operator*()); })` 实测
    // **反劣**（4.4 额外物化 guard 临时 + 双 landing pad，ndiff 3→+20）。
    // ORIG 0x080c8c46 段 `mov (%eax),%ebx` 单段直落 callee-saved，4.4 恒
    // `mov (%eax),%eax; mov %eax,%ebx` 两段（N18a 反向），源形不可造 ⇒ 封顶。
    (**guard).operator<<(static_cast<unsigned int>(input.m_field0));  // uint 形参

    SIG_CERASHOP_ERROR_LOG sig;           // ctor 0x08328110 清零 +0..+0x18
    sig.m_field0 = input.m_field0;
    sig.m_field4 = input.m_field4;
    sig.m_field14 = input.m_field8;
    sig.m_field10 = input.m_field10;
    // ORIG 0x08320408：call G_CEnvironment → mov 0x378(%eax),%eax（dword 读
    // = m_serverEnvir.m_serverGroup）→ movzbl %al 截断；具名访问复现单 disp 读。
    sig.m_field8 = static_cast<unsigned char>(
        G_CEnvironment()->m_serverEnvir.m_serverGroup);
    sig.m_fieldc = G_CEnvironment()->get_channel_no();
    sig.m_field18 = input.m_fieldc;

    guard.operator->()->put_binary(&sig, 0x1c);   // P103 operator-> 链
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}

// ---------------------------------------------------------------------------
// _ProcessIPGError —— ORIG T 0x083237f6 (0x182)
// 【§6.2】P167 差集归零；AtoiAt/ReadIntAt/BufAt 已宏化（P49），109 vs 105
// 残余为寻址折叠/寄存器形态（N7/N18b 族）。
// resData->status(+0x1c)==2 且 atoi(resData+0x173)==1 时走 helper 取消链；
// 否则 logFormat "IPG ERROR (Goods No: %d)(%s)"；末尾 user!=0 转 ProcessError。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_CeraShopDeps] verdict=DIFF-N 封顶（orig 109 / our 110，gap=1；
// p167 CLEAR 3/3）。本轮修法：P59/P222 分支极性翻转（ORIG 0x08323837 `je <user==0
// 块>`，then 为 9 参重载 slot7、else 为 2 参重载 slot9 + return 直跳 epilogue）、
// P235 具名成员访问（resData->m_type 消 ReadIntAt 宏的 add+mov）、P319 整型加法
// BufAt 复现 add+mov 并使虚指针回落栈槽（与 ORIG `call *-0x28(%ebp)` 同形）、
// P223 负条件 goto 尾空标签复现 4.1 if-join 岛（`call ProcessError; jmp; nop`）。
// 残余唯一 1 条：logFormat 0x177 实参 4.4 恒插 `mov %eax,%edx` 中转
// （int 形参/const char* cast 两形实测不消）⇒ 语义等价，byte 层 N 封顶。
#include "W6_CeraShopDeps/_ProcessIPGError.inc"
                                 IPG::SIPGData* resData, unsigned char errCodeA,
                                 int errCodeB, unsigned int productNo, int subCode,
                                 int code2, char flag)
{
    // [b28d-S278] P235 具名成员访问消 add：ORIG 0x0832380e `mov 0x1c(%eax),%eax`
    // 单条 disp（ReadIntAt 宏的指针算术 4.4 恒发 add+mov）。
    if (resData->m_type == 2)
    {
        if (atoi(BufAt(resData, 0x173)) == 1)
        {
            // [b28c-P59/P222] ORIG 0x08323837 `je <user==0 块>`：then 为 user!=0
            // 的 9 参重载（vptr+0x1c slot7），else 为 user==0 的 2 参重载
            // （vptr+0x24 slot9，TaiwanCash.h:239/245 槽序实证）+ return 直跳
            // epilogue（+0x17a），故写正向 if(user!=0)/else{...;return;}。
            if (user != 0)
            {
                GlobalData::s_pIPGHelper->IPGOutputCancel(
                    user,
                    BufAt(resData, 0x159),
                    BufAt(reqData, 0x69),
                    BufAt(reqData, 0x71),
                    BufAt(reqData, 0x7d),
                    BufAt(reqData, 0x91),
                    BufAt(reqData, 0xf5),
                    static_cast<void*>(0),
                    static_cast<Taiwan::BillTransaction*>(0));
            }
            else
            {
                GlobalData::s_pIPGHelper->IPGOutputCancel(reqData, resData);
                return;
            }
        }
        else
        {
            LogManager::logFormat(
                1, "CeraShop.cpp",
                "void WongWork::CCeraShop::_ProcessIPGError(CUser*, WongWork::IPG::SIPGData*, "
                "WongWork::IPG::SIPGData*, unsigned char, int, unsigned int, int, int, char)",
                0xb47, "IPG ERROR (Goods No: %d)(%s)",
                static_cast<int>(productNo), BufAt(resData, 0x177));
        }
    }

    // [b28d-S278 P223] ORIG 0x08323929 尾部为 4.1 if-join 岛：
    // `cmpl;je <nop 占位>; body; jmp join; nop; join:`。4.4 对正向 if 恒
    // jump-thread ⇒ 唯一复现形 = 负条件 goto 到共享尾空标签。
    if (user == 0)
    {
        goto ipg_error_done;
    }
    ProcessError(user, static_cast<unsigned short>(errCodeA), errCodeB,
                 productNo, subCode, code2, flag);
ipg_error_done:;
}

// gift 族（coinGift×2/itemGift/avatarGift/creatureGift）ORIG 均内联
// memset 0x100（rep stos 慢路）+ getCurCharacName/findString/sprintf 三调用，
// 本批删除 FormatGiftMessage/GiftHistoryLog helper（P49/§4.1 发明符号），逐
// 函数内联复刻 ⇒ P167 差集归零；coinGift 族余 gap≈N78 rep 段（28 vs 5 insn）。
// ---------------------------------------------------------------------------
// _processCoinGift —— ORIG T 0x0832619c (0x195)：四参重载
// 代币物品 idx=1、count=currencyType 构造 Inven_Item → 赠送邮件 + 统计日志。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_CeraShopDeps] verdict=AE（DIFF→AE）。修法：①P28 声明序——item 先、
// msgBuf 后（ORIG 0x0832619c rep stos 在 ResetItemAttr call +0x041 之后 +0x046）；
// ②真 bug：SendCeraShopGiftStatisticLog 第 3/4 实参应取 item.m_addInfo(-0x43) /
// item.m_addInfo2(-0x3e)，原写常量 1L/currencyType（数据源取错，P92 族）。
#include "W6_CeraShopDeps/_processCoinGift.inc"
                                 unsigned int buyIdx, int cost)
{
    Inven_Item item;
    item.m_addInfo = 1;                               // movl $1 → idx=1
    item.m_field1 = 2;                                // ITEM_TYPE_2（movb $2,-0x44）
    item.m_addInfo2 = static_cast<int>(currencyType); // movzwl → set_add_info2
    item.m_fieldb = 2;                                // endurance=2
    item.ResetItemAttr();

    // [b28d-S278 实证] ORIG 0x0832619c：msgBuf 的 rep stos 在 ResetItemAttr call
    // （+0x041）之后（+0x046）⇒ 声明序为 item 先、msgBuf 后（P28 声明序即槽序）。
    char msgBuf[256] = {0};

    sprintf(msgBuf,
            g_scriptStringManager_.findString(4, "game_server_msg_02", (bool*)0),
            user->getCurCharacName());

    user->m_historyLog.SendMailForCeraShopGift(
        static_cast<unsigned int>(cost), '\x01', 1u,
        static_cast<unsigned int>(currencyType));
    // [b28d-S278 真 bug 修正] ORIG +0x11b：统计额取 item.m_addInfo(-0x43) 与
    // item.m_addInfo2(-0x3e)，原写常量 1/currencyType 取错数据源（P92 族）。
    user->SendCeraShopGiftStatisticLog(buyIdx, static_cast<unsigned int>(cost),
                                       static_cast<long>(item.m_addInfo),
                                       static_cast<unsigned int>(item.m_addInfo2));

    CMailBoxHelper::ReqDBSendNewMailCashShop(user, item, 0u,
                                             static_cast<unsigned int>(cost), true,
                                             msgBuf, static_cast<int>(strlen(msgBuf)));
}

// ---------------------------------------------------------------------------
// _processCoinGift —— ORIG T 0x08326332 (0x1a4)：五参重载（count 选用赠品种类）
// kindFlag：count==0xec→4、count==0xca→5、否则 0。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_CeraShopDeps] verdict=AE（DIFF→AE）。修法同四参重载：P28 声明序
// （ORIG 0x08326332 rep stos 在 ResetItemAttr 之后）+ 统计日志第 3/4 实参改
// item.m_addInfo(-0x54)/item.m_addInfo2(-0x4f)（原写 count/currencyType）。
#include "W6_CeraShopDeps/_processCoinGift_1.inc"
                                 unsigned int buyIdx, int cost, int count)
{
    Inven_Item item;
    item.m_addInfo = count;                           // movl param_5 → idx=count
    item.m_field1 = 2;                                // ITEM_TYPE_2
    item.m_addInfo2 = static_cast<int>(currencyType);
    item.m_fieldb = 2;
    item.ResetItemAttr();

    // [b28d-S278 实证] ORIG 0x08326332：msgBuf rep stos 在 ResetItemAttr（+0x041）
    // 之后（+0x046）⇒ item 先声明、msgBuf 后（P28）。
    char msgBuf[256] = {0};

    sprintf(msgBuf,
            g_scriptStringManager_.findString(4, "game_server_msg_02", (bool*)0),
            user->getCurCharacName());

    char kindFlag = '\0';
    if (count == 0xec)
    {
        kindFlag = '\x04';
    }
    else if (count == 0xca)
    {
        kindFlag = '\x05';
    }

    user->m_historyLog.SendMailForCeraShopGift(
        static_cast<unsigned int>(cost), kindFlag,
        static_cast<unsigned int>(count),
        static_cast<unsigned int>(currencyType));
    user->SendCeraShopGiftStatisticLog(buyIdx, static_cast<unsigned int>(cost),
                                       static_cast<long>(item.m_addInfo),
                                       static_cast<unsigned int>(item.m_addInfo2));

    CMailBoxHelper::ReqDBSendNewMailCashShop(user, item, 0u,
                                             static_cast<unsigned int>(cost), true,
                                             msgBuf, static_cast<int>(strlen(msgBuf)));
}

// ---------------------------------------------------------------------------
// _processItemGift —— ORIG T 0x083264d6 (0x112)
// 入参为按值 Inven_Item（调用方已完成拷贝）；giftKind 作邮件参数与统计额。
// ---------------------------------------------------------------------------
#include "W6_CeraShopDeps/_processItemGift.inc"
                                 unsigned int buyIdx, int giftKind)
{
    // [b28d-L14 实证] ORIG 0x083260c2：rep stos 零初始化 + getCurCharacName 作
    // sprintf 第三实参内联，结果经 %ebx 跨 findString call 保活（P52/P214③）。
    char msgBuf[256] = {0};
    sprintf(msgBuf,
            g_scriptStringManager_.findString(4, "game_server_msg_02", (bool*)0),
            user->getCurCharacName());

    user->m_historyLog.SendMailForCeraShopGift(
        static_cast<unsigned int>(giftKind), '\0',
        static_cast<unsigned int>(item.m_addInfo),
        static_cast<unsigned int>(item.m_addInfo2));
    user->SendCeraShopGiftStatisticLog(buyIdx, static_cast<unsigned int>(giftKind),
                                       static_cast<long>(item.m_addInfo),
                                       static_cast<unsigned int>(item.m_addInfo2));

    CMailBoxHelper::ReqDBSendNewMailCashShop(user, item, 0u,
                                             static_cast<unsigned int>(giftKind), true,
                                             msgBuf, static_cast<int>(strlen(msgBuf)));
}

// ---------------------------------------------------------------------------
// _processAvatarGift —— ORIG T 0x08326036 (0x166)
// deadline = now + periodDays*86400（periodDays∈{0,0x16d} 保持 0）→ avatar 赠送邮件。
// ---------------------------------------------------------------------------
#include "W6_CeraShopDeps/_processAvatarGift.inc"
                                   char c2, char const* agencyName,
                                   unsigned int buyIdx, int cost)
{
    int deadline = 0;
    if (periodDays != 0 && periodDays != 0x16d)
    {
        deadline = GlobalData::s_systemTime_.getCurSec() + periodDays * 0x15180;
    }

    // [b28d-L14 实证] ORIG 0x08326036：msgBuf 声明在 deadline 计算之后
    // （rep stos 0x40 dwords），getCurCharacName 内联进 sprintf 实参（P52）。
    char msgBuf[256] = {0};
    sprintf(msgBuf,
            g_scriptStringManager_.findString(4, "game_server_msg_02", (bool*)0),
            user->getCurCharacName());

    user->m_historyLog.SendMailForCeraShopGift(
        static_cast<unsigned int>(cost), '\x02', static_cast<unsigned int>(slot), 1u);
    user->SendCeraShopGiftStatisticLog(buyIdx, static_cast<unsigned int>(cost),
                                       static_cast<long>(slot), 1u);

    CMailBoxHelper::ReqDBSendNewAvatarMailCashShop(
        user, cost, slot, deadline, c1, c2, agencyName, msgBuf,
        static_cast<int>(strlen(msgBuf)));
}

// ---------------------------------------------------------------------------
// _processCreatureGift —— ORIG T 0x083265e8 (0x151)
// 宠物赠送：ITEM_TYPE_CREATURE_EQUIP(5) 走 ReqDBSendNewCreatureMail，
// 其余走 ReqDBSendNewMailCashShop。
// ---------------------------------------------------------------------------
#include "W6_CeraShopDeps/_processCreatureGift.inc"
                                     int a4, int a5, int a6)
{
    (void)a5;   // ORIG 栈参存在但本函数未消费（对照报告完整指令清单）
    (void)a6;

    // [b28d-L14 实证] ORIG 0x083265e8 同 rep stos 零初始化 + 内联实参形（P52）。
    char msgBuf[256] = {0};
    sprintf(msgBuf,
            g_scriptStringManager_.findString(4, "game_server_msg_02", (bool*)0),
            user->getCurCharacName());

    user->m_historyLog.SendMailForCeraShopGift(
        static_cast<unsigned int>(a4), '\x03',
        static_cast<unsigned int>(item.m_addInfo),
        static_cast<unsigned int>(item.m_addInfo2));
    user->SendCeraShopGiftStatisticLog(buyIdx, static_cast<unsigned int>(a4),
                                       static_cast<long>(item.m_addInfo),
                                       static_cast<unsigned int>(item.m_addInfo2));

    if (static_cast<unsigned int>(item.m_field1) == 5u)
    {
        CMailBoxHelper::ReqDBSendNewCreatureMail(user, a4, item, msgBuf,
                                                 static_cast<int>(strlen(msgBuf)));
    }
    else
    {
        CMailBoxHelper::ReqDBSendNewMailCashShop(user, item, 0u,
                                                 static_cast<unsigned int>(a4), true,
                                                 msgBuf, static_cast<int>(strlen(msgBuf)));
    }
}

// ---------------------------------------------------------------------------
// ProcessBuyRequest —— ORIG T 0x0832047a (0x18b3)
// MSG 条目逐个 FindGoods → 非 GM 走 sticker/gift 禁售检查 → GM/常规路径查
// GetRelatedItem/PVP 禁购清单/_CheckBuyingCondition(0x8323f86) →
// _CheckValidCount 通过后重置 UsedCera 并逐条折扣计价、64 位余额扣减
// （std::max<long long>(0LL,·) 夹紧）→ 组装 stCeraShopIPGRequest_t 经
// CStreamGuard 发往 s_pIPGHelper 的 IPGOutputSimulation(+0x18)/IPGOutput
// (+0x14)（ceraPrice==0 分支）。控制流、错误码、cMyTrace 行号与字符串均照
// 报告 /tmp/pbr_report.md（ORIG 汇编 + Ghidra C）逐块转写；MSG 访问一律
// 经 SigTypes_A.h 具名字段视图（条目步长 0x1cf，P161 组件视图）。
// 说明【忠实度局限】：0x8323f86 六参校验在 ORIG 以 6 数据槽装载
// （goodsNo / itemIdx:stackCount 组合 long / expiry / byte37 / byte38sext），
// 对齐现存 CCeraShop_Special.cpp 实现体的头声明，末参 e 以 0 补位。
// 【§6.2 验收标注】orig=1551 our=1562；P167 调用点差集归零（仅余 N78 三处
// rep stos→memset call 差：deadPad/priceStr/serialBuf 清零区）；其余 |gap|
// 为 N82 帧/槽号与 4.1/4.4 条件物化（sete/setne、块排布）残余 ⇒ 语义完成、
// byte 层 N 封顶，勿再耗轮次。
// ---------------------------------------------------------------------------
// [b28c-L7][SIGFIX:WongWork::CCeraShop::ProcessBuyRequest] int→bool（判词见
// CCeraShop.h:153）；体内两处 return 值实测仅 1（无错误码语义）。
// [b28d-solo-W6_CeraShopDeps] verdict=DIFF-N82 封顶（orig 1551 / our 1588，
// gap=37=2.4%；p167 CLEAR 127/127）。本轮修法：P0/P227 全表达式临时链
// （NumberToString 结果无槽、itemCount 在 cMyTrace ctor 前求值）、P214③
// goodsList/relatedItems 数组零初始化复现 ORIG 两段 rep stos、P16 早返形
// （firstGoods==0 错误块直落 + jmp 尾）。残余：帧 0x43c vs ORIG 0x40c 及全帧
// 槽号平移（N82：ORIG 给 12 个 cMyTrace 各占独立 0x10 槽，4.4 复用同槽）
// + N7 间接调用目标寄存器 + 跨版本寄存器/寻址形态 ⇒ §6.2 三件套判语义完成、
// byte 层 N 封顶。
#include "W6_CeraShopDeps/ProcessBuyRequest.inc"
{
    // [b28d-S278 P0/P227] ORIG 0x0832047a：NumberToString 结果落 %ebx 无栈槽、
    // itemCount 在 cMyTrace ctor **之前**求值入 %esi ⇒ 原码是全表达式临时链
    // （具名 idStr/trace 两语句各多 store/load 并让实参求值后移到 ctor 之后）。
    cMyTrace(__PRETTY_FUNCTION__, 0x92, 0)(
        "NEXON BILLING : ProcessBuyRequest(m_id(%s),count(%d))",
        NumberToString(user->get_acc_id(), 0),
        static_cast<int>(msg.m_scalars.m_itemCount));

    // ORIG 0x77：36-dword 清零区（-0x2a8..-0x218，后续无任何读点，-O0 死
    // 清零照抄，P171）；随后 -0x84/-0x80 两槽写 0（-0x80 即 avatarField
    // 预置，-0x84 死写）。
    // [b28d-L14 实证] ORIG 0x0832047a 该区为 rep stos（36 dwords）⇒ 显式零初始化
    // 数组（P214③）；旧 memset 生成 call 形（P167 EXTRA memset ×3 之一）。
    unsigned int deadPad[36] = {0};
    (void)deadPad;
    int dead88 = 0;
    (void)dead88;
    unsigned int avatarSelField = 0;

    CCeraShopGoods* firstGoods =
        G_CDataManager()->FindGoods(msg.m_entrySection.m_arr[0].m_productNo);
    if (firstGoods == 0)
    {
        // [b28d-S278 P16] ORIG 0x08320538 `jne <主体>`：错误块直落 + `mov $1; jmp 尾`
        // ⇒ 原码为早返形（if(!=0)/else 会让 4.4 把错误块排到函数末尾，整段窗口错位）。
        ProcessError(user, 0x15, 0, msg.m_entrySection.m_arr[0].m_productNo, 1, 1,
                     static_cast<char>(msg.m_scalars.m_buyFlag));
        return true;
    }

    {
        if (WongWork::CEventMgr::IsChildrensDayEventing())
        {
            user->ClearChildrensDayEvent();
        }

        // [b28d-S278 P214③] ORIG 0x083205a9/0x083205bf 两段 `rep stos`（各 0x24
        // dwords）⇒ 原码是数组零初始化；逐元素 for 循环 4.4 恒生成循环体。
        CCeraShopGoods* goodsList[36] = {0};
        int relatedItems[36] = {0};

        // [b28d-S278 负例] ORIG -0x78 双循环复用，但函数级共用变量实测不可复现
        // 槽位（我方 -0x78 被 checkIdx 决策树占用、i 落他槽，净 +1 条）⇒ 回退。
        for (int i = 0; i < static_cast<int>(msg.m_scalars.m_itemCount); ++i)
        {
            goodsList[i] = G_CDataManager()->FindGoods(
                msg.m_entrySection.m_arr[i].m_productNo);
            if (goodsList[i] == 0)
            {
                int negIdx = -1;
                ProcessError(user, 0x15, negIdx,
                             msg.m_entrySection.m_arr[i].m_productNo,
                             i + 1, static_cast<int>(msg.m_scalars.m_itemCount),
                             static_cast<char>(msg.m_scalars.m_buyFlag));
                goodsList[i] = 0;

                cMyTrace t(__PRETTY_FUNCTION__, 0xc3, 0);
                t("[%s][%d]", __PRETTY_FUNCTION__, 0xc3);
                continue;
            }

            int groupIdx = goodsList[i]->GetGroupIdx();
            if (user->IsGameMasterMode() != 1)
            {
                // 非 GM：sticker/gift 禁售检查（ORIG 0x29c..0x46a）。
                if (goodsList[i]->GetSticker() == 99)
                {
                    ProcessError(user, 8, groupIdx,
                                 msg.m_entrySection.m_arr[i].m_productNo,
                                 i + 1,
                                 static_cast<int>(msg.m_scalars.m_itemCount),
                                 static_cast<char>(msg.m_scalars.m_buyFlag));
                    goodsList[i] = 0;
                    cMyTrace t(__PRETTY_FUNCTION__, 0xde, 0);
                    t("[%s][%d]", __PRETTY_FUNCTION__, 0xde);
                    continue;
                }
                if (static_cast<char>(msg.m_scalars.m_buyFlag) == 1 &&
                    goodsList[i]->isGiftDisallowance_() != 0)
                {
                    ProcessError(user, 8, groupIdx,
                                 msg.m_entrySection.m_arr[i].m_productNo,
                                 i + 1,
                                 static_cast<int>(msg.m_scalars.m_itemCount),
                                 static_cast<char>(msg.m_scalars.m_buyFlag));
                    goodsList[i] = 0;
                    cMyTrace t(__PRETTY_FUNCTION__, 0xe8, 0);
                    t("[%s][%d]", __PRETTY_FUNCTION__, 0xe8);
                    continue;
                }
            }
            // LAB_083208e9：GM 直落 / 非 GM 检查通过落入。
            if (goodsList[i]->GetGroupIdx() != 1)
            {
                relatedItems[i] =
                    reinterpret_cast<int>(goodsList[i]->GetRelatedItem());
                if (relatedItems[i] == 0)
                {
                    ProcessError(user, 0x11, groupIdx,
                                 msg.m_entrySection.m_arr[i].m_productNo,
                                 i + 1,
                                 static_cast<int>(msg.m_scalars.m_itemCount),
                                 static_cast<char>(msg.m_scalars.m_buyFlag));
                    goodsList[i] = 0;
                    cMyTrace t(__PRETTY_FUNCTION__, 0x116, 0);
                    t("[%s][%d]", __PRETTY_FUNCTION__, 0x116);
                    continue;
                }
            }

            // PVP 渠道禁购清单（ORIG 0x57e..0x68e 决策树，条目 itemIdx）。
            bool pvpRestricted = false;
            int checkIdx = goodsList[i]->GetItemIdx();
            if (checkIdx == 0x334)
            {
                pvpRestricted = true;
            }
            else if (checkIdx < 0x335)
            {
                if (checkIdx < 0xc6)
                {
                    if (checkIdx >= 0xc4 || checkIdx == 0xc2)
                    {
                        pvpRestricted = true;
                    }
                    else if (checkIdx == 0xc3)
                    {
                        user->isAffectedPremium(
                            static_cast<ENUM_PREMIUM_TYPE>(0x21));   // 副作用查询
                    }
                }
                else if (checkIdx == 0x2d0 ||
                         (checkIdx > 0x2cf &&
                          (static_cast<unsigned int>(checkIdx - 0x2e7) < 2)))
                {
                    pvpRestricted = true;
                }
            }
            else if (checkIdx <= 0x289832)
            {
                if (checkIdx >= 0x289831 || checkIdx == 0x2897c2 ||
                    checkIdx == 0x289806 || checkIdx == 0x398)
                {
                    pvpRestricted = true;
                }
            }
            else if (checkIdx == 0x28beba || checkIdx == 0x28d154)
            {
                pvpRestricted = true;
            }

            if (pvpRestricted)
            {
                char blockPvp = '\0';
                if (G_GameWorld()->IsPVPChannel())
                {
                    blockPvp = '\x01';
                    if (G_GameWorld()->IsPvPSkilTreeChannel() &&
                        user->get_pvp_grade() >= 10)
                    {
                        blockPvp = '\0';
                    }
                }
                if (blockPvp != '\0')
                {
                    ProcessError(user, 7, groupIdx,
                                 msg.m_entrySection.m_arr[i].m_productNo,
                                 i + 1,
                                 static_cast<int>(msg.m_scalars.m_itemCount),
                                 static_cast<char>(msg.m_scalars.m_buyFlag));
                    goodsList[i] = 0;
                    cMyTrace t(__PRETTY_FUNCTION__, 0x16c, 0);
                    t("[%s][%d]", __PRETTY_FUNCTION__, 0x16c);
                    continue;
                }
            }

            // _CheckBuyingCondition（ORIG 0x757：字段逐次从 msg 重读，P183）。
            unsigned short expiry =
                static_cast<unsigned short>(goodsList[i]->GetExpiryDate());
            if (!this->_CheckBuyingCondition(
                    user, msg.m_entrySection.m_arr[i].m_productNo,
                    static_cast<unsigned long>(goodsList[i]->GetItemIdx()),
                    static_cast<unsigned int>(goodsList[i]->GetStackCount()),
                    expiry,
                    msg.m_entrySection.m_arr[i].m_avatarType0,
                    static_cast<unsigned short>(static_cast<signed char>(
                        msg.m_entrySection.m_arr[i].m_avatarType1))))
            {
                ProcessError(user, 0x4f, groupIdx,
                             msg.m_entrySection.m_arr[i].m_productNo,
                             i + 1,
                             static_cast<int>(msg.m_scalars.m_itemCount),
                             static_cast<char>(msg.m_scalars.m_buyFlag));
                goodsList[i] = 0;
                cMyTrace t(__PRETTY_FUNCTION__, 0x185, 0);
                t("[%s][%d]", __PRETTY_FUNCTION__, 0x185);
            }
        }   // 条目循环（校验段）

        if (!_CheckValidCount(const_cast<const CCeraShopGoods**>(goodsList)))
        {
            int negOne = -1;
            ProcessError(user, 1, negOne, msg.m_entrySection.m_arr[0].m_productNo,
                         1, 1, static_cast<char>(msg.m_scalars.m_buyFlag));
            return true;
        }

        user->resetUsedCera();

        // ORIG 清零顺序：priceStr(0x24) rep stos + serialBuf(0x15) 内联慢路；
        // countStr/itemName 不清零（N78：4.4 生成 memset call）。
        // [b28d-L14 实证] 两 memset 均改显式零初始化数组（P214③，消 EXTRA memset）。
        char priceStr[0x24] = {0};
        char serialBuf[0x15] = {0};
        char countStr[8];
        char itemName[0x64];

        unsigned int nowSec =
            static_cast<unsigned int>(GlobalData::s_systemTime_.getCurSec());
        long long remMoney =
            static_cast<long long>(static_cast<int>(user->getCurCharacMoney()));
        long long remWP = static_cast<long long>(user->GetWinPoint());
        unsigned int ceraPrice = 0;
        int goldPrice = 0;
        unsigned int wpPrice = 0;

        for (int i = 0; i < static_cast<int>(msg.m_scalars.m_itemCount); ++i)
        {
            if (goodsList[i] == 0)
            {
                continue;
            }
            if (goodsList[i]->GetGroupIdx() != 0)
            {
                ceraPrice = static_cast<unsigned int>(goodsList[i]->GetCeraPrice());
                goldPrice = goodsList[i]->GetGoldPrice();
                wpPrice = static_cast<unsigned int>(goodsList[i]->GetWinPoint());
            }
            else
            {
                int groupSub = 0;
                CItem* related =
                    static_cast<CItem*>(goodsList[i]->GetRelatedItem());
                if (related->IsAvatarItem())
                {
                    related = static_cast<CItem*>(goodsList[i]->GetRelatedItem());
                    const std::vector<stAvatarTypeSelect_t>* selects =
                        static_cast<const std::vector<stAvatarTypeSelect_t>*>(
                            static_cast<CEquipItem*>(related)
                                ->getAvatarTypeSelect());
                    if (selects->empty() ||
                        static_cast<signed char>(
                            msg.m_entrySection.m_arr[i].m_avatarType1) >=
                            static_cast<int>(selects->size()))
                    {
                        ProcessError(user, 0x17, groupSub,
                                     msg.m_entrySection.m_arr[i].m_productNo,
                                     i + 1,
                                     static_cast<int>(msg.m_scalars.m_itemCount),
                                     static_cast<char>(msg.m_scalars.m_buyFlag));
                        goodsList[i] = 0;
                        cMyTrace t(__PRETTY_FUNCTION__, 0x253, 0);
                        t("[%s][%d]", __PRETTY_FUNCTION__, 0x253);
                        continue;
                    }
                    if (msg.m_entrySection.m_arr[i].m_avatarType0 >= 100)
                    {
                        ProcessError(user, 0x17, groupSub,
                                     msg.m_entrySection.m_arr[i].m_productNo,
                                     i + 1,
                                     static_cast<int>(msg.m_scalars.m_itemCount),
                                     static_cast<char>(msg.m_scalars.m_buyFlag));
                        goodsList[i] = 0;
                        cMyTrace t(__PRETTY_FUNCTION__, 0x25e, 0);
                        t("[%s][%d]", __PRETTY_FUNCTION__, 0x25e);
                        continue;
                    }
                    // ORIG 四字段各发一次 const operator[]（P157/P183）。
                    ceraPrice = static_cast<unsigned int>(
                        (*selects)[static_cast<signed char>(
                            msg.m_entrySection.m_arr[i].m_avatarType1)].m_fieldc);
                    goldPrice =
                        (*selects)[static_cast<signed char>(
                            msg.m_entrySection.m_arr[i].m_avatarType1)].m_field4;
                    wpPrice = static_cast<unsigned int>(
                        (*selects)[static_cast<signed char>(
                            msg.m_entrySection.m_arr[i].m_avatarType1)].m_field8);
                    avatarSelField = static_cast<unsigned int>(
                        (*selects)[static_cast<signed char>(
                            msg.m_entrySection.m_arr[i].m_avatarType1)].m_field0);
                }
                else
                {
                    ProcessError(user, 0x17, groupSub,
                                 msg.m_entrySection.m_arr[i].m_productNo,
                                 i + 1,
                                 static_cast<int>(msg.m_scalars.m_itemCount),
                                 static_cast<char>(msg.m_scalars.m_buyFlag));
                    goodsList[i] = 0;
                    cMyTrace t(__PRETTY_FUNCTION__, 0x247, 0);
                    t("[%s][%d]", __PRETTY_FUNCTION__, 0x247);
                    continue;
                }
            }

            // LAB_083212dc：会员折扣（cera/wp 无符号乘除，gold 有符号）。
            unsigned short discountRate =
                user->GetPremiumInfo()->getCeraShopDiscountRate();
            if (discountRate != 0)
            {
                ceraPrice -=
                    static_cast<unsigned int>(discountRate) * ceraPrice / 100;
                goldPrice -= static_cast<int>(discountRate) * goldPrice / 100;
                wpPrice -=
                    static_cast<unsigned int>(discountRate) * wpPrice / 100;
            }

            // 额度校验（11=cera 不足，10=金币不足，12=WP 不足）。
            int errorReason = 0;
            if (user->GetCera() < ceraPrice)
            {
                errorReason = 0xb;
            }
            else
            {
                if (user->getCurCharacInvenR()->get_money() < goldPrice)
                {
                    errorReason = 10;
                }
                else if (user->GetWinPoint() < wpPrice)
                {
                    errorReason = 0xc;
                }
            }

            // ORIG 0xf55..0x1037：64 位余额复核 + 扣减，
            // std::max<long long>(0LL, rem-cost) 夹紧（两条）。
            if (remMoney < static_cast<long long>(goldPrice))
            {
                errorReason = 10;
            }
            if (remWP < static_cast<long long>(wpPrice))
            {
                errorReason = 0xc;
            }
            remMoney = std::max(0LL, remMoney - static_cast<long long>(goldPrice));
            remWP = std::max(0LL, remWP - static_cast<long long>(wpPrice));

            if (errorReason != 0)
            {
                unsigned int uWp = user->GetWinPoint();
                unsigned int uGold =
                    static_cast<unsigned int>(user->getCurCharacInvenR()->get_money());
                unsigned int uCera = user->GetCera();
                LogManager::logFormat(
                    1, "CeraShop.cpp", __PRETTY_FUNCTION__, 0x2d6,
                    "IPG ERROR [Dispatcher_BuyCeraShopItem::dispatch_sig] "
                    "UserID[%s] ACera[%d] AGold[%d] AWP[%d]",
                    user->getCurCharacName(), static_cast<int>(ceraPrice),
                    goldPrice, static_cast<int>(wpPrice));
                LogManager::logFormat(
                    1, "CeraShop.cpp", __PRETTY_FUNCTION__, 0x2d7,
                    " / UCera[%d] UGold[%d] UWP[%d]\n",
                    uCera, uGold, uWp);
                LogManager::logFormat(
                    1, "CeraShop.cpp", __PRETTY_FUNCTION__, 0x2d9,
                    "Error Goods No : %d",
                    msg.m_entrySection.m_arr[i].m_productNo);

                int errGroupIdx = goodsList[i]->GetGroupIdx();
                ProcessError(user, static_cast<unsigned short>(errorReason),
                             errGroupIdx,
                             msg.m_entrySection.m_arr[i].m_productNo,
                             i + 1,
                             static_cast<int>(msg.m_scalars.m_itemCount),
                             static_cast<char>(msg.m_scalars.m_buyFlag));
                goodsList[i] = 0;
                cMyTrace t(__PRETTY_FUNCTION__, 0x2e1, 0);
                t("[%s][%d]", __PRETTY_FUNCTION__, 0x2e1);
                continue;
            }

            if (user->IsRestrictedGoods(goodsList[i]->GetFeaturedIdx()))
            {
                int errGroupIdx = goodsList[i]->GetGroupIdx();
                ProcessError(user, 0x14, errGroupIdx,
                             msg.m_entrySection.m_arr[i].m_productNo,
                             i + 1,
                             static_cast<int>(msg.m_scalars.m_itemCount),
                             static_cast<char>(msg.m_scalars.m_buyFlag));
                goodsList[i] = 0;
                cMyTrace t(__PRETTY_FUNCTION__, 0x2ed, 0);
                t("[%s][%d]", __PRETTY_FUNCTION__, 0x2ed);
                continue;
            }

            // stream 组装（P103：operator-> 链保留；写入序照 ORIG 0x1359..）。
            Stream* reqStream =
                GlobalData::s_stream_pool->Acquire("CeraShop.cpp", 0x37b);
            CStreamGuard guard(reqStream, false);
            stCeraShopIPGRequest_t* req =
                guard.operator->()->GetInBuffer<stCeraShopIPGRequest_t>();
            req->m_productNo =
                static_cast<unsigned int>(msg.m_entrySection.m_arr[i].m_productNo);
            req->m_groupIdx = goodsList[i]->GetGroupIdx();
            req->m_stackCount =
                static_cast<unsigned short>(goodsList[i]->GetStackCount());
            req->m_ceraPrice = ceraPrice;
            req->m_goldPrice = static_cast<unsigned int>(goldPrice);
            req->m_winPoint = wpPrice;
            req->m_itemIdx = goodsList[i]->GetItemIdx();
            req->m_avatarType0 = msg.m_entrySection.m_arr[i].m_avatarType0;
            req->m_avatarType1 = msg.m_entrySection.m_arr[i].m_avatarType1;
            req->m_avatarField = avatarSelField;
            req->m_featuredIdx = goodsList[i]->GetFeaturedIdx();
            req->m_itemCount = msg.m_scalars.m_itemCount;
            req->m_seq = i + 1;
            req->m_curSec = nowSec;
            req->m_buyFlag = msg.m_scalars.m_buyFlag;
            req->m_accId = msg.m_scalars.m_accId;
            req->m_characNo = msg.m_scalars.m_characNo;
            req->m_subCount = msg.m_entrySection.m_arr[i].m_subCount;
            for (int s = 0;
                 s < static_cast<int>(msg.m_entrySection.m_arr[i].m_subCount);
                 ++s)
            {
                *reinterpret_cast<unsigned int*>(&req->m_subEntries[s][0]) =
                    static_cast<unsigned int>(
                        msg.m_entrySection.m_arr[i].m_subEntries[s].m_itemIdx);
                req->m_subEntries[s][4] =
                    msg.m_entrySection.m_arr[i].m_subEntries[s].m_value;
            }
            req->m_optCount = msg.m_entrySection.m_arr[i].m_optCount;
            for (int o = 0;
                 o < static_cast<int>(msg.m_entrySection.m_arr[i].m_optCount);
                 ++o)
            {
                *reinterpret_cast<unsigned int*>(&req->m_optEntries[o][0]) =
                    static_cast<unsigned int>(
                        msg.m_entrySection.m_arr[i]
                            .m_optEntries[o + 0x16].m_itemUid);
                *reinterpret_cast<unsigned short*>(&req->m_optEntries[o][4]) =
                    msg.m_entrySection.m_arr[i].m_optEntries[o + 0x16].m_a;
                *reinterpret_cast<unsigned short*>(&req->m_optEntries[o][6]) =
                    msg.m_entrySection.m_arr[i].m_optEntries[o + 0x16].m_b;
            }

            OS_API::snprintf(priceStr, 0x24, "%d", static_cast<int>(ceraPrice));
            OS_API::snprintf(countStr, 8, "%d",
                             msg.m_entrySection.m_arr[i].m_productNo);

            if (goodsList[i]->GetItemIdx() != 1)
            {
                CItem* priced =
                    reinterpret_cast<CItem*>(relatedItems[i]);
                if (priced->is_stackable() &&
                    goodsList[i]->GetStackCount() > 1)
                {
                    OS_API::snprintf(itemName, 0x62,
                                     "\xbd\xc3\xb4\xdc\xc7\xd2\xc0\xaf %s %d\xb0\xb3",
                                     priced->GetItemName(),
                                     goodsList[i]->GetStackCount());
                }
                else
                {
                    strcpy(itemName, priced->GetItemName());
                }
            }
            else
            {
                OS_API::snprintf(itemName, 0x62,
                                 "\xbd\xc3\xb4\xdc\xc7\xd2\xc0\xaf %d\xb0\xb3",
                                 goodsList[i]->GetStackCount());
            }

            if (ceraPrice == 0)
            {
                const char* charName = user->getCurCharacName();
                char* uniqueNo = CGenUniqueNo::GenUniqueNo(serialBuf);
                GlobalData::s_pIPGHelper->IPGOutputSimulation(
                    user, countStr, priceStr, uniqueNo, itemName, charName,
                    static_cast<void*>(&guard));
            }
            else
            {
                const char* charName = user->getCurCharacName();
                char* uniqueNo = CGenUniqueNo::GenUniqueNo(serialBuf);
                GlobalData::s_pIPGHelper->IPGOutput(
                    user, countStr, priceStr, uniqueNo, itemName, charName,
                    static_cast<void*>(&guard));
            }
            // guard RAII 析构释放流（ORIG 1876 D1）。
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// ProcessIPGUserData —— ORIG T 0x8321d2e (0x1ac7)
// 【§6.2 验收标注】orig=1564 our≈1634；P167 调用点差集归零（含本批补全：
// _ProcessIPGError 双调用点、creature 双 expiration/usable、get_acc_id 现场、
// ChildrensDay period 双调用、GetRepeatEvent 双调用、make_pair<ulong&,uchar&>
// 转换形、const_iterator 显式转换对、m_itemList 成员直读、opt push_back 直引）。
// we-bigger 残余为 4.4 EH/cleanup 物化与 N82 槽号形态。
// IPG 结果回包处理：resData->status(+0x1c) 4=回填 cera、0=状态包、2=业务结算
// （avatar/coin/item/item-special/creature/additional 六路分派）→ 成功路径
// 扣款与响应包 0x43/0x35/0xe 广播 + ChildrensDay 补发 + Bonus 概率奖励。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_CeraShopDeps] verdict=DIFF-N82 封顶（orig 1564 / our 1627，
// gap=63=4.0%；p167 CLEAR 192/192）。本轮修法：删 bSend 标志局部（ORIG 无该槽：
// type==4/0 处理完直落尾部 send 块 08321ea3/08321f0d jmp 08323791，type==2 由
// success 的 bool 归一化决定 08323755，user==0 早返 08321d5b je 083237dc）、
// P235 具名 m_type 消 add+mov。残余：帧 0x31c vs ORIG 0x2cc + 全帧槽号平移（N82，
// difflib 大面积窗口错位）+ 4.4 EH cleanup 物化（N91/N104 族）⇒ §6.2 三件套判
// 语义完成、byte 层 N 封顶。
#include "W6_CeraShopDeps/ProcessIPGUserData.inc"
                                   IPG::SIPGData* resData, Stream* stream)
{
    PacketGuard outPkt;                                   // local_d8
    // [b28d-S278] ORIG 无 bSend 标志局部：type==4/0 路径处理完直接落尾部 send 块
    // （08321ea3/08321f0d jmp 08323791），type==2 由 success 的 bool 归一化决定
    // （08323755 test/normalize），user==0 早返（08321d5b je 083237dc）⇒ 删标志槽。

    if (resData->m_type == 4)
    {
        if (user == 0)
        {
            return;
        }
        const int total =
            AtoiAt(resData, 0x99) + AtoiAt(resData, 0xa5) + AtoiAt(resData, 0x81) +
            AtoiAt(resData, 0x75) + AtoiAt(resData, 0x8d) + AtoiAt(resData, 0x69);
        user->SetCera(static_cast<unsigned int>(total));

        outPkt.put_header( 0, 0x35);
        outPkt.put_byte( 1);
        outPkt.put_int( user->GetCera());
    }

    if (resData->m_type == 0)
    {
        if (atoi(BufAt(resData, 0x26e)) == 1)
        {
            int a = 0;
            int b = 0;
            *stream >> a;
            *stream >> b;
            if (user != 0)
            {
                GlobalData::s_pIPGHelper->IPGQuery(user);   // ORIG vtable slot11
            }
            return;
        }
        outPkt.put_header( 0, 0x35);
        outPkt.put_byte( 0);
        outPkt.put_int( 0);
    }
    else if (resData->m_type == 2)
    {
        if (user->getCurCharacR() == 0)
        {
            return;
        }

        stCeraShopIPGRequest_t* q = stream->GetOutBuffer<stCeraShopIPGRequest_t>();
        const int groupIdx = q->m_groupIdx;              // +0x04
        const int seq = q->m_seq;                        // +0x24
        const int itemCountField = q->m_itemCount;       // +0x20
        const int featuredIdxReq = q->m_featuredIdx;     // +0x28
        const unsigned int productNo = q->m_productNo;   // +0x00
        // +0x08 起 8 字节镜像装载：低 32 位 m_itemIdx，bit32..47 为 m_stackCount
        // （ORIG C 整读 ulong；其后消费仅用低 32 位物品号）。
        const unsigned long long itemIdx64 =
            static_cast<unsigned long long>(
                static_cast<unsigned int>(q->m_itemIdx)) |
            (static_cast<unsigned long long>(q->m_stackCount) << 32);
        const unsigned short stackCount = q->m_stackCount;   // +0x0c
        const unsigned char avatarSelB = q->m_avatarType0;   // +0x2c
        const unsigned char avatarSelA = q->m_avatarType1;   // +0x2d
        const unsigned int ceraPriceReq = q->m_ceraPrice;    // +0x10
        const unsigned int winPointReq = q->m_winPoint;      // +0x14
        const int goldSpend = static_cast<int>(q->m_goldPrice);  // +0x18
        const long curSec = static_cast<long>(q->m_curSec);  // +0x30
        std::vector<Inven_Item> resultItems;              // local_ec（RAII）
        const unsigned char buyFlag = q->m_buyFlag;       // +0x34
        const int characNoReq = q->m_characNo;            // +0x38
        const unsigned int accIdReq = q->m_accId;         // +0x40

        stCeraShopItemParam_t itemParam;                  // local_104（RAII）
        for (int s = 0; s < static_cast<int>(q->m_subCount); ++s)
        {
            // ORIG 0x8322086：make_pair(ulong&, uchar&) 左值绑定 → pair<int,int>
            // 转换临时（P61/P135/P178 形）→ push_back。
            unsigned long& subUid = *reinterpret_cast<unsigned long*>(
                &q->m_subEntries[s][0]);
            unsigned char& subVal = q->m_subEntries[s][4];
            itemParam.m_pairs.push_back(
                std::pair<int, int>(std::make_pair(subUid, subVal)));
        }
        for (int o = 0; o < static_cast<int>(q->m_optCount); ++o)
        {
            // ORIG 0x83220xx：push_back 直接引用 req 内元素（无本地拷贝/memcpy）。
            itemParam.m_sel.push_back(*reinterpret_cast<tagPackageItemSelection*>(
                &q->m_optEntries[o][0]));
        }

        int restrictCode = atoi(BufAt(resData, 0x173));   // local_108
        bool success = false;
        const int errArgB = groupIdx;
        if (!(restrictCode == 1 && user != 0 && user->getCurCharacR() != 0))
        {
            // ORIG 0x83221d3/0x832222d：两独立调用点（-0xc → 0x1a，否则 0x19）。
            if (restrictCode == -0xc)
            {
                _ProcessIPGError(user, reqData, resData, '\x1a',
                                 errArgB, productNo, seq, itemCountField,
                                 static_cast<char>(buyFlag));
            }
            else
            {
                _ProcessIPGError(user, reqData, resData, '\x19',
                                 errArgB, productNo, seq, itemCountField,
                                 static_cast<char>(buyFlag));
            }
        }
        else
        {
            int resultState = -1;                         // local_10c → 0
            int slotKind = 2;                             // local_5c
            int insertSlot = -1;                          // local_60
            restrictCode = 0;
            if (groupIdx == 0)
            {
                if (buyFlag == 1)
                {
                    _processAvatarGift(user, static_cast<int>(itemIdx64), featuredIdxReq,
                                       static_cast<char>(avatarSelB),
                                       static_cast<char>(avatarSelA),
                                       BufAt(reqData, 0x7d), accIdReq, characNoReq);
                }
                else
                {
                    CInventory* invW = user->getCurCharacInvenW();
                    insertSlot = invW->AddAvatarItem(
                        static_cast<int>(itemIdx64), featuredIdxReq, false,
                        static_cast<char>(avatarSelB),
                        static_cast<char>(avatarSelA), BufAt(reqData, 0x7d),
                        eAvatarItemAddReason(0), false, 0);
                    if (insertSlot >= 0)
                    {
                        user->SendUpdateItemList(CUser::eSendTarget(1),
                                                 ENUM_ITEMSPACE(1), insertSlot);
                        if (WongWork::CEventMgr::IsChildrensDayEventing())
                        {
                            CCeraShopGoods* goods = reinterpret_cast<CCeraShopGoods*>(
                                G_CDataManager()->FindGoods(groupIdx));
                            CItem* found =
                                G_CDataManager()->find_item(static_cast<int>(itemIdx64));
                            if (goods != 0 && found != 0 &&
                                goods->GetSticker() == 0x16)
                            {
                                const int kindValue = found->GetItemType();
                                user->SetChildrensDayEventValue(kindValue, insertSlot,
                                                                featuredIdxReq);
                            }
                        }
                    }
                    else
                    {
                        const char* errMsg = g_scriptStringManager_.findString(
                            4, "game_server_msg_104", 0);
                        CMailBoxHelper::ReqDBSendNewAvatarMail(
                            user, user->getCurCharacNo(),
                            static_cast<int>(itemIdx64), featuredIdxReq,
                            static_cast<char>(avatarSelB),
                            static_cast<char>(avatarSelA), BufAt(reqData, 0x7d),
                            errMsg, static_cast<int>(strlen(errMsg)));
                    }
                }
            }
            else if (groupIdx == 1)
            {
                if (buyFlag == 1)
                {
                    _processCoinGift(user, stackCount, accIdReq, characNoReq);
                }
                else
                {
                    _processCoin(user, stackCount, true);
                }
            }
            else if (groupIdx == 2 || groupIdx == 7 || groupIdx == 9)
            {
                slotKind = 1;
                CItem* src = G_CDataManager()->find_item(static_cast<int>(itemIdx64));
                Inven_Item item;
                if (src != 0)
                {
                    src->make_item(item);     // ORIG vtable slot2（make_item 填 Inven_Item）
                    item.m_addInfo = static_cast<int>(itemIdx64 & 0xffffffffu);
                    item.m_fieldb = stackCount;
                }
                if (buyFlag == 1)
                {
                    _processItemGift(user, item, accIdReq, characNoReq);
                }
                else
                {
                    _processItem(user, item, item.m_addInfo2, restrictCode, curSec,
                                 false, true, resultItems, itemParam);
                }
            }
            else if (groupIdx == 4)
            {
                slotKind = 1;
                CItem* src = G_CDataManager()->find_item(static_cast<int>(itemIdx64));
                if (src == 0)
                {
                    success = false;
                }
                else
                {
                    Inven_Item item;
                    src->make_item(item);
                    item.m_addInfo = static_cast<int>(itemIdx64 & 0xffffffffu);
                    item.m_fieldb = stackCount;
                    const bool okSpecial = ProcessSpecialItem(
                        user, static_cast<int>(itemIdx64), featuredIdxReq, restrictCode,
                        curSec, false, resultItems, itemParam, '\0', '\0', false, 1);
                    if (!okSpecial)
                    {
                        CInventory* invW = user->getCurCharacInvenW();
                        insertSlot = invW->insertItemIntoInventory(
                            item, eItemAddReason(0x1e), true, false);
                        if (insertSlot == -1)
                        {
                            restrictCode = 4;
                        }
                        else
                        {
                            user->SendUpdateItemList(CUser::eSendTarget(1),
                                                     ENUM_ITEMSPACE(0), insertSlot);
                            user->SaveInventory();
                        }
                    }
                }
            }
            else if (groupIdx == 3)
            {
                CItem* src = G_CDataManager()->find_item(static_cast<int>(itemIdx64));
                Inven_Item item;
                if (src != 0)
                {
                    src->make_item(item);
                    item.m_addInfo = static_cast<int>(itemIdx64 & 0xffffffffu);
                    item.m_fieldb = stackCount;
                    if (buyFlag == 1)
                    {
                        // ORIG 0x83229af/0x83229bc：两分支各自重发调用（P48）。
                        const int expirationDate = src->getExpirationDate();
                        const int usablePeriod = src->getUsablePeriod();
                        _processCreatureGift(user, item, accIdReq, characNoReq,
                                             usablePeriod, expirationDate);
                    }
                    else
                    {
                        const int expirationDate = src->getExpirationDate();
                        const int usablePeriod = src->getUsablePeriod();
                        _processCreature(user, item, restrictCode, resultState,
                                         true, usablePeriod, expirationDate);
                    }
                }
            }
            else if (groupIdx == 10)
            {
                _processAdditionalItem(user, itemIdx64, stackCount, avatarSelB,
                                       avatarSelA, resultItems, itemParam, '\0');
                int followGroup = -1;
                CItem* pkg = G_CDataManager()->find_item(static_cast<int>(itemIdx64));
                if (pkg != 0 && pkg->isCerashopPackageItem())
                {
                    followGroup = 7;
                }
                if (followGroup != 7)
                {
                    resultItems.clear();
                }

                CDataManager* dm = G_CDataManager();
                CItemLimitEditionMgr* limitMgr = dm->getItemLimitEditionMgr();
                CItemLimitEdition* limit = limitMgr->getItemInfo(productNo);
                if (limit == 0)
                {
                    throw ENUM_ERROR(0x11);   // ORIG __cxa_throw(int 0x11)
                }
                // ORIG：restrictLocal 初值 = getFeaturedIdx()；仅当其==0 且
                // IsSpecialItem(relateditem) 命中时才改取 GetRestrictCode。
                int restrictLocal = limit->getFeaturedIdx();
                if (restrictLocal == 0)
                {
                    int hitVal[2] = { 0, 0 };
                    bool isSpecialFlag = false;
                    if (IsSpecialItem(limit->getRelateditem(), hitVal[1],
                                      hitVal[0], isSpecialFlag))
                    {
                        // ORIG 0x8322d5f `mov 0xc(%eax)` 成员直读（4.1 内联 accessor
                        // 展开形，4.4 产 call ⇒ 用成员直读复刻）。
                        restrictLocal = G_CDataManager()->m_itemList->GetRestrictCode(
                            limit->getRelateditem());
                    }
                }
                if (restrictLocal != 0)
                {
                    SaveFeaturedIdx(user, limit->getFeaturedIdx(),
                                    GlobalData::s_systemTime_.getCurSec(), 0);
                }
            }

            if (restrictCode == 0)
            {
                if (ceraPriceReq != 0)
                {
                    // ORIG 0x8322e4a：get_acc_id(user) 现场调用（非 req+0x40 值）。
                    GlobalData::s_BreakAwaySys->updateUsedCera(
                        user->get_acc_id(), ceraPriceReq);
                }
                if (goldSpend != 0)
                {
                    GetInstanceCirculationStatistic()->AddCirculationStatistic(
                        CIRCULATION_STATISTIC_FIELD(0x26), user,
                        static_cast<unsigned int>(goldSpend));
                }
                user->getCurCharacInvenW()->use_money(goldSpend, eMoneySubReason(8), true);
                user->UseWinPoint(winPointReq, eWPSubReason(2));
                user->UseCera(ceraPriceReq);
                user->addUsedCera(ceraPriceReq);
                if (featuredIdxReq != 0 &&
                    !user->IsRestrictedGoods(featuredIdxReq))
                {
                    user->SetRestrictedGoods(featuredIdxReq);
                    SaveFeaturedIdx(user, featuredIdxReq, static_cast<int>(curSec), 0);
                }

                outPkt.put_header( 1, 0x43);
                outPkt.put_byte( 1);
                outPkt.put_byte( static_cast<int>(
                    static_cast<signed char>(buyFlag)));
                outPkt.put_int( groupIdx);
                outPkt.put_int( static_cast<int>(productNo));
                outPkt.put_int( seq);
                outPkt.put_int( itemCountField);
                outPkt.put_int( resultState);
                outPkt.put_short(
                                              static_cast<int>(resultItems.size()));
                {
                    // ORIG：begin/end 各一次 iterator→const_iterator 转换构造，
                    // while 循环 operator!= 逐轮调用（Ghidra local_11c/local_120）。
                    std::vector<Inven_Item>::const_iterator itBeg(resultItems.begin());
                    std::vector<Inven_Item>::const_iterator itEnd(resultItems.end());
                    while (itBeg != itEnd)
                    {
                        outPkt.put_int(itBeg->m_addInfo);
                        outPkt.put_int(itBeg->m_addInfo2);
                        ++itBeg;
                    }
                }

                unsigned long bonusItem = 0xffffffffUL;     // local_124
                unsigned int bonusCount = 0;                // local_128
                if (buyFlag == 1 && accIdReq != 0)
                {
                    DB_UpdateGiftCount::makeRequest(accIdReq, 1);
                }
                if (seq == itemCountField)
                {
                    if (WongWork::CEventMgr::IsChildrensDayEventing() &&
                        user->IsOKChildrensDayEventCondition())
                    {
                        WongWork::CWebEvent::ReqSaveEventItem(
                            user->get_acc_id(),
                            static_cast<unsigned int>(user->getCurCharacNo()),
                            0x65ae, 1u, 0x30d47, 0u, true);
                        PacketGuard notiPkt;                // local_140
                        notiPkt.put_header( 0, 0xe);
                        notiPkt.put_byte( 1);
                        int idxPos = notiPkt.get_index();
                        int filled = 0;
                        notiPkt.put_short( 10);
                        for (int d = 0; d < 10; ++d)
                        {
                            const int targetSlot =
                                user->GetChildrensDayEventTargetItemSlot(d);
                            if (targetSlot < 0 ||
                                user->GetChildrensDayEventTargetItemPeriod(d) == -1)
                            {
                                break;
                            }
                            // ORIG 0x83232bf：条件后再次现场调用（P48 双调用点）。
                            const int targetPeriod =
                                user->GetChildrensDayEventTargetItemPeriod(d);
                            const Inven_Item bufItem =
                                user->getCurCharacInvenR()->GetInvenSlot(
                                    2, targetSlot);
                            CInventory* invW = user->getCurCharacInvenW();
                            const int newSlot = invW->AddAvatarItem(
                                bufItem.m_addInfo, targetPeriod, false,
                                static_cast<char>(bufItem.m_field1), '\xff',
                                "E_CHILDRENS_DAY", eAvatarItemAddReason(2), false, 0);
                            if (newSlot >= 0)
                            {
                                user->getCurCharacInvenR()->MakeItemPacket(
                                    INVEN_TYPE(2), newSlot, notiPkt);
                                ++filled;
                            }
                        }
                        notiPkt.put_short(idxPos, filled);
                        notiPkt.finalize( true);
                        user->Send(notiPkt);
                    }

                    // ORIG 0x8323408/0x8323439：条件与体内各一次现场调用；
                    // vtable+0x34 直调无判空（P171 照抄；bonus 事件该槽空体）。
                    if (GlobalData::s_event_manager->GetRepeatEvent(0x11)
                            ->IsEventing(0))
                    {
                        CCeraShopBonusItemEvent* bonus =
                            static_cast<CCeraShopBonusItemEvent*>(
                                GlobalData::s_event_manager->GetRepeatEvent(0x11));
                        const int rate = bonus->getBonusRate();
                        if (get_rand_int(1000) <= rate)
                        {
                            const int usedCera = static_cast<int>(user->getUsedCera());
                            G_CDataManager()->m_itemList->getCeraShopBonusItem(
                                usedCera, bonusItem, bonusCount);
                            if (bonusItem != 0xffffffffUL)
                            {
                                std::vector<Inven_Item> bonusItems;    // local_150
                                std::vector<std::pair<int,int> > bonusPairs; // local_15c
                                _processAdditionalItem(user, bonusItem,
                                                       static_cast<unsigned short>(bonusCount),
                                                       '\0', '\xff', bonusItems,
                                                       itemParam, '\0');
                            }
                        }
                    }
                    outPkt.put_int( static_cast<int>(bonusItem));
                    outPkt.put_int( static_cast<int>(bonusCount));

                    PacketGuard ceraPkt;                    // local_134
                    ceraPkt.put_header( 0, 0x35);
                    ceraPkt.put_byte( 1);
                    ceraPkt.put_int( user->GetCera());
                    ceraPkt.finalize( true);
                    user->Send(ceraPkt);

                    ceraPkt.clear();
                    ceraPkt.put_header( 0, 0xe);
                    ceraPkt.put_byte( 0);
                    ceraPkt.put_short( 2);
                    user->getCurCharacInvenR()->MakeItemPacket(INVEN_TYPE(1), 0, ceraPkt);
                    user->getCurCharacInvenR()->MakeItemPacket(INVEN_TYPE(1), 2, ceraPkt);
                    ceraPkt.finalize( true);
                    user->Send(ceraPkt);
                }
                success = true;
            }
            else
            {
                _ProcessIPGError(user, reqData, resData,
                                 static_cast<unsigned char>(restrictCode),
                                 groupIdx, productNo, seq, itemCountField,
                                 static_cast<char>(buyFlag));
                cMyTrace t(
                    __PRETTY_FUNCTION__,
                    0x955, 0);
                t("[%s][%d]",
                  "void WongWork::CCeraShop::ProcessIPGUserData(CUser*, WongWork::IPG::SIPGData*, WongWork::IPG::SIPGData*, Stream*)",
                  0x955);
                success = false;
            }
        }

        if (!success)
        {
            return;
        }
    }

    outPkt.finalize( true);
    user->Send(outPkt);
}

}  // namespace WongWork

// ---------------------------------------------------------------------------
// ORIG W 0x08179fb4 _ZNK5CItem21isCerashopPackageItemEv：逐指令转写。
// 【翻案 b25c】旧 N 标注（setcc-fold）过时：单 return 短路链（P45）探针
// /tmp/probe_short.cpp q2 定论 4.4 与 ORIG 逐字同构（cmp;je 共享真岛 +
// cmp;jne + mov$1/mov$0），if-return 链才折 setcc。26/26 全等。
// 【b28c-36 复验】fresh ndiff=16 / NEAR（26=26 指令数）：残差 2 处全为
// **N7 间接 call 目标寄存器**——ORIG `mov (%eax),%edx; mov this,%eax; call *%edx`
// （目标 %edx/this %eax），4.4.7 恒 `mov (%eax),%eax; mov this,%edx; call *%eax`
// （目标 %eax/this %edx）。P230 补：4.4.6 后端间接 call 目标用 %edx（与 ORIG 同形），
// 本波禁自切路由，待 Main 三态后端 sweep 后按 N7 敏感族复测。源码形无解。
// 经 vtable 槽 +0x0c 虚方法（子类型索引，与 CI_SubType 先例同型）连续两次
// 调用，返回值 ==0xe 或 ==0xf 判定礼包类目 → true，否则 false。
// ---------------------------------------------------------------------------
#include "W6_CeraShopDeps/isCerashopPackageItem.inc"
{
    // vtable +0x0c = GetItemType()（_GetItemType 批次实证 make_item@+0x08 后继槽）。
    return GetItemType() == 0x0e || GetItemType() == 0x0f;
}
