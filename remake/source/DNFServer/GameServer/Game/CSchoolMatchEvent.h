// df_game_r Game/ CSchoolMatchEvent（学院匹配 repeat 事件）。
// 依据 docs/class_func_reports/CSchoolMatchEvent/ 全部 7 份报告逐函数转写。
// 布局：CEventBase 基区（+0x00 vptr / +0x04 stADDINFO / +0x08 eventing flag），
//       +0x0c 起 std::vector<int> m_channels（ctor 0x08273ecc 基类后 vector C1；
//       AppendInfo 遍历 put_short、StartEvent(Word_Param) std::find/push_back）。
// 虚表 0x08bfd208 槽序同 CEventBase：0/1 析构、2/3 StartEvent()、4/5 EndEvent()、
//       6 StartEvent(Word_Param)、7 StartAction、8 AppendInfo（报告地址
//       0x08273ecc/0x08273f1a/0x08273f7c/0x08273f9a/0x08273fec/0x0827423c/
//       0x082742a8）。
#ifndef DNF_GAME_CSCHOOLMATCHEVENT_H_
#define DNF_GAME_CSCHOOLMATCHEVENT_H_

#include <vector>

#include "CGuildServerProxy.h"  // CEventBase / Word_Param（StartEvent(Word_Param) 按值）

class PacketGuard;

class CSchoolMatchEvent : public CEventBase
{
public:
    CSchoolMatchEvent();                               // 0x08273ecc
    virtual ~CSchoolMatchEvent();                      // D1 0x08273f1a / D0 0x08273f7c

    virtual void StartEvent();                         // 0x08273f9a（vtable+0x10）
    virtual void EndEvent();                           // 0x0827423c（vtable+0x14）
    virtual void StartEvent(Word_Param param);         // 0x08273fec（vtable+0x18）
    virtual void AppendInfo(PacketGuard& packet);      // 0x082742a8（vtable+0x20）

    std::vector<int> m_channels;   // +0x0c 学院频道号表
};

#endif  // DNF_GAME_CSCHOOLMATCHEVENT_H_
