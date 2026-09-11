// ORIG 0x088bda1c：
//   ScanTypeOrStr(isType, s, true)；失败或 isType==true → false。
//   data.type = getChangeStatusType(s)；
//   ScanStr(&s)；失败 → false；
//   data.percent = (s == "%") ? 1 : 0；
//   ok=false；data.value = (float)ScanInt(&ok)；ok==false → false；否则 true。
// [b28d-ScriptValue P211 档①] store 方向：ORIG @0x088bda7b 为 RHS 值先 +
// `mov %eax,%edx` 中转（4 条）⇒ 字段(int) 与 callee 返回**异型**。正解 = 本 TU
// 镜像声明 `unsigned int getChangeStatusType(...)`（返回类型不入 mangled、ABI 同；
// 调用点 cast 会被前端折叠，故须改声明）。字段与全部消费段零改动。
bool scanChangeStatusData(ChangeStatusData& data)
{
    std::string s;
    bool isType = false;
    if (!ScanTypeOrStr(isType, s, true)) return false;
    if (isType) return false;
    data.type = getChangeStatusType(s);
    if (!ScanStr(&s)) return false;
    data.percent = (s == "%") ? 1 : 0;
    bool ok = false;
    data.value = (float)ScanInt(&ok);
    if (!ok) return false;
    return true;
}