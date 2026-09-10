AdvanceAltarData* CharacAdvanceAltarManager::getCurrentAdvanceAltarData() const
{
    // ORIG W thunk mov (%eax),%eax；NK 成员但返回非 const 指针（调用方原地改写），
    // C++ 下需显式去除 const 视图（与 ORIG 语义一致）。
        return reinterpret_cast<AdvanceAltarData*>(m_field0);
