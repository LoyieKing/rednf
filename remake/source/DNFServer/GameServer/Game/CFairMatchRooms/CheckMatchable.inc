bool CFairMatch::CheckMatchable(IMatch* other)
{
    // b28d-solo-CFairMatchRooms：ORIG 槽序 myUser@-0x14 / otherUser@-0x10 /
    // i@-0xc（4.1 先声明更深）⇒ 两个 CUser* 均声明在函数头，myUser 在前。
    CUser* myUser = 0;
    CUser* otherUser = 0;
    // b28d-solo-CFairMatchRooms：ORIG 两个循环共用同一 i 槽（-0xc）⇒ i 提到
    // 函数头声明一次（4.4 对两个独立 for-init i 各占一槽，整体槽深 +4）。
    int i;
    for (i = 0; i < 4; ++i) {
        otherUser = other->GetUser(static_cast<unsigned>(i));
        if (otherUser != 0) break;
    }
    if (otherUser == 0) return false;

    for (i = 0; i < 4; ++i) {
        myUser = GetUser(static_cast<unsigned>(i));
        if (myUser != 0) break;
    }
    // b28d-solo-CFairMatchRooms：ORIG 尾 `cmpl $0,-0x10(%ebp); jne <mov$1>;
    // mov $0,%eax; jmp; <mov$1>` ⇒ P56/P47 分支形（`return myUser != 0;`
    // 生 setne 物化，与 ORIG 的 mov$0/mov$1 双岛不符）。
    if (myUser == 0) return false;
    return true;
