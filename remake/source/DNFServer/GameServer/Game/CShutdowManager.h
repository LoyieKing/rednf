#ifndef DNF_GAME_CSHUTDOWMANAGER_H_
#define DNF_GAME_CSHUTDOWMANAGER_H_

// df_game_r 还原 —— CShutdowManager（G2-4 批次，关服管理人）。
// 方法签名对照 ORIG（_ZN15CShutdowManager* 族）。
class CShutdowManager
{
public:
    CShutdowManager();
    ~CShutdowManager() {}

    void SendLastMsgDBQueue(class CUser* user);

    char m_pad[0x10];
};

#endif  // DNF_GAME_CSHUTDOWMANAGER_H_