// ============================================================================
// df_game_r 还原 —— W8LT_DBTask（BatchW2_03）
// ORIG 符号（nm 核对）：
//   T _ZN6DBTaskC2Ev / C1Ev           0x083fbfc6  sz 0x13
//   T _ZN6DBTaskD2Ev / D1Ev           0x083fbfda  sz 0x13
//   T _ZN6DBTask16RegistDispatcherEv  0x083fbfee  sz 0x3168
//   T _ZN6DBTask18UnregistDispatcherEv 0x083ff156 sz 0x72
//   T _ZN6DBTask12HandleEventsEN11MsgQueueMgr9QUEUE_IDXERi 0x083ff1c8 sz 0x22b
//   T _ZN6DBTask9DBProcessEP6StreamRi  0x083ff3f4 sz 0x1fd
//   W _ZN16stPvPTempTable_tD1Ev/D2Ev   0x08458e92  sz 0x5
// 报告：docs/class_func_reports/DBTask/{DBTask.md,~DBTask.md}（ctor/dtor 忠实
// 转发），其余四函数按 objdump 反汇编逐指令转写；字符串常量取自 .rodata。
//
// 对象布局（DBTask）：+0x00 int 当前队列 idx；+0x04 起 DBDispatcher*[852]
// （RegistDispatcher 循环边界 cmpl $0x353 == 851、最大注册槽 slot[848]+?——
// 见 RegistDispatcher 注释；sizeof = 4 + 852*4 = 0xd54）。
//
// 派生 dispatcher 类建模口径：ORIG 各 C1 反汇编全部为「调用 DBDispatcher 基类
// ctor + 编译器 vptr 赋值」共 ~10 指令的标准空构造（逐个 objdump 核对，证据见
// /tmp/df_batches/w8lt/regist_dispatcher.asm）；对象尺寸均 0x4（operator new 的
// 尺寸实参）。因此此处空派生类与 ORIG 完全一致。批界与阻塞条目在
// RegistDispatcher 内以注释保留 ORIG 地址。
#ifndef DNF_GAME_W8LT_DBTASK_H_
#define DNF_GAME_W8LT_DBTASK_H_

#include "MsgQueueMgr.h"

class Stream;
class DBDispatcher;

class DBTask
{
public:
    static const int DISPATCHER_COUNT = 852;   // ORIG Unregist/Regist 循环上界 0..851

    DBTask();                                    // 0x083fbfc6
    ~DBTask();                                   // 0x083fbfda

    void RegistDispatcher();                     // 0x083fbfee
    bool UnregistDispatcher();                   // 0x083ff156（恒返 true）
    bool HandleEvents(MsgQueueMgr::QUEUE_IDX queueIdx,
                      int& signal);              // 0x083ff1c8
    bool DBProcess(Stream* stream, int& signal); // 0x083ff3f4

private:
    int m_curQueueIdx;            // +0x00（HandleEvents 写入当前处理的队列枚举值）
    DBDispatcher* m_dispatchers[852];  // +0x04（消息 id -> dispatcher 表）
};

#endif  // DNF_GAME_W8LT_DBTASK_H_
