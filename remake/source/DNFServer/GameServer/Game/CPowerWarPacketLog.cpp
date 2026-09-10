// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPowerWarPacketLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CPowerWarPacketLog 实现。
// 报告：docs/class_func_reports/CPowerWarPacketLog/（CPowerWarPacketLog /
// InsertPacketLog / PrintPowerWarPacketLogReport / Reset / ~CPowerWarPacketLog）。
#include <algorithm>
#include <vector>

#include "CPowerWarPacketLog.h"
#include "GameTypes.h"   // cMyTrace

// ORIG C1 0x0848085a：map 成员默认构造 + Reset（无 vptr 写入）。
#include "CPowerWarPacketLog/CPowerWarPacketLog.inc"
{
    Reset();
}

// ORIG D1 0x0848089c：Reset() + map 成员析构（Reset 已 clear，析构为空操作）。
#include "CPowerWarPacketLog/dtor_CPowerWarPacketLog.inc"
{
    Reset();
}

// ORIG Reset 0x084808de：遍历 map，delete 每个 STPacketLog（析构+释放），
// 最后 map::clear()。
#include "CPowerWarPacketLog/Reset.inc"
{
    for (std::map<int, STPacketLog*>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        STPacketLog* log = it->second;
        if (log != 0)
            delete log;
    }
    m_map.clear();
}

// ORIG InsertPacketLog 0x08480970：以 (unsigned int)t 为 key find；
// 命中 → ++(元素->m_count)；未命中 → new STPacketLog(0x14) 初始化
// （m_count=1、m_field8=t、m_sendType=type、m_field4=i、m_name.assign(name)）
// 后 make_pair(t, log) insert。
#include "CPowerWarPacketLog/InsertPacketLog.inc"
                                         eSendType type, char const* name)
{
    std::map<int, STPacketLog*>::iterator it = m_map.find(t);
    if (it != m_map.end())
    {
        STPacketLog* found = it->second;
        found->m_count = found->m_count + 1;
    }
    else
    {
        STPacketLog* log = new STPacketLog;
        log->m_count = 1;
        log->m_field8 = t;
        log->m_sendType = type;
        log->m_field4 = i;
        log->m_name.assign(name);
        m_map.insert(std::make_pair(t, log));
    }
}

// ORIG PrintPowerWarPacketLogReport 0x08480ac0：
//   cMyTrace 三段头 → 收集 vector<STPacketLog*> → sort(Compare) →
//   逐条 cMyTrace 打印 → 尾分隔线。
#include "CPowerWarPacketLog/PrintPowerWarPacketLogReport.inc"
{
    // b28d-60：P221 全表达式临时链——ORIG 槽深序 trace1 -0x7c → trace3 -0x5c
    //（先声明更深），具名声明在 4.4 恒反向（-0x2c → -0x4c）⇒ 改「构造即调用」。
    cMyTrace(__PRETTY_FUNCTION__, 0x3a1, 0)("-------------------------------------------------------------------------------------------------------------");
    cMyTrace(__PRETTY_FUNCTION__, 0x3a2, 0)("|                                   POWAR WAR PACKET RESULT REPORT                                          |");
    cMyTrace(__PRETTY_FUNCTION__, 0x3a3, 0)("-------------------------------------------------------------------------------------------------------------");

    std::vector<STPacketLog*> vec;
    for (std::map<int, STPacketLog*>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        vec.push_back(it->second);
    }
    std::sort(vec.begin(), vec.end(), STPacketLog::Compare);

    // [N6S6ZeroB 2026-09-11] ORIG 0x08480ac0 槽序实证：收集循环迭代器在
    // -0x90（`lea -0x90(%ebp),%eax` @+0xbf，for-init 形态 ⇒ 4.4 延迟到最后分配，
    // 落更深槽），打印循环迭代器在 -0x8c（@+0x23d）。两循环原为互换（我们
    // -0x8c / -0x90）⇒ 打印循环改「普通块变量 + 空 init 的 for」，两槽归位，
    // ndiff 8→0，verdict NEAR→ae。
    std::vector<STPacketLog*>::iterator vit = vec.begin();
    for (; vit != vec.end(); ++vit)
    {
        STPacketLog* log = *vit;
        cMyTrace(__PRETTY_FUNCTION__, 0x3b1, 0)(
            "- PACKET CLASS:%d NO:%5d, COUNT:%8d, NAME:%s",
            log->m_field4, log->m_field8, log->m_count, log->m_name.c_str());
    }

    cMyTrace(__PRETTY_FUNCTION__, 0x3b6, 0)("-------------------------------------------------------------------------------------------------------------");
}
