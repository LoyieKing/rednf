    // [G7-sub 2026-09-10 复核·写域边界] 本 .inc 仅含函数前段（slot 取得 + 4 条守卫 +
    // m_field204 写入；函数余部 0x.cpp:1403..1468）。逐点核对 ORIG 0x085b415a：
    //   ① `GetMemberSlotNo` 落槽 -0x18 ✓；② `slot==-1 || 3<a` 两条守卫跳共享 false 尾块
    //      （085b4180 `je 44cb` / 085b418a `jg 44cb`）✓ goto fail 形已对齐；
    //   ③ HaveAlreadySelectedCard/_checkGoldCardSelectable 的就地 `mov $0;jmp 44d0` ✓。
    // 余差 5 条（我方 250 vs 245）**全部落在 .cpp 段**：三处 bool callee（GetRepeatEvent /
    // IsReturnUserParty / IsEventCharacParty）我方写 `!callee()` 产 `xor $1; je`，
    // ORIG 为无 xor 的 `test; jne`（P168a②/P292 极性）⇒ 需 .cpp 属主改条件极性。
    // 本 .inc 无可改项；p167 CALLSITE CLEAR。
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
