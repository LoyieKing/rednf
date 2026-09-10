bool CQuestShop::loadData(CUser* user, char* data)
{
    _reset();   // ORIG call *(vtable+0x1c)（CData 虚槽 7 _reset）
    // b28d-36（修法 P61/P167）：ORIG 的 data 形参非 const（char* data），
    // make_pair 第二参是非 const 左值 ⇒ 实例 `IRiRt`；旧 const 视图生成
    // `IRiRKt`（p167 EXTRA _ZSt9make_pairIRiRKtE…）。
    SigQuestShopLoadBlock* block =
        reinterpret_cast<SigQuestShopLoadBlock*>(data); // -0x10
    SigQuestShopLoadBlock* p = block;               // -0xc（ORIG data→-0x10→-0xc 两步）
    m_qp = static_cast<int>(p->m_main.m_qp);        // mov 0x4→m_qp(+0x38)
    setInitCount(static_cast<int>(p->m_main.m_initCount));
    m_piece = static_cast<int>(p->m_main.m_questPiece); // 0xa6→(+0x44)
    for (int i = 0; i <= 0x4c; ++i)
    {
        // ORIG 0x085efa8d：`mov -0x28(%ebp),%edx; mov -0xc(%ebp),%eax;
        // movzwl 0xc(%eax,%edx,2),%eax`（**i 先、base mem 直用**）——只有完整类型
        // 具名成员访问能出此序；整指针 reinterpret_cast 视图恒使 4.4 先载 base
        // （P235）。元素左值按 make_pair 实例 `IRiRt` 要求转 `unsigned short&`。
        if (p->m_main.m_statusCountsU[i] == 0)          // movzwl + test ax / je
            continue;
        // ORIG make_pair<IRiRt>(i, m_statusCounts[i]) 左值 + pair 移动 + insert
        m_buyingStatus.insert(
            std::make_pair(i, p->m_main.m_statusCountsU[i]));
    }
    if (block->m_tagFlag != 0)                      // movzbl 0x154(%eax)（ORIG 用 -0x10=block）
    {
        p = reinterpret_cast<SigQuestShopLoadBlock*>(
            &block->m_tag);   // ORIG SetTag 实参从 block(-0x10) 算 +0xaa 经 -0xc 中转
        SetTagCharacQuestShopData(
            reinterpret_cast<CHARAC_QUEST_SHOP_DATA*>(p));
    }
    if (user->getCurCharacLastPlayTick() <=
        static_cast<long>(0x500723cf))              // cmp/setle +0x12a
        clearQP(user);
    return 1;                                       // +0x139
