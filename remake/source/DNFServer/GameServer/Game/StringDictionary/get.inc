// ----------------------------------------------------------------------------
// get(const char* key) const（0x8ab2528）
//   h = getStringHashValue(key); 桶 = h >> 24;
//   pair = equal_range(buckets_[桶], buckets_[桶+1], probe{h,key,0});
//   在 [first, second) 内 strcmp(key) 命中则返回 entity.value，否则 0。
// ----------------------------------------------------------------------------
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N86 封顶）→ n1_v 新口径 AE（gap=-1，blocks=0）⇒ 旧判词作废。
const char* StringDictionary::get(const char* key) const
{
    // [b28d-48] N86 封顶（NDIFF 47，70/70 条）：equal_range 模板临时槽 +
    // 迭代器转换构造跨版本差（probe 已按 ORIG 改 `{}` 值初始化）。
    const unsigned int h = getStringHashValue(key);
    const unsigned int bucket = h >> 24;

    // [b28d-48] ORIG 0x08ab2528 段：先 3×`movl $0`（{} 值初始化 -0x20/-0x1c/-0x18）
    // 再 `mov -0x10,%eax; mov %eax,-0x20`（probe.hash=h）——逐字段赋值形顺序反。
    // [b28e-S4] ORIG 0x08ab2546~0x08ab255e 只写 probe.hash（3 条 movl $0 清零 +
    //   `mov -0x10,%eax; mov %eax,-0x20`），无 probe.key 写入 ⇒ 删该行。
    StringEntity probe = {};
    probe.hash = h;

    std::pair<std::vector<StringEntity>::iterator,
              std::vector<StringEntity>::iterator> range =
        std::equal_range(buckets_[bucket], buckets_[bucket + 1], probe);

    // [b28e-S4] ORIG 0x08ab2599 `jmp` 直落条件体（equal_range 结果原地用，无
    //   `it = range.first` 拷贝），循环体尾 `local_14 = it++`（后缀
    //   operator++(int)）⇒ 直接迭代 range.first 并用后缀 ++。
    for (; range.first != range.second; range.first++)
    {
        if (std::strcmp(range.first->key, key) == 0)
        {
            return range.first->value;
        }
    }
    return 0;
}