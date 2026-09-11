// ----------------------------------------------------------------------------
// ORIG W 0x08697474 _ZN5CUser6GetETCEs：map 取键值后删除。
// 汇编：end()→find(key)→eq 返 0；op[](key) 取 int；erase(key)；返 val。
// ----------------------------------------------------------------------------
// [b28d-27] NEAR N93：ORIG 0x08697474；ORIG end()槽 -0x14 / find()槽 -0x10（4.1
// 先声明占深槽），4.4 恒反向（先声明浅槽）；交换声明序会改调用序（ORIG end 先、
// find 后）⇒ 封顶。p167 CLEAR（5/5），语义闭合。
// [b28d-solo-CUserMissingA2] 复验：p167 CLEAR 5/5。两形实测——①交换声明序（it
// 先/e 后）⇒ NEAR→DIFF（调用序反）；②it 包内层块作用域 ⇒ 槽序仍反。ORIG
// end=-0x14 / find=-0x10（4.1 先声明深），4.4 恒反向 ⇒ N93 封顶。
int CUser::GetETC(short key)
{
    // [b28d-solo-CUserMissingA2²] 第三形复测（P326 匿名临时内联）实测
    // NEAR→DIFF（near 1→0 但 diff 8→9，四数劣化）⇒ 已回滚，N93 封顶确认。
    std::map<short, int>::iterator e = m_mapEtc.end();
    std::map<short, int>::iterator it = m_mapEtc.find(key);
    if (it == e)
        return 0;
    int val = m_mapEtc[key];
    m_mapEtc.erase(key);
    return val;
}