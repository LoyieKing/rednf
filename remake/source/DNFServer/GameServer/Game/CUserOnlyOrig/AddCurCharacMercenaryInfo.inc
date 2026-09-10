void CUser::AddCurCharacMercenaryInfo()
{
    // ORIG 0x0868e5a8：`test %eax,%eax; sete %al; test; jne END` ==0 物化形（技巧9）
    if (static_cast<bool>(getCurCharacR() == 0))
        return;
    // ORIG 0x0868e5bb-0x0868e5c0：再调 getCurCharacR，(unsigned short) 显式 cast
    // 单条 `movzwl 0x27(%eax),%ebx` 直落 callee-saved（技巧2；m_level 字段 short，
    // ORIG 此点 movzwl=源码 cast，CUser.h:520 注释同例）
    // probe p5/f14：register short asm("bx") 产 `movzwl mem,%ebx` 单段直落
    //（ORIG 同形；register ushort + 显式比较 cast 反生两段）；比较侧 (short)(short)
    // 复现 ORIG `cmp %ax,%bx; setl`（16 位 signed）。
    // P0/N96 去绑定（单段 movzwl→%ebx 不可造，N18a）。(short) 有符号比较
    //（P196 族：ORIG setl=signed 16 位，双 ushort cast 反产 setb 语义分叉）。
    if ((short)getCurCharacR()->m_level <
        (short)G_CDataManager()->m_mercenarySystem.m_field4)
        return;

    CHARAC_LOAD_MERCENARY info;
    info.Reset(); /* ORIG 调 Reset（+0x30/0x31=0xff、+0x32=0xa）*/
    // ORIG 0x0868e5f4/0x0868e607/0x0868e617：三字段各重调 getCurCharacR（技巧37 免槽）
    info.m_level = (unsigned short)getCurCharacR()->m_level;               // +0x04 mov %ax
    info.m_characNo = getCurCharacR()->m_characNo;         // +0x00 mov (%eax)
    strncpy(info.m_name, getCurCharacR()->m_name, 0x1d);   // +0x06 ← src charac+0x04
    info.m_avatarBonus = CalcMercenaryAvatarBonus();  // +0x32 uchar
    m_mercenaryMgr.AddCharac(info);
