#ifndef DNF_GAME_CMYTRACENOOP_H_
#define DNF_GAME_CMYTRACENOOP_H_

// cMyTraceNoop —— 禁用的追踪器（ORIG ctor 0x822ad5a / operator() 0x822ad60，均为空）。
// MySQL.cpp 以空对象构造使用；独立 TU 提供唯一定义。
class cMyTraceNoop
{
public:
    cMyTraceNoop();
    void operator()(const char*, ...);
};

#endif  // DNF_GAME_CMYTRACENOOP_H_