#ifndef DNF_GAME_CSERVEREVENT_H_
#define DNF_GAME_CSERVEREVENT_H_

// df_game_r 还原 —— CServerEvent（G2-4 批次，服务端事件状态）。
// 布局/方法签名对照 ORIG（_ZN12CServerEvent* 族）。
class CServerEvent
{
public:
    CServerEvent() {}
    ~CServerEvent() {}

    // ORIG 调用点无 this 压栈（WarRoom::ClearReward 086bc907 call 前仅清零局部），
    // 故为静态成员；定义处无需 static 关键字
    static int GetExpRate();
    int AddExpRate(int rate);
    int SubExpRate(int rate);
    int IsEventing(int idx);
    void TurnOnEvent(int idx, int durationSecond);
    void TurnOffEvent(int idx);
    int GetEventInterval(int idx);

    char m_pad[0x40];
};

#endif  // DNF_GAME_CSERVEREVENT_H_