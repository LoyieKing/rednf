// ---- 迁移自 GameStubs.cpp 的 CDungeon 桩方法（ORIG 弱符号，签名已核对）----
CDungeon::CDungeon()
    : m_typeInheritance(0),
      m_riskFlag(false),
      m_partyMemberCoinLimit(0),
      m_enterEachMap(false),
      m_openEvenEnemy(false)
{
    // ORIG 0x08373f3c：初始化列表按成员声明位置赋值（vtable 后立即 +0x04，
    // 0x6bc vector 构造后 0x6c9/0x6cc/0x6d0/0x6d1），其它 POD 不初始化。
}