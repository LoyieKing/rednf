bool CParty::SetCardNumber(CUser* user, char a, eClearRewardCardType_t type)
{
    // b28d-49 判词：N 封顶。P167 已 CLEAR（b28d-solo-CParty2：check_empty_count 处
    // 由 R 版改回 W 版，ORIG 0x085b43aa 实证）；余差为条件物化/装载序 N82 族。@0x85b415a
    int slot = GetMemberSlotNo(user);                   // -0x18
    // ORIG 085b4180 `je 44cb` + 085b418a `jg 44cb`：前 3 条守卫跳共享 false 尾块（44cb）；
    // HaveAlreadySelectedCard/_checkGoldCardSelectable 用就地 `mov $0;jmp 44d0`。
    if (slot == -1 || 3 < (int)a)
        goto fail;
    if (HaveAlreadySelectedCard((char)slot, type))
        return false;
    if ((unsigned char)m_field204[(int)type][(int)a] != 0xff)
        goto fail;
    if (!_checkGoldCardSelectable(user, type, slot))
        return false;

    m_field204[(int)type][(int)a] = (char)slot;
    if ((int)type != 1)
        goto ret_true;                                  // ORIG 085b4230 `jne 44c4`

// §P331 第二形（实测 227→140）：ORIG 每次 reward 字段访问都重物化 `imul 3;shl 3`
// (=slot*0x7a) + `add $0x79d`（imul$0x7a 7 处 / add$0x79d 7 处，本方原 1/0 —— 被 bd +
// reward 两个引用吸收）。slot 此后不再改变 ⇒ 逐次寻址同物，无可观察行为变化。
