// [b28d-23 N·N93] it(-0x10)/e(-0xc) 双迭代器槽方向跨版本相反（4.1 先声明占深槽，4.4 相反），声明序绑死 find/end 求值序，迭代器无默认构造不可延后初始化 ⇒ N。ORIG 0x080eeca4
bool CDimensionActivationEvent::checkFreeRevivalCondition(int dungeonDiff)
{
    // [B24D判词 N] 语义正确：两次 G_CDataManager（P48 无 CSE）+m_dimensionAct.
    //   m_coinMap find/end（P96）+命中 value 判空。P167 调用点差集 6/6 CLEAR；
    //   diff=lowering。
    // ORIG 0x080eeca4：两次 G_CDataManager()（O0 无 CSE），以 dungeonDiff 在
    // m_dimensionAct.m_coinMap 中 find/end（可变 iterator，P96）；命中且 value
    // 的 +0x04 bool != true → 可免费复活（true），否则 false。分支布局照 ORIG
    // 两个独立条件直跳统一失败出口（P133），bool 返回单出口。
    // [N82/P112 封顶] ORIG 槽序 it=-0x10(深) e=-0xc 与 4.4 相反；e 赋值形引入
    // -0x1c 临时+op= 调用劣化，迭代器无默认构造不可延后初始化 ⇒ 槽互换 4 行封顶。
    // [P6AuditOldN.P6S2 2026-09-11] P339 交换声明序实测反劣：NEAR(38=38,blocks=5)
    //   → DIFF(38=38, 首块即 find/end 调用序对调 + 多 sub $0x4) ⇒ 已回滚。
    //   维持槽互换封顶结论。
    std::map<int, DimensionCoinInfo>::iterator it = G_CDataManager()->m_dimensionAct.m_coinMap.find(dungeonDiff);
    std::map<int, DimensionCoinInfo>::iterator e = G_CDataManager()->m_dimensionAct.m_coinMap.end();
    if (it != e)
    {
        if (it->second.m_field4 != 1)
            return true;
    }
    return false;
}