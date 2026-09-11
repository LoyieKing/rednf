// [b28d-solo-CPartyTelePort][N] 语义完成，byte 层跨版本封顶：
// ① 返回值归一化：ORIG 0x085bc034 `movzbl -0x35(%ebp),%eax`（4.1 直返 char），
//    4.4 对 `return <char>;`（函数返回 bool）恒发 `cmpb $0,+setne %al`
//    （/tmp/rp.cpp g1..g6 六形探针全归一化）；返回类型由 ORIG
//    __PRETTY_FUNCTION__ 串 "bool CPartyTelePort::process_teleport()"
//    （.rodata 0x08cbb220）锁定，不可改类型。
// ② 槽分配策略（N82附）：ORIG dir/posy 落深槽 -0x78/-0x74（第二循环
//    -0x70/-0x6c），4.4 复用于 -0x38/-0x34（-0x28/-0x24），cMyTrace 反向；
//    声明序/位置调整无效（4.4 聚合优先低槽 + 标量分区）。
// 其余指令序列（归一化地址/槽号后）与 ORIG 逐条一致；p167 调用点差集 CLEAR。
// [N6S2NegB 2026-09-11 复核] 真残差收敛为 1 条（n1_v: DIFF orig=305 our=306 gap=-1）：
//   ORIG 0x085bc034 尾 `movzbl -0x35(%ebp),%eax`，我方 `cmpb $0x0,-0x35(%ebp); setne %al`；
//   其余 305 条（槽号归一后）逐条全等，差集仅 3 处 .o 未重定位立即数显示伪差
//   （prett 串 / "party_teleport_*" / &GlobalData::s_systemTime_）。
//   根因链（探针 /tmp/probe_pt/w1..w7）：`char` 局部 return 到 bool 函数 ⇒ 4.4 恒插
//   cmpb+setne（w1/w4/w5）；`bool` 局部 + **bool callee** ⇒ 赋值仅 `mov %al,槽`、末尾
//   `movzbl 槽,%eax`（w6 = ORIG 形）；`char` 局部 + bool callee ⇒ 末尾仍 setne（w7）。
//   ⇒ ORIG 真相是 `bool check_cur_teleport_state()` + process_teleport 的 result 为 bool；
//   我方两者皆 char。[落地 2026-09-11] 父泳道 N6Free231to250 改 `CPartyTelePort.h:48`
//   为 bool，本 TU 同步三处：.inc 签名、`bool result = true/false`、本函数 `bool result;`。
//   落地读数（compare_tu_game_orig 口径）：本件 DIFF→**NEAR**（301=301；槽号归一后与 ORIG
//   逐条全等）。NEAR 已为口径上限：残差仅 ①clean 不归一栈槽号（我方 `-0x45..` vs ORIG
//   `-0x35..`，4.4 槽分配 +0x10 见 ②）②ORIG 侧符号化立即数 `$&_ZZ…PRETTY_FUNCTION__` /
//   `$"party_teleport_*"` / `$&_ZN10GlobalData13s_systemTime_E` vs 我方 .o 侧 `$<A>`
//   （clean 单侧归一，两侧永不相等）。TU 四数 near 0→1 / diff 3→2，其余不变；
//   check_cur_teleport_state 仍 AE 56=56。
bool CPartyTelePort::process_teleport()
{
    // [N6S2NegB 2026-09-11] ORIG 0x085bc034 `movzbl -0x35(%ebp),%eax`（bool 槽直取）
    // + 0x085bbbe2 `mov %al,-0x35(%ebp)`（callee 返回 bool 无转换）⇒ result 为 **bool**。
    bool result;
    if (m_party == 0)
    {
        return 0;
    }

    result = check_cur_teleport_state();
    int moveResult = 0;
    int auraAvatarTime = 0;
    if (result != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (m_party->checkValidUser(i) != 1)
            {
                continue;
            }
            if (get_teleport_member_state(i) == 4)
            {
                continue;
            }
            CUser* user = (CUser*)m_party->get_user(i);
            if (user != 0)
            {
                // [b28d-solo-CPartyTelePort] ORIG 0x85bbc68：dir/posy 有槽
                // （-0x78/-0x74，P0 具名局部）；posx/area/village 无槽往返
                // ⇒ 源码在实参位内联 getter。
                int dir = (int)get_teleport_direction();
                int posy = (int)get_teleport_posy();
                moveResult = G_GameWorld()->check_move_area(user,
                    (int)get_teleport_village(), (int)get_teleport_area_index(),
                    (int)get_teleport_posx(), posy, dir, true, 0);
                if (m_party->getManager() == user)
                {
                    moveResult = user->is_equip_aura_avatar(0, auraAvatarTime);
                }
                if (moveResult > 0)
                {
                    set_teleport_member_state(i, 0);
                    set_teleport_state(2);
                    break;
                }
            }
        }

        if (get_teleport_state() == 1)
        {
            send_teleport_effect_at_different_place(1);
            for (int i = 0; i <= 3; ++i)
            {
                if (m_party->checkValidUser(i) != 1)
                {
                    continue;
                }
                if (get_teleport_member_state(i) == 4)
                {
                    continue;
                }
                CUser* user = (CUser*)m_party->get_user(i);
                if (user != 0)
                {
                    user->set_charac_visible_teleport_(true);
                    int dir = (int)get_teleport_direction();
                    int posy = (int)get_teleport_posy();
                    // [b28d-solo-CPartyTelePort] ORIG 0x85bbeb4：move_area 返回
                    // bool 直测（无 moved 槽）；trace 用 P227 全表达式临时链
                    // （实参 getCurCharacNo 在 ctor 前求值并跨 ctor 保活 %ebx）。
                    if (G_GameWorld()->move_area(user, (int)get_teleport_village(),
                            (int)get_teleport_area_index(), (int)get_teleport_posx(),
                            posy, dir, true, 1, 0, 0) != 1)
                    {
                        cMyTrace(__PRETTY_FUNCTION__, 0x43f9, 5)
                            ("party_teleport_error:(char_no:%d)", user->getCurCharacNo());
                    }
                    user->set_charac_visible_teleport_(false);
                }
            }
            send_teleport_effect_at_equal_place(2);
        }
        else
        {
            send_teleport_status((unsigned char)moveResult);
        }
    }

    if (get_teleport_state() == 1)
    {
        reset_teleport_data();
        CUser* manager = (CUser*)m_party->getManager();
        if (manager != 0)
        {
            // [b28d-solo-CPartyTelePort] ORIG 0x85bbf95：`movl $&GlobalData::
            // s_systemTime_,(%esp)`（对象直调，P304）；curSec/optValue/charNo
            // 均无槽往返 ⇒ 内联表达式 + P227 临时链。
            int newTime = GlobalData::s_systemTime_.getCurSec() + auraAvatarTime;
            manager->set_aura_avatar_option_value(0, newTime);
            manager->UpdateAuraAvatarOption(0, manager->get_aura_avatar_option_value(0));
            cMyTrace(__PRETTY_FUNCTION__, 0x4419, 0)
                ("party_teleport_success:(char_no:%d)", manager->getCurCharacNo());
        }
    }
    return result;
}