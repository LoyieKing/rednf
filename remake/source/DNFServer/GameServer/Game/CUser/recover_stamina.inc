int CUser::recover_stamina()
{
    // ORIG 0x8657859-0x8657874：两次独立装载 `cmp $0x2,%eax; jle` / `cmp $0x5,%eax; je`，
    // 均落 0x865797d `mov $0,%eax` dead 岛后 fall epi（P0/N96：dead eax 置零与
    // 尾 dead 读为 4.1 寄存器遗留，4.4 无源码等价发射，去绑定后记 N 封顶）。
    if (m_chState <= 2 || m_chState == 5)
        goto RET0;
    {
        if ((bool)(getCurCharacR()->m_stamina <= 0x63))
        {
            addCurcharacStamina(9);
            // ORIG 0x86578b9-0x86578c0：cmp $0x64,%al; seta 物化。
            if ((bool)(getCurCharacR()->m_stamina > 0x64))
            {
                setCurCharacStamina(0x64);
            }
            PacketGuard pg;
            pg.put_header(0, 0x4);
            pg.put_byte(getCurCharacR()->m_stamina);
            pg.finalize(true);
            Send(pg);
        }
        // ORIG 0x8657969-0x8657978：pg dtor merge 后 dead 读（技巧 16 原地双零扩展）；
        // 两分支共落。
        // ORIG 0x8657969 尾：call getCurCharacR + `movzbl 0x75(%eax),%eax;
        // movzbl %al,%eax` = 成功路返回 stamina 字段（第二 movzbl 为 4.1 int
        // 返回截断产物，4.4 单段零扩展，残差 1 条记跨版本 N）。
        return getCurCharacR()->m_stamina;
    }
    goto RET0;   // 不可达守卫镜像（保持 ORIG 块序：成功 fall 在前、mov$0 岛在后）
RET0:;
    return 0;
