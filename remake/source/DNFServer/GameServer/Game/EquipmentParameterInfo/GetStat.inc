bool GetStat(std::vector<float>* v)
{
    // ORIG 0x89915d4 体尾 `call GetStatValue; leave; ret`：无显式 return，
    // 返回值=GetStatValue 在 %eax 的残留（调用方 @0x89bf56f 直接 test %al）。
    // 此处如实保留该形态（体指令数不变，mangled 不含返回类型）。
    v->push_back(12.0f);
    GetStatValue(v);
#if !remake_verify
    return 1;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}