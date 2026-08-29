// Action.cpp — StackableAction::Action 独占实现（run/check/~Action）。
// ORIG: check 0x0827fa26 / run 0x0827fa9a / D1 0x0827f760。
// 类定义在 Action.h（共享头，BingoEventLink4.cpp include）；GlobalData.cpp 持有
// 该 TU 内私有同布局视图（含 C1/proc_*/init_function_type 定义），本文件不触碰。
// check 查 m_map1（+0x00）、run 查 m_map2（+0x18）：find(type) → 未命中返 0，
// 命中则调 boost::function operator()(p)。
#include "Action.h"

namespace StackableAction
{

int Action::check(int type, input_param& p)
{
    std::map<int, boost::function<int(input_param&)> >::iterator it = m_map1.find(type);
    if (it != m_map1.end())
    {
        return it->second(p);
    }
    return 0;
}

int Action::run(int type, input_param& p)
{
    std::map<int, boost::function<int(input_param&)> >::iterator it = m_map2.find(type);
    if (it != m_map2.end())
    {
        return it->second(p);
    }
    return 0;
}

Action::~Action()
{
}

}  // namespace StackableAction
