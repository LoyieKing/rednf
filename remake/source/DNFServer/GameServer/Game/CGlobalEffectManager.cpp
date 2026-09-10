// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CGlobalEffectManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CGlobalEffectManager / CGlobalEffect / TimerGlobalEffectItem*。
// 逐函数依据 docs/class_func_reports/（CGlobalEffectManager/*.md、CGlobalEffect/makeNotifyInfo.md、
// TimerGlobalEffectItemBegin|Timeout/registTimer.md）与 ORIG objdump 逐指令转写。
// 依赖符号（G_GameWorld/PacketGuard/CSystemTime/G_TimerQueue 等）均已由
// GameWorld.cpp / PacketGuard.cpp / CSystemTime.cpp / TimerQueue.cpp 提供。
#include "CGlobalEffectManager.h"

#include <math.h>
#include <string.h>

#include "GameWorld.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "TimerQueue.h"
#include "CSystemTime.h"
#include "GlobalData.h"

// 值编码常数：ORIG makeNotifyInfo 用 10000.0f 缩放（.rodata 0x8c854b0 = 0x461c4000 = 10000.0f）。
static const float kValueScale = 10000.0f;

// ============================================================================
// CGlobalEffect（0x2c）
// ============================================================================

// ORIG 0x084b7880：put_int(type) / put_int((int)(value*10000.0f) 截断) /
//               put_int(strlen(name)) / put_str(name,len) / put_int(endTime)。
#include "CGlobalEffectManager/makeNotifyInfo.inc"
{
    packet.put_int( static_cast<int>(m_type));
    packet.put_int(static_cast<int>(m_value * kValueScale));
    // P48/P0：ORIG 无 CSE，strlen(name) 调两次，各自结果寄存器直传入参（无 len 局部槽）。
    packet.put_int( static_cast<int>(strlen(m_name)));
    packet.put_str( m_name, static_cast<int>(strlen(m_name)));
    packet.put_int( static_cast<int>(m_endTime));
}

// ORIG 0x084b800a：type@+0、value@+4、name@+8（无条件 strncpy 0x1e、无尾 0）、endTime@+0x28。
#include "CGlobalEffectManager/CGlobalEffect.inc"
                             long endTime)
    : m_type(type), m_value(value)
{
    strncpy(m_name, name, sizeof(m_name) - 2);  // 0x1e，匹配 ORIG
    m_endTime = endTime;
}

// ORIG 0x082a7214：trivial dtor（仅 ret）。
#include "CGlobalEffectManager/dtor_CGlobalEffect.inc"
{
}

// ORIG 0x084b7fda：return m_type。
#include "CGlobalEffectManager/getType.inc"
{
    return m_type;
}

// ORIG 0x084b7fe4：return m_value。
#include "CGlobalEffectManager/getValue.inc"
{
    return m_value;
}

// ORIG 0x084b7ff8：return m_endTime <= curSec。
#include "CGlobalEffectManager/isTimeout.inc"
{
    return m_endTime <= curSec;
}

// ============================================================================
// CGlobalEffectManager（0x18）
// ============================================================================

// ORIG 0x082a721a：两个 vector<CGlobalEffect> 构造（成员自动）。
#include "CGlobalEffectManager/CGlobalEffectManager.inc"
{
}

// ORIG 0x08234fca：return m_activeEffects.size() + m_reservedEffects.size()。
#include "CGlobalEffectManager/getEffectNum.inc"
{
    return static_cast<unsigned int>(
        m_activeEffects.size() + m_reservedEffects.size());
}

// ORIG 0x084b7d2c：遍历 m_activeEffects，任一 m_type==type 返回 true。
#include "CGlobalEffectManager/isAffectedEffect.inc"
{
    // [b28c-16] P13：ORIG begin/end 头部各求值一次（it=-0xc、e=-0x10），
    // 4.4 对 cond 内 end() 每次迭代重调 ⇒ 具名缓存 end。
    std::vector<CGlobalEffect>::const_iterator it = m_activeEffects.begin();
    std::vector<CGlobalEffect>::const_iterator e = m_activeEffects.end();
    for (; it != e; ++it)
    {
        if (it->getType() == type)
            return true;
    }
    return false;
}

// ORIG 0x084b7daa：遍历 m_activeEffects，累加所有 m_type==type 的 m_value。
#include "CGlobalEffectManager/getEffectValue.inc"
{
    float sum = 0.0f;
    // [b28c-16] P13：end 缓存具名局部（ORIG sum=-0xc、it=-0x10、e=-0x14）。
    std::vector<CGlobalEffect>::const_iterator it = m_activeEffects.begin();
    std::vector<CGlobalEffect>::const_iterator e = m_activeEffects.end();
    for (; it != e; ++it)
    {
        if (it->getType() == type)
            sum += it->getValue();
    }
    return sum;
}

// ORIG 0x084b7ec4：put_header(0,0xb5)/put_byte(1)/put_int(size)，每项 makeNotifyInfo。
#include "CGlobalEffectManager/makeGlobalEffectInfo.inc"
{
    packet.put_header( 0, 0xb5);
    packet.put_byte( 1);
    packet.put_int( static_cast<int>(m_activeEffects.size()));
    // [b28c-16] P13：end 缓存具名局部（ORIG it=-0xc、e=-0x10）。
    std::vector<CGlobalEffect>::const_iterator it = m_activeEffects.begin();
    std::vector<CGlobalEffect>::const_iterator e = m_activeEffects.end();
    for (; it != e; ++it)
    {
        it->makeNotifyInfo(packet);
    }
}

// ORIG 0x084b7e48：PacketGuard，makeGlobalEffectInfo，finalize(true)，G_GameWorld()->send_all。
#include "CGlobalEffectManager/notifyGlobalEffectInfo.inc"
{
    PacketGuard packet;
    makeGlobalEffectInfo(packet);
    packet.finalize( true);
    G_GameWorld()->send_all(packet);
}

// ORIG 0x084b7932：若已受影响直接返回；按 beginSec 是否非零分派到活动/保留
// vector，并登记 TimerGlobalEffectItemBegin/Timeout。
#include "CGlobalEffectManager/registNewEffect.inc"
                                           const char* name, long beginSec,
                                           long reserveSec)
{
    if (isAffectedEffect(type))
        return;

    if (beginSec == 0)
    {
        // ORIG 0x084b796a：getCurSec 结果直接 `add 0x1c(%ebp),%eax` 后落
        // 0x10(%esp) 实参槽，**无** endTime 具名槽（我方曾多 -0x10/-0xc 两槽
        // + 4 条 mov）；且 0x084b798c ctor 后 0x084b799e 立即 push_back，
        // 0x084b79c6 析构**先于** notify ⇒ 源形为临时量绑定 push_back(T&&)，
        // 非具名局部 + std::move（std::move 会多 lea/mov/call 三条/分支）。
        m_activeEffects.push_back(CGlobalEffect(
            type, value, name, GlobalData::s_systemTime_.getCurSec() + reserveSec));
        notifyGlobalEffectInfo();
        TimerGlobalEffectItemTimeout::registTimer(type, reserveSec);
    }
    else
    {
        m_reservedEffects.push_back(CGlobalEffect(
            type, value, name, GlobalData::s_systemTime_.getCurSec() + beginSec + reserveSec));
        TimerGlobalEffectItemBegin::registTimer(type, beginSec);
        TimerGlobalEffectItemTimeout::registTimer(type, beginSec + reserveSec);
    }
}

// ORIG 0x084b7c50：m_reservedEffects 中 m_type==type 者移到 m_activeEffects 并擦除。
#include "CGlobalEffectManager/beginReservedEffect.inc"
{
    if (m_reservedEffects.empty())
        return;

    for (std::vector<CGlobalEffect>::iterator it = m_reservedEffects.begin();
         it != m_reservedEffects.end(); )
    {
        if (it->getType() == type)
        {
            m_activeEffects.push_back(*it);
            // b28d-50：ORIG 0x084b7cd4 erase 的 sret 落 -0xc 槽后**不回收**（直接
            // jmp 到循环条件），即原码丢弃返回值；写 `it = erase(it)` 会多一条
            // `mov -0x1c,%eax; mov %eax,-0x10` 回写并使帧 0x28→0x38。vector::erase
            // 返回的迭代器与入参 it 同址（元素左移），丢弃语义等价（P171 照抄）。
            m_reservedEffects.erase(it);
        }
        else
        {
            ++it;
        }
    }
    notifyGlobalEffectInfo();
}

// ORIG 0x084b7a94：构造 0xb5/byte0 包，遍历 m_activeEffects 收集超时项（m_endTime<=curSec），
// 每项 makeNotifyInfo 并擦除，末尾回填计数并 send_all。
#include "CGlobalEffectManager/checkTimeout.inc"
{
    PacketGuard packet;
    packet.put_header( 0, 0xb5);
    packet.put_byte( 0);
    // put_int 占位计数；随后用 put_int(index,&count) 回填。
    int countIndex = packet.get_index();
    // [判据⑩/顺序] ORIG 0x084b7ae3/0x084b7aea/0x084b7aee 顺序 = count 清零 →
    //   bool 预清 → put_int(0) ⇒ 两局部声明须在 put_int(0) 之前。
    int count = 0;
    // [判据⑫ clr_b orig=1 our=0] ORIG 0x084b7ae3/0x084b7aea 为
    //   `movl $0x0,-0x10` + `movb $0x0,-0x9`（count 之后紧邻的具名 bool 预清，
    //   全函数无读取点 = 原码遗留死变量）⇒ 补回以复现该条指令。
    bool reserved = false;
    (void)reserved;
    packet.put_int( 0);

    for (std::vector<CGlobalEffect>::iterator it = m_activeEffects.begin();
         it != m_activeEffects.end(); )
    {
        if (it->getType() == type && it->isTimeout(curSec))
        {
            it->makeNotifyInfo(packet);
            ++count;
            it = m_activeEffects.erase(it);
        }
        else
        {
            ++it;
        }
    }

    packet.put_int( countIndex, count);
    packet.finalize( true);
    G_GameWorld()->send_all(packet);
}

// ============================================================================
// TimerGlobalEffectItem（ORIG 静态 helper，登记全局定时器）
// ============================================================================

// ORIG 0x08637e78：InsertTimer(OBJ_TYPE_2, 0, msg=0x77, beginSec, type, 0)。
#include "CGlobalEffectManager/registTimer.inc"
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
        static_cast<TIMER_MESSAGE>(0x77), static_cast<int>(beginSec),
        static_cast<int>(type), 0);
}
