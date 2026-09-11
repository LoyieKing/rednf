// [b28d-11b] 修法：&& 链拆嵌套 if（P225 推广）79→74；残余 N：4.4 折叠条件链 + body 块序 @0x086c8008
void GameWorld::send_power_war_hp_of_vill(int vill)
{
    // verdict=AE（早退形 + pVillage 具名槽 + setl 极性翻正）
    // [b28d-solo-GameWorld] _ZN9GameWorld25send_power_war_hp_of_villEi：ORIG 0x086c8026
    // `cmp vill,%eax(size); setl %al; test; jne 尾` ⇒ 第二守卫是 `size < vill`（非
    // `vill <= size`，后者 4.4 产 setge 极性反）；四守卫均为早退形。
    if (vill == 0)
    {
        return;
    }
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
    // ORIG 0x086c804c-0x086c805b：`&m_pVill[vill]` 具名落槽 -0x10（P0），
    // 循环内每次经该槽取 m_areaCount/m_pAreas（非逐次重算 imul/lea）。
    Village* pVillage = &m_pVill[vill];
    PacketGuard packet;
    packet.put_header(0, 0xe3);
    int index = packet.get_index();
    packet.put_short(0);
    int count = 0;
    for (int i = 0; i < pVillage->m_areaCount; i++)
    {
        if (pVillage->m_pAreas[i].get_area_index() != -1)
        {
            pVillage->m_pAreas[i].make_power_war_hp_of_vill(packet, count);
        }
    }
    packet.put_short(index, count);
}