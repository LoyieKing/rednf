void AuthMobileRestrictPolicy::getRestrictValue(Taiwan::SigLoadRestrictPolicy& policy)  // 0816d9a0
{
    // b28c-L20: AE — 首字段 store 翻正同 SecurityRestrictPolicy::getRestrictValue：
    // getCode() 返回类型 int→RestrictCategory::Enum（P211 异型 ⇒ RHS 先+中转，
    // 探针 /tmp/l20b/probe2.cpp）。orig=54 our=53 gap=1 → 54=54 ndiff 0。
    policy.m_restrictValue = getCode();   // ORIG call 816ee12
    policy.m_count = 0;
    for (unsigned int i = 1; i < m_svc.size(); ++i)
    {
        // ORIG 0816d9cc：enumToString 实参内联（无命名局部；P24）。
        strncpy(policy.m_data[i],
                MobileRestrict::enumToString(static_cast<MobileRestrict::Enum>(i)), 0x2d);
        policy.m_extra[i] = static_cast<unsigned char>(m_svc[i]);
        ++policy.m_count;
    }
