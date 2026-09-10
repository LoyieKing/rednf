long long CRandomOptionItemHandleTest::_get_auction_id()
{
    // ORIG 布局：or/test/je(DB) —— 非空缓存直落返回，DB 链后置（if-else 排布）。
    // [b28c-lane17] 判词：本形已复现 ORIG 的 or/test 折叠；残余为单次 this 装载。
    // [b28d-31] 复核：字段 void*→unsigned int 同型化（P254 档③）消两段中转
    // （71→70）；残余 `mov 0x8(%ebp),%eax` this 重载（ORIG 0x085f3ccd 单次装载）
    // 四形（reinterpret 原序/交换、C cast、字段同型化）实测均不可翻 ⇒ N18a 封顶。
    if ((m_slot4 | m_slot0) != 0)
    {
        return *(long long*)this;
    }
    else
    {
        MySQL* db =
            GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_C, E_SERVER_GROUP_NONE);
        db->set_query("seLect max(auction_id) from auction_main");
        if (!db->exec(true))
            return 0;
        if (!db->fetch())
            return 0;
        if (!db->is_valid_data(0))
            return 0;
        if (!db->get_longlong(0, *(long long*)this))
            return 0;
        return *(long long*)this;
    }
