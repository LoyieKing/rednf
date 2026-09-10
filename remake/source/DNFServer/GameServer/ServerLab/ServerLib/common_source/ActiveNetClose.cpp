// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ActiveNetClose/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// nsl::ActiveNetClose.cpp
#include "ActiveNetClose.h"
#include "TCPUser.h"
#include "TraceLog.h"
#include "GlobalInstance.h"

namespace nsl {

#include "ActiveNetClose/ActiveNetClose.inc"
{
    bInData = false;
}

#include "ActiveNetClose/pushActiveClose.inc"
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(mLock);
    register unsigned int id = pUser->getUserId();
    activeCloseMap[id] = pUser;
    bInData = true;
    G_TraceLog()->sysLog(7, "active close push user tcpuserid :(%d) map size:(%d)", id, activeCloseMap.size());
}

#include "ActiveNetClose/onActvieClose.inc"
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(mLock);
    std::map<unsigned int, TCPUser*>::iterator iter = activeCloseMap.begin();
    register unsigned int key = iter->first;
    TCPUser* pUser = iter->second;
    bool ret_val = false;
    if (pUser->onActiveClose(key))
    {
        activeCloseMap.erase(key);
        outId = key;
        outUserPtr = pUser;
        ret_val = true;
        G_TraceLog()->sysLog(7, "onActiveClose erase tcpuserid :(%d) map size:(%d)", key, activeCloseMap.size());
    }
    if (activeCloseMap.size() == 0)
    {
        bInData = false;
    }
    else
    {
        bInData = true;
    }
    return ret_val;
}

ActiveNetClose* G_ActiveNetClose()
{
    return GlobalInstance<ActiveNetClose>::inst_ptr();
}

} // namespace nsl
