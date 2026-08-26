#ifndef DNF_GAME_CSERVEREVENT_H_
#define DNF_GAME_CSERVEREVENT_H_

// df_game_r 还原 —— CServerEvent（G2-4 批次，服务端事件状态）。
// 布局/方法签名对照 ORIG（_ZN12CServerEvent* 族）。
class CServerEvent
{
public:
    CServerEvent() {}
    ~CServerEvent() {}

    // ORIG 调用点无 this 压栈，故为静态成员；定义处无需 static 关键字
    static int GetExpRate();
    int AddExpRate(int rate);
    int SubExpRate(int rate);
    int IsEventing(int idx);
    void TurnOnEvent(int idx, int durationSecond);
    void TurnOffEvent(int idx);
    int GetEventInterval(int idx);

    // ORIG 静态数据（B：m_nExpRate 0x943c138 / m_nCurEvent 0x943c13c /
    // m_nCurEventInterval 0x943c144）——GetExpRate/AddExpRate/SubExpRate/
    // TurnOnEvent/TurnOffEvent/GetEventInterval 逐指令转写自 ORIG。
    static int m_nExpRate;
    static int m_nCurEvent[0x100];
    static int m_nCurEventInterval[0x100];

    char m_pad[0x40];
};

#endif  // DNF_GAME_CSERVEREVENT_H_
