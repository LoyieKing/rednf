// ============================================================================
// destroy（ORIG 0x896dac6）
// [b2c-P167] ORIG 0x0896daff 调用 **_deEv**（`_ZNKSt17_Rb_tree_iteratorISt4pairIKi
// P17STExpertJobScriptEEdeEv` = operator*，返回 pair& 后 `mov 0x4(%eax),%eax` 取
// second）；旧写法 `it->second` 发 **ptEv** + `add $0x4` ⇒ 按 P167 改 `(*it).second`。
// b28d-solo-CExpertJobList 判词（AE）：P13 缓存 end()（ORIG 0x0896dada 前一次性求值、
// 条件只比较）+ P141 删手写 delete 判空（ORIG 仅 test/je 自带守卫）；42=42。
// ============================================================================
void CExpertJobList::destroy()
{
    // b28d-solo [P13：ORIG 0x0896dada 前一次性求值 end()（循环条件只比较不重调）]。
    for (std::map<int, STExpertJobScript*>::iterator it = m_jobMap.begin(),
             e = m_jobMap.end(); it != e; ++it)
    {
        STExpertJobScript* script = (*it).second;
        delete script;
    }
}