#ifndef DNF_GAME_CAPPLOADCHECKER_H_
#define DNF_GAME_CAPPLOADCHECKER_H_

// ============================================================================
// df_game_r Game/ CAppLoadChecker —— 服务器队列负载滞回监控。
// ORIG 源文件名（StreamPool::Acquire 实参）：ServerLoadChecker.cpp。
// 布局（ctor 0x85ffc6a 清零全部 0x14 字节）：
//   +0x00 int 基准值×4（setQueue 写入），+0x10..+0x13 char 状态字节×4
//   （-6..+6 滞回等级）。四个 checkLoad_* 阈值阶梯一致：
//   上行 10/25/40/80/150/250，下行同值，±6 处粘滞只返回不降级。
// ============================================================================

class CAppLoadChecker
{
public:
    CAppLoadChecker();                                              // 0x85ffc6a

    void setQueue(int dbq, int logdbq, int timerq, int packetq);     // 0x86004b0
    bool CheckLoad(int dbq, int logdbq, int timerq, int packetq);    // 0x86004d8
    void RequestDB(int dbq, int logdbq, int timerq, int packetq);    // 0x8600580

    bool checkLoad_dbq(int cur);      // 0x85ffcb2
    bool checkLoad_logdbq(int cur);   // 0x85ffeb0
    bool checkLoad_timerq(int cur);   // 0x86000b0
    bool checkLoad_packetq(int cur);  // 0x86002b0

private:
    int m_baseDbq;         // +0x00
    int m_baseLogdbq;      // +0x04
    int m_baseTimerq;      // +0x08
    int m_basePacketq;     // +0x0c
    char m_stateDbq;       // +0x10
    char m_stateLogdbq;    // +0x11
    char m_stateTimerq;    // +0x12
    char m_statePacketq;   // +0x13
};  // sizeof = 0x14

// guard 单例（__cxa_guard @0x9473200，实例 @0x9473208）。ORIG 0x85ffc24。
CAppLoadChecker* CAppLoadCheckerInstance();

#endif  // DNF_GAME_CAPPLOADCHECKER_H_
