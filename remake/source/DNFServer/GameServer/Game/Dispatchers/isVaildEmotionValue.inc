int Dispatcher_ChangeEmotion::isVaildEmotionValue(short value)
{
    // 【batch-17 实测记录，勿再改写】三形对比（ORIG 54）：
    //   A 缓存 vector 引用 + for(it=begin(); it!=end(); ++it)      → 45（gap +9，最优）
    //   B 两迭代器先默认构造 + begin/end 各重发 G_CDataManager     → 64（gap -10）
    //   C 声明即初始化 + 双臂重发 G_CDataManager                   → 居中但差集仍缺
    // ⇒ 该函数残 9 insn 属 4.1 out-of-line STL 实例发射差（N89 族，
    //   docs/function_audit/N89_EXCLUSION*.tsv 判据），非源码可闭合；保留 A 形。
    // [b28d-solo-Dispatchers] P96：ORIG 0x081d1ebe 用**非 const** 容器 begin/end
    //   （p167 MISSING `_ZNSt6vectorIiSaIiEE5beginEv/3endEv` + `const_iterator(int*)`
    //   转换构造 C2IPiE），const 引用会走 K 版 ⇒ 改非 const 引用、迭代器仍 const_iterator。
    // [b28d-solo-Dispatchers] P183：ORIG 0x081d1ebe 两处各自完整调 G_CDataManager()
    //   （p167 MISSING `_Z14G_CDataManagerv` ×1 = tags 引用缓存省掉的那次）；
    //   P326：显式 const_iterator 转换临时复现 ORIG 的 C2IPiE 转换 ×2 + 同型 ne。
    for (std::vector<int>::const_iterator it =
             G_CDataManager()->m_emotionTags.m_tags.begin();
         it != std::vector<int>::const_iterator(
                   G_CDataManager()->m_emotionTags.m_tags.end()); ++it)
    {
        if (*it == (int)value)
            return 1;
    }
    return 0;
