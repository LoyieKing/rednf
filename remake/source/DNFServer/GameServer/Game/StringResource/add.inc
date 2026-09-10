bool StringResource::add(int id, const CharString& value)
{
    // ORIG @0x8ad47d0：insert(std::pair<int, CharString>(id, value)) —— 临时
    // 值 pair 直构（C1IRiRKS0_ 引用实参形）→ EOS_ 右值转 pair<const int,...>
    // → insert。make_pair 会先物化 make_pair 调用改变装载序（P61/P81）。
    return m_strings.insert(std::pair<int, CharString>(id, value)).second;
