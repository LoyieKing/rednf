void User_Teleport::set_teleport_data(STTeleportData const& data)
{
    // ORIG 0x082842b8：find_from_world(data.field00)；命中则取 vill/area/pos/
    // direction 组装局部 stPartyTelePort 后 set_party_teleport。
    // ORIG 0x082842bf：target 槽先置 0（声明初始化），find 结果再赋值——
    // 两段式复现 `movl $0x0,-0xc(%ebp)`。
    CUser* target = 0;
    target = G_GameWorld()->find_from_world((unsigned short)data.m_field00);
    if (target != 0)
    {
        // ORIG 0x082842b8：局部 stPartyTelePort 不 reset（未设字段为栈垃圾），
        // 仅填 5 字段后 set_party_teleport——保持 ORIG 原样，不额外清零。
        // ORIG 0x0828433b：tp 基址 -0x16(%ebp)（4.1 紧排非 4 对齐）；4.4 恒
        // 4-对齐落 -0x18，槽偏移差 2 字节属布局伪影（N）。
        stPartyTelePort tp;
        tp.m_village = (unsigned char)target->getCurCharacVill();
        tp.m_areaIndex = (unsigned char)target->get_area(false);
        tp.m_posx = (short)target->get_posX();
        tp.m_posy = (short)target->get_posY();
        tp.m_direction = (unsigned char)target->get_direction();
        set_party_teleport(tp);
    }
    // ORIG 0x0828434a：body 尾 call 后 jmp 到 merge(nop@0828434c)——
    // 函数尾显式 return（技巧 7 尾岛）。
    return;
}