// ORIG 0x84029ae：game_channel / channel_occ_info 两条删除；exec 成功路径
// cMyTrace(file,0x1262/0x126a,0) 记录，失败路径返 0。
// [b28c-27] N18a：GetDBHandle 第二实参 `(ENUM_SERVER_GROUP)view->m_field4` 的
// ORIG 单步 `mov 0x4(%eax),%edx` vs 4.4 恒 `mov 0x4(%eax),%eax; mov %eax,%edx`
// 中转；字段/形参异型化与 register 形均实测不可翻（batch-19 已封顶）。
// [b28d-4] AE：P254 enum 化翻 N18a 中转：视图 m_field4 int→ENUM_SERVER_GROUP + 删 cast ⇒ 复现 ORIG 0x84029ae mov 0x4(%eax),%edx 单段直落
bool DB_DeleteChannel::DeleteChannel(SIG_DELETE_CHANNEL* sig)
{
    // 技巧21 翻声明序：4.4 先声明占远槽；ORIG 0x84029ae r(-0xd) 比
    // db(-0xc) 远 ⇒ r 先声明。
    bool r;
    // 残留 1 处（[batch-19 复核 N18a 封顶，非 blocked]）：ORIG 0x84029ae
    // `mov 0x4(%eax),%edx` 单步直落 %edx；4.4 恒经 %eax 中转双 mov
    // （register asm("edx") 探针反产 4 步链，已回退）。语义等价：
    // 只是把同一个 uint 读经 %eax 再挪 %edx，无副作用差异。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_2, ((DB_DeleteChannelView*)sig)->m_field4);
    db->set_query("deLete from game_channel where gc_no=%d",
                  ((DB_DeleteChannelView*)sig)->m_gcNo);
    r = db->exec(true);
    if (!r)
        return 0;
    cMyTrace(__PRETTY_FUNCTION__, 0x1262, 0)(
        "deLete from game_channel where gc_no=%d", ((DB_DeleteChannelView*)sig)->m_gcNo);
    db->set_query("deLete from channel_occ_info where gc_no=%d",
                  ((DB_DeleteChannelView*)sig)->m_gcNo);
    r = db->exec(true);
    if (!r)
        return 0;
    cMyTrace(__PRETTY_FUNCTION__, 0x126a, 0)(
        "deLete from channel_occ_info where gc_no=%d", ((DB_DeleteChannelView*)sig)->m_gcNo);
    return 1;
}