// 全局 ::roundFloat（ORIG W 0x0853abe5：call round 后 fnstcw/fldcw(0xc 截断)+fistpl
// 取整为 int，直接 mov %eax 返回——返回类型为 int，调用点均为 call 后直接用 eax）
int roundFloat(float v)
{
    return static_cast<int>(round(static_cast<double>(v)));
}