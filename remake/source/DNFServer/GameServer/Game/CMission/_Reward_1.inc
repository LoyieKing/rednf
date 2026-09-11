// [round3-L29 2026-09-10 复核] 我方 127 / ORIG 125：difflib 显示我方多两段
//   「mov $0x1,%ebx; jmp」/「mov $0x1,%ebx; lea; call」= 早返回 true 的物化。
// [2026-09-10 round5 定稿 **已修**] 按 P292 正极性包裹改写：ORIG 0x085e41af
//   `test %al,%al; je 085e42e1`（je 落**函数尾部共享块** `mov $0x1,%eax`，cond 真
//   走 body 直落）⇒ 源形必须是 `if (it != m_comboMap.end()) { body } return 1;`
//   （早返回 `if (end()==it) return 1;` 会就地物化 mov $1,%ebx + jmp）。
//   改后：origN=ourN=125、帧 80=80、判据⑪ PURE SHIFT c=0；.o 口径 DIFF→**NEAR**
//   （残余仅 2 处字符串地址重定位伪差：ORIG 0x8cc2c5c/0x8cc2c70 vs 我方 .o 的
//   addend 0x0/0x14 = N124/N132）⇒ 链接口径 AE。
// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
int CMission_combo_clear::_Reward(CUser& user) const
{
    combo_script_key key;
    key.m_job = (char)user.get_charac_job();
    key.m_growType = (short)user.getCurCharFirstGrowType();

    std::map<combo_script_key, combo_script_data>::const_iterator it =
        m_comboMap.find(key);
    // b28d-50：ORIG 0x085e43xx 实参序 (%esp)=end、0x4(%esp)=it（同 _isAcceptCondition，
    // P74补②/P220 反序）。
    // [2026-09-10 P292 正极性包裹] ORIG 0x085e41af `test %al,%al; je 085e42e1`
    //   —— je 落**函数尾部共享块** `mov $0x1,%eax`（+0x1ad），cond 真走 body 直落
    //   ⇒ 源码是 `if (it != end) { body } return 1;`；早返回形 `if (end == it)
    //   return 1;` 会就地物化 `mov $0x1,%ebx; jmp`（我方 127 vs ORIG 125 两段差）。
    // [L6 #61-75 CMission 2026-09-11] ORIG 0x085e41a4-0x085e41aa 实参序：
    //   `(%esp)` = end()（-0x10）、`0x4(%esp)` = it（-0x18）⇒ 源码为 `end() != it`
    //   （4.4 对 `it != end()` 产 `(%esp)`=it、`0x4(%esp)`=end 的反序）。
    if (m_comboMap.end() != it)
    {
        std::vector<std::pair<int, int> > items;
        items.push_back(it->second.m_bonus);

        std::vector<int> outSlots;
        // b28d-50：ORIG 预清 `movl $0x0,-0xc(%ebp)` 在 call 前（P198①两句式）。
        int ret = 0;
        ret = ((CInventory*)user.getCurCharacInvenW())
                  ->insert_event_items(items, outSlots,
                                       (eMoneyAddReason)0x1e,
                                       (eItemAddReason)0x33, true,
                                       "game_server_msg_173",
                                       "game_server_msg_174");
        if ((ret > 0) && (outSlots.size() > 2))
        {
            user.SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0,
                                outSlots[0]);
        }
    }
    return 1;
}