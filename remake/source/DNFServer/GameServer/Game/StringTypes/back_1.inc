wchar_t WideString::back() const
{
    // [b28d-solo-StringTypes] 归零（ORIG 0x08ad9f36）：同 CharString::back，
    // 三目复现 4.1 直判 `test;je` + 零值岛后置（P47 族）。
    return length() ? buffer_[length() - 1] : 0;
