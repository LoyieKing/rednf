bool WideString::isuspace(wchar_t ch)
{
    // ORIG 0x08adca62：Unicode 空白白名单。
    if ((((((ch == 9) || (ch == 10)) || (ch == 0xb)) ||
          (((ch == 0xc || (ch == 0xd)) || ((ch == 0x20 || ((ch == 0xa0 || (ch == 0x2000)))))))) ||
         ((ch == 0x2001 ||
          ((((((ch == 0x2002 || (ch == 0x2003)) || (ch == 0x2004)) ||
             ((ch == 0x2005 || (ch == 0x2006)))) ||
            ((ch == 0x2007 || ((ch == 0x2008 || (ch == 0x2009)))))) || (ch == 0x200a)))))) ||
        ((((ch == 0x200b || (ch == 0x202f)) || (ch == 0x205f)) ||
          ((ch == 0x3000 || (ch == 0xfeff))))))
    {
        return true;
    }
    return false;
