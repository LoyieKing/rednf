int RDARScriptStringManager::getKeyIndex(int table, const wchar_t* key) const
{
    // ORIG 0x8aa5cba：key==0 或 table 越界 → -1
    if (key == 0)
        return -1;
    if (static_cast<unsigned>(table) < dictionaries_.size())
    {
        // ORIG 0x8aa5cba：-0x18 槽存 operator[] 返回的**引用地址**，每次使用
        // 再解引用（`mov -0x18(%ebp),%eax; mov (%eax),%eax`）⇒ 用引用局部
        // 而非指针值缓存（P183/P157）；ORIG 无 getCount 具名局部（循环尾
        // `cmp -0x14(%ebp),%eax; setg` 每轮重调）⇒ 条件内联。
        StringDictionary* const& d = dictionaries_[table];
        for (int i = 0; i < d->getCount(); ++i)
        {
            const char* v = d->getValue(i);
            if (v != 0)
            {
                // ORIG：toUnicode 结果无槽往返 ⇒ 实参内联（P0）
                if (wcscmp(toUnicode(v), key) == 0)
                {
                    // ORIG：getKey 结果落 -0xc 槽后槽比零 ⇒ 具名局部（P0 槽往返）
                    const char* k = d->getKey(i);
                    if (k != 0)
                        return i;
                }
            }
        }
    }
    return -1;
