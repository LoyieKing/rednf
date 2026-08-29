// ServicePolicy —— 服务策略数据容器（= std::vector<short> 包装）
// ORIG 符号（nm -S --defined-only df_game_r 核实，全弱符号 → 头内 inline 定义）：
//   W 0x0816ed2e ctor（C1/C2 同址）：vector<short>::C1 + clear
//   W 0x0816ed84 dtor（D1/D2）：vector<short>::D1
//   W 0x0816ed70 Init：vector<short>::clear
// 布局：std::vector<short>（sizeof 0x0c，c6root gcc4.4.7 实测）。
// InterfaceServicePolicy +0x0c 处为 ServicePolicy 子对象（SecurityRestrictPolicy
// ctor 0x0816da7e / setPolicyApplyValue 0x0816d490 实证）。
#ifndef DNF_GAME_SERVICEPOLICY_H_
#define DNF_GAME_SERVICEPOLICY_H_

#include <vector>

class ServicePolicy
{
public:
    ServicePolicy() : m_values() { m_values.clear(); }  // ORIG W 0x0816ed2e
    ~ServicePolicy() {}                                 // ORIG W 0x0816ed84
    void Init() { m_values.clear(); }                   // ORIG W 0x0816ed70

    std::size_t size() const { return m_values.size(); }
    short& operator[](std::size_t i) { return m_values[i]; }
    void push_back(short const& v) { m_values.push_back(v); }

private:
    std::vector<short> m_values;   // 布局 = vector<short>
};

#endif  // DNF_GAME_SERVICEPOLICY_H_
