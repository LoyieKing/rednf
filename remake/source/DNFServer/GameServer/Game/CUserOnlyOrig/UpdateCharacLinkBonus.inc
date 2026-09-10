void CUser::UpdateCharacLinkBonus(unsigned int exp, unsigned int gold,
                                  unsigned int state,
                                  ENUM_CHARAC_LINK_BONUS_STATE bonusState)
{
    // P0：ORIG 头 `mov 0x18(%ebp),%eax; mov %eax,-0xc(%ebp)`——bonusState 拷入
    // 具名局部槽，末次 operator<< 从槽重读（0x14/0x10 形参直读不受影响）
    unsigned int bs = static_cast<unsigned int>(bonusState);
    // [A06 2026-09-10 P259] ORIG 0x0865240c 无 stream 具名槽：`call Acquire` 结果
    // %eax 直落 guard ctor 实参（O11 `call`; O13 `mov %eax,0x4(%esp)`），-0xc 槽归 bs。
    // 旧具名 `Stream* stream` 多一对 store/load 并顶浅全部槽（gap 3）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1c11), true);
    // ORIG：guard.operator*()->operator<< 六处显式代理链；GetUID 传 int（选
    // operator<<(int)，否则误落 unsigned 重载——const 甄别）。
    guard.operator*()->operator<<((int)0x14c);
    guard.operator*()->operator<<((int)GetUID());
    guard.operator*()->operator<<(state);
    guard.operator*()->operator<<(exp);
    guard.operator*()->operator<<(gold);
    guard.operator*()->operator<<(bs);
    GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);

    setCharacLinkBonusExp(0);
    setCharacLinkBonusGold(0);
