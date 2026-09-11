// ---------------------------------------------------------------------------
// add —— ORIG T 0x08ad47d0：map.insert(make_pair(id,value))，返回新增与否。
// set —— ORIG T 0x08ad48e6。
// get —— ORIG T 0x08ad4878：find != end 时 *out=&second 返回 1，否则返回 0。
// ---------------------------------------------------------------------------
bool StringResource::add(int id, const CharString& value)
{
    // ORIG @0x8ad47d0：insert(std::pair<int, CharString>(id, value)) —— 临时
    // 值 pair 直构（C1IRiRKS0_ 引用实参形）→ EOS_ 右值转 pair<const int,...>
    // → insert。make_pair 会先物化 make_pair 调用改变装载序（P61/P81）。
    return m_strings.insert(std::pair<int, CharString>(id, value)).second;
}