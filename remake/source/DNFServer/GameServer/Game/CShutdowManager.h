#ifndef DNF_GAME_CSHUTDOWMANAGER_H_
#define DNF_GAME_CSHUTDOWMANAGER_H_

// df_game_r 还原 —— CShutdowManager（G2-4 批次，关服管理人）。
// 方法签名对照 ORIG（_ZN15CShutdowManager* 族）。
class CShutdowManager
{
public:
    CShutdowManager();
    // 析构为隐式 trivial：ORIG GetInstanceShutdowManager@0x82a6fbb 单例
    // static CShutdowManager 的 assembly 只有 ctor 调用、无 __cxa_atexit
    // 注册 ⇒ ORIG 析构是 trival；用户声明空 dtor 会触发 atexit 注册致 diff。

    unsigned int SendLastMsgDBQueue(class CUser* user);   // ORIG 0x82a37c8，恒返回 1
    void SendShutdownSignal();   // ORIG 0x82a35a6 T（定义在 GameMain.cpp）
    bool WaitUntilExitThread();  // ORIG 0x82a396e：join 8 个线程全局后返回 true

    // 字段由 Reset/EnableShutdown 反汇编实证：+0x00 byte（shutdown 使能位）、
    // +0x04 int、+0x08 int；其余保留 pad 至 0x10。
    void Reset();                       // ORIG W 0x082a6f88
    void EnableShutdown(bool on);       // ORIG W 0x082a6f70
    void OnAllUserLogout();             // ORIG T 0x082a36bc
    // ORIG W 0x084ed1ea/0x084ed1fe/0x084ed20a（Inter_LastMsgResponse 消费；
    // 定义属主未落地，U 引用合规——InterWave1 批）
    void IncrementLastMsgResponseCount();
    unsigned int GetLastMsgResponseCount();
    unsigned int GetLastMsgCount();

    char m_flag;                        // +0x00
    char m_pad1[3];
    int m_field4;                       // +0x04
    int m_field8;                       // +0x08
    char m_padC[4];
};

CShutdowManager* GetInstanceShutdowManager();  // ORIG 0x82a6fbb（W）

#endif  // DNF_GAME_CSHUTDOWMANAGER_H_