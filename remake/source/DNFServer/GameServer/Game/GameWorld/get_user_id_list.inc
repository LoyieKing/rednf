void GameWorld::get_user_id_list(int vill, int area,
                                 std::vector<unsigned short>& out)
{
    // ORIG 0x86c8…：五条件独立跳转岛；`cmp 0xc(%ebp),%eax; setl %al; test; jne`
    // ⇒ 历史 register bool asm("al") 物化（技巧22）；b28d-lane11 按 P0/N96 清绑；
    // village 指针落 -0xc 具名槽（技巧32）
    // verdict=AE（P198② 谓词整体内联，去具名 sizeBad）
    // [b28d-solo-GameWorld] _ZN9GameWorld16get_user_id_listEiiRSt6vectorItSaItEE：ORIG
    // 0x086c8328 `call size; cmp 0xc(%ebp),%eax; setl %al; test; jne` 无槽往返 ⇒ 具名
    // bool sizeBad 落槽（多 mov %al,-0xd + cmpb）⇒ P198② 谓词整体内联。
    if ((int)g_townScriptFileList.size() < vill)
    {
        return;
    }
    if (vill <= -1)
    {
        return;
    }
    if (vill >= MAX_VILLAGE_NUM)
    {
        return;
    }
    Village* pVillage = &m_pVill[vill];
    if (area >= pVillage->m_areaCount)
    {
        return;
    }
    if (area <= -1)
    {
        return;
    }
    pVillage->m_pAreas[area].get_user_id_list(out);
