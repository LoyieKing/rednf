unsigned char CUserCharacInfo::getCurEquipslotFlag()
{
    // ORIG 0x8696c23 true 路单一 `movzbl 0x1275(%eax),%eax`、false 0x8696c2c
    // `mov $0xffffffff,%eax`（dword -1）。int 返回三元/if 形均反产二段 promote
    // （IsCurCharacGhost 同款教训）→ uchar 返回 + if 形，false 保留 dword -1。
    if (m_selected)
        return m_selected->m_equipslotFlag;
    return -1;
}