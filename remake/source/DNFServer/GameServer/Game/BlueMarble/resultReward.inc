int BlueMarble::resultReward()
{
    Inven_Item item;
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            // b28c-1：ORIG continue 目标 = 岛1（0x080d7e6c nop+jmp），P223 多岛形。
            goto NEXT0;
        }
        item.reset();
        setResultItem(i, item);
        if (item.m_addInfo != 0)
        {
            insertItem(i, item, (eItemAddReason)0x42);
            // ORIG 0x80d7e6a：insertItem 后显式 jmp 循环尾块@80d7e70（4.1 岛形，
            // 范式24 goto 落点岛阻 4.4 fall-through 合并）。
            goto NEXT1;
        }
        // b28c-1：P223 多岛形（ORIG 0x80d7e6c nop+jmp 岛 + 0x80d7e6f nop 岛）
        //  ⇒ 条件假分支同样 goto 到相邻标签；NEXT0 岛自带 goto NEXT1 以复现
        //  ORIG 0x080d7e6d 第二条 `jmp +0x92`（标签共址会被 4.4 合并少 1 条）。
        goto NEXT2;
    NEXT0:
        ;
        continue;
    NEXT2:
        ;
    NEXT1:
        ;
    }
