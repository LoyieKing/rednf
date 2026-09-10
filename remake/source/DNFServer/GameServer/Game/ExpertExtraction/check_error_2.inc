int CEnchanterExtraction::check_error(CUser* user, char a, short b, short c)
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
