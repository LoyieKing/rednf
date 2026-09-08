// ============================================================================
// df_game_r 还原 —— ARAD::Arad_Dispatcher / Arad_Dispatcher_Manager /
// Arad_Dispatcher_BuyItemCheck_Impl + ARAD::DATABASE::ITEM_QUERY 查询。
// 依据 docs/class_func_reports/ARAD__Arad_Dispatcher{,_Manager}/、
// ARAD__Arad_Dispatcher_BuyItemCheck_Impl/、ARAD__DATABASE__ITEM_QUERY/ 报告逐函数转写。
// C++98 / -m32 / O0 基线。
// ============================================================================

#include "Arad_Dispatcher.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

#include "GlobalData.h"        // s_db_mgr / s_stream_pool / s_msgq_mgr / s_pCeraShop / s_pItemVendingMachine
#include "Stream.h"            // Stream operator>> / GetOutBuffer<T>
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "DBMgr.h"             // DBMgr 完整类型（GlobalData::s_db_mgr->GetDBHandle）
#include <stddef.h>
#include "MySQL.h"
#include "LogManager.h"
#include "SigTypes.h"          // MSG_BUY_CERASHOP_ITEM（0x3f84）
#include "MoreTypes.h"         // ARAD::INTERNAL_PACKET::PURCHASE_COUNT_CHECK
#include "CCeraShop.h"         // WongWork::CCeraShop::ProcessError
#include "ItemVendingMachine.h"
#include "CUser.h"

// 布局断言：ORIG Manager ctor 0x0818fdd4 两组各 0x354 指针数组，size = 0x1AA0。
typedef char AradDispatcherManagerSizeAssert[
    sizeof(ARAD::Arad_Dispatcher_Manager) == 0x354 * 4 * 2 ? 1 : -1];

// b28c-L26 [判词 N·跨版本 new-expression landing-pad 抑制，模板实例变体]
// 实测在 Manager ctor 定义**之前**放成员函数显式实例化
// （`template void GlobalInstance<ARAD::Arad_Dispatcher_Manager>::create();`）
// 仍产 24 条形（无 cleanup 岛）——4.4 对模板成员的代码生成延至 TU 末尾，
// 届时 ctor 体恒可见 ⇒ 判 nothrow 抑 landing pad。P210/BM11「定义前置」翻案
// 仅适用**非模板** caller（open()/NexonCash C1）；对 GlobalInstance<T>::create()
// 这类头模板成员无源码解（禁改头/禁删 ctor）。ORIG 37 条 = 双检 + new(0x1aa0)
// + ctor call + `mov %ebx,%eax; mov %eax,m_p` + 正常路径逐条等价，差额 13 条
// 全落在 ctor 抛异常的 delete+_Unwind_Resume 清理岛及其 push %edi/%esi/帧 0x1c。
// 已回退该无效实例化声明，保持纯自然形。
// [b28d-23 N·N100/模板] 复测维持：模板成员 create 的 landing pad 抑制无源码解
// （P210 定义前置仅适用非模板 caller）⇒ N。ORIG 0x0818e956

// ---------------------------------------------------------------------------
// 本 TU 实例化：GlobalInstance<ARAD::Arad_Dispatcher_Manager>::create()  @0x0818e956
// （报告 docs/class_func_reports/StrikerScript/create.md 属撞名——其 `## 反编译 C` 段为
// StrikerScript::create@0x8a9dc70，与本符号无关；verdict 以 nm 精确切片的 ORIG 汇编为权威，
// §6.3）。
// [b23判词·跨版本 N（new-expression landing-pad 抑制）+ AE 假阳性] 双检（两次
// `mov m_p,%eax; test; jne`）、`movl $0x1aa0` → new → Manager ctor → `mov %ebx,%eax;
// mov %eax,m_p` → `jmp` + `nop` 岛与 ORIG 正常路径逐条相等；37−24=13 条差额全部落在
// ctor 抛异常的清理岛（operator delete + _Unwind_Resume）及其连带的 push %edi/%esi 与
// 帧 0x1c/0x14（根因同 open()，见其判词：基类 ctor/dtor 本 TU 可见空定义 ⇒ 4.4 推断
// nothrow 抑制 landing pad，/tmp 探针 A/B/C/D 实证）。另有 3 处 m_p 数据地址与
// 2 处 call 目标显示为 .o 重定位占位（§6.3 假阳性），relink 后转 IDENTICAL_AE。
// ---------------------------------------------------------------------------

namespace ARAD
{

// ============================================================================
// Arad_Dispatcher_Manager::open —— ORIG 0x0818fe38。
// [b28 BM11] P210 定义前置翻案（推翻 b23「landing-pad 抑制不可复现」判词，与
// NexonCash C1 同法）：open 定义前置于 BuyItemCheck_Impl ctor 定义之前 ⇒ Impl
// ctor 本 TU 仅原型可见 ⇒ 4.4 对 new-expression 保守生成 EH cleanup
// （delete+Unwind_Resume 岛 + push %edi/%esi + 帧 0x2c/槽 -0x1c，ORIG 全形）。
// ============================================================================
bool Arad_Dispatcher_Manager::open()
{
    // ORIG 0x0818fe38：operator new(8) → BuyItemCheck_Impl ctor → 注册 → return 0。
    Arad_Dispatcher_BuyItemCheck_Impl* impl =
        new Arad_Dispatcher_BuyItemCheck_Impl();
    GlobalInstance<Arad_Dispatcher_Manager>::inst()->register_dispatcher(impl);
    return false;
}

// ============================================================================
// Arad_Dispatcher_BuyItemCheck_Impl —— ORIG C1 @0x0818e9cc：基类码 0x1fa。
// P210：本 ctor 定义必须先于基类 Arad_Dispatcher(int) ——基类 ctor 本 TU 可见时
// 4.4 按其后继帧精确继承 outgoing（帧 0x18→0x8，与 ORIG 0x18 反）。
// ============================================================================
Arad_Dispatcher_BuyItemCheck_Impl::Arad_Dispatcher_BuyItemCheck_Impl()
    : Arad_Dispatcher(0x1fa)
{
}

// ============================================================================
// Arad_Dispatcher —— ORIG C1 @0x0818edd0：vptr 由编译器写；m_code = code（+0x04）。
// ============================================================================
Arad_Dispatcher::Arad_Dispatcher(int code)
    : m_code(code)
{
}

// ORIG D1 @0x0818e900：仅写基类 vptr；D0 条件 operator delete(this)。
Arad_Dispatcher::~Arad_Dispatcher()
{
}

int Arad_Dispatcher::db_dispatch(int /*code*/, int /*uid*/, Stream* /*stream*/)
{
    // ORIG 0x0818e84c：mov $0x0,%eax; ret
    return 0;
}

int Arad_Dispatcher::inter_dispatch(CUser* /*user*/, char* /*body*/, int /*len*/)
{
    // ORIG 0x0818e856：mov $0x0,%eax; ret
    return 0;
}

void Arad_Dispatcher::register_dispatcher(Arad_Dispatcher* d)
{
    // ORIG 0x0818e860：GlobalInstance<Manager>::inst()->register_dispatcher(d)
    GlobalInstance<Arad_Dispatcher_Manager>::inst()->register_dispatcher(d);
}

void Arad_Dispatcher::unregister_dispatcher(int code)
{
    // ORIG 0x0818e87c：GlobalInstance<Manager>::inst()->unregister_dispatcher(code)
    GlobalInstance<Arad_Dispatcher_Manager>::inst()->unregister_dispatcher(code);
}

// ============================================================================
// Arad_Dispatcher_Manager —— ORIG C1 @0x0818fdd4：两个 0x354 项指针数组清零。
// ============================================================================
Arad_Dispatcher_Manager::Arad_Dispatcher_Manager()
{
    for (int i = 0; i < 0x354; ++i)
        m_interDispatchers[i] = 0;
    for (int i = 0; i < 0x354; ++i)
        m_dbDispatchers[i] = 0;
}

Arad_Dispatcher_Manager::~Arad_Dispatcher_Manager()
{
    // ORIG D1 @0x0818fe32：空。
}

bool Arad_Dispatcher_Manager::register_dispatcher(Arad_Dispatcher* d)
{
    // ORIG 0x0818fe98：dispathc_code() 两次调用（ORIG 无 code 栈局部，两次重取）；
    // inter[code]=d；db[code]=d；返回 0。
    // [b23判词·AE假阳性] 20/20 归一化逐条相等（含 `lea 0x354(%edx),%ecx` 第二组寻址形——
    // 两组指针数组连续声明使 db 基址折为 this+0xD50）；残余 2 处 call 目标显示为
    // .o 重定位占位（§6.3 假阳性），relink 后转 IDENTICAL_AE。
    m_interDispatchers[d->dispathc_code()] = d;
    m_dbDispatchers[d->dispathc_code()] = d;
    return 0;
}

bool Arad_Dispatcher_Manager::unregister_dispatcher(int code)
{
    // ORIG 0x0818fed6：inter[code] 判空→虚析构调用块（Ghidra unreachable）→置空返回 0；
    // 否则 db[code] 同样处理；两处均非空返回 1。按报告反编译 C 转写：
    // [b28d-23 语义补齐/P184 死块照抄] ORIG 0x0818fee8 有具名局部
    // `d = inter[code]`（-0x10 槽往返，P0）且块内为 `delete d` 的 4.1 展开
    // （内建判空死块 0x0818feee + 虚 D0 调用 0x0818fef4..f04，Ghidra 标
    // unreachable）。原实现漏 delete ⇒ 补回；db 组同（-0xc 槽 + 0x354 折叠）。
    Arad_Dispatcher* d = m_interDispatchers[code];
    if (d == 0)
    {
        delete d;
        m_interDispatchers[code] = 0;
        return false;
    }
    Arad_Dispatcher* d2 = m_dbDispatchers[code];
    if (d2 == 0)
    {
        delete d2;
        m_dbDispatchers[code] = 0;
        return false;
    }
    return true;
}

bool Arad_Dispatcher_Manager::inter_dispatch(int code, CUser* user,
                                             char* body, int len)
{
    // ORIG 0x0818ff6c：查 inter 组，虚槽3 调用；err!=0 时记日志并对 user 断连。
    Arad_Dispatcher* d = m_interDispatchers[code];
    if (d == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatch_Manager.cpp",
            "bool ARAD::Arad_Dispatcher_Manager::inter_dispatch("
            "ARAD::Arad_Dispatcher_Manager::INTERNAL_PACKET_CODE, CUser*, char*, int)",
            0x7d, "Not found Inter_Dispatch_Code[%d]", code);
        return false;
    }
    // ORIG 0x0818ffeb：`cmpl $0,-0x10; je end(mov$1)`＝ err!=0 才进错误块
    // （尾 mov$1 落函数尾，反向 choose 岛不可用 P63/P191）。
    const int err = d->inter_dispatch(user, body, len);
    if (err != 0)
    {
        unsigned int accId = 0;
        if (user != 0)
        {
            accId = user->get_acc_id();
            user->DisConnSig((DISCONN_SIG)2, 1, err + 1000000);
        }
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatch_Manager.cpp",
            "bool ARAD::Arad_Dispatcher_Manager::inter_dispatch("
            "ARAD::Arad_Dispatcher_Manager::INTERNAL_PACKET_CODE, CUser*, char*, int)",
            0x8d, "Inter Dispatch Result Error Inter_Dispatch_Code[%d] user_acc_id[%d]",
            code, accId);
        return false;
    }
    return true;
}

bool Arad_Dispatcher_Manager::db_dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x08190080：查 db 组（+0x354），虚槽2 调用；ErrLine!=0 记日志；找到即返回 1。
    Arad_Dispatcher* d = m_dbDispatchers[code];
    if (d == 0)
        return false;
    const int errLine = d->db_dispatch(code, uid, stream);
    if (errLine != 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatch_Manager.cpp",
            __PRETTY_FUNCTION__,
            0xab, "[ERROR] DB Dispatch Result dipCode[%d] uid[%d], ErrLine(%d)",
            code, uid, errLine);
    }
    return true;
}

Arad_Dispatcher_BuyItemCheck_Impl::~Arad_Dispatcher_BuyItemCheck_Impl()
{
}

int Arad_Dispatcher_BuyItemCheck_Impl::inter_dispatch(CUser* user,
                                                      char* body, int /*len*/)
{
    // P82/P99：本地 pack(1) 尾部视图使 4.4 折叠 `mov 0x3a(%eax),%eax`
    // （裸 (b+0x3a) cast 恒 add 分解，探针 /tmp/l18p/p.cpp）。
    #pragma pack(push, 1)
    struct ItemIdxView { char m_pad[0x3a]; unsigned int m_idx; };
    #pragma pack(pop)
    // ORIG 0x0818ea3e：
    //   body[0]==1 → ItemVendingMachine::BuyItem(user, *(MSG*)(body+1))；
    //               成功返回 0，失败 logFormat(…0x40…) 返回 -1。
    //   body[0]!=1 → CCeraShop::ProcessError(s_pCeraShop,user,0x74,0,0,1,1,0)，返回 0。
    // ORIG 0x0818ea44：body 形参 spill 至 -0xc 槽（P0 具名局部）；0x0818ea4a
    // `movzbl (%eax),%eax; xor $0x1,%eax; test; je`＝ `!*(bool*)b` 直判形
    // （xor 精确判 b==1，P171 原码 bool 视角照抄）。
    char* b = body;
    if (!*(bool*)b)
    {
        GlobalData::s_pCeraShop->ProcessError(user, 0x74, 0, 0, 1, 1, '\0');
        return 0;
    }
    if (!GlobalData::s_pItemVendingMachine->BuyItem(
            user, *(const MSG_BUY_CERASHOP_ITEM*)(b + 1)))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatcher_BuyItemCheck_Impl.cpp",
            "virtual int ARAD::Arad_Dispatcher_BuyItemCheck_Impl::inter_dispatch("
            "CUser*, char*, int)", 0x40,
            "[ERROR] BuyItem() Result Error ItemIdx(%d)",
            ((const ItemIdxView*)b)->m_idx);
        return -1;
    }
    return 0;
}

// [b28d-23 NEAR·N82] 残余 N82 寻址/寄存器（158/157）；裸 register ret 保留（P248 三条件：跨 call 保活+4.4 不自升+ORIG 用 %ebx）⇒ N。ORIG 0x0818eb0c
int Arad_Dispatcher_BuyItemCheck_Impl::db_dispatch(int code, int uid,
                                                   Stream* stream)
{
    // [b21] 修正真 bug：MsgQueueMgr::put 旧用 DB_Q(=2)，ORIG 传 $0x1 改 DISPATCH_Q
    //   （同族 DB 派发器归队队列 idx 1，与 DBInsertGiveBoxEventReward dispatch 同案）。
    //   残余 N82：BuyItemInfo 五字段 Stream::operator>> 寻址 4.4 用 add/寄存器形 vs
    //   ORIG lea(+off)；日志/__PRETTY 字符串地址为 .o 重定位产物（链接期解析）。语义
    //   闭合封 N。
    // ORIG 0x0818eb0c：读 BuyItemInfo ← stream；GetOutBuffer<MSG_BUY_CERASHOP_ITEM>；
    // arad_sp_buy_item_check 通过后组装 PURCHASE_COUNT_CHECK 内部包入 DB 队列。
    // b28c-L26 清绑（P0/N96 硬约束）：原 `register int ret asm("ebx")` 的
    // **asm 绑定已删**（P0 全域禁绑）。保留裸 register 关键字的依据＝本件双形
    // fresh 实测（orig_insns 158）：
    //   · `register int ret;`  → our 157，行级 diff 85
    //   · `int ret;`           → our 157，行级 diff 135（反劣 +50 行）
    // 本点 ret 为出口码、跨 logFormat/Acquire/put 多次 call 存活，ORIG 各出口
    // 恒 `mov $imm,%ebx` ⇒ 属「值确需跨 call 保活」类，裸 register 令 4.4 以
    // %ebx 免槽承载、装载序回到 ORIG 形；与 lane20A/lane12 的「纯本地条件判定
    // 点裸 register 反劣（帧破 push/pop）」判据不冲突（那两点无跨 call 寿命）。
    register int ret;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)7,
                                                  (ENUM_SERVER_GROUP)0);
    ARAD_DATAS::BuyItemInfo info;
    // b28：ORIG 0x0818eb31 GetDBHandle 后直接 `lea -0x28+0xc >>`——info 无预清
    // call（P131）；原 memset(0x14) 在 4.4 展开 call memset = +5 条伪差，删。
    *stream >> info.mid;
    *stream >> info.characNo;
    *stream >> info.ipgNo;
    *stream >> info.checkType;
    *stream >> info.limitCount;
    MSG_BUY_CERASHOP_ITEM* msg = stream->GetOutBuffer<MSG_BUY_CERASHOP_ITEM>();
    if (msg == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatcher_BuyItemCheck_Impl.cpp",
            "virtual int ARAD::Arad_Dispatcher_BuyItemCheck_Impl::db_dispatch("
            "int, int, Stream*)", 0x66,
            "[ERROR] MSG_BUY_CERASHOP_ITEM GetOutBuffer => MID[%d], CHARIDX[%d], "
            "ITEMIDX[%d], CHECKTYPE[%d]",
            info.mid, info.characNo, info.ipgNo, (int)info.checkType);
        ret = -1;
        goto out;
    }
    // b28：去块包裹+早 goto——4.4 对「块内对象+goto 提前退」恒每臂展开 guard D1
    // （ORIG 0x0818ed3e 直 jmp 尾共享 D1@0x818ed60）；统一尾使 D1 单点发射。
    // chk 用 char* 视图形：ORIG 判空/写均 `mov -0x1c` 直读槽（P183 反向：`(char*)chk`
    // cast 形 4.4 恒重算 lea+1，+1 条）。
    if (DATABASE::ITEM_QUERY::arad_sp_buy_item_check(info) != 0)
    {
        ret = -1;
    }
    else
    {
        // P134：Acquire 内联进 CStreamGuard ctor 消中间槽（ORIG 直 `mov %eax,0x4`）。
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire(
                "localjapan/Arad_Dispatcher_BuyItemCheck_Impl.cpp", 0x71),
            true);
        guard.operator*()->operator<<(code);
        guard.operator*()->operator<<(uid);
        char* chk = (char*)
            guard.operator->()->GetInBuffer<ARAD::INTERNAL_PACKET::PURCHASE_COUNT_CHECK>();
        if (chk == 0)
        {
            LogManager::logFormat(
                1, "localjapan/Arad_Dispatcher_BuyItemCheck_Impl.cpp",
                "virtual int ARAD::Arad_Dispatcher_BuyItemCheck_Impl::db_dispatch("
                "int, int, Stream*)", 0x7b,
                "ITEM_PURCHASE_COUNT_CHECK MID[%d], CHARIDX[%d], ITEMIDX[%d], "
                "CHECKTYPE[%d]",
                info.mid, info.characNo, info.ipgNo, (int)info.checkType);
        }
        else
        {
            *chk = info.buyable;
            memcpy(chk + 1, msg, 0x3f84);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
        ret = 0;
    }
out:
    return ret;
}

// ============================================================================
// ARAD::GetQuarterOfYear —— ORIG T 0x0819e8d0。
// time/localtime 后 quarter=tm_mon/3+1（0x55555556 除魔数）、year=(tm_year+1900)%2000
// （0x10624dd3/sar7 除魔数 = /2000 + imul 0x7d0 取余），sprintf("%02d%d", year, quarter)。
// ============================================================================
// [b28d-23 N·N82/P75] 实参装载序/帧差（69/73）⇒ N。ORIG 0x0819e8d0
void GetQuarterOfYear(char* buf)
{
    time_t t = time(0);
    struct tm tmp = *localtime(&t);
    const int quarter = tmp.tm_mon / 3 + 1;
    const int yearFull = tmp.tm_year + 1900;
    const int year = yearFull - (yearFull / 2000) * 2000;
    sprintf(buf, "%02d%d", year, quarter);
}

int DATABASE::ITEM_QUERY::arad_sp_buy_item_check(ARAD_DATAS::BuyItemInfo& info)
{
    // ORIG T 0x0818cfab：prod_sale_entry_<quarter> count(*) 与 limitCount 比较。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)7,
                                                  (ENUM_SERVER_GROUP)0);
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            "int ARAD::DATABASE::ITEM_QUERY::arad_sp_buy_item_check("
            "ARAD_DATAS::BuyItemInfo&)", 0x454, "%s", "Database null!!");
        return -1;
    }
    char quarter[5] = {0};  // ORIG `movl $0; movb $0` 两写消 5 字节零
    GetQuarterOfYear(quarter);
    if (info.checkType == 1)
    {
        db->set_query(
            "select count(*) from prod_sale_entry_%s where  ipg_no = %d and "
            " m_id = %d   and  charac_no = %d",
            quarter, info.ipgNo, info.mid, info.characNo);
    }
    else
    {
        db->set_query(
            "select count(*) from prod_sale_entry_%s where  ipg_no = %d and "
            " m_id = %d",
            quarter, info.ipgNo, info.mid);
    }
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            "int ARAD::DATABASE::ITEM_QUERY::arad_sp_buy_item_check("
            "ARAD_DATAS::BuyItemInfo&)", 0x472,
            "[ERROR] DB Exec Error => MID[%d], CHARIDX[%d], ITEMIDX[%d], CHECKTYPE[%d]",
            info.mid, info.characNo, info.ipgNo, (int)info.checkType);
        return -1;
    }
    if (!db->fetch())
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            "int ARAD::DATABASE::ITEM_QUERY::arad_sp_buy_item_check("
            "ARAD_DATAS::BuyItemInfo&)", 0x47b,
            "[ERROR] DB Fetch Error => MID[%d], CHARIDX[%d], ITEMIDX[%d], CHECKTYPE[%d]",
            info.mid, info.characNo, info.ipgNo, (int)info.checkType);
        return -1;
    }
    int count = 0;
    if (!db->get_int(0, count))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            "int ARAD::DATABASE::ITEM_QUERY::arad_sp_buy_item_check("
            "ARAD_DATAS::BuyItemInfo&)", 0x485,
            "RESULT_COUNT MID[%d], CHARIDX[%d], ITEMIDX[%d], CHECKTYPE[%d]",
            info.mid, info.characNo, info.ipgNo, (int)info.checkType);
        return -1;
    }
    info.buyable = 1;
    if (info.limitCount <= count)
        info.buyable = 0;
    return 0;
}


int Arad_Dispatcher::dispathc_code()
{
    // ORIG 0x0818e898 T：return *(int*)(this+4)（非虚）。
    return m_code;
}

}  // namespace ARAD

