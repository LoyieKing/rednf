// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// SaveServiceRestrictPolicy：ORIG 成员 dispatch T @0x0815ad3c（报告
// docs/class_func_reports/SaveServiceRestrictPolicy/dispatch.md）、
// ctor W @0x0815e06a、dtor W @0x0815e0be/~0x0815e08e。
// 成员/布局无证据，sizeof 未证实故不加 static_assert。
#ifndef DNF_GAME_SAVESERVICERESTRICTPOLICY_H_
#define DNF_GAME_SAVESERVICERESTRICTPOLICY_H_

class Stream;

#include "TaiwanInterfaceDBDispatcher.h"
class SaveServiceRestrictPolicy : public TaiwanInterfaceDBDispatcher
{
public:
    SaveServiceRestrictPolicy() throw();   // ORIG W @0x0815e06a
    virtual ~SaveServiceRestrictPolicy();  // ORIG W @0x0815e0be / D2 @0x0815e08e
    // ORIG T 0x0815ad3c _ZN25SaveServiceRestrictPolicy8dispatchEiiP6Stream（TaiwanDBDispatcher.cpp）
    virtual bool dispatch(int code, int uid, Stream* stream);
};
#endif  // DNF_GAME_SAVESERVICERESTRICTPOLICY_H_
