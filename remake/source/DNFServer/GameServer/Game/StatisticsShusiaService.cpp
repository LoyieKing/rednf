// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StatisticsShusiaService/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <map>

#include "GameTypes.h"

class StatisticsShusiaService
{
public:
    StatisticsShusiaService();
    ~StatisticsShusiaService();

    void reset();
    void incDungeonFailure(int dungeonIdx);
    void incServiceUsage(int serviceIdx);
    int getDungeonFailure(int dungeonIdx);
    int getServiceUSage(int serviceIdx);

    Mutex m_mutex;              // +0x00
    std::map<int, int> m_dungeonFailure;  // +0x18
    std::map<int, int> m_serviceUsage;    // +0x30
};

#include "StatisticsShusiaService/StatisticsShusiaService.inc"
    : m_mutex(), m_dungeonFailure(), m_serviceUsage()
{
}

#include "StatisticsShusiaService/dtor_StatisticsShusiaService.inc"

#include "StatisticsShusiaService/reset.inc"
{
    Guard<Mutex> guard(&m_mutex);
    m_dungeonFailure.clear();
    m_serviceUsage.clear();
}

#include "StatisticsShusiaService/incDungeonFailure.inc"
{
    // ORIG @0x0860c1ba。
    // [P0/N82 消指令] ORIG 0x0860c2b4 直接 call
    // `_ZNSt4pairIKiiEC1IRiiEEOT_OT0_`（= 0x08610c38 同族，pair<int const,int>
    // 的转发 ctor）把临时对象作 insert 的实参；旧形 `std::make_pair(idx,1)` 造出
    // pair<int,int> 再经转换 ctor 拷进 value_type 临时 ⇒ 多一次构造 call + 8B 槽
    // （帧 0x40 vs ORIG 0x30）。改成 value_type 临时直传。
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_dungeonFailure.find(dungeonIdx);
    if (it == m_dungeonFailure.end())
    {
        m_dungeonFailure.insert(std::pair<const int, int>(dungeonIdx, 1));
    }
    else
    {
        it->second += 1;
    }
}

#include "StatisticsShusiaService/incServiceUsage.inc"
{
    // ORIG @0x0860c358。同上：ORIG 0x0860c3d4/0x0860c3da 以
    // `_ZNSt4pairIKiiEC1IRiiEEOT_OT0_`（0x08610c38）就地构造临时后直接
    // insert（无拷贝 ctor，无 -0x1c 中间槽）。
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_serviceUsage.find(serviceIdx);
    if (it == m_serviceUsage.end())
    {
        m_serviceUsage.insert(std::pair<const int, int>(serviceIdx, 1));
    }
    else
    {
        it->second += 1;
    }
}

#include "StatisticsShusiaService/getDungeonFailure.inc"
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_dungeonFailure.find(dungeonIdx);
    if (it != m_dungeonFailure.end())
    {
        return (*it).second;   // ORIG 用 operator*（de）
    }
    return 0;
}

#include "StatisticsShusiaService/getServiceUSage.inc"
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_serviceUsage.find(serviceIdx);
    if (it != m_serviceUsage.end())
    {
        return (*it).second;   // ORIG 用 operator*（de）
    }
    return 0;
}
