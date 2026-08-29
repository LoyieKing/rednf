// HdrCreate3 补建：ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// DBSelectCreateDnfEventInfo：ORIG W ctor 0x0815e2a4（继承 TaiwanInterfaceDBDispatcher，
// 基类 code 0x9，证据：global_constructors_keyed_to_TaiwanDBDispatcher 报告 0815e2a4 区
// movl $0x9,0x4(%esp); call TaiwanInterfaceDBDispatcher::C1）+ vtable/typeinfo。
// makeRequest T 0x0815ba94（mangled _ZN24DBSelectCreateDnfEventInfo11makeRequestEv 无 this
// → 静态成员）；dispatch T 0x0815bb8c（待后续批次）。
#ifndef DNF_GAME_DBSELECTCREATEDNFEVENTINFO_H_
#define DNF_GAME_DBSELECTCREATEDNFEVENTINFO_H_

#include "TaiwanInterfaceDBDispatcher.h"

class Stream;

class DBSelectCreateDnfEventInfo : public TaiwanInterfaceDBDispatcher
{
public:
    DBSelectCreateDnfEventInfo() throw();   // ORIG W 0x0815e2a4
    virtual ~DBSelectCreateDnfEventInfo();
    static void makeRequest();      // ORIG T 0x0815ba94
    virtual bool dispatch(int, int, Stream*);   // ORIG T 0x0815bb8c
};

#endif  // DNF_GAME_DBSELECTCREATEDNFEVENTINFO_H_
