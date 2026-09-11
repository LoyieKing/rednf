// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 APSystemLeafC3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string.h>

#include "APSystemTypes.h"   // CActionPointEx / _ActionPoint 权威布局
#include "CDataManager.h"    // CActionPointEtcParameter 成员声明
#include "CUserProc.h"       // APSystem::_ActionGroupIndex 完整定义

namespace APSystem
{

// ----------------------------------------------------------------------------
// _ActionPoint::_ActionPoint —— ORIG W 0x0848f5b4（无独立报告，objdump 转写；
// range [0x0848f5b4,0x0848f5d7]）
//   push %ebp; mov %esp,%ebp; sub $0x18,%esp
//   movl $0xb,0x8(%esp); movl $0x0,0x4(%esp)
//   mov 0x8(%ebp),%eax; mov %eax,(%esp); call memset@plt
//   leave; ret
// ----------------------------------------------------------------------------
#include "APSystemLeafC3/_ActionPoint.inc"
{
    memset(this, 0, sizeof(*this));   // ORIG 常量 0xb（pack(1) 后 sizeof 即 0xb）
}

// ----------------------------------------------------------------------------
// CActionPointEx::CActionPointEx —— ORIG W 0x0889566e（无独立报告，objdump
// 转写；调用者 importActionPointSystemEtcScript.md 栈对象 -0x154 同构实证）
//   call _ZN8APSystem12_ActionPointC1Ev      // 基类子对象 [+0x00,+0x0b)
//   movw $0x0,0xc(%eax)                      // +0x0c word = 0
//   movb $0x0,0xe/0xf/0x10(%eax)             // +0x0e/+0x0f/+0x10 = 0
//   call _ZNSt6vectorIiSaIiEEC1Ev @+0x14     // m_itemList 默认构造
//   movl $0x7,0x20(%eax)                     // m_weekday = 7
//   call _ZNSsC1Ev @+0x24 / @+0x28           // m_str24/m_str28 默认构造
// 平铺权威头无法表达基类关系（改布局会破坏 ScriptLeafA2 等按偏移直访的既有
// 调用方），基类清零以等价成员区 memset 复刻；vector/string 由编译器默认构造
// 自动完成（与 ORIG 同为成员构造调用）。已知差异：ORIG 中 +0x0c..+0x10 清零
// 位于 mem-initializer 阶段（先于成员构造），此处位于构造函数体（后于成员构
// 造）；最终对象状态一致。
// ----------------------------------------------------------------------------
#include "APSystemLeafC3/CActionPointEx.inc"
{
    // [b28d-34] 复核维持 DIFF：ORIG 0x0889566e 构造序（基类 call → +0xc/+0xe/
    // +0xf/+0x10 字段 → +0x14 容器 → +0x20 → +0x24/+0x28 string）与
    // APSystemTypes.h 平铺布局妥协互斥（P242 族）；语义等价。
    // [S5AradDisp 2026-09-11 落地两旋钮：ours 57→56（gap −2→−1）]
    //  ① `: m_weekday(7)` mem-init：ORIG 0x088956b0 的 `movl $0x7,0x20(%eax)`
    //     位于 +0x14 容器构造之后、+0x24 string 之前（0x088956ad/0x088956c5）；
    //     具名成员 mem-init 令 4.4 按声明序在成员构造序列内并发（此前在体内=序错）。
    //  ② `m_record.m_mapKey = 0`：ORIG 0x08895684 `movw $0x0,0xc(%eax)` 两条；
    //     原 `&m_bytes02[0x0a]` 取址式恒发 `mov this; add $0xc; movw (%eax)` 三条
    //     （P183 反向）。
    //  残差（判 DIFF 的根因，无源码解）：ORIG 首位 0x0889567c `call
    //  _ZN8APSystem12_ActionPointC1Ev` = 3 条（mov this; mov (esp); call），
    //  我方平铺布局只能 `memset(this,0,0xb)` = 5 条且在成员构造之后发射；
    //  欲同形需 .h 令 CActionPointEx 真正继承 _ActionPoint（越界，未做）。
    // [P6AuditOldN-S1 2026-09-11 负结论 · p167fix call 差集线索] 试形
    //   `new (reinterpret_cast<_ActionPoint*>(&m_field00)) _ActionPoint();`
    //   ⇒ our 56→**60**（gap −1→−5，反劣）：4.4 -O0 不内联 `operator new(size_t,void*)`
    //   （多 `movl $0xb,(%esp); call <operator new>` + `test %edx; je` null 检查），
    //   且 ctor 发射点仍在成员构造之后 ⇒ 已回滚。欲得 ORIG 3 条形需 .h 真继承
    //   （`class CActionPointEx : public _ActionPoint`）—— 属 P7 头部轴，本 TU 越界。
    // [PoolI 2026-09-11 布局正解 · FragC 同步] _ActionPoint 已是真子对象成员
    // （m_ap，packed 0xb @+0x00），C1Ev 由编译器随 mem-init 列表自动发射；
    // +0xc/+0xe/+0xf/+0x10 四写法改走 mem-init（ORIG 把 immediate store 发在
    // vector C1 之前；函数体赋值会落到成员构造之后 ⇒ 顺序反）。
    // 旧 memset(&m_field00,0,0xb) / m_record.m_mapKey=0 / m_bytes02[0x0c/0x0d/0x0e]=0
    // 随头改（成员 m_field00/m_record/m_bytes02 已移除）全部删除。
}

// ----------------------------------------------------------------------------
// IsValidActionGroupIndex —— 报告 IsValidActionGroupIndex.md 全文转写
// ORIG T 0x08894956 range [0x08894956,0x08894975]：
//   mov 0x8(%ebp),%eax; test %eax,%eax; jle L_false   // v <= 0 -> false
//   cmp $0x2d,%eax; jg L_false                        // v > 45 -> false
//   mov $0x1,%eax / L_false: mov $0x0,%eax            // else true
// 静态成员：唯一实参（_ActionGroupIndex 单 int 按值传递）即 [ebp+8]。
// ----------------------------------------------------------------------------
#include "APSystemLeafC3/IsValidActionGroupIndex.inc"
{
    return (int)group >= 1 && (int)group <= 0x2d;
}

}  // namespace APSystem
