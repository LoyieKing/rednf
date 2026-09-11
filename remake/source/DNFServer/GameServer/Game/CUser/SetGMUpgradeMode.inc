void CUser::SetGMUpgradeMode(ENUM_GM_ITEM_UPGRADE mode)
{
    // ORIG 0x822fea7-0x822fead：mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,0x711dc(%eax)
    // （this-先 + 无帧 pop ebp）。b28d-solo-CUser AE: P211 同型律——ORIG LHS-first ⇒ 字段须与
    //   形参 enum 同型；CUser.h:908 已由 Main 代落 `unsigned int`→`ENUM_GM_ITEM_UPGRADE`
    //   （探针 /tmp/probe_store.cpp：uint 字段+enum 形参 ⇒ RHS-first；enum 字段 ⇒ LHS-first）。
    //   sizeof/偏移(0x711dc)/mangled 不变。
    m_gmUpgradeMode = mode;
}