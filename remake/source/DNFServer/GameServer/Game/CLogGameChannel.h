#ifndef _WONGWORK_CLOGGAMECHANNEL_H_
#define _WONGWORK_CLOGGAMECHANNEL_H_

// df_game_r 还原 —— WongWork::CLogGameChannel（LogGameChannel 定时落库）。
// 参照 docs/class_func_reports/WongWork__CLogGameChannel/ + df_game_r 反汇编。
// 布局（ORIG ctor 0x082a71c4）：
//   +0x00 int m_inUser      累计进入用户（gc_new）
//   +0x04 int m_outUser     累计离开用户（gc_out）
//   +0x08 int m_currentUser 当前在线用户（gc_now）
//   +0x0c int m_period      日志定时器周期（秒，默认 10）
// 定时器：SetNextLogTimer 以 m_period*60 秒为间隔向 TimerQueue 注册 TIMER_MESSAGE_0x58，
// 到期触发后调用 ReqSaveLog 打包 SIG_SAVE_LOGGAMECHANNEL 投递 LOGDB_Q。

struct SIG_SAVE_LOGGAMECHANNEL;
class MySQL;

namespace WongWork
{
class CLogGameChannel
{
public:
    CLogGameChannel();                                  // 0x082a71c4 W
    void StartLogTimer();                               // 0x085503ea
    void SetNextLogTimer();                             // 0x085503fe
    void IncInUser();                                   // 0x085506de
    void IncOutUser();                                  // 0x08550700
    void ResetInOutUser();                              // 0x085506c6
    void ReqSaveLog();                                  // 0x08550456
    void DoSaveLog(SIG_SAVE_LOGGAMECHANNEL* pSig);      // 0x08550612
    MySQL* GetLogTargetDB();                            // 0x085503c4

private:
    int m_inUser;        // +0x00  gc_new
    int m_outUser;       // +0x04  gc_out
    int m_currentUser;   // +0x08  gc_now
    int m_period;        // +0x0c  定时器周期（秒，默认 10）
};
}

#endif // _WONGWORK_CLOGGAMECHANNEL_H_
