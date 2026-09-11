// [b28d-solo-RandomRentalA2 P210] empty() 定义后置于 change_option 之后：
// 4.4 对「本 TU 已见定义的 callee」精确继承 outgoing（change_option 帧塌缩
// sub $0x4），ORIG 4.1 保守 sub $0x18；定义后置使 4.4 只见声明 ⇒ 恢复保守帧
// （探针 /tmp/rr_probe4.cpp f0 实证：未知 callee 时 4.4 恒 sub $0x18）。
bool RandomOptionField::empty() const
