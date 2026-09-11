// ============================================================================
// CAlchemistExtraction::check_error —— ORIG T 0x084a3716
// b28d-54 判词：ORIG 0x084a3716 尾部 `call *%ecx; mov %eax,-0xc(%ebp);
//   mov -0xc(%ebp),%eax`（槽往返）⇒ 原码有具名局部 `e = _check_error(...); return e;`
//   （P0：ORIG 有槽往返 ⇒ 必须声明局部；直接 `return f()` 丢 store/load 对）。
// ============================================================================
int CAlchemistExtraction::check_error(CUser* user, char a, short b, short c)
{
    int e = CExtraction::check_error(user, a, b, c);
    if (e != 0)
    {
        return e;
    }
    if (!_check_need_slot(user))
    {
        return 4;
    }
    e = _check_error(user, b, c);
    return e;
}