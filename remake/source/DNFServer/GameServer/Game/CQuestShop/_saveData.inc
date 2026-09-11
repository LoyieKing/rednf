// ---- ORIG T 0x85efb66 size 0x20e：_saveData 虚槽 6（入 DB_Q）--------------
bool CQuestShop::_saveData(CUser* user)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("QuestShop.cpp", 0x21b),
                       true);                       // ORIG ctor(stream,1)
    (*guard)->operator<<(0x279);                    // ORIG de→ls（P132 operator* 链）
    (*guard)->operator<<(static_cast<int>(user->GetUID()));
    // ORIG GetInBuffer<SIG_QUEST_SHOP_DATA> size 0x155；SIG_QUEST_SHOP_DATA
    // 即 0x155 双实例结构（SigTypes_D.h），_saveData 直接装载。
    SigQuestShopLoadBlock* sig =
        guard.operator->()->GetInBuffer<SigQuestShopLoadBlock>(); // ORIG pt→GetInBuffer（P103）
    if (sig == 0)
        return 0;                                   // ebx=0
    memset(sig, 0, 0x155);
    SigQuestShopLoadBlock* view = sig;              // -0x10（ORIG sig→view 两步）
    view->m_main.m_field0 =
        static_cast<unsigned int>(user->get_charac_no(-1)); // mov %edx,(%eax)
    view->m_main.m_qp = static_cast<unsigned int>(getQp());
    view->m_main.m_initCount = static_cast<unsigned int>(getInitCount());
    view->m_main.m_questPiece = static_cast<unsigned int>(getPiece()); // 0xa6
    // b28d-36（修法 P13/P28）：ORIG 0x085efc7e 段先 end()→-0x20、后 begin()→-0x24，
    // 循环 cond 只重调 operator!=（不重算 end）⇒ 原码缓存 end 迭代器；每轮 end()
    // 版多一个 -0x18 临时槽并把 guard 顶到 -0x20（ORIG -0x1c）。
    std::map<unsigned char, short>::iterator itEnd = m_buyingStatus.end();
    for (std::map<unsigned char, short>::iterator it =
             m_buyingStatus.begin();
         it != itEnd; ++it)
    {
        const unsigned char key = it->first;        // -0xb
        const unsigned short value = static_cast<unsigned short>(it->second);
        if (it->first > 0x4c)                       // cmp $0x4c seta → skip
            continue;
        view->m_main.m_statusCounts[key] = value;   // mov %cx,0xc(key*2)
    }
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard); // DB_Q @GlobalData
    return 1;                                       // ebx=1
}