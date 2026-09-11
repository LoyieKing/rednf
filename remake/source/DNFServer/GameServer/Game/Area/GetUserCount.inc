// ---------------------------------------------------------------------------
// GetUserCount：统计 m_UsersInArea 中 state > 2 的在线用户
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 8=8；70=70）。修法：P50 先声明后赋值 + P198①
// user 预清 + **while+continue 真循环形**（取代历轮 goto 尾条件形）——4.4 对互斥分支各自
// 的 `it++` 独立发射 ppEi 并复现 4.1 的匿名 sret 槽方向（推翻旧 N93/N98「不可翻」判词）。
int Area::GetUserCount() const
{
    // [b28d-48] ORIG 086c31f2 切片：user 预清(-0x10) → it 默认构造(-0x20) →
    // begin sret(-0x2c) 赋值 → count(-0xc)=0 → do 形循环（end sret -0x1c、
    // operator++ 两分支各一 sret -0x18/-0x14）⇒ 源为 while(true)+体内 return
    // + 具名 user 预清（P198①/P50 反例复活）。
    // [b28d-48 收口] DIFF 65 → NEAR 4（长度 68=68、助记符序列全同）。组合修法：
    // P50 先声明后赋值迭代器（默认构造槽）+ P283 goto 尾条件（begin 后 jmp 底部）
    // + P28 后置 `it++`×2（ORIG 两处均 ppEi 带独立 sret）+ P198① user 预清。
    // 残余 4 = 四个 sret 临时槽号互换（ORIG it++/-0x18、end/-0x1c、it++/-0x14
    // vs 4.4 -0x1c/-0x14/-0x18）——N93 块级临时槽方向跨版本相反，不可翻。
    CUser* user = 0;
    std::map<unsigned short, CUser*>::const_iterator it;
    it = m_UsersInArea.begin();
    int count = 0;
    while (it != m_UsersInArea.end())
    {
        user = it->second;
        if (user == 0 || user->get_state() <= 2)
        {
            it++;                                    // ORIG 0x086c3268 ppEi sret -0x18
            continue;
        }
        ++count;
        it++;                                        // ORIG 0x086c3299 ppEi sret -0x14
    }
    return count;
}