bool CUser::send_itemspace(int space)   // 返回形随 CUser.h:2843 bool 化（P22 调用点直测形）
{
    // P168a/X2 五段化（2026-09-05 CUserC8，Main 派单）：ORIG 0x86?? 每 MakeItemList
    // case `call; xor $0x1,%eax; test %al,%al; je` **带分支消费者**（判据③通过），
    // char 暂存槽 -0x9 与 `cmp $1/setne` 岛形均为我方旧形失分点（判据②岛→直分支）。
    // 裸 `!fn()` 对本批 MakeItemList(bool 返回) 实测产 xor 直落（判据①），case7 用
    // !=1 系 char 域实测等价保留。
    // X1 精确形（Main 广播定字 2026-09-05，int callee 调用点自救——MakeItemList/
    // SendCreatureItemList 均 int 返回，裸 `!fn()` 实测产 test eax;sete 三段，
    // uchar 绑 %eax + `^= 1` 后 `if (v)` 得 `xor $0x1,%eax; test %al,%al; je`
    // 与 ORIG 0x8675 段系同形；赋值零成本（call 结果已在 %eax），块作用域隔离）。
    PacketGuard packet;
    switch (space)
    {
    case 0:
    {
        unsigned char v0 =
            (unsigned char)getCurCharacInvenR()->MakeItemList((INVEN_TYPE)1, &packet);
        v0 ^= 1;
        if (v0) return 0;
        break;
    }
    case 1:
    {
        unsigned char v1 =
            (unsigned char)getCurCharacInvenR()->MakeItemList((INVEN_TYPE)2, &packet);
        v1 ^= 1;
        if (v1) return 0;
        break;
    }
    case 2:
    {
        unsigned char v2 =
            (unsigned char)getCurCharacCargoR()->MakeItemList(&packet);
        v2 ^= 1;
        if (v2) return 0;
        break;
    }
    case 7:
    {
        unsigned char v3 = (unsigned char)
            getCurCharacInvenR()->GetCreatureMgrR()->SendCreatureItemList();
        v3 ^= 1;
        if (v3) return 0;
        unsigned char v4 =
            (unsigned char)getCurCharacInvenR()->MakeItemList((INVEN_TYPE)3, &packet);
        v4 ^= 1;
        if (v4) return 0;
        break;
    }
    case 0xc:
        // ORIG 0x865dc90 段：`call IsExistAccountCargo; test %al,%al; je <ret1岛>`——
        // 真支 fall-through 直取成员对象地址（`mov 0x8(%ebp),%eax; add $0x6effc,%eax`）
        // 调 m_accountCargo.SendItemList()，⇒ 原码是成员对象直调而非 GetAccountCargo()
        // （此处 4.1 头文件内联的 getter 已在我们 TU 外联，多出一次 call）。
        if (IsExistAccountCargo())
            return m_accountCargo.SendItemList();
        return 1;
    default:
        return 0;
    }
    Send(packet);
    return 1;
