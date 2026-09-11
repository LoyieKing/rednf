void GameWorld::send_to_vill(int vill, PacketGuard& packet)
{
    // ORIG 0x086c7f48：je 0x86c7ffb(vill==0 岛) / setl 0x86c7f67 / jle 0x86c7f76 / jge 0x86c7f84；
    // pVillage=-0x10、i=-0xc 具名槽（0x86c7f95/0x86c7f98）；
    // COND setg 物化 0x86c7ff2、body 尾 `cmp $-1; setne %al; test; je` 0x86c7fbe-0x86c7fc3
    if (vill == 0)
    {
        return;
    }
    // b28d-lane11：P0/N96 清绑。
    // verdict=AE（P198② 内联 size 谓词 + 去具名 goOn + idxOk 内联 setne）
    // [b28d-solo-GameWorld] _ZN9GameWorld12send_to_villEiR11PacketGuard：P198② 谓词
    // 内联（ORIG 0x086c7f67 setl 物化无槽往返，具名 sizeBad 反落槽 -0x15）。
    if ((int)g_townScriptFileList.size() < vill)
    {
        return;
    }
    if (vill <= 0)
    {
        return;
    }
    if (vill >= MAX_VILLAGE_NUM)
    {
        return;
    }
    Village* pVillage = &m_pVill[vill];
    int i = 0;
    // b28d-lane11：P0/N96 清绑。
    // [b28d-solo-GameWorld] 去具名 goOn（落槽 -0x9 使 pVillage/i 槽下移 4）⇒ 直接
    // 条件恢复 ORIG pVillage=-0x10/i=-0xc；ORIG 0x086c7ff2 `setg %al; test; jne` 系
    // 4.1 槽比物化（4.4 恒 cmp/jle 直跳，P197④ 不可造）。
    for (; (pVillage->m_areaCount > i); i++)
    {
        // [b28d-solo-GameWorld] idxOk 内联：ORIG 0x086c7fbe `call get_area_index;
        // cmp $-1,%eax; setne %al; test; je`（call 结果寄存器直比 ⇒ 4.4 可出 setne）。
        if (pVillage->m_pAreas[i].get_area_index() != -1)
        {
            pVillage->m_pAreas[i].send_to_all(packet);
        }
    }
}