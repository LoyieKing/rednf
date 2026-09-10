int CUser::getLinkCharacNo(char index)
{
    // ORIG 0x0869736e：`movsbl -0xc,%ebx` idx 跨 get_charac_count 存活（技巧22），
    // `cmp %eax,%ebx; jge`/`cmpb $0,-0xc; jns` → mov$1/$0 物化 `test%al; je body`
    // （bad = count<=idx || idx<0，setCharacLinkBonusSetting 同族实证形）；body 内
    // `movsbl -0xc(%ebp),%eax` 从槽重读 index（P48，不用 ebx）。
    // P0/N96 去绑定：sext 左操作数跨 call 升格 %ebx（b28 fresh AE 实证形）。
    if ((static_cast<int>(index) >= get_charac_count()) || (index < 0))
        return 0;
    return m_characList[static_cast<int>(index)].m_characNo;
