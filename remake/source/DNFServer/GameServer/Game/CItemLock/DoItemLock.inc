// ORIG 0x8542192：物品未带锁号时从空闲池取出一个号，写 {unlockSec=now,
// state=1} 入 map；alter 后把新号写回物品并发请求包；日志 reason=0。
// [b28d-solo-CItemLock] 本轮修法：insert 实参改内联直构（无具名 value；槽序
// make_pair/-0x18 → pair/-0x24 → insert sret/-0x2c 与 ORIG 一致）。既有权宜：
// 删多余 memset、具名 uchar lock、m_state 先于 m_unlockSec 赋值、删 empty()。
// [b28d-solo-CItemLock] VERDICT: AE —— 内联 insert 直构实参 + 内联 history receiver。
int CItemLock::DoItemLock(CUser* user, int space, int slot)
{
    Inven_Item* item =
        user->GetCurCharacInventoryRef((unsigned short)space,
                                       (unsigned short)slot);
    if (item == 0)
        return 0;
    // [b28d-41] ORIG 0x85421d3：`call GetLock; mov %al,-0x2d; movzbl -0x2d,%eax;
    // test %al,%al; je` ⇒ 具名 uchar 局部（P0 槽往返）；内联两次调用形产 setne
    // 归一 + 帧 +0x10。
    unsigned char lock = item->m_amp.GetLock();
    if (lock != 0)
        return 0;
    // ORIG 0x85421f6 无 empty() 调用，直接 top()→pop()（P171：ORIG 不写即删）。
    lock = m_lockStack.top();
    m_lockStack.pop();

    stItemLockInfo info;
    // ORIG 0x854221a 顺序：C1Ev 清零后先 `movb $0x1,-0x34`（m_state）再 getCurSec
    // → `mov %eax,-0x38`（m_unlockSec）；无显式 memset（ctor 自身逐字节清零）。
    info.m_state = 1;
    info.m_unlockSec = GlobalData::s_systemTime_.getCurSec();

    // [b28d-solo-CItemLock] 内联直构实参（同 PushItemLock 判据）：ORIG
    // 0x854222d make_pair@-0x18 → converting ctor@-0x24 → insert(const&) sret@-0x2c。
    m_lockMap.insert(
        std::pair<std::map<unsigned char, stItemLockInfo>::key_type,
                  std::map<unsigned char, stItemLockInfo>::mapped_type>(
            std::make_pair(lock, info)));

    alter();
    item->m_amp.SetLock(lock);
    UpdateItem(user, item);
    SendRequestItemLock(user, space, slot);

    // [b28d-solo-CItemLock] 内联 receiver（§4.3 跨 TU 隔离例外见顶注）：ORIG
    // 0x85422e3 无 history 具名槽，`mov 0xc(%ebp),%ecx; add $0x79700,%ecx` 在
    // item 字段实参之后求值（P0/P52 内联形）。
    user->m_historyLog.ItemLockUsingLog(0, space, slot, item->m_addInfo,
                           item->m_addInfo2);
    return 1;
}