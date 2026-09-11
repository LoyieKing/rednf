// ORIG T 0x08a9d81a：m_type == type。
// [b28d-D66 修法] P248 档②：字段真型是 enum T（不是 unsigned int）。字段 unsigned 时
// 4.4 产 `mov (%eax),%edx; mov 0xc(%ebp),%eax; cmp %eax,%edx`（DIFF 9）；StrikerScript.h
// 的 m_type 改 enum T 后 4.4 复现 ORIG 的 `mov (%eax),%eax; cmp 0xc(%ebp),%eax` 同型
// enum==enum mem 直比形（探针 /tmp/d66_probe4.cpp A/B 双结构 + setType 逐字节不变）。
bool StrikerDirectionType::isType(T type)
{
    if (m_type == type)
        return true;
    return false;
}