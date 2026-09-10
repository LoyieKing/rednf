void CWorldDrop::generateRandomItem(stGenerateRefData_t const& ref,
                                    stGenerateResult_t& result)
{
    // [b28d-solo-WongWorkDrop_A] ORIG 形：具名指针局部 ctx = &ref+0x34
    //（-0x20 槽），ctx[0]/ctx[1] 多次直读；threshold 直接 prod/100（无负数
    // 特判分支，ORIG 无 js）；pick 复用 roll 槽；push 展开为内联体。
    // ctx 基址取具名字段 m_field34（§4.3/P82；+0x35 = m_field35 字节视图）。
    StRefCtxView const* ctx =
        reinterpret_cast<StRefCtxView const*>(&ref.m_field34);
    int roll = m_random.generateNumber(0, 100000);     // CGenerateRandomNumber(this+8)

    if (ctx->m_level > 199)                            // ORIG movzbl cmp $0xc7 ja
        return;

    WorldDropInfo* slot = 0;                           // ORIG 预清 movl $0,-0x18
    if (ctx->m_type == 1)                              // warArea 分支
        slot = &m_warAreaDrop[ctx->m_level];
    else
        slot = &m_worldDrop[ctx->m_level];
    // threshold = first × dropRate / 100（ORIG imul 0x51eb851f 魔数除法）
    int threshold = (slot->m_first * ref.m_dropRate) / 100;
    if (roll > threshold)                              // jg 退出
        return;

    roll = m_random.generateNumber(1, slot->m_total);  // 复用 roll 槽
    int acc = 0;                                       // local_14
    for (int i = 0; static_cast<int>(slot->m_chunks.size()) > i; ++i)
    {
        acc += slot->m_chunks[i].m_rate;               // 元素+0x04 权重
        if (roll < acc)
        {
            Inven_Item inv;                            // 元素+0x00 idx
            inv.m_addInfo = slot->m_chunks[i].m_itemIdx;
            G_CDataManager()->m_itemList->create_item(
                static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
            result.m_dropItems.push_back(inv);
            return;
        }
    }
