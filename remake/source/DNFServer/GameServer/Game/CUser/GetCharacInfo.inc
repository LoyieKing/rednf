// ORIG T 0x0864bc04 sz0x1b5：填充 SIG_LOAD_CHARAC。
//   +0x0 acc_id / +0x4 charac_no(*curR 首字段) / +0x8 result(0 初始, 失败置 1) /
//   +0xc CHARAC_DATA（getCharacInfoDetail）/ +0x686 tag_charac_no /
//   +0x68a striker_skill_index / +0x68b TAG_CHARAC_DATA（getTagCharacInfo）。
bool CUser::GetCharacInfo(SIG_LOAD_CHARAC* sig) const
{
    sig->m_accId = get_acc_id();
    // ORIG 0x0864bcc8：无条件 mov (%eax) 解引用（getCurCharacR 恒非空，无检查）
    sig->m_characNo = (unsigned int)getCurCharacR()->m_characNo;
    int characIdx = get_charac_idx(sig->m_characNo);
    sig->m_result = 0;

    // b28c-L5：清 `register bool asm("al")` 存量绑定（P0/N96）。ORIG 0x0864bdf2
    //   `call getCharacInfoDetail; xor $0x1,%eax; test %al,%al; je <正常路径>`
    //   = P168a① 「callee 返回 bool ⇒ 调用点一律裸形」（该 callee 的 ORIG 体尾
    //   仅 `mov $0x1/$0x0,%eax` 两出口 → P159 串/尾形双证 bool，声明已由 Main 代落
    //   CUser.h:3121 int→bool）。X1 约束形/物化 bool 局部均产 sete|setne 三段，
    //   唯有 bool callee + 裸 `!call()` 出单条 xor。
    // err trace 保具名（ORIG err@-0x30 远槽），主块 trace 临时链改全表达式形。
    if (!getCharacInfoDetail(characIdx, sig->m_characNo,
                             reinterpret_cast<CHARAC_DATA*>(&sig->m_characData)))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1052, 0)
            ("getCharacInfoDetail() return = false");
        sig->m_result = 1;
        return false;
    }

    int tagIdx = get_charac_idx((unsigned int)getCurCharacTagCharacNo());
    if (tagIdx >= 0 && tagIdx < (int)m_characList.size())
    {
        // b28c-L5：删 (unsigned int) cast。ORIG 0x0864bd1x `mov 0xc(%ebp),%edx;
        //   mov %eax,0x686(%edx)` = 值留 %eax、base 占 %edx 的**同型直存**形；
        //   原字段 unsigned int ← int 返回值异型使 4.4 多一段 edx 中转（P211）。
        //   字段已随 SigTypes_C.h 改回 int（广播）。
        sig->m_tagCharacNo = getCurCharacTagCharacNo();
        sig->m_strikerSkillIndex = getCurCharacStrikerSkillIndex();
    }
    getTagCharacInfo(tagIdx, sig->m_tagCharacNo,
                     reinterpret_cast<TAG_CHARAC_DATA*>(&sig->m_tagCharacData));
    {
        // ORIG 0x864bcf6/0x864bd01：两值先求值直落 callee-saved %esi/%ebx，
        // 跨 ctor/operator() 存活；主 trace 临时链（P227 全表达式形）翻转与
        // err-trace 的槽分配序（ORIG 主@-0x20/err@-0x30）。
        // b28c-L5：原 `register int …` 无 asm 绑定（合规），但 4.4 对具名
        // register 局部仍按 P0 落槽 ⇒ 保留具名局部写法（ORIG 两值确在 ctor 前
        // 物化并经 %esi/%ebx 跨 call 保活，P227 实证该形态由「实参位内联 +
        // 全表达式临时链」自然产生，无需绑定）。
        // b28c-L5 两处装载形修正：
        //   ① `mov 0x59(%eax),%esi`（ORIG 单 disp，%eax=sig）⇒ 必须走**真成员链**
        //      `sig->m_characData.m_field4d`（4.4 折 base+0xc+0x4d 为一条 disp32）；
        //      原 reinterpret_cast<CHARAC_DATA*>(&m_characData) 使 4.4 先 `add $0xc`
        //      物化子对象地址再读 +0x4d（+1 条）。
        //   ② `mov 0x4(%eax),%ebx`（ORIG 一步直落 callee-saved）⇒ 删多余 (int) cast：
        //      字段 unsigned int 而目标寄存器承载 int 时 4.4 恒插 `mov %eax,%ebx`
        //      中转（P211/N18a 族，varargs 实参位无需显式拓宽）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1066, 0)(
            "CharLastPlayTick char(%d) t(%d)", sig->m_characNo,
            sig->m_characData.m_field4d);
    }
    return true;
}