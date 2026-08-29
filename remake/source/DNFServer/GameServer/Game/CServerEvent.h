#ifndef DNF_GAME_CSERVEREVENT_H_
#define DNF_GAME_CSERVEREVENT_H_

// df_game_r 还原 —— CServerEvent（G2-4 批次，服务端事件状态）。
// 布局/方法签名对照 ORIG（_ZN12CServerEvent* 族）。
// ORIG 证据（df_game_r 95 版 objdump）：
//   - 所有方法调用点均无 this 压栈（dispatch_sig/TimerEventChristmas 直呼
//     AddExpRate/TurnOnEvent/SubExpRate/TurnOffEvent 只压实参），故全部为静态成员。
//   - nm 无 C1/C2/D1/D2 符号 → ORIG 构造/析构为编译器隐式 trivial（P12）。
//   - IsEventing 返回 bool：ORIG 为 `cmp $0x1; sete %al; ret` 无 movzbl 规范化（P9）。
class CServerEvent
{
public:
    static int GetExpRate();
    static int AddExpRate(int rate);
    static int SubExpRate(int rate);
    static bool IsEventing(int idx);
    static void TurnOnEvent(int idx, int durationSecond);
    static void TurnOffEvent(int idx);
    static int GetEventInterval(int idx);

    // ORIG 静态数据（B：m_nExpRate 0x943c138 / m_nCurEvent 0x943c13c /
    // m_nCurEventInterval 0x943c144）——GetExpRate/AddExpRate/SubExpRate/
    // TurnOnEvent/TurnOffEvent/GetEventInterval 逐指令转写自 ORIG。
    static int m_nExpRate;
    static int m_nCurEvent[0x100];
    static int m_nCurEventInterval[0x100];
};

#endif  // DNF_GAME_CSERVEREVENT_H_
