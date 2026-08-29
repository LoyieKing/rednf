// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + docs/class_func_reports/TaiwanDBDispatcher/
// TaiwanDBDispatcher：ORIG ctor T @0x0815a134、dtor T @0x0815a176、init T @0x0815a1f8、
// getDispatcher T @0x0815a7f6、addDispatcher T @0x0815a85a、delDispatcher T @0x0815a8a0、
// 嵌套 FnDelete::operator() W @0x0815dfce（std::for_each 删除 functor）。
// dispatcher 表为 map<TaiwanInternalPack::T, TaiwanInterfaceDBDispatcher*>（ctor/dtor/
// getDispatcher/addDispatcher/delDispatcher 报告均证实，与 TaiwanInternalDispatcher 同构）。
#ifndef DNF_GAME_TAIWANDBDISPATCHER_H_
#define DNF_GAME_TAIWANDBDISPATCHER_H_

#include <map>
#include <utility>

#include "MoreTypes.h"   // TaiwanInternalPack::T

class TaiwanInterfaceDBDispatcher;   // 权威定义见 TaiwanInterfaceDBDispatcher.h

class TaiwanDBDispatcher
{
public:
    // for_each 删除 functor（ORIG W @0x0815dfce：delete item->second）
    class FnDelete
    {
    public:
        void operator()(std::pair<const TaiwanInternalPack::T,
                                  TaiwanInterfaceDBDispatcher*>& item);  // ORIG W @0x0815dfce
    };

    TaiwanDBDispatcher();   // ORIG T @0x0815a134
    ~TaiwanDBDispatcher();  // ORIG T @0x0815a176

    bool init();                                                        // ORIG T @0x0815a1f8（Ghidra C bool）
    TaiwanInterfaceDBDispatcher* getDispatcher(TaiwanInternalPack::T code);  // ORIG T @0x0815a7f6
    bool addDispatcher(TaiwanInternalPack::T code, TaiwanInterfaceDBDispatcher* disp);  // ORIG T @0x0815a85a（Ghidra C bool）
    void delDispatcher(TaiwanInternalPack::T code);                     // ORIG T @0x0815a8a0

private:
    std::map<TaiwanInternalPack::T, TaiwanInterfaceDBDispatcher*> m_dispatchers;  // ORIG 0x18 字节 map
};

#endif  // DNF_GAME_TAIWANDBDISPATCHER_H_
