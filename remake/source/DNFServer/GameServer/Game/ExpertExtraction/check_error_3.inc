// ============================================================================
// CDollControllerExtraction::check_error —— ORIG T 0x084a43aa
// b28d-54 判词：同 CAlchemistExtraction::check_error（ORIG 尾部槽往返 ⇒ 具名局部）。
// ============================================================================
int CDollControllerExtraction::check_error(CUser* user, char a, short b, short c)
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