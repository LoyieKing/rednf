// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + ResetStayTimeEvent/README.md
#ifndef DNF_GAME_RESETSTAYTIMEEVENT_H_
#define DNF_GAME_RESETSTAYTIMEEVENT_H_

class Stream;  // 前置：ORIG dispatch 形参 P6Stream

// ORIG 符号：
//   T 0x0815b76a _ZN18ResetStayTimeEvent8dispatchEiiP6Stream
//   W 0x0815e1c0 ctor（C1/C2 同址）；W 0x0815e214 dtor（D0 区）
// 返回类型 demangled 缺失，按惯例写 void【推断】。
class ResetStayTimeEvent : public TaiwanInterfaceDBDispatcher
{
public:
    ResetStayTimeEvent() throw();               // ORIG W 0x0815e1c0
    virtual ~ResetStayTimeEvent();      // ORIG W 0x0815e214
    // ORIG T 0x0815b76a _ZN18ResetStayTimeEvent8dispatchEiiP6Stream（TaiwanDBDispatcher.cpp）
    virtual bool dispatch(int code, int uid, Stream* stream);
};

#endif  // DNF_GAME_RESETSTAYTIMEEVENT_H_
