// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 RentalMachineDimensionLeaf/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN22RentalMachineDimensionC2Ev`（ORIG 0x826f9fe）；ctor 已是首强符号。
#include <iostream>

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

#include "RentalMachineDimensionLeaf/RentalMachineDimension.inc"


#include "RentalMachineDimensionLeaf/RentalMachineDimensionImpl.inc"


#include "RentalMachineDimensionLeaf/dtor_RentalMachineDimensionImpl.inc"


#include "RentalMachineDimensionLeaf/dtor_RentalMachineDimension.inc"

