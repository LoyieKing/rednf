// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + docs/class_func_reports/TaiwanInternalDispatcher/
// TaiwanInternalDispatcher：ORIG ctor T @0x081601d0、dtor T @0x08160212、init T @0x08160294、
// getDispatcher T @0x0816069a、addDispatcher T @0x081606fe、delDispatcher T @0x08160744、
// 嵌套 FnDelete::operator() W @0x08161e9a（std::for_each 删除 functor）。
// dispatcher 表为 map<TaiwanInternalPack::T, TaiwanInterfaceInterDispatcher*>（ctor/dtor/
// getDispatcher/addDispatcher/delDispatcher 报告均证实：ctor 调 map::map+clear、
// dtor 经 for_each+FnDelete 后 ~map、getDispatcher @0x0816069a 的 find/end/operator!=、
// addDispatcher @0x081606fe 的 operator[]）。
#ifndef DNF_GAME_TAIWANINTERNALDISPATCHER_H_
#define DNF_GAME_TAIWANINTERNALDISPATCHER_H_

#include <map>
#include <utility>

#include "MoreTypes.h"   // TaiwanInternalPack::T

class TaiwanInterfaceInterDispatcher;   // 权威定义见 TaiwanInterfaceInterDispatcher.h

class TaiwanInternalDispatcher
{
public:
    // for_each 删除 functor（ORIG W @0x08161e9a：delete item->second）
    struct FnDelete
    {
        void operator()(std::pair<const TaiwanInternalPack::T,
                                  TaiwanInterfaceInterDispatcher*>& item);  // ORIG W @0x08161e9a
    };

    TaiwanInternalDispatcher();   // ORIG T @0x081601d0
    ~TaiwanInternalDispatcher();  // ORIG T @0x08160212

    bool init();                                                        // ORIG T @0x08160294（Ghidra C bool）
    TaiwanInterfaceInterDispatcher* getDispatcher(TaiwanInternalPack::T code);  // ORIG T @0x0816069a
    bool addDispatcher(TaiwanInternalPack::T code, TaiwanInterfaceInterDispatcher* disp);  // ORIG T @0x081606fe（Ghidra C bool）
    void delDispatcher(TaiwanInternalPack::T code);                     // ORIG T @0x08160744

private:
    std::map<TaiwanInternalPack::T, TaiwanInterfaceInterDispatcher*> m_dispatchers;  // ORIG 0x18 字节 map
};

#endif  // DNF_GAME_TAIWANINTERNALDISPATCHER_H_
