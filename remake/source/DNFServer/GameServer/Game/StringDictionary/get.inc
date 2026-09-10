const char* StringDictionary::get(const char* key) const
{
    // [b28d-48] N86 封顶（NDIFF 47，70/70 条）：equal_range 模板临时槽 +
    // 迭代器转换构造跨版本差（probe 已按 ORIG 改 `{}` 值初始化）。
    const unsigned int h = getStringHashValue(key);
    const unsigned int bucket = h >> 24;

    // [b28d-48] ORIG 0x08ab2528 段：先 3×`movl $0`（{} 值初始化 -0x20/-0x1c/-0x18）
    // 再 `mov -0x10,%eax; mov %eax,-0x20`（probe.hash=h）——逐字段赋值形顺序反。
    StringEntity probe = {};
    probe.hash = h;
    probe.key = key;

    std::pair<std::vector<StringEntity>::iterator,
              std::vector<StringEntity>::iterator> range =
        std::equal_range(buckets_[bucket], buckets_[bucket + 1], probe);

    for (std::vector<StringEntity>::iterator it = range.first;
         it != range.second; ++it)
    {
        if (std::strcmp(it->key, key) == 0)
        {
            return it->value;
        }
    }
    return 0;
