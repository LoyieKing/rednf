void SecurityRestrictPolicy::getRestrictValue(Taiwan::SigLoadRestrictPolicy& policy)
{
    // b28c-L20: AE — 首字段 store 装载序由 LHS-first 翻正为 ORIG 的 RHS-first+中转
    // （`call getCode; mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,(%eax)`）：
    // InterfaceServicePolicy::getCode() 返回类型 int→RestrictCategory::Enum（头内，
    // mangled/函数体 asm 不变）。P211 双向开关：int 字段←int 返回恒 LHS 先；
    // 返回类型与字段异型才产生 ORIG 的中转 3 条形（探针 /tmp/l20b/probe2.cpp
    // f_int/f_enum；源侧加 cast 无效 = probe3.cpp f_cast_enum_field 实测仍 LHS 先）。
    // 反证字段本身为 int：Taiwan::SigLoadRestrictPolicy::set @0x0815de02 形参 int 写
    // 同字段为 LHS 先无中转。ndiff 5→0（orig=54 our=53→54=54）。
    policy.m_restrictValue = getCode();
    policy.m_count = 0;
    for (unsigned int i = 1; i < m_svc.size(); ++i)
    {
        // P44 实参即时求值：enumToString 结果不落栈槽（ORIG 直接 eax 传参）。
        strncpy(policy.m_data[i],
                SecurityRestrict::enumToString(static_cast<SecurityRestrict::Enum>(i)),
                0x2d);
        policy.m_extra[i] = static_cast<unsigned char>(m_svc[i]);
        ++policy.m_count;
    }
