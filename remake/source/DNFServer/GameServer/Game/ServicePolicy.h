// ServicePolicy —— 服务策略数据容器（ORIG = std::vector<short> 派生）
// ORIG 符号（nm -S --defined-only df_game_r 核实，全弱符号 → 头内 inline 定义）：
//   W 0x0816ed2e ctor（C1/C2 同址）：vector<short>::C1 + clear
//   W 0x0816ed84 dtor（D1/D2）：vector<short>::D1
//   W 0x0816ed70 Init：vector<short>::clear
// 布局：std::vector<short>（sizeof 0x0c，c6root gcc4.4.7 实测）。
// InterfaceServicePolicy +0x0c 处为 ServicePolicy 子对象（SecurityRestrictPolicy
// ctor 0x0816da7e / setPolicyApplyValue 0x0816d490 实证）。
// ORIG 代码以 vector 成员方法直接调用（push_back(short&&)/operator[]/size），
// 故 ServicePolicy 必须公开继承 vector，而非持有 vector 成员（否则 callee 为
// ServicePolicy::push_back 等包装符号，与 ORIG 的 _ZNSt6vectorIsSaIsEE* 不符）。
#ifndef DNF_GAME_SERVICEPOLICY_H_
#define DNF_GAME_SERVICEPOLICY_H_

#include <vector>

class ServicePolicy : public std::vector<short>
{
public:
    ServicePolicy() { clear(); }   // ORIG W 0x0816ed2e（vector::C1 + clear）
    ~ServicePolicy() {}            // ORIG W 0x0816ed84
    void Init() { clear(); }       // ORIG W 0x0816ed70（vector::clear）
};

#endif  // DNF_GAME_SERVICEPOLICY_H_
