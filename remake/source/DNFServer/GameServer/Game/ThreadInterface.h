#ifndef DNF_THREADINTERFACE_H
#define DNF_THREADINTERFACE_H

// ThreadInterface —— 权威 ORIG 类（df_game_r 反汇编验证）。
//
// 反汇编证据（objdump -d /home/wangyilei/下载/df_game_r）：
//   C1  0x08630b38  置 vptr = 0x8ce6b98
//   D1  0x08630b46  置 vptr（不 delete）
//   D0  0x08630b6c  D1 + operator delete
//   stop 0x08630c14 空体
//   begin 0x08630b8a pthread_create(&m_tid, ..., &ThreadInterface::dispatch_proxy,
//                                   this)；成功后 m_tidCopy = m_tid
//   dispatch_proxy 0x08630c1a 读 vptr+0xc（虚表槽3）并以 (this,this) 调用线程体
//   join  0x08630c46 pthread_join(m_tid)
//   getThreadID 0x082a4218（W）读 +8（m_tidCopy）
//
// 布局：+0 vptr、+4 pthread_t m_tid、+8 pthread_t m_tidCopy。
// 虚表（0x8ce6b90 起）：槽0=stop()、槽1=~ThreadInterface()、槽2=operator delete、
//                       槽3=dispatch(void*)（ORIG 纯虚 → __cxa_pure_virtual）。
// 声明顺序 stop / ~ThreadInterface / dispatch 使 GCC 生成与该顺序一致的虚表
// （已验证：GCC 11 32 位 O0 生成 [stop, D1, D0, dispatch]）。
//
// 派生类（DBThread/DispatchThread/NetworkThread/UdpThread/MonitorTcpThread/
// AcceptThread/exchange_server::CExchangeServerThread）均由各自 TU 以本类为基类，
// 覆盖 dispatch(void*) 提供线程体。ORIG 中 dispatch 为纯虚；重建以空默认体代替，
// 避免强制派生类写线程循环桩（线程体实现属后续批次）。

#include <pthread.h>
#include <iostream>

class ThreadInterface
{
public:
    // 0x08630b38 仅 vptr；m_tid/m_tidCopy 未初始化。ORIG 为独立 T 符号（非头内联）：
    // 外联定义使派生 ctor 体产生真实 `call ThreadInterfaceC1`（ORIG 0x0847e268 实证），
    // 且 new 表达式按 potentially-throw 生成 cleanup（ORIG Init 各线程 new 均有
    // delete+Unwind pad，如 0x0829a4f3）。定义落 GlobalData.cpp（Main 批准属主）。
    ThreadInterface();
    virtual void stop() {}                 // 0x08630c14（槽0，空体）
    // [b28d-solo-W6_IPGDeps] 声明序修正：stop 必须在虚析构之前，否则 GCC 把
    // [D1,D0,stop,dispatch] 排成 vtable，stop 落 slot2（vptr+8）；ORIG
    // _ZTV13ThreadInterface @0x8ce6b90（objdump -s 实证，vptr=_ZTV+8）为
    // slot0=stop(0x08630c14)/slot1=D1(0x08630b46)/slot2=D0(0x08630b6c)/
    // slot3=pure_virtual。CIPGMgr::Finalize 0x08104618 `call *vptr+0` 即 stop。
    virtual ~ThreadInterface() {}          // 0x08630b46（槽1）/ 0x08630b6c（槽2 D0）
    // 槽3 线程体。ORIG 纯虚（nm 无 ThreadInterface::dispatch 符号）；六个派生
    // 线程类（DBThread/DispatchThread/NetworkThread/UdpThread/MonitorTcpThread/
    // AcceptThread）均已覆盖（ThreadDispatchers 批次，T 符号见各 dispatch 报告）。
    virtual void dispatch(void* arg) = 0;

    // [b28c-L18] P119：ORIG nm 实证 begin T 0x08630b8a / dispatch_proxy T 0x08630c1a
    // （外联强符号，与 C1 0x08630b38/join 0x08630c46 同段同 TU）⇒ 不能依赖头内联
    // （W 形）发射。定义外联落 GlobalData.cpp（C28L23 代落，body 按 ORIG 逐指令：
    // cerr 两段串 "[ThreadInterface::begin] Can't begin thread."（0x8ce6620）+
    // " "（0x8ce664c）+ SolsEi(rc) + endl；rc 具名槽往返）。
    bool begin();                          // 0x08630b8a
    // 定义落 OnlyOrigBatch11.cpp（ORIG 独立符号：join T 0x08630c46 /
    // getThreadID W 0x082a4218，非 const）。
    int join();                            // 0x08630c46
    unsigned long getThreadID();           // 0x082a4218（读 +8，非 const）
    static void* dispatch_proxy(void* p);  // 0x08630c1a（读 vptr+0xc，以 (this,arg) 调用）

    pthread_t m_tid;      // +0x04
    pthread_t m_tidCopy;  // +0x08
};

#endif  // DNF_THREADINTERFACE_H
