void CRandomOptionItemHandleTest::test_reset() {}      // ORIG 纯 ret
// ============================================================================
// CRandomOptionItemHandleTest::_get_auction_id —— ORIG T 0x085f3cc4
// （OrigItemGen20 批；报告 random_option__CRandomOptionItemHandleTest/_get_auction_id.md）。
//   缓存命中判定为 ORIG 单次 or+test（mov this+4→edx; mov this→eax; or; test; je，
//   gcc4.1 对 a==0&&b==0 的德摩根合并；gcc4.4 不合并，故显式 or 保持形态）。
//   空缓存走 DB：GetDBHandle(s_db_mgr, 0xc, 0) → set_query("seLect max(auction_id)…")
//   → exec(true)/fetch()/is_valid_data(0)/get_longlong(0, this) 四级成功链，
//   任一级失败返回 0（ORIG call 后 xor $1/test/je + mov $0,%eax/%edx）。
// ============================================================================
