// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + GlobalData.cpp
// StackableAction::Action：ORIG C1 T @0x0827f6d8 / D1 T @0x0827f760 /
// init_function_type T @0x0827f7c8 / check T @0x0827fa26 / run T @0x0827fa9a /
// proc_bingo T @0x0827fb14 / proc_addFatigue T @0x0827fbd6 / proc_lotteryCount T @0x0827fb86。
// 外层 StackableAction 为 namespace → 本头独立以 namespace 包裹。
// has_def=1：复制定义自 GlobalData.cpp:766（input_param 输入参数结构 + class Action，
// boost::noncopyable 基类 + 两个 map<int,boost::function> @+0x00/+0x18）。
// ⚠️ ORIG 布局含 boost::noncopyable 基类；本头复制定义未继承 noncopyable（保持与
// GlobalData.cpp 现状一致），ctor/方法体仍由 GlobalData.cpp 提供。
#ifndef DNF_GAME_ACTION_H_
#define DNF_GAME_ACTION_H_

#include <map>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace StackableAction
{

// ORIG：输入参数结构（+0x08 CUser*、+0x0c CItem* 为 proc_* 实际使用字段）。
struct input_param
{
    char m_pad0[8];    // +0x00（未还原）
    class CUser* m_user;     // +0x08
    class CItem* m_item;     // +0x0c
};
class Action : private boost::noncopyable
{
public:
    Action();                              // ORIG T @0x0827f6d8（体：GlobalData.cpp）
    ~Action();                             // ORIG T @0x0827f760
    void init_function_type();             // ORIG T @0x0827f7c8
    int check(int type, input_param& p);   // ORIG T @0x0827fa26【推断】返回类型 demangled 缺失
    int run(int type, input_param& p);     // ORIG T @0x0827fa9a【推断】返回类型 demangled 缺失
    int proc_bingo(input_param& p);        // ORIG T @0x0827fb14【推断】返回类型 demangled 缺失
    int proc_addFatigue(input_param& p);   // ORIG T @0x0827fbd6【推断】返回类型 demangled 缺失
    int proc_lotteryCount(input_param& p); // ORIG T @0x0827fb86【推断】返回类型 demangled 缺失

    std::map<int, boost::function<int(input_param&)> > m_map1;  // +0x00
    std::map<int, boost::function<int(input_param&)> > m_map2;  // +0x18
};

}  // namespace StackableAction

#endif  // DNF_GAME_ACTION_H_
