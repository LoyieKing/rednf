CUser* CGameManager::getUser(int slotId, int increId)
{
    // b28d-lane12: ORIG 0x082947a4 实证——Guard 作用域为**块**（0x082947ee 析构
    // 先于 0x082947f3 `cmpl $0,-0x10` 判空）；else 臂为早返回链（incre==0→0、
    // incre==id→user、冗余 incre==0→user（P171 照抄）、id==0→user、否则 0）。
    // b28d-solo-CGameManager 判词：尾链已改三目（P47）；gap 0（74=74），余差为
    // 三处 je 落点 +0xec/+0xec/+0xf1 vs ORIG 同落 +0xf3（4.4 对末项恒反转 jne）→ N82附。
    short slot = (short)(slotId >> 0x10);
    short id = (short)slotId;
    CUser* user;
    {
        Guard<Mutex> guard(&m_mutex);
        user = m_userPool.Get((int)slot);
    }
    if (user == 0)
    {
        // P37/P227：ORIG 0x08294816 `movswl -0x14(%ebp),%ebx` 在 ctor 前物化
        // slot 并跨 ctor 保活 ⇒ 全表达式临时链形（具名 tr 对象在实参处才读）。
        cMyTrace(__PRETTY_FUNCTION__, 0x95c, 5)(
            " No way! getUser , slot_id(%d) is abnormal!", (int)slot);
        return 0;
    }
    short incre = user->GetIncreID();
    if (incre == 0)
        return 0;
    // b28d-solo-CGameManager 判词：AE（P47 镜像翻正）。ORIG 0x0829487c 三处
    // `je <ret_user>` **同一落点**（+0xf3 `mov -0x10(%ebp),%eax`），false 路径
    // 直落 `mov $0,%eax; jmp` ⇒ 源码是**否定守卫 + 尾 return**（`if(!(...)) return 0;
    // return user;`），4.4 逐条复现（探针 /tmp/solo_probe1.cpp f1/f2，4.4.6）。
    // 三目形 `? user : 0` 块序相反（ret_user 前置，三处 je 落 +0xec/+0xec、末项
    // 反转 jne）——S154 判 N82附 系源形未穷举，本波翻正。
    if (!(incre == id || incre == 0 || id == 0))
        return 0;
    return user;
}