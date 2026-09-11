// ---------------------------------------------------------------------------
// _PutItemForEnchant（报告 _PutItemForEnchant.md，ORIG T 0x849d344 sz 0x423）
// kind：0=登记费用、1=主槽材料、2=对家槽怪物卡。money 仅 kind==0 使用。
// ---------------------------------------------------------------------------
int CEnchanter::_PutItemForEnchant(CUser* user, int kind, stItemPos_t* pos,
                                   int money)
{
    register int notified;
    register int result;
    // [b28d-solo-ExpertJobOnlyEnchanter] 判词·N（314 vs ORIG 312，prologue/帧 0x30
    // 与全部调用点已对齐；p167 见 TU 报告）。已修：①allowed 正条件包裹
    // （P214①，原负条件反产 jne 落 true 岛）；②m_user 守卫改正向 if + 块外 return
    // （P214①/h3 形，原负条件产 jne 落 true 岛）；③PacketGuard 内层块 + 统一
    // `return result`（P223 尾标签 + ORIG 0x416 共享 mov $0,%ebx）；④register 无初值
    // 声明 + else 臂 `notified = 0`（P231/P248④，原初始化形多两条 mov）。
    // 残余 N：①callee-saved 角色互换——4.4 分配器把 notified/result 固定为
    //   ebx/esi（ORIG 反之），声明序/声明位置（函数头↔使用点）/作用域四形实测均不翻
    //   （N7/N82附）；②4.4 -O0 无 tail-merge/crossjump——`notified=1` 两处与
    //   `result=0` 两处不合并且多 1 jmp+nop（N121 族）；③EH cleanup 块相对序反转
    //   （ORIG 正常尾在前、unwind 居中；4.4 恒正常尾内联、unwind 压最末，N104）。
    // 语义完整（p167 差集零）；以上均为 4.1/4.4 后端块布局差异。
    if (user->getCurCharacR() == 0)
    {
        result = 3;
        goto enchant_skip_zero;      // ORIG 0x0849d360 `mov $0x3,%ebx; jmp 0x41b`
    }

    // ORIG 全部 unique_id 比较为 16 位（cmp %ax）；allowed 不落栈（寄存器物化）
    if ((unsigned short)user->get_unique_id() !=
        (unsigned short)m_peer->get_unique_id())
    {
        // [b28d-solo-ExpertJobOnlyEnchanter] P214①：ORIG 短路物化为正条件
        // `jne L_false; call; xor $1; test; je L_false; mov $1; jmp; L_false: mov $0;
        // test %al; je L_err`（无栈槽）⇒ 必须写「正向 if 包裹 + else 早返」，
        // 负极性 `if (!(A&&B)) return` 会反产 jne 落 true 岛（探针 /tmp/probe_allowed.cpp
        // g2 与 ORIG 逐条同构，g1/g3 均反）。
        if (kind == 2 && IsUseOtherUserMonsterCard() != true)  // ORIG bVar1
        {
            if ((unsigned short)user->get_unique_id() !=
                (unsigned short)m_user->get_unique_id())
            {
                result = 3;              // ORIG 0x0849d3dc `mov $0x3,%ebx`
                goto enchant_skip_zero;
            }
        }
        else
        {
            result = 3;                  // ORIG 0x0849d3e6 `mov $0x3,%ebx`
            goto enchant_skip_zero;
        }
    }

    // ORIG kind 分发：mov type→eax + cmp $1/je、cmp $2/je、test/jne → switch
    // （叶块排放序 0、1、2，P46）
    switch (kind)
    {
    case 0:
        if (money < 0)
        {
            result = 0x5f;               // ORIG 0x0849d413 `mov $0x5f,%ebx`
            goto enchant_skip_zero;
        }
        if (m_field38 > 0)
        {
            result = 0x14;               // ORIG 0x0849d427 `mov $0x14,%ebx`
            goto enchant_skip_zero;
        }
        if (!m_user->CheckMoney(money))
        {
            result = 0x5f;
            goto enchant_skip_zero;
        }
        if (user->getCurCharacInvenR()->get_money() < money)
        {
            result = 0x16;
            goto enchant_skip_zero;
        }
        m_field38 = money;
        break;
    case 1:
        if (!m_materials[0].Empty())
        {
            result = 0x14;
            goto enchant_skip_zero;
        }
        {
            int err = 0;
            if ((err = m_materials[0].SetData(user, pos->m_itemSpace,
                                              pos->m_slot)) != 0)
            {
                result = err;
                goto enchant_skip_zero;
            }
            if ((err = m_materials[0].CheckType(false, 0)) != 0)
            {
                result = err;
                goto enchant_skip_zero;
            }
        }
        break;
    case 2:
        if (!m_materials[1].Empty())
        {
            result = 0x14;
            goto enchant_skip_zero;
        }
        {
            int err = 0;
            if ((err = m_materials[1].SetData(user, pos->m_itemSpace,
                                              pos->m_slot)) != 0)
            {
                result = err;
                goto enchant_skip_zero;
            }
            if ((err = m_materials[1].CheckType(true, 2)) != 0)
            {
                result = err;
                goto enchant_skip_zero;
            }
        }
        break;
    default:
        break;
    }

    // [b28d-solo-ExpertJobOnlyEnchanter] P214①/h3 形：ORIG 两个短路 je 同落 false 岛
    // 后 `test %al,%al; je <尾部 return 0>`（尾部 return 岛直落）⇒ 必须写「正向 if
    // 包裹 + 块外 return」，负条件 `if(!(A&&B)) return 0;` 反产 jne 落 true 岛
    // （探针 /tmp/probe_guard.cpp h1/h2 反、h3 与 ORIG 逐条同构）。
    // [b28d-solo-ExpertJobOnlyEnchanter] P231/P248④：ORIG 尾段无 `mov $0` 预清，
    // result/notified 全程 callee-saved 无槽（ebx/esi），仅 else 臂显式
    // `mov $0x3,%ebx; mov $0x0,%esi` ⇒ 无初值 register 声明 + else 臂显式
    // `notified = 0;`。`return result;` 统一落函数尾出口（ORIG 0x41b）；
    // m_user 无效臂的 result=0 与 `if (notified) result = 0;` 共享
    // `mov $0x0,%ebx`（ORIG 0x416 单点）。
    if (m_user != 0 && m_user->getCurCharacR() != 0)
    {
    {
    PacketGuard packet;
    packet.put_header(0, 0x9d);
    packet.put_byte(kind);
    switch (kind)
    {
    case 0:
        packet.put_int(m_field38);
        break;
    case 1:
        packet.put_int((int)m_materials[0].m_itemIndex);
        break;
    case 2:
        packet.put_int((int)m_materials[1].m_itemIndex);
        break;
    default:
        break;
    }
    packet.finalize(true);

    if ((unsigned short)user->get_unique_id() ==
        (unsigned short)m_user->get_unique_id())
    {
        if (m_peer != 0 && m_peer->getCurCharacR() != 0)
        {
            m_peer->Send(packet);
            goto enchant_set_notified;
        }
        result = 3;
        notified = 0;
        goto enchant_notified_done;
    }
    m_user->Send(packet);
enchant_set_notified:
    notified = 1;
enchant_notified_done: ;
    }
    // ORIG 0x412-0x41b：`test %esi,%esi; je 0x41b; mov $0x0,%ebx; 0x41b: mov %ebx,%eax`
    // ⇒ 负条件 goto 跳过 result=0；m_user 无效臂与 notified!=0 臂共享同一
    // `result = 0`（P223 尾标签形；正条件 if/else 形 4.4 多 jmp 且两处 mov 不合并）。
    if (!notified)
        goto enchant_skip_zero;
    }
        result = 0;
enchant_skip_zero:
    return result;
}