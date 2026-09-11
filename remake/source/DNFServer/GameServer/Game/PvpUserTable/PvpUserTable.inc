// ORIG GetResult/GetLeavePenalty 以 fnstcw→mov $0xc,%ah（RC=11 截断）→fldcw→
// fistpl 转 int（0x85d51bd 等实证），即 C 的 float→int 截断转换，非 ±0.5 舍入。
PvpUserTable::PvpUserTable()
{
    Clear();
}