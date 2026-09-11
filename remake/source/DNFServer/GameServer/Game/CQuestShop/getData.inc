// ---- ORIG T 0x85ef868 size 0x1c4：getData 虚槽 2（双实例导出）-------------
void CQuestShop::getData(char* out) const
{
    char* p = out;                                   // -0x18
    memset(p, 0, 0x155);                             // ORIG P68 装载序
    SIG_QUEST_SHOP_DATA* view =
        reinterpret_cast<SIG_QUEST_SHOP_DATA*>(p);   // -0x14
    view->m_main.m_qp = static_cast<unsigned int>(getQp());       // 0x4(%eax)
    view->m_main.m_initCount = static_cast<unsigned int>(getInitCount()); // 0x8
    view->m_main.m_questPiece = static_cast<unsigned int>(getPiece()); // 0xa6
    // ORIG 0x85ef868：end() 只在循环前调一次（-0x1c），cond 段仅 operator!=；
    // 且调用序 end→begin（085ef8e1 end / 085ef8f9 begin）。判定时重读 it->first
    // （第三次 ptEv，O0 无 CSE）。
    std::map<unsigned char, short>::const_iterator itEnd =
        m_buyingStatus.end();                        // -0x1c
    std::map<unsigned char, short>::const_iterator it =
        m_buyingStatus.begin();                      // -0x20
    for (; it != itEnd; ++it)
    {
        const unsigned char key = it->first;        // -0xf
        const unsigned short value = static_cast<unsigned short>(it->second);
        if (it->first > 0x4c)                       // seta → skip 写入
            continue;
        // ORIG 0x85ef93e：`movzbl -0xf,%edx; movzwl -0xe,%ecx; mov -0x14(%ebp),%eax;
        // mov %cx,0xc(%eax,%edx,2)`（值先、base 后）⇒ 必须走**完整类型具名成员**
        // 直访问（P235：reinterpret_cast 视图形使 4.4 恒 base 先载）。
        view->m_main.m_statusCounts[key] = value;
    }
    view = reinterpret_cast<SIG_QUEST_SHOP_DATA*>(
        &((SIG_QUEST_SHOP_DATA*)p)->m_tag);     // +0x0aa 段（ORIG 复用 -0x14）
    // ORIG 0x85ef97d/0x85ef99b：第二循环**复用**同一对迭代器变量（begin 返回临时
    // -0x2c → `mov 临时,%eax; mov %eax,-0x20`；end 临时 -0x2c → -0x1c），即源形为
    // 「函数级声明 + 循环前**赋值**」（P50 复活：ORIG 有独立 sret 临时槽 + 拷贝两段形；
    // 此前写成循环内新声明 ⇒ 4.4 直构省略临时，gap=-4）。
    it = m_tagBuyingStatus.begin();                  // -0x20（sret 临时 -0x2c 后拷贝）
    itEnd = m_tagBuyingStatus.end();                 // -0x1c（同上）
    for (; it != itEnd; ++it)
    {
        const unsigned char key = it->first;        // -0xb
        const unsigned short value = static_cast<unsigned short>(it->second);
        if (it->first > 0x4c)
            continue;
        view->m_main.m_statusCounts[key] = value;
    }
}