// ---- readTime / uint32Time ----
// [b28d-solo P292] 返回类型 bool：ORIG 调用点 `xor $1,%eax; test %al,%al`（bool
// callee 的 `!f()`）；int 返回在 4.4 产 `test %eax; sete %al`。mangled 不含返回类型。
// b28d-solo-CCeraShopAddRestrict: AE（124=124，仅 .o 重定位显示差；P229/N104）。
bool readTime(short* out)
{
    // b28d-45 试形：ORIG 0x08abf16x 先 `movl $0,-0x10`（n）后 `movb $0,-0x11`（ok）
    // ⇒ 交换声明序对齐发射序。
    int n = 0;
    bool ok = false;
    try
    {
        out[n++] = (short)ScanInt(&ok);
        if (!ok)
            throw "start year";
        out[n++] = (short)ScanInt(&ok);
        if (!ok)
            throw "start month";
        out[n++] = (short)ScanInt(&ok);
        if (!ok)
            throw "start day";
        out[n++] = (short)ScanInt(&ok);
        if (!ok)
            throw "start time";
        // [b28d-solo P229/N104] ORIG 0x08abf2c3：正常路径尾在 `call __cxa_throw`
        // 之后立即 `mov $0x1,%ebx; jmp <函数尾>`（try 正常出口），4.4 把该物化
        // 落在函数尾共享块 ⇒ 源码须把 `return 1` 写在 try 块内。
        return true;
    }
    catch (const char* msg)
    {
        // [Fn28Gap 2026-09-11] ORIG 0x08abf2f9: movl $0x8e2d8ec,0x8(%esp) =
        // _ZZN19CerashopAddRestrict8readTimeEPsE12__FUNCTION__（r,9B="readTime\0"）
        // ⇒ 原码 __FUNCTION__。
        LogManager::logFormat(1, "../RDARScriptCeraShopAddRestrictType.cpp", __FUNCTION__,
                              37, "readTime() Load fail. %s", msg);
        return false;
    }
}