bool CEventScriptMng::is_eventing(int key)
{
    // ORIG 0x810ba36：正条件包裹（js/jg 均跳尾部 return 0），
    // operator[] 后直虚调用（P140：ORIG 无判空，0x810ba36 段
    // call operator[] → mov (%eax),%eax → vptr+0x8 直 call *%edx）。
    if (key >= 0 && key <= 0x19c)
    {
        return m_entities[key]->is_eventing();
    }
    return false;
