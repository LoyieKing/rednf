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
    ThreadInterface() {}                   // 0x08630b38 仅 vptr；m_tid/m_tidCopy 未初始化
    virtual ~ThreadInterface() {}          // 0x08630b46 / 0x08630b6c
    virtual void stop() {}                 // 0x08630c14（槽0，空体）
    // 槽3 线程体。ORIG 纯虚（nm 无 ThreadInterface::dispatch 符号）；六个派生
    // 线程类（DBThread/DispatchThread/NetworkThread/UdpThread/MonitorTcpThread/
    // AcceptThread）均已覆盖（ThreadDispatchers 批次，T 符号见各 dispatch 报告）。
    virtual void dispatch(void* arg) = 0;

    bool begin()                           // 0x08630b8a 逐指令转写
    {
        int rc = pthread_create(&m_tid, NULL,
                                &ThreadInterface::dispatch_proxy, this);
        if (rc != 0)
        {
            // ORIG 输出流 std::cerr；文案「推断」。
            std::cerr << "thread create failed rc=" << rc << std::endl;
        }
        m_tidCopy = m_tid;                 // ORIG：*(this+8) = *(this+4)
        return true;
    }
    // 定义落 OnlyOrigBatch11.cpp（ORIG 独立符号：join T 0x08630c46 /
    // getThreadID W 0x082a4218，非 const）。
    int join();                            // 0x08630c46
    unsigned long getThreadID();           // 0x082a4218（读 +8，非 const）
    static void* dispatch_proxy(void* p)   // 0x08630c1a（读虚表+0xc，以 (p,p) 调用）
    {
        static_cast<ThreadInterface*>(p)->dispatch(p);
        return 0;
    }

    pthread_t m_tid;      // +0x04
    pthread_t m_tidCopy;  // +0x08
};

#endif  // DNF_THREADINTERFACE_H
