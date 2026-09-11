// b28c-1 N77: 4.1 new T() value-init 前置 rep stos 0x18 dword，4.4 不 emit（去具名 p 后 15→13）。
// [b28d-18 N77] ORIG new(nothrow) T() value-init 前置 rep stos 0x18 dword + 临时指针槽；4.4 按 CWG434 不 emit（N77/P90） | ORIG 0x0849e102
expert_job::CExpertJobMgr::CExpertJobMgr()
{
    // ORIG 0x0849e102：首条 `movl $0x0,(%eax)` 预清槽 + new(nothrow) T()
    //（4.1 value-init 前置 rep stos 清零，4.4 按 CWG434 不 emit = N77/P90
    //  跨版本记录）+ 结果统一写回 + 成员回读判 0 后 printf。
    m_factory = 0;
    // b28c-1：ORIG 0x0849e102 尾 `mov %ebx,%eax; mov 0x8(%ebp),%edx;
    //  mov %eax,(%edx)` 直存 ⇒ 无具名中间指针（去 -0x1c 中转）。
    m_factory = new (std::nothrow) CExpertJobFactory();
    if (m_factory == 0)
    {
        printf("memory error func(%s) line(%d)",
               __PRETTY_FUNCTION__, 0x57f);
    }
}