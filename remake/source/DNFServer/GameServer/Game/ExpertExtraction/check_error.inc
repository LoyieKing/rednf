// ============================================================================
// CExtraction::check_error —— ORIG T 0x084a2a5e（报告 check_error.md）
//   PVP 频道拒 0x60；职业不符 0x13；材料1 必须存在且可堆叠（0x11）；
//   材料2（被提取装备）：非头像/非生物/类型合法（0x11）、稀有度==2（0x8c）、
//   已鉴定（0xae）、可分解（0x17）、非绑定（0x11）、未锁（0xd5）、
//   安全保护 CheckItemRarity（0x28）返回其错误码。
// ============================================================================
// [b28d-solo-ExpertExtraction] AE 修复：attachType 改单 case switch，复现 ORIG
//   `mov -0x10(%ebp),%eax; cmp $0x2,%eax; jne`（P286/P209；原 `if` 恒折 cmpl mem 直比）。
//   残余 N（P75 跨版本实参装载调度）：CheckItemRarity 末段 ORIG 4.1 序为
//   `c2→%edx; s_pSecuProtectionField→%eax; &item2→%ecx`，4.4.7 恒
//   `this→%eax; &item2→%edx; c2→%edx`；cast/(CUser*)/reinterpret_cast receiver/
//   单行合并 4 形实测均不翻。p167 17/17 CLEAR（语义完整，gap=0）。
int CExtraction::check_error(CUser* user, char a, short b, short c)
{
    if (G_GameWorld()->IsPVPChannel())
    {
        return 0x60;
    }
    if (user->GetCurCharacExpertJobType() != (int)a)
    {
        return 0x13;
    }
    // [b23S3C] P48 去缓存 + P0 律：ORIG 0x084a2a74/0x84a2b47 两次独立 call
    // getCurCharacInvenR、返回值免槽直落实参（`mov %eax,0x4(%esp)`），原码无
    // invenR 具名局部（该局部使 c1/c2 槽整体下移 4，与 ORIG -0x18/-0x14 错位）。
    Inven_Item item1 = user->getCurCharacInvenR()->GetInvenSlot(1, (int)b);
    CItem* c1 = G_CDataManager()->find_item(item1.m_addInfo);
    if (c1 == 0)
    {
        return 0x11;
    }
    if (!c1->is_stackable())
    {
        return 0x11;
    }
    Inven_Item item2 = user->getCurCharacInvenR()->GetInvenSlot(1, (int)c);
    CItem* c2 = G_CDataManager()->find_item(item2.m_addInfo);
    if (c2 == 0)
    {
        return 0x11;
    }
    if (c2->IsAvatarItem())
    {
        return 0x11;
    }
    if (c2->IsCreatureItem())
    {
        return 0x11;
    }
    if (c2->GetItemType() == 0xb)
    {
        return 0x11;
    }
    if (c2->get_rarity() != 2)
    {
        return 0x8c;
    }
    if (!item2.m_amp.isIdentified())
    {
        return 0xae;
    }
    if (c2->IsImpossibleDisjoint())
    {
        return 0x17;
    }
    // [b23S3C] P0 律：ORIG 0x084a2c6b `mov %eax,-0x10(%ebp)` + 回读比较 ⇒ 具名
    // attachType 局部（缺它整帧少 4 字节：c1/c2 落 -0x14/-0x10、两个 Inven_Item
    // sret 结构落 -0x51/-0x8e，与 ORIG -0x55/-0x92 全帧错位）。
    int attachType = c2->GetAttachType();
    switch (attachType)
    {
    case 2:
        return 0x11;
    default:
        break;
    }
    if (user->CheckItemLock(1, (int)c))
    {
        return 0xd5;
    }
    int r = GlobalData::s_pSecuProtectionField->CheckItemRarity(
        user, (SECURITY_PROTCTION)0x28, c2, &item2);
    if (r != 0)
    {
        return r;
    }
    return 0;
}