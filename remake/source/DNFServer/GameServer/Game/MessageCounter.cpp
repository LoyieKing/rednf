// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 MessageCounter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// MessageCounter.cpp —— 按报告 docs/class_func_reports/message_counter__*/*.md 忠实转写。
#include "MessageCounter.h"

#include <new>
#include <string.h>

// [T2b-GI · P338/P327] 补 <iostream> 引入 file-scope 动态初始化 __ioinit，恢复本 TU
// `_GLOBAL__I_` 发射；键名目标 `_ZN15message_counter13CTimeInterval6CreateEv`
// （ORIG 0x857015f）。零语义改动。
#include <iostream>

#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "SigTypes.h"
#include "CSystemTime.h"

namespace message_counter
{

// ---- ORIG 域常量 _ZN15message_counterL13UPDATE_PERIODE（r 0x08c5a78c = 600）----
// gcc 4.4：namespace 内 static const 未使用也发射（内部链接，名含 L）。
static const int UPDATE_PERIOD = 600;

// SIG_INSERT_RESPONSE_TIME（0x7d8）访问视图：
//   +0x00 int  baseSec；+0x04 int count；
//   entries[100] 每 0x14 @+8 + j*0x14：
//     {+0x00 uint id; +0x04 int count; +0x08 int bytesLow;
//      +0x0c int bytesHigh; +0x10 int avgRounded}

// [b28d-SymMsgCtr·检查点18手法] ORIG 无 _ZN15message_counter12_GLOBAL__N_113sig_*
// 等局部符号（inline 在 -O0 仍发射 ⇒ 唯一可行手法是宏）。逐字保留副作用：每形参
// 在展开体中仅出现一次（无双重求值），写序 = 原函数体语句序，无短路逻辑。
// [R29-4] `+4` 字段改用数组下标形（`((int*)(s))[1]`）：ORIG 全部为单条
// disp 折叠 `movl $0x0,0x4(%eax)` / `mov 0x4(%eax),%eax`（0x0856fc94/0x0856fdb0），
// `(s)+4` 形在 4.4 恒多发 `add $0x4,%eax`（4 处调用 ⇒ +4 条）。
#define sig_init_head(s, base) do { \
    reinterpret_cast<int*>((s))[0] = (base); \
    reinterpret_cast<int*>((s))[1] = 0; } while (0)
#define sig_entry_count(s) (reinterpret_cast<const int*>((s))[1])
#define sig_bump_count(s) (++reinterpret_cast<int*>((s))[1])
// [R29-3] ORIG WriteLog 0x0856fcb5 起：entry 4 个字段是 **4 条独立语句**
// （每条各自 `mov -0x14(%ebp),%eax; mov 0x4(%eax),%ebx` 重读 sig->count ⇒ 4 次），
// bytes 低/高为**一次 64 位写**（`mov 0x10(%eax),%edx; mov 0xc(%eax),%eax;
// mov %eax,0x10(..); mov %edx,0x14(..)`）⇒ 拆 4 条 + 64 位视角。
#define sig_write_entry_id(s, slot, v) \
    (*reinterpret_cast<unsigned int*>((s) + 8 + (slot) * 0x14 + 0x00) = (v))
#define sig_write_entry_count(s, slot, v) \
    (*reinterpret_cast<int*>((s) + 8 + (slot) * 0x14 + 0x04) = (v))
#define sig_write_entry_bytes(s, slot, v) \
    (*reinterpret_cast<unsigned long long*>((s) + 8 + (slot) * 0x14 + 0x08) = (v))
#define sig_write_entry_avg(s, slot, v) \
    (*reinterpret_cast<int*>((s) + 8 + (slot) * 0x14 + 0x10) = (v))

// [T2b-GI · P327] 定义前移：ORIG 该 TU 首个强外部符号 = 本函数 ⇒ GI 键名来源
// （`_GLOBAL__I__ZN15message_counter13CTimeInterval6CreateEv`，ORIG 0x857015f）。
// 只移函数定义；无 file-scope 对象移动 ⇒ 构造序零变化。ORIG 原址 0x0856f974。
#include "MessageCounter/Create.inc"
{
    // [b28d-51] 判词 N（ORIG 0x0856f974，N77 族）：三 movl $0 前置清零 = 4.1 对
    // `new T()` value-init 老规则（CWG434 后 4.4 对 user-ctor 类不 emit；手写
    // memset/placement-new 均改布局）。语义/构造集齐，byte 层封顶。
    // [b28c-27] N77 复核：ORIG 三 movl $0 前置清零 = 4.1 对 `new T()` value-init
    // 老规则（CWG434 后 4.4 对 user-ctor 类不 emit；手写 memset/cast 清零形均改
    // 调用布局）。语义等价（ctor 已写 vptr+基类成员），ndiff 3 封顶。
    // ORIG @0x0856f974：operator new(0xc) → 三槽 u32 清零 →
    // 构造派生 CUnixTimeInterval（ORIG call _ZN15message_counter17CUnixTimeIntervalC1Ev，
    // 置派生 vtable；基类构造为空实现）→ 返回。
    // [b28-L11 判 N·N77] 三 movl $0 前置清零 = 4.1 对 `new T()` value-init 的
    // 老规则（CWG434 后 4.4 对 user-ctor 类不再 emit memset/直存段；手写
    // memset/cast 清零形均改调用布局，N77 判词在册）。语义等价（ctor 已写
    // vptr+基类成员），ndiff 3 封顶。
    return new CUnixTimeInterval();   // ORIG：_Znwj(0xc) → C1（置 vptr）
}

// ===================== MessageKey =====================

// ORIG @0x08570204。
#include "MessageCounter/MessageKey.inc"
    : m_id(id), m_type(type), m_size(size)
{
}

// ORIG @0x08570192：先比 id（signed，< 返 1）；相等再比 type（unsigned char，
// jae→0 / < 返 1）；相等再比 size（unsigned short，同上），否则返 0。
bool MessageKey::operator<(const MessageKey& rhs) const
{
    if (m_id < rhs.m_id)
        return true;
    if (m_id == rhs.m_id)
    {
        // uchar/ushort 成员直接比较（ORIG 单 movzbl/movzwl，无 uint 中转槽）；
        // == 守卫形使 4.4 生成 jne 直跳共享 ret0 块（P63 共享出口）。
        if (m_type < rhs.m_type)
            return true;
        if (m_type == rhs.m_type)
        {
            if (m_size < rhs.m_size)
                return true;
        }
    }
    return false;
}
// ===================== CTimeInterval（基类） =====================

// P210：派生 ctor 定义前置于基类 ctor 定义之前——4.4 对「TU 内尚未见定义」的
// callee 不精确继承 outgoing，取保守大帧（ORIG sub $0x18；b28-L11 复验旧
// 「16k+4 不可达」判词：帧差根因是定义顺序而非 pad 量化）。

// ORIG @0x085702c6：仅设 vptr。
#include "MessageCounter/CTimeInterval.inc"
{
}

#include "MessageCounter/Destory.inc"
{
    // ORIG @0x0856f9b0：operator delete(param)，无析构链。
    ::operator delete(p);
}

#include "MessageCounter/SetLastTime.inc"

#include "MessageCounter/GetTimeInterval.inc"
{
    return 0;
}

// ===================== CUnixTimeInterval（派生） =====================

// [R29-2] CUnixTimeInterval() 用户 ctor 已删（声明同步删，P299/CWG434）：
// ORIG nm 中 C1Ev/C2Ev 为 W size 0x1c ⇒ 隐式 ctor；仅此才能让
// `new CUnixTimeInterval()` 的 value-init 前置清零发射（ORIG Create 0x0856f974
// 三 movl $0 + call C1Ev）。隐式 ctor 体 = 调基类 C1Ev + 置派生 vptr，与旧
// 空体用户 ctor 逐条等价。

// ORIG @0x08570236：gettimeofday(&this->m_last, 0)。
#include "MessageCounter/SetLastTime_1.inc"
{
    gettimeofday(&m_last, 0);
}
// ORIG @0x08570254：(now-last)_sec*1000 + (now-last)_usec/1000.0，
// FP 取整后转 long long 返回。
#include "MessageCounter/GetTimeInterval_1.inc"
{
    struct timeval now;
    gettimeofday(&now, 0);

    // [b28d-51] ORIG 0x0857028f 段：x87 全程在栈上（无具名 double 局部往返），
    // fldcw 设 RC=11(截断) 后直接 fistpll 返回，无 `>=0.5` 进位调整。
    return (long long)((double)((int)(now.tv_sec - m_last.tv_sec) * 1000) +
                       ((double)((int)(now.tv_usec - m_last.tv_usec)) / 1000.0));
}
// ===================== CMessageCounter =====================

// ORIG ctor @0x0844de36：map 默认构造 + this+0x18=param。
#include "MessageCounter/CMessageCounter.inc"
    : m_map(),
      m_idBias(idBias)
{
}

#include "MessageCounter/dtor_CMessageCounter.inc"

// ORIG IncreaseMessage @0x0856f9c4：命中则 ++count → 64 位累加耗时字节 →
// 滑动平均 avg = avg*((n'-1)/n') + newBytes*(1/n')；未命中插入首记录。
// [R29-1 按 ORIG 报告逐条重写] 六项：
// ① `it->second` 逐次重读（ORIG 6× _Rb_tree_iterator::ptEv，原我方具名引用 `value`
//    只 1 次 ⇒ p167 MISSING 6）；
// ② 比较用正向 `!=`（ORIG `neq`；我方原 `==` 生 `eq` ⇒ EXTRA）；
// ③ bytes 累加为 64 位 `+=`（ORIG 0x0856fa8a `mov;mov;add;adc;mov;mov`；
//    原我方分 low/high + 进位判断多 1 槽 3 指令）；
// ④ `wOld`/`wNew` 两具名 double，声明于 find 之后（ORIG 预清 `fldz;fstpl` ×2 位于
//    find 之后 end 之前，槽 -0x18/-0x10，wOld 更深 ⇒ 先声明）；
// ⑤ 未命中插入记录 = { count=1, bytes=(unsigned long long)bytes,
//    avg=(double)bytes }（ORIG 0x0856fb3e `movl $0x1,-0x7c`、bytes→-0x78/-0x74、
//    `fildll -0x98; fstpl -0x70`）——**原我方写 `{0,0,0,0.0}` 是语义 bug**
//    （首记录丢 bytes/avg、count 从 0 起）；
// ⑥ 平均更新书写序 `m_avg * wOld + (double)bytes * wNew`（ORIG 首条 `fldl 0x14(%eax)`
//    即 avg ⇒ P243 左操作数先 fld）。
#include "MessageCounter/IncreaseMessage.inc"
                                      unsigned short size, long long bytes)
{
    MessageKey key((int)id, type, size);
    std::map<MessageKey, MessageValue>::iterator it = m_map.find(key);
    double wOld = 0.0;
    double wNew = 0.0;
    if (it != m_map.end())
    {
        ++it->second.m_count;
        // [N2-S5 负结论] ORIG 0x0856fa94 `add -0x98(%ebp),%eax; adc -0x94(%ebp),%edx`
        // 为「左值在 eax/edx + 内存源」形；`+=` 实测 4.4 反选「先加载右值
        // bytes 到 eax/edx」，且显式 `m_bytes = m_bytes + bytes` 形更劣
        // （132/139 → 132/141）⇒ 保持 `+=`；本处 2 条 + %esi 保活连带
        // （push/pop + esp 恢复 epilogue）归 4.4 regmove 差异。
        it->second.m_bytes += (unsigned long long)bytes;

        wOld = (double)(it->second.m_count - 1) / (double)it->second.m_count;
        wNew = 1.0 / (double)it->second.m_count;
        it->second.m_avg = it->second.m_avg * wOld + (double)bytes * wNew;
        return;
    }

    // 未命中：插入首记录（ORIG 0x0856fb3e 段）。左值实参 make_pair(key, zero)，
    // ORIG 发射 _ZSt9make_pairIRN15message_counter10MessageKeyERNS0_12MessageValueEE。
    MessageValue zero;
    zero.m_count = 1;
    zero.m_bytes = (unsigned long long)bytes;
    zero.m_avg = (double)bytes;
    m_map.insert(std::make_pair(key, zero));
}

// ORIG WriteLog @0x0856fbb0：LOGDB_Q(#4) 投递 SIG_INSERT_RESPONSE_TIME；
// 满 100 条续包；SIG: int baseSec@0 / int count@4 /
// entries[100]@+8 每 0x14。收尾 map.clear()。
// [b28d-51] 判词 N（ORIG 0x0856fbb0，N82/N89 族）：map 迭代器 + snprintf 变参
// 装载序跨版本差；语义/调用集齐，byte 层封顶。
#include "MessageCounter/WriteLog.inc"
{
    if (m_map.empty())
        return;

    // [R29-3] 按 ORIG 0x0856fbb0 重写：删 3 个「ORIG 无槽」具名局部
    // （`StreamPool& pool` / `MsgQueueMgr& queueMgr` / `Stream* stream`，P0/P183：
    // ORIG 0x0856fbcb 直接 `mov 0x940bd6c,%eax` 作 this 即调 Acquire，无槽往返）；
    // CStreamGuard 构造用 Acquire 内联实参（ORIG 0x0856fbe8：Acquire 结果直接
    // `mov %eax,0x4(%esp)`）；`<<` 走 `(*guard)`（P132：ORIG `call CStreamGuard::deEv`
    // 先于 `call CStreamGuard::lsEi`）；GetInBuffer 走 `guard->`（P103：ORIG
    // `call CStreamGuard::ptEv` 先于 GetInBuffer）。baseSec 复用 nowSec 槽
    // （ORIG -0x10 被 `sub 余数` 改写后即 baseSec）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MessageCounter.cpp", 99),
                       false);

    (*guard)->operator<<(0x26c);
    (*guard)->operator<<(-1);

    // [N2-S5] ORIG 0x0856fcb5 起一律 `mov 0x4(%eax),%ebx` / `movl $0x0,0x4(%eax)`
    // 单条 disp；而 TU 宏的数组下标形 `((int*)s)[1]` 在 4.4 -O0 恒发射
    // `add $0x4,%eax; mov (%eax)` 两条（探针 /tmp/probe2.cpp：f1 下标形 3 条 vs
    // f2 成员访问 2 条、g1 两条 vs g2 两条直存）⇒ 本函数体改走**函数局部视图
    // 结构体**成员访问（TU 级宏/sig 视图保持不动）：+0 baseSec、+4 count、
    // entries[100] @+8 每 0x14（8+100*0x14 = 0x7d8 与 SIG 视图吻合）。
    struct SigEntryView
    {
        unsigned int m_id;            // +0x00
        int m_count;                  // +0x04
        unsigned long long m_bytes;   // +0x08（ORIG 一次 64 位写 0x10/0x14）
        int m_avgRounded;             // +0x10（fistpl RC=11 截断）
    };
    struct SigView
    {
        int m_baseSec;                // +0x00
        int m_count;                  // +0x04
        SigEntryView m_entries[100];  // +0x08
    };

    SigView* sig = reinterpret_cast<SigView*>(
        guard->GetInBuffer<SIG_INSERT_RESPONSE_TIME>());

    int nowSec = GlobalData::s_systemTime_.getCurSec();
    int mod = nowSec % UPDATE_PERIOD;
    nowSec = nowSec - mod;
    sig->m_baseSec = nowSec;
    sig->m_count = 0;

    for (std::map<MessageKey, MessageValue>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        // [R29-3] 删 `key`/`value`/`slot`/`idField` 四个具名局部：ORIG 每字段
        // 各重读 sig->count（4 次）并逐次 `it->first`/`it->second`（ptEv）；
        // avg 按 ORIG `fistpl`（RC=11 截断，无 ±0.5 调整）写 `(int)`。
        sig->m_entries[sig->m_count].m_id =
            ((it->first.m_id == 0)
                 ? 0u
                 : (unsigned int)(it->first.m_id + m_idBias)) +
            (unsigned int)it->first.m_size;
        sig->m_entries[sig->m_count].m_count = it->second.m_count;
        sig->m_entries[sig->m_count].m_bytes = it->second.m_bytes;
        sig->m_entries[sig->m_count].m_avgRounded = (int)it->second.m_avg;
        ++sig->m_count;

        if (sig->m_count == 100)
        {
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);

            guard.Set(GlobalData::s_stream_pool->Acquire("MessageCounter.cpp", 0x7b));
            (*guard)->operator<<(0x26c);
            (*guard)->operator<<(-1);
            sig = reinterpret_cast<SigView*>(
                guard->GetInBuffer<SIG_INSERT_RESPONSE_TIME>());
            sig->m_baseSec = nowSec;
            sig->m_count = 0;
        }
    }

    if (sig->m_count == 0)
    {
        GlobalData::s_stream_pool->Free(guard);
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }

    m_map.clear();
}

}  // namespace message_counter
