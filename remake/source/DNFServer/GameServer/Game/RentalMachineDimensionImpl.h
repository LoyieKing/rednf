// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + GlobalData.cpp
// RentalMachineDimension::RentalMachineDimensionImpl：ORIG 嵌套于 RentalMachineDimension
// （nm demangled: RentalMachineDimension::RentalMachineDimensionImpl::C1 @0x0826fa6c W /
//  D1 @0x0826fb0c W）。外层 RentalMachineDimension 无权威头 → 按 HdrCreate3 规则在本头
// 先声明外层再补嵌套定义。成员复制定义自 GlobalData.cpp:654 局部 struct
// （char m_data[0x104] + std::set<unsigned int> m_set @0x104，约 0x11c；
//  ORIG ctor memset m_data 清零【推断】与 PcRoom 同构）。
// ⚠️ GlobalData.cpp 当前为全局（非嵌套）struct RentalMachineDimensionImpl，符号与 ORIG
// 嵌套形式不匹配；本头不改 cpp，后续迁移时需收敛（见批次冲突汇总）。
#ifndef DNF_GAME_RENTALMACHINEDIMENSIONIMPL_H_
#define DNF_GAME_RENTALMACHINEDIMENSIONIMPL_H_

#include <set>

class RentalMachineDimension
{
public:
    class RentalMachineDimensionImpl
    {
    public:
        RentalMachineDimensionImpl();   // ORIG W @0x0826fa6c（体：memset(m_data,0,sizeof(m_data))，GlobalData.cpp）
        ~RentalMachineDimensionImpl();  // ORIG W @0x0826fb0c

        char m_data[0x104];            // +0x00
        std::set<unsigned int> m_set;  // +0x104
    };  // ≈0x11c【推断】（ORIG 总大小未由报告证实，不加断言）
};

#endif  // DNF_GAME_RENTALMACHINEDIMENSIONIMPL_H_
