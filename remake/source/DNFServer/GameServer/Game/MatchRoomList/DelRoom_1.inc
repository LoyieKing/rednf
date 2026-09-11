bool CMultiMatchRoomList::DelRoom(IMatch* room)
{
    char name[255] = {0};
    // b28d-L3：ORIG 0x8567c30 调 GetUserName 后 `mov %eax,%ebx` 跨 CSwitchLog
    // ctor 保活，log 的 %s 实参用该返回值（非 name 数组退化地址，0x8567c62
    // `mov %ebx,0xc(%esp)`）⇒ 具名指针局部承载返回值。
    // P231 纯 register 存储类（非 asm 绑定，P0 合规）：4.4.6 对具名指针局部落槽
    // -0xc 并连锁全帧槽平移；register 提示令值随 %ebx 保活（对齐 ORIG）。
    register char* pName = GetUserName(room, name);
    // b28d-M2S3（P139/§6.5）：同 AddRoom，ORIG 用 `_ZZ...DelRoom...__PRETTY_FUNCTION__`。
    // 复测 99 条 + 尾部 1 nop（§6.1 豁免）、仅余 2 处重定位立即数显示差 ⇒ identical 级。
    CSwitchLog log(__PRETTY_FUNCTION__, 0x1209, 0, 0);
    log("pvp@log match(%x), %s ", room, pName);

    for (std::map<int, CWaitingRoomList>::iterator it = m_roomMap.begin();
         it != m_roomMap.end(); ++it)
        it->second.erase(room);
    return true;
}