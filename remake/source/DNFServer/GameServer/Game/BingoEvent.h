// BingoEvent.h —— BingoEvent 权威头（Bingo 域，OrigBingoEvt 批次）。
//
// ORIG BingoEvent：docs/class_func_reports/BingoEvent/（13 报告全文已读）。
//   C1 0x080c9984：CEventBase() + vptr + new CBingo(0x1c) 挂 boost::shared_ptr
//                  （成员 +0x0c，getBingoSystem 0x080c9fea 返回其地址）。
//   D1 0x080c9a06：vptr + shared_ptr 析构 + CEventBase 析构；D0 0x080c9a68。
//   虚表槽：0/1 析构、2 StartEvent()、3 EndEvent()、4 StartEvent(Word_Param)、
//   5 StartAction、9 dailyresetData（与 CEventBase 声明顺序一致）。
// CBingo 前向声明：完整布局/方法实现归 BingoEventLink4.cpp（局部类）与
// CBingoTest.h（C1/D1/getBingoRandomVaule，StubCBingoDtor 属主）。
#ifndef DNF_GAME_BINGOEVENT_H_
#define DNF_GAME_BINGOEVENT_H_

#include <boost/shared_ptr.hpp>

#include "CGuildServerProxy.h"   // CEventBase / Word_Param / CUser 前向

namespace StackableAction
{
struct input_param;
}

class CBingo;

class BingoEvent : public CEventBase
{
public:
    BingoEvent();                                    // ORIG T 0x080c9984
    virtual ~BingoEvent();                           // ORIG T 0x080c9a06 (D1) / 0x080c9a68 (D0)
    virtual void StartEvent();                       // ORIG T 0x080c9a86
    virtual void EndEvent();                         // ORIG T 0x080c9d30
    virtual void StartEvent(Word_Param param);       // ORIG T 0x080c9b68
    virtual void StartAction();                      // ORIG T 0x080c9bb8
    virtual void dailyresetData();                   // ORIG T 0x080c9d82

    void* getBingoSystem();                          // ORIG T 0x080c9fea
    void sendOneMatchLineReward(CUser& user);        // ORIG T 0x080ca3d4
    void sendMaxMatchLineReward(CUser& user);        // ORIG T 0x080ca622
    int reqBingoQuizMark(CUser& user, int quizValue); // ORIG T 0x080ca262
    int reqBingoMark(StackableAction::input_param& param); // ORIG T 0x080c9ff6

    boost::shared_ptr<CBingo> m_bingo;   // +0x0c
};

#endif  // DNF_GAME_BINGOEVENT_H_
