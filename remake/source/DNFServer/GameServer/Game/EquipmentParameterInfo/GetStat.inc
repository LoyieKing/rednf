bool GetStat(std::vector<float>* v)
{
    // ORIG 0x89915d4 体尾 `call GetStatValue; leave; ret`：无显式 return，
    // 返回值=GetStatValue 在 %eax 的残留（调用方 @0x89bf56f 直接 test %al）。
    // 此处如实保留该形态（体指令数不变，mangled 不含返回类型）。
    v->push_back(12.0f);
#if !remake_verify
    // [bughunt15] ORIG 0x089915fa..0x08991600：`call GetStatValue; leave; ret`，返回值即
    // GetStatValue 的 %eax（调用方 0x089bf56f `test %al` 决定是否 push 到 m_428）。
    // 旧 DNF-FIX 恒 return 1 ⇒ 解析失败的 [stat] 也会被 push。
    return GetStatValue(v);
#else
    GetStatValue(v);
#endif
}