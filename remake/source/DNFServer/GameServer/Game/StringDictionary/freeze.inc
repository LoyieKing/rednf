void StringDictionary::freeze()
{
    // [b28c-11] AE：probe 改 `StringEntity probe = {};`（三字段先清零再写 hash，
    // ORIG 顺序）+ 循环变量 unsigned（setbe 而非 setle，P2）。82/82 条全等。
    std::sort(entities_.begin(), entities_.end());

    for (unsigned int i = 0; i < 0x100; ++i)
    {
        StringEntity probe = {};
        probe.hash = i << 24;
        buckets_[i] = std::lower_bound(entities_.begin(), entities_.end(), probe);
    }
    buckets_[0x100] = entities_.end();
