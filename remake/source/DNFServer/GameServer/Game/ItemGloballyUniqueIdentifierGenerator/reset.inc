void itemGloballyUniqueIdentifier_t::reset()
{
    // ORIG 0x85139e8：循环变量为 char（movsbl 读/addb 自增/cmpb $9 判界）。
    // [b28c-33 判词 NEAR-N4] ORIG 写侧为 `movsbl i,%eax; mov this,%edx; movb $0,
    // (%edx,%eax,1)`（索引先+scaled 直存）；4.4 对 `(char*)this[i]` 恒折
    // `mov this,%edx; movsbl i,%eax; lea (%edx,%eax,1); movb $0,(%eax)`（+1 条）。
    // 探针 l33d f1..f5：仅「成员数组」形出 scaled，本类无 +0 起 char[10] 成员
    // （需 union 视图改头，收益 1 条，未做）。15/16。
    for (char i = 0; i <= 9; ++i)
        reinterpret_cast<char*>(this)[i] = 0;
