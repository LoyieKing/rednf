// ORIG 0x0867f4c8：CleanPad 人机认证应答比对（验证码头 5×ushort 逐项日志后 memcmp）。
// b28d-solo-W8_CU_G: DIFF/N 封顶（160/160，ndiff 148→37）— 本波修 3 处：
// ①P227 全表达式临时链 `cMyTrace(f,l,v)(...)`：ORIG 0x0867f4d1..0x0867f598 在 ctor
//   前右起求值全部 varargs + NumberToString（具名 trace + 语句形恒 ctor 先）；
// ②CUser.h:2445 匿名 union 双视角 `m_capchaCertifyW[5]`（P235，Main 代落）⇒ 5 处
//   读取由 add+movzwl 三步折回 ORIG 单条 `movzwl 0x8e0fxx(%eax),%eax`；
// ③varargs 中 orig 组实参顺序改为 [0..4]（ORIG 压栈序 0xc/%esi=orig[0] … 0x1c/-0x34
//   =orig[4]，P23/压栈序判读）⇒ 求值序 [4]→[0] 与 ORIG 全等。
// b28d-W8b: **DIFF→AE**（160/160）— 块布局判据（P47/P59）：ORIG 0x0867f62e
// `test %eax,%eax; je +0x203` 把成功块尾置、失败块直落；负极性 `==0` 写法产
// `jne <失败块>` + 成功块直落（指令数相同但 jcc/块位反）。翻转 if 顺序后逐条对齐。
// 余：@0x0867f5e7 字符串地址归一化伪差（§6.3）。15/15 调用点全等，语义完成。
bool CUser::checkHumanCertify(int len, unsigned short* answer)
{
    // b21 verdict=DIFF（N 残余）：160/166。15=15 调用点全等（get_acc_id×n/NumberToString/
    //   cMyTraceC1+cMyTracecl/DB_CountOfHumanCertify::makeRequest/memcmp/
    //   DB_ForceSaveSimpleSSO::makeRequest×2/isCleanPadVerifyLimit/setHumanCertified/
    //   SendCmdErrorPacket）。orig/ans 5 组 ushort 物化顺序与 trace 实参求值序差(N82)、
    //   `const unsigned short* orig` 具名局部占槽(ORIG 直读 this+0x8e0ec 偏移族)。语义全等，
    //   byte 层 N 封顶。
    {
        // b28d-solo-W8_CU_G：P227 全表达式临时链 —— ORIG 0x0867f4d1..0x0867f598 在
        // cMyTrace ctor 之前右起求值全部 varargs（answer[4..0]→槽、orig[4..0]→槽/寄存器）
        // 与 NumberToString；具名 trace + 语句形恒 ctor 先。
        // 另：ORIG 每次 `mov 0x8(%ebp),%eax; movzwl 0x8e0f4(%eax),%eax` 重载 this 直读
        // 偏移（P183）⇒ 不设 orig 指针局部；用 CUser.h 匿名 union 双视角
        // `m_capchaCertifyW[5]`（P235，Main 代落）让 4.4 折叠单条 disp。
        cMyTrace(__PRETTY_FUNCTION__, 0x7e07, 0)(
            "checkHumanCertify m_id:%s, orig:%x%x%x%x%x, ans:%x%x%x%x%x",
            NumberToString(get_acc_id(), 0),
            static_cast<unsigned int>(m_capchaCertifyW[0]),
            static_cast<unsigned int>(m_capchaCertifyW[1]),
            static_cast<unsigned int>(m_capchaCertifyW[2]),
            static_cast<unsigned int>(m_capchaCertifyW[3]),
            static_cast<unsigned int>(m_capchaCertifyW[4]),
            static_cast<unsigned int>(answer[0]), static_cast<unsigned int>(answer[1]),
            static_cast<unsigned int>(answer[2]), static_cast<unsigned int>(answer[3]),
            static_cast<unsigned int>(answer[4]));
    }

    DB_CountOfHumanCertify::makeRequest(get_acc_id());

    // b28d-W8b: 块布局判据（P47/P59 族）—— ORIG 0x0867f62e `test %eax,%eax;
    // je +0x203` 把**成功块放尾部**、失败块直落（memcmp!=0 走直落失败路径）；
    // 负极性 `==0` 写法则产 `jne <失败块>` + 成功块直落（块序互换，160/160 但
    // jcc/块位反）。改为正极性 `!=0` + 失败块前置、成功块尾置，逐条复现。
    if (memcmp(answer, m_capchaCertify, static_cast<size_t>(len)) != 0)
    {
        ++m_humanCertifyErrorCnt;
        if (isCleanPadVerifyLimit() && m_field8e3ed)
        {
            DB_ForceSaveSimpleSSO::makeRequest(get_acc_id(), m_humanCertifyErrorCnt);
            SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x91), 8);
        }
        setHumanCertified(true);
        return false;
    }

    if (m_humanCertifyErrorCnt != 0)
    {
        DB_ForceSaveSimpleSSO::makeRequest(get_acc_id(), 0);
    }
    m_humanCertifyErrorCnt = 0;
    setHumanCertified(true);
    return true;
}