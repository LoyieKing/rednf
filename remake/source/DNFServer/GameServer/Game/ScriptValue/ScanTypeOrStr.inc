bool ScanTypeOrStr(bool& isType, std::string& s, bool consume); // 0x88bc9bf

// [b28d-ScriptValue P211 档①] 本 TU 镜像声明（定义侧在 GameScriptEnumHelpers.cpp；
// 返回类型不入 mangled、ABI 同：eax 传值、值域非负）。故本 TU 不 include 该权威头，
// 以免 int/unsigned 冲突声明。ORIG 调用点实证：
//   getChangeStatusType 0x088bda7b `call; mov %eax,%edx; mov 0x8(%ebp),%eax;
//     mov %edx,(%eax)` = RHS 值先 + eax→edx 中转 ⇒ 字段(int) 与 callee 返回异型（P211）。
//   getTargetToEnumValue 0x088bd76b `mov $0x0,%edx; mov %eax,-0x78(%ebp);
//     mov %edx,-0x74(%ebp); fildll` = 零扩展 64 位（int 版被 4.4 前端折叠成 fildl）。
unsigned int getChangeStatusType(std::string& s);      // 0x8892b0d
unsigned int getTargetToEnumValue(std::string& s);     // 0x88c2c34
