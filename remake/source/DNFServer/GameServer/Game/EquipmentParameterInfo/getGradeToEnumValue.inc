int getGradeToEnumValue(std::string& s);               // 0x89bcd5b
// [b28d-solo-EquipmentParameterInfo] P211 档① 局部镜像声明旋钮：ORIG 调用点
// （getStatusType 0x089a74dc、ScriptValue scanChangeStatusData 0x088bda54）均为
// `mov %eax,%edx; mov this,%eax; mov %edx,(mem)` = RHS 值先 + eax→edx 中转；
// 4.4 对「int 目标 ← int callee」恒折 LHS 地址先（探针 /tmp/b28d_epi/p1.cpp g1 vs
// g4，p5.cpp gu1：仅当 callee 返回类型与 int 异型才产 RHS-first）。返回类型不入
// mangled、ABI 同（eax 传值、值域非负），故本 TU 镜像声明改 unsigned int 复现
// ORIG 形；本文件 `(float)` 消费点同步加 `(int)` 内层 cast 保持 fildl（p5.cpp fu1）。
unsigned int getChangeStatusType(std::string& s);      // 0x8892b0d（定义侧返回 int，ABI 同）
