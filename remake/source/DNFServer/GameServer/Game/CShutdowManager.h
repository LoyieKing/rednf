#ifndef DNF_GAME_CSHUTDOWMANAGER_H_
#define DNF_GAME_CSHUTDOWMANAGER_H_

// df_game_r 还原 —— CShutdowManager（G2-4 批次，关服管理人）。
// 方法签名对照 ORIG（_ZN15CShutdowManager* 族）。
class CShutdowManager
{
public:
    CShutdowManager();
    ~CShutdowManager() {}

    unsigned int SendLastMsgDBQueue(class CUser* user);   // ORIG 0x82a37c8，恒返回 1   // ORIG 0x82a37c8，恒返回 1
    void SendShutdownSignal();  // ORIG 0x82a35a6 T（定义在 GameMain.cpp）
    bool WaitUntilExitThread();  // ORIG 0x82a396e：join 8 个线程全局后返回 true

    char m_pad[0x10];
};

CShutdowManager* GetInstanceShutdowManager();  // ORIG 0x82a6fbb（W）

#endif  // DNF_GAME_CSHUTDOWMANAGER_H_