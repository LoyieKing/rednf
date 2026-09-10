void CUser::_update_charac_stat(bool add)
{
    // [b28d-47] P0：ORIG 0x086646c8 中 secondGrow/firstGrow/characTable/job 全程
    // 寄存器直传（movsbl %al,%esi/%ebx、mov 0x14(%eax),%edi，无栈槽往返）⇒ 原码是
    // 单表达式右值，仅 stat 具名占 -0x1c。禁具名中间局部（每处 +2~4 条槽往返）。
    // 权威成员访问 m_characList->m_characs[job]（reinterpret_cast 下标使 4.4 多发
    // `mov 0x14(%eax),%eax; mov %eax,%edi`；ORIG 是 `mov 0x14(%eax),%edi` 单条）。
    const _Additioal_info* stat =                          // +0x52 call get_growtype_stat(job*0x7dc 行)
        static_cast<const _Additioal_info*>(
            G_CDataManager()->m_characList->m_characs[get_charac_job()]
                .get_growtype_stat(getCurCharFirstGrowType(),
                                   getCurCharSecondGrowType()));

    // ORIG：getCurCharacAddInfoW 在 add/subtract 两分支各自重调（禁缓存，P183/P48）。
    if (add)                                               // +0x5a cmpb param
        static_cast<_Additioal_info*>(getCurCharacAddInfoW())
            ->add(*stat);                                  // +0x75 _Additioal_info::add 0x08146300
    else
        static_cast<_Additioal_info*>(getCurCharacAddInfoW())
            ->subtract(*stat);                             // +0x91 _Additioal_info::subtract 0x08693d04
