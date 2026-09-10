int CAssaultMgr::OnMoveArea(CUser* user)
{
    // b28d-solo-CAssaultMgr: AE — 本波修两处：①`place==0/!IsArea` 双 continue
    // 改嵌套正向 if 包裹（P296，消 xor$1 + 双 jmp+nop 岛）；②area 删 (char)
    // cast（ORIG 0x82ef6bf get_area 返回 int 直传 `mov %eax,%ebx`，getCurCharacVill
    // 才 movsbl）⇒ 157→149 与 ORIG 150 逐条 AE（余 1 条尾 nop 对齐豁免）。
    // b28d-L3: DIFF/N82 — ORIG 0x082eb... 临时对象槽 -0x24 vs 我方 -0x28（N82/P112 槽序）
    // ORIG 0x82ef610：向进入袭击区域的玩家汇总可见图标（包 0x86）。
    // 整函数 try/catch(int)（尾 landing `cmp $0x1,%edx; je resume` +
    // begin_catch `mov (%eax),%eax; mov %eax,-0xc` + end_catch），统一尾
    // `mov $0x1,%eax; ret` ⇒ 返回 int 1（P159）。view 判定为**单条 && 短路链**
    // （0x82ef6f0 ExistUser→xor$1 / CheckPlayable test / GetState cmp4 jne
    // 三级直落 false 岛 `mov $0,%eax`，+0x122 test 汇聚；if/else-if 三态
    // 赋值形产 setcc+cmpb 岛，不同形）。
    // 收敛口径：o=149 n=156。语义修正已全落（catch(int)/int 返回/view 短路/
    // dead 双清零/vill int 无截断+area (char) 收窄/movsbl 位点）。残余：
    // vill 跨 get_area 调用 ORIG 用 callee-saved %ebx 免槽直落，4.4 分配器
    // 因既有 ebx/esi 活跃改落栈槽（register asm("ebx") 实测反扰乱装载序
    // 已回滚自然形）；it/place/计数槽 +0x4 平移与 nop 岛 = N82/N76/
    // 跨版本 nop;jmp 岛族，byte 封顶。
    try
    {
    PacketGuard packet;
    packet.put_header(0, 0x86);
    // ORIG：countIdx/count 先双清零（`movl $0,-0x28; movl $0,-0x14`，
    // dead init 照抄 P171），随后 get_index 覆写 countIdx。
    int countIdx = 0;
    int count = 0;
    countIdx = packet.get_index();
    packet.put_short(count);
    for (std::map<unsigned int, CAssaultPlace*>::iterator it = m_places.begin();
         it != m_places.end(); ++it)
    {
        CAssaultPlace* place = it->second;
        // b28d-solo-CAssaultMgr: ORIG 0x82ef6b0 两处守卫（place==0 / !IsArea）
        // 均 `je <循环出口>` 且无 xor ⇒ 原码是嵌套正向 if 包裹（P296），
        // `continue` 形在 4.4 恒产 `xor $1` + 双 `jmp+nop` 中继岛（+7）。
        // vill 内联为实参（P0：ORIG 无槽往返，`call; mov %eax,%ebx`）。
        if (place != 0)
        {
            // ORIG 0x82ef6bf：get_area 结果 `mov %eax,%ebx` 无 movsbl（返回 int
            // 直接传，原码无 (char) cast）；getCurCharacVill 才 movsbl（返回 char）。
            if (place->IsArea(user->getCurCharacVill(),
                              user->get_area(false)))
            {
                if (!place->ExistUser(user) && place->CheckPlayable() &&
                    place->GetState() == 4)
                {
                    place->MakeViewAssaultIconOnePlayer(&packet, true);
                    count += 1;
                }
            }
        }
    }
    if (count != 0)
    {
        packet.put_short(countIdx, count);
        packet.finalize(true);
        user->Send(packet);
        }
    }
    catch (int)
    {
    }
    return 1;
