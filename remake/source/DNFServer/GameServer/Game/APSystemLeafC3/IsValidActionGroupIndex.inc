// ----------------------------------------------------------------------------
// IsValidActionGroupIndex —— 报告 IsValidActionGroupIndex.md 全文转写
// ORIG T 0x08894956 range [0x08894956,0x08894975]：
//   mov 0x8(%ebp),%eax; test %eax,%eax; jle L_false   // v <= 0 -> false
//   cmp $0x2d,%eax; jg L_false                        // v > 45 -> false
//   mov $0x1,%eax / L_false: mov $0x0,%eax            // else true
// 静态成员：唯一实参（_ActionGroupIndex 单 int 按值传递）即 [ebp+8]。
// ----------------------------------------------------------------------------
bool CActionPointEtcParameter::IsValidActionGroupIndex(_ActionGroupIndex group)
{
    return (int)group >= 1 && (int)group <= 0x2d;
}