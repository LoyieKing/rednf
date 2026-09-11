// ============================================================================
// createGlobalEffectInfo（ORIG 0x89bae94）：源为 int 三元组
// (field0, field4, rate*10000)；size 非 3 的倍数返回 0。修正了原桩错误的
// vector<char>& 形参（ORIG mangled 为 vector<stGlobalEffect_t>&）。
// ============================================================================
// [b28d-solo-CDataManagerScripts] AE：①CStackableItem.h stGlobalEffect_t m_field4→float / m_field8→int（R2 广播，sizeof/偏移/mangled 不变）；②赋值序 field0→field4(fstps)→field8 + 后置 `*it++` ⇒ 逐条对齐；ORIG 0x089bae94。
int createGlobalEffectInfo(std::vector<stGlobalEffect_t>& out,
                           std::vector<int>* source)
{
    out.clear();
    if (source->size() % 3 != 0)
        return 0;
    std::vector<int>::iterator it = source->begin();
    while (it != source->end())
    {
        stGlobalEffect_t effect;
        effect.m_field0 = *it++;
        effect.m_field4 = static_cast<float>(*it++) / 10000.0f;
        effect.m_field8 = *it++;
        out.push_back(effect);
    }
    return 1;
}