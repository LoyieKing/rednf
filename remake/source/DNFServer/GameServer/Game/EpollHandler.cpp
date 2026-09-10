// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EpollHandler/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r EpollHandler 实现（ORIG 0x857c084-0x857c2e3，AE 口径还原）。
// [GI-ioinit L21 · P338/P327] 补 <iostream>：本 TU 原为避免 __ioinit 而只声明
// `std::cerr`，但 ORIG 该 TU 有 `_GLOBAL__I__ZN12EpollHandlerC2Ev`（0x857c324）
// ⇒ 必须先恢复 __ioinit 动态初始化（ctor 已是首强符号）。
#include <iostream>

#include "EpollHandler.h"
#include "CEnvironment.h"

#include <errno.h>
#include <ostream>
#include <string.h>

namespace std
{
extern ostream cerr;   // 仅声明（真实定义在 libstdc++），避免 TU 内 __ioinit 静态初始化
}

#include "EpollHandler/EpollHandler.inc"
{
    init();
}

#include "EpollHandler/dtor_EpollHandler.inc"
{
    destroy();
    release();
}

#include "EpollHandler/init.inc"
{
    m_fd = epoll_create(600);
    if (m_fd < 0)
    {
        std::cerr << "[Epoll::init] Can't init epoll create" << std::endl;
        return 0;
    }

    m_events = (epoll_event*)new char[0x1c20];
    if (m_events == NULL)
    {
        std::cerr << "[Epoll::init] Can't alloc event memory" << std::endl;
        return 0;
    }
    return 1;
}

#include "EpollHandler/destroy.inc"
{
    if (m_events != NULL)
    {
        ::operator delete[](m_events);
    }
    m_events = NULL;
}

#include "EpollHandler/release.inc"
{
}

#include "EpollHandler/SetEpoll.inc"
{
    if (bET)
    {
        m_event.events = 0x8000001d;
    }
    else
    {
        m_event.events = 0x1d;
    }
    m_event.data.ptr = data;

    Guard<Mutex> guard(&m_mutex);
    if (epoll_ctl(m_fd, EPOLL_CTL_ADD, fd, &m_event) < 0)
    {
        return errno;
    }
    return 0;
}

#include "EpollHandler/ResetEpoll.inc"
{
    memset(&m_event, 0, sizeof(m_event));
    m_event.events = EPOLLIN;

    Guard<Mutex> guard(&m_mutex);
    if (epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, &m_event) < 0)
    {
        return errno;
    }
    return 0;
}

#include "EpollHandler/GetEpollFD.inc"
{
    return m_fd;
}

#include "EpollHandler/GetEpollEvents.inc"
{
    return m_events;
}

// ---- 全局单例访问器 ----
// ORIG 0x82a70ab W（_Z13G_EpollHandlerv）：GlobalInstance<EpollHandler>::inst_ptr()
// （create 惰性 new 并缓存至 m_p，ORIG m_p 位于 BSS 0x941f89c）。
EpollHandler* G_EpollHandler()
{
    return GlobalInstance<EpollHandler>::inst_ptr();
}
template class GlobalInstance<EpollHandler>;
