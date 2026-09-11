// ==== Dispatcher_RefundSkill::check_error (ORIG 0x081dad1c) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081dad1c orig/ours=77/75 ndiff-blocks=4
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    -0xc(%ebp),%eax | cmp    $0xffffffff,%eax | mov    -0xc(%ebp),%eax
int Dispatcher_RefundSkill::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081dad22：user==0 → -1（cmpl jne）。
    if (user == 0)
        return -1;
    // ORIG 槽序（4.4 规则 probe slots4 实证：先声明→远槽、后声明→近槽）：
    // req=-0x10（ORIG 0x081dad35 store）、treeIndex=-0xc（ORIG 0x081dadb9 store）。
    MSG_REFUND_SKILL* req = reinterpret_cast<MSG_REFUND_SKILL*>(&msg);
    // [b28d-solo-S162] N 封顶：ORIG 0x081dadbc/dadc4 为 reload 后寄存器比较
    // （`mov -0xc(%ebp),%eax; cmp $0xffffffff,%eax; je` / `mov; test; jne`）。
    // 4.4 对局部与常量 ==/!= 恒折 mem 直比——探针 /tmp/b28d_disp/probe_ti.cpp
    // n1..n8（signed/unsigned/(int) cast/短路）与 probe_ti3.cpp w1..w6 全折；
    // volatile 版产 load+cmp 但多 setcc 物化（probe_ti2.cpp v1）。禁 register 绑定 P0。
    int treeIndex;

    // ORIG 0x081dad43：test+sete 物化（技巧9）。
    if ((bool)(user->getCurCharacR() == 0))
        return -1;

    // ORIG 0x081dad61：cmp $3 + setne 物化。
    if ((bool)(user->get_state() != 3))
        return 7;

    // PvPSkilTree 频道只允许对应树技能 id（ORIG 0x081dad82 bool 直判 test %al；
    // 两分支各自 -0x10 重载 + cmp $0x2897c7/$0x2897c6，0x081dad86/dad9a）。
    if (G_GameWorld()->IsPvPSkilTreeChannel()) {
        if (req->m_0d != 0x2897c7)
            return 0x16;
    }
    else {
        if (req->m_0d != 0x2897c6)
            return 0x16;
    }

    // 技能树序号与包内选择必须一致：-1/0 树禁选 1，其余必选 1。
    // ORIG 0x081dadb9：结果整字存 -0xc（caller 视角 int 返回，无 movsbl）；
    treeIndex = user->GetCurCharacSkillTreeIndex();
    // ORIG 0x081dadbc/dadc4：两比较各自 `mov -0xc(%ebp),%eax` 重装载。
    // LZERO 体须是第二 jne 的自然 fall-through：
    // 空 then + else 内二次判断（probe p4 v2 实证；直写 goto LZERO 版多一条 jmp）。
    if (treeIndex == -1) {
    } else {
        if (treeIndex != 0)
            goto LELSE;
    }
    if (req->m_11 == 1)
        return 0x13;
    goto LOUT;
LELSE:
    if (req->m_11 != 1)
        return 0x13;
LOUT:
    ;

    if (req->m_13 == 0)
        return 0x13;                                            // 数量不得为 0
    return 0;
}