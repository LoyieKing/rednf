float CStackableItem::GetExpAffectRatio()
{
    // b28c-L8：ORIG 0x08694746 = `mov 0x8(%ebp),%eax; add $0x2e8,%eax; flds (%eax)`
    // （指针经 add 推进后 flds 单元址）。旧形 `*(float*)&m_int2e8` 4.4 会折成
    // `flds 0x2e8(%eax)`；改经匿名 union float 视图 + 返回类型与字段同型即复现。
    return m_flt2e8;
