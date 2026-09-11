// ----------------------------------------------------------------------------
// setValue(int index, const char* value)（0x8ab26a8）
//   越界跳过；否则重新 addString(key) 与 addString(value)（保持存储内一致性），
//   entities_[index] = { getStringHashValue(newKey), newKey, newValue }。
// ----------------------------------------------------------------------------
void StringDictionary::setValue(int index, const char* value)
{
    // [b28c-11] AE：守卫改正向 `if (index >= 0 && index < size)` 复现 ORIG
    // js/jle/mov$1/jmp/mov$0/test/je 物化链；StringEntity e = {} 三清零先于 hash。
    // 59/59 条全等。
    if (index >= 0 && index < static_cast<int>(entities_.size()))
    {
        const char* newKey = addString(entities_[index].key);
        const char* newValue = addString(value);

        StringEntity e = {};
        e.hash = getStringHashValue(newKey);
        e.key = newKey;
        e.value = newValue;
        entities_[index] = e;
    }
}