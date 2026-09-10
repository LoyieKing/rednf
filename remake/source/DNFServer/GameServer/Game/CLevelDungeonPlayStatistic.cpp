// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CLevelDungeonPlayStatistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CLevelDungeonPlayStatistic.h"

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// [b28c-32] verdict=DIFF：N18a（ORIG it->second 成员读发 8 字节两段载入，4.4 恒折单条）。
#include "CLevelDungeonPlayStatistic/InsertDatabase.inc"
{
    if (m_levelPlayMap.size() == 0)
        return;
    // ORIG 栈槽（低→高）：end@-0x18、it@-0x14、guard@-0x10；
    // gcc4.4 按声明序从高地址往下分配，故声明序 guard → it → endIt。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x569),
                       true);
    **guard << 0x107;
    **guard << -1;
    **guard << (int)m_levelPlayMap.size();
    std::map<int, stDungeonPlayInfo>::iterator it = m_levelPlayMap.begin();
    std::map<int, stDungeonPlayInfo>::iterator endIt = m_levelPlayMap.end();
    while (it != endIt)
    {
        // pair 起始即 key；value（stDungeonPlayInfo）@+4，故 value+4=m_userCount、
        // value+8=m_playCount（ORIG 经 iterator::operator-> 读 +0/+4/+8）。
        // [N6S3 2026-09-11 封顶] ORIG 0x0860ef00-0x0860ef06 与 0x0860ef2a-0x0860ef30：
        //   `mov 0x8(%eax),%edx; mov 0x4(%eax),%eax` 后 `mov %eax,%ebx`（取低 32）
        //   / `mov %edx,%ebx`（取高 32）＝ 把 value（pair+4，8 字节）**整体两段
        //   载入**再截半；base 直用 operator-> 返回值（无取址物化）。
        //   我方两个 int 成员（.h:17-18）4.4 恒折单条 `mov 0x4(%eax),%ebx` /
        //   `mov 0x8(%eax),%ebx`（gap=+4 的来源）。函数体内任何 8 字节双关
        //   （`*(unsigned long long*)&it->second`，含先绑引用）都被 -O0 物化
        //   &second：实测多出 `add $0x4,%eax` + 引用入槽/dead 低半 = 每次 +3 条
        //   （134/131，gap=-3，反劣化）⇒ 已回滚。要复现须 .h 级 8 字节视角
        //   （union { struct {int m_userCount; int m_playCount;}; unsigned long long m_pack; }），
        //   禁改 .h ⇒ 上报父泳道 N6Free231to250。
        **guard << it->first;
        **guard << it->second.m_userCount;
        **guard << it->second.m_playCount;
        ++it;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_levelPlayMap.clear();
}
