bool CStaticDisposition::MoveUser(CForcedMoveCharacter* fm)  // ORIG 0x82e65ee
{
    // ORIG 0x82e65ee：`mov 0xc(%ebp),%eax; mov (%eax),%eax; mov %eax,%edx`
    // 原地两级装载 ⇒ 下标表达式直传（N96 清绑）。残余：4.4 把
    // `mov (%eax),%eax; mov %eax,%edx` 折为 `mov (%eax),%edx`——N18a 单段
    // regmove 4.4 恒折、源形不可造（探针 /tmp/x1.cpp m1），byte 封顶。
    // b28d-L3 试：下标 cast 为 unsigned 触发 4.4.6 转换中转（P211 异型化）。
    return m_mats[(unsigned int)fm->m_field00].MoveUser(fm);
}