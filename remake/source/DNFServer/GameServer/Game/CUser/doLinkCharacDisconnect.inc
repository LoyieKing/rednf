// 0x08652c8e
// ORIG：联动角色断线处理。标志开启时：若数据加载/建角/删角被锁定则记错误日志；
// 否则把联动角色槽位移到列表尾（ChangeCharacSlot 两次）、清标志并刷新角色视图。
// b28d-lane5 N封顶: P11 符号扩展; N82/P112 槽号分配 | ORIG 0x08652c8e | _ZN5CUser22doLinkCharacDisconnectEv
void CUser::doLinkCharacDisconnect()
{
    // b28d-solo-CUser：callee 已 bool 化（CUser.h:2718 Main 代落）⇒ 正向包裹复现
    //   ORIG 0x08652c8e `call; test %al,%al; je <跳过 body>`（P292 表 if(f()) bool；
    //   负向 `==0) return;` 反产 `xor $1; test; jne`）。
    if (isLinkCharacDisconnectFlag())
    {
    // ORIG 0x08652cb2..0x08652cf5：**两段** `locked = locked || f()` 各发一次
    //   `mov $1,%eax / mov $0,%eax / mov %al,-0xd`（4.1 || 汇聚物化落槽，P198/P214）。
    bool locked = isLocked4DataLoad() || isLockedCreateCharac();
    locked = locked || isLockedDeleteCharac();
    if (locked)
    {
        LogManager::logFormat(1, "user.cpp",
            __PRETTY_FUNCTION__, 0x1cc1,
            "LINK_LOG : checkLinkCharacDisconnect ERROR!! m_id(%s)",
            NumberToString(get_acc_id(), 0));
        return;
    }
    // ORIG 0x08652d4e `movsbl %al,%eax; mov %eax,-0xc`（getter 本体 movzbl 0x703da
    //   0x08697408 ⇒ 源码在使用点显式 (signed char) 截断）；调用点各自
    //   `mov -0xc,%eax; sub $1,%eax` + `lea -1(%eax),%edx`（size-1 直传，勿加 cast）。
    //   P48：两 ChangeCharacSlot 各自重调 size()。
    int slotIdx = (signed char)getDisconnectLinkCharacSlotIdx();
    ChangeCharacSlot((unsigned int)(slotIdx - 1), m_characList.size() - 1);
    ChangeCharacSlot((unsigned int)(slotIdx - 1), m_characList.size() - 1);
    setLinkCharacDisconnectFlag(false);
    UpdateCharacView();
    }
}