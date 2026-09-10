int Dispatcher_SkillInit::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081e5a2c
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() != 3)
    {
        return 7;
    }
    MSG_SKILL_INIT* req = reinterpret_cast<MSG_SKILL_INIT*>(&msg);   // -0x10（先声明远槽）
    // b28c-L19：ORIG 0x081e5a72 起 `mov -0xc(%ebp),%eax; cmp $0xffffffff,%eax` 载寄存器
    // 比较（P209：无符号参数/局部比较恒载寄存器；signed 才 mem 直比）⇒ 槽声明 unsigned。
    // [round5 A01 · C05② 复查（S1 一次性旋钮，已试毕回退）] 按 C05「`int` 局部 ⇒ 内存直比」
    // 把 idx 改 int 后 fresh 复编（19:42:17）读数**逐值不变**（identical/ae/near/diff =
    // 1008/2472/3/274，该件仍 DIFF、仍 `cmpl $0xffffffff,-0xc(%ebp)`）⇒ 本件属 N18b
    // （常量 vs 局部比较的内存直比），C05② 的 int 旋钮**不命中**，按 S1 立即恢复此处 unsigned。
    unsigned int idx = user->GetCurCharacSkillTreeIndex();
    // b28c-L19 清绑（P0/N96 全域禁 register asm）：改自然短路形。
    // ORIG 0x081e5a43：`cmp $-1; je LZERO; test; jne LELSE; LZERO:` = 短路或形
    // `idx == -1 || idx == 0`（两比较各自重载 idx，LZERO 为第二 jne 的自然 fall-through）。
    // 残差 N（b28c-L19 复验，20→4）：4.4 对局部比较恒 mem 直比（ORIG 载寄存器，
    // unsigned 化 idx 无效）+ 尾段 `cmp $0; setge` vs 4.4 not/shr 折叠。
    if (idx == -1 || idx == 0)
    {
        if (req->m_0d == 1)
        {
            return 0x13;
        }
        req->m_0d = 0;
    }
    else
    {
        if (req->m_0d != 1)
        {
            return 0x13;
        }
        req->m_0d = 1;
    }
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        // ORIG 0x081e5ad4：cmp $0x0;setge %al;test %al,%al;jne skip —— 4.1 将
        // `grade>=0` 物化进 %al(setge)；4.4 裸 `>=0` 恒折 not;shr 符号位提取
        // （register bool asm(al) 亦不触发 setge，P196 fold，记 N）。
        // [b28d-solo-Dispatchers] P225 形复测（`while (expr >= 0) { goto L; }`）
        //   实测仍产 `not %eax; shr $0x1f`（ndiff 10 不变）⇒ 保持裸 if 形，
        //   残差按 N 封顶（4.4 对 >=0 恒符号位提取）。
        if (user->get_pvp_grade() >= 0)
            goto LPASS;
        return 0xc;
LPASS:;
    }
    return 0;
