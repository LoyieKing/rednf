float CArtifact::GetExperienceAmount()
{
    // [b28d-39] ORIG 0x08336fc8 帧 0x38、具名 float -0xc、返回物化槽 -0x1c；
    // 单一具名 float 局部即精确复现（旧 2×取址 double 撑帧使返回槽 -0x2c、帧 0x48，已删）。
    float amount;
    if (m_pItem == 0)
        return 0.0f;
    if (m_pItem->is_stackable())
        return 0.0f;
    // ORIG 0x8336ffc：call 结果 fstps -0xc（具名局部往返）→ 尾 `mov %eax,-0x1c;
    // flds -0x1c`（统一返回槽）。P0 槽往返 ⇒ 原码有具名 float 局部。
    amount = m_pItem->get_experience_amount();
    return amount;
}