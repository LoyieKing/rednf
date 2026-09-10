bool DB_LoadEtc::LoadQuestShop(SIG_LOAD_ETC* sig)
{
    // [Lane44-新判据①] ORIG 两处均为单条 `lea 0xd5a7/0xd651(%eax),%edx`（视图成员取址）；
    // 4.4 对 `reinterpret_cast<T*>(数组名)` 恒产 `add $K,%eax; mov %eax,%edx` ⇒ 用局部
    // trivial 镜像视图成员取址复现（探针实测 4.4 产单条 lea）。
    struct QuestShopView
    {
        char                   m_pad[0xd5a7];
        CHARAC_QUEST_SHOP_DATA m_main;      // +0xd5a7
        CHARAC_QUEST_SHOP_DATA m_tag;       // +0xd651（0xd5a7+0xaa）
    };
    bool ok = _LoadQuestShop(sig->m_characNo,
                             &((QuestShopView*)sig)->m_main);
    if (!ok)
    {
        // ORIG 0x08404a90：ctor 前 m_characNo 装 %ebx 跨 ctor 保活（P227 临时链）。
        cMyTrace(__PRETTY_FUNCTION__, 0x16c4, 5)(
            "DB_LoadInventory::_LoadQuestShop ERROR::charac_no(%d)",
            sig->m_characNo);
        return false;
    }

    if (sig->m_fieldC != 0)
    {
        ok = _LoadQuestShop(sig->m_fieldC,
                            &((QuestShopView*)sig)->m_tag);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x16d5, 5)(
                "DB_LoadInventory::_LoadQuestShop ERROR::tag_charac_no(%d)",
                sig->m_fieldC);
            return false;
        }
        sig->m_questShopTagLoaded = 1;           // +0xd6fb
    }
    return true;
