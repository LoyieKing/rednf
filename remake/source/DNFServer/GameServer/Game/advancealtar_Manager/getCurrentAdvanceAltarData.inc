// ---- ORIG W 0x081348c4（objdump thunk：mov (%eax),%eax）——const 版定义，
//      头内唯一声明（非 const 版已删，避免调用方产出错误 mangled）。----
AdvanceAltarData* CharacAdvanceAltarManager::getCurrentAdvanceAltarData() const
{
    // ORIG W thunk mov (%eax),%eax；NK 成员但返回非 const 指针（调用方原地改写），
    // C++ 下需显式去除 const 视图（与 ORIG 语义一致）。
        return reinterpret_cast<AdvanceAltarData*>(m_field0);
}