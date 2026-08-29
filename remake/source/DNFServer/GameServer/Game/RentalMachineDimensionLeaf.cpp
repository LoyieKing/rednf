// ============================================================================
// RentalMachineDimensionLeaf.cpp — Rental 域叶片批次（独占 TU；CMake 由 Main 统一登记）
//
// 覆盖符号（全部经 df_game_r elf32-i386 nm 核实，参照 = build/dnf_data/
// home/template/init/df_game_r，与 other/等级补丁/80 一致）：
//   T 0x0826eafc _ZN22RentalMachineDimensionC1Ev / C2Ev
//       RentalMachineDimension::RentalMachineDimension()
//   T 0x0826eba2 _ZN22RentalMachineDimensionD1Ev / D2Ev
//   T 0x0826ec04 _ZN22RentalMachineDimensionD0Ev
//       RentalMachineDimension::~RentalMachineDimension()
//   W 0x0826fa6c _ZN22RentalMachineDimension26RentalMachineDimensionImplC1Ev / C2Ev
//       RentalMachineDimension::RentalMachineDimensionImpl::RentalMachineDimensionImpl()
//   W 0x0826fb0c _ZN22RentalMachineDimension26RentalMachineDimensionImplD1Ev / D2Ev
//       RentalMachineDimension::RentalMachineDimensionImpl::~RentalMachineDimensionImpl()
// 附带生成的 ORIG 弱符号（成员 std::auto_ptr<Impl> 实例化，nm 同址实证）：
//   W 0x0826fafe _ZNSt8auto_ptrIN22RentalMachineDimension26RentalMachineDimensionImplEEC1EPS1_
//   W 0x0826fb24 _ZNSt8auto_ptrIN22RentalMachineDimension26RentalMachineDimensionImplEED1Ev
//   W 0x0826fbee _ZNKSt8auto_ptrIN22RentalMachineDimension26RentalMachineDimensionImplEEptEv
//
// ORIG 事实（objdump 0x826eafc-0x826ec21 / 0x826fa5e-0x826fb49 逐指令转写）：
//   * Impl 为【嵌套于 RentalMachineDimension 的类】（nm mangled 前缀 22/26 证实），
//     成员 = char m_data[0x104] @+0x00 + std::set<unsigned int> m_set @+0x104
//     （sizeof==0x11c：C1 中 new $0x11c 实证）。
//   * Impl C1：先 set 默认构造（成员子对象，+0x104），再 memset(this,0,0x104)
//     （首参 this 先载 eax 再压常量，P68 形态 → 写 (char*)this）。
//   * Impl D1：仅 set 析构（用户析构空体；ORIG 有 D1 W，P71 规则保留用户析构）。
//   * 外层成员 m_impl @+0x04 为 std::auto_ptr<Impl>（C1 中 call auto_ptr C1EPS1_、
//     D1 中 call auto_ptr D1、其它方法经 ptEv 解引用——nm 三实例符号实证）。
//   * 外层 C1：IRentalMachine C1 → 设 vptr → new $0x11c → Impl C1 →
//     auto_ptr C1(&m_impl, ptr) → initLoggingEnvironment()。
//   * 外层 D1：设 vptr → auto_ptr D1 → IRentalMachine D1 → deleting 标志(0)。
//   * 外层 D0：call D1 + operator delete。
//   * vtable 槽序（继承 IRentalMachine）：0/1 虚析构、2 Rent、3 Ready、
//     4 CheckRentableItem（InstanceRentalSystem.h 权威注释）。
//   * 虚函数 Rent/Ready/CheckRentableItem 与 initLoggingEnvironment 本体在
//     GlobalData.cpp（ORIG T 同址），本 TU 只声明不定义。
//
// ⚠️ 收敛说明：GlobalData.cpp 仍持有旧版平铺定义（全局 struct
// RentalMachineDimensionImpl + 裸指针 m_impl，其 Impl mangled 与 ORIG 嵌套
// 形式不符），由 Main 收尾迁移；本 TU 自包含定义不 include 那两个冲突头。
// ============================================================================

#include <memory>
#include <set>
#include <string.h>

#include "InstanceRentalSystem.h"   // IRentalMachine / RentInput / RentOutput

class CUser;
class EventRewardItemInfo;

// ============================================================================
// RentalMachineDimension（ORIG 嵌套 Impl + auto_ptr 成员权威布局，见上）
// ============================================================================
class RentalMachineDimension : public IRentalMachine
{
public:
    RentalMachineDimension();          // ORIG T @0x0826eafc
    virtual ~RentalMachineDimension(); // ORIG D1 @0x0826eba2 / D0 @0x0826ec04

    // 槽2 Rent / 槽3 Ready / 槽4 CheckRentableItem：ORIG T（本体 GlobalData.cpp）
    virtual int Rent(CUser& user, const InstanceRentalSystem::RentInput& input,
                      InstanceRentalSystem::RentOutput& output);
    virtual bool Ready();
    virtual bool CheckRentableItem(unsigned int itemIdx);

    // ========================================================================
    // ORIG 嵌套 Impl：C1 @0x0826fa6c / D1 @0x0826fb0c（W，本 TU 提供）
    // ========================================================================
    class RentalMachineDimensionImpl
    {
    public:
        RentalMachineDimensionImpl();   // memset(this, 0, 0x104)（set 成员先构造）
        ~RentalMachineDimensionImpl();  // 空体（set 析构由编译器生成）

        char m_data[0x104];             // +0x00
        std::set<unsigned int> m_set;   // +0x104
    };  // sizeof == 0x11c

private:
    void initLoggingEnvironment();      // ORIG T @0x0826efdc（本体 GlobalData.cpp）

    std::auto_ptr<RentalMachineDimensionImpl> m_impl;  // +0x04
};

// ----------------------------------------------------------------------------
// Impl C1 —— ORIG W 0x0826fa6c（objdump 转写）
//   mov 0x8(%ebp),%eax; add $0x104,%eax; mov %eax,(%esp); call setC1Ev
//   mov 0x8(%ebp),%eax; movl $0x104,0x8(%esp); movl $0x0,0x4(%esp)
//   mov %eax,(%esp); call memset@plt; leave; ret
// ----------------------------------------------------------------------------
RentalMachineDimension::RentalMachineDimensionImpl::RentalMachineDimensionImpl()
{
    memset((char*)this, 0, 0x104);   // set 成员子对象已先于体构造
}

// ----------------------------------------------------------------------------
// Impl D1 —— ORIG W 0x0826fb0c
//   mov 0x8(%ebp),%eax; add $0x104,%eax; mov %eax,(%esp); call setD1Ev
//   leave; ret
// ----------------------------------------------------------------------------
RentalMachineDimension::RentalMachineDimensionImpl::~RentalMachineDimensionImpl()
{
}

// ----------------------------------------------------------------------------
// C1 —— ORIG T 0x0826eafc
//   call IRentalMachineC1Ev; movl $vtable,(%eax)
//   movl $0x11c,(%esp); call _Znwj; mov %eax,%ebx
//   call ImplC1Ev; mov %ebx,%eax
//   lea 0x4(%ebp 处 this),%edx; mov %eax,0x4(%esp); mov %edx,(%esp)
//   call auto_ptr<Impl>C1EPS1_
//   call initLoggingEnvironmentEv; ret（异常路径 _Unwind_Resume）
// ----------------------------------------------------------------------------
RentalMachineDimension::RentalMachineDimension()
    : m_impl(new RentalMachineDimensionImpl)
{
    initLoggingEnvironment();
}

// ----------------------------------------------------------------------------
// D1 —— ORIG T 0x0826eba2
//   movl $vtable,(%eax); lea 0x4(%eax); call auto_ptr<Impl>D1Ev
//   call IRentalMachineD1Ev; mov $0x0,%eax; test %al,%al; je; ret
// ----------------------------------------------------------------------------
RentalMachineDimension::~RentalMachineDimension()
{
}
