CAICharacter* CAICharacterList::get(unsigned int idx)
{
    // [b28d-D13] ORIG 0x08373d2c 额外 `lea -0x18;lea -0x1c;call` 段（迭代器
    // 转换/拷贝构造）⇒ 试 const_iterator（P96 判据两形互试）。
    std::map<int, CAICharacter*>::const_iterator it = m_charMap.find((int)idx);
    // [b28d-D13] ORIG 0x08373d5a 尾块 `mov $0` 岛在最后 ⇒ 正极性 if（P220/P47）。
    if (it != m_charMap.end())
        return it->second;
    return 0;
}