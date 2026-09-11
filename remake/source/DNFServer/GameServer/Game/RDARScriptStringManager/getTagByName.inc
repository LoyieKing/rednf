// b28d-solo: ORIG 0x8aa59f0 尾部 `mov $0x1,%eax` + 两处 `mov $0x0,%eax`
// ⇒ 真实返回 bool（P159）；三个守卫全部正极性包裹（无 continue），
// `wcsstr(toUnicode(sv), name)` 实参内联（ORIG 无 std::wstring wv 临时）。
bool RDARScriptStringManager::getTagByName(std::vector<std::wstring>* a,
                                           std::vector<std::wstring>* b,
                                           int table, const wchar_t* name) const
{
    // ORIG 0x8aa59f0：遍历表；每条 value（去空格）若 wcsstr 包含 name 且 key 非空，
    // 则把 key/value 分别 push 进 a/b。
    if (a == 0 || b == 0)
        return false;
    // ORIG 0x8aa5a21：`setae %al; test %al,%al; je <body>; mov $0,%eax; jmp <end>`
    // ⇒ 早返形（范围失败块内联在函数中段）。
    if (static_cast<unsigned>(table) >= dictionaries_.size())
        return false;
    {
        // ORIG -0x18 槽存 operator[] 引用地址（P157/P183），循环尾每轮重调 getCount。
        StringDictionary* const& d = dictionaries_[table];
        for (int i = 0; i < d->getCount(); ++i)
        {
            const char* v = d->getValue(i);
            if (v != 0)
            {
                std::string sv = v;
                // ORIG 0x8aa59f0：仅 std::remove（未 erase，尾部空格仍保留在串中）。
                std::remove(sv.begin(), sv.end(), ' ');
                // ORIG：toUnicode(sv) 直接作 wcsstr 实参（无 std::wstring wv 临时）。
                if (wcsstr(toUnicode(sv), name) != 0)
                {
                    const char* k = d->getKey(i);
                    if (k != 0)
                    {
                        a->push_back(std::wstring(toUnicode(k)));
                        b->push_back(std::wstring(toUnicode(v)));
                    }
                }
            }
        }
    }
    return true;
}