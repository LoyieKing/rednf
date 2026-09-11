// [b28d-30] 复核 NEAR：died 槽 -0x11 vs -0xa（1 字节槽隙，N82/P190 族），ORIG 0x08635e96。
// [b28d-solo-TimerDispatcher] NEAR 封顶 N98（4.4 按首次使用序分配槽）：ORIG died 槽
// -0x11（两 byte 槽间 8B 隙）vs 4.4 -0xa；long long/volatile/double pad 三形实测反劣。
// [TimerDispatcher-3 2026-09-10 判据①] 分支目标序列全等；唯一差 = died 槽 -0x11↔-0xa
// （N98/N82 槽隙，无语义缺口）。
bool TimerCreature::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x08635e96：getUser 第二实参为常量 1（movl $0x1,0x8(%esp)）；
    // 三段独立早退（jne 岛 + 两条 cmp/setne 物化，ORIG +0x29/+0x46/+0x62）。
    // [TD4 2026-09-10 判据⑩] ORIG 槽位浅→深 = saveInventory(-0x9) → user(-0x10) →
    // died(-0x11)；按「声明序=槽由深到浅」⇒ 原码 died 声明在 user **之前**。
    // 旧形把 died 声明在 saveInventory 之后 ⇒ died 落 -0xa（紧贴 save）。
    bool died;
    CUser* user = G_CGameManager()->getUser(p1, 1);
    if (user == 0)
        return 0;
    if (user->get_unique_id() != p2)
        return 0;
    if (user->getCurCharacNo() != p3)
        return 0;
    // CheckDiedCreature 是 user_creature::CCreatureMgr 成员（ORIG 0x08339acc）：
    // user->getCurCharacInvenW()->GetCreatureMgrW() 取管理器实例。
    // ORIG +0x99：CheckDiedCreature 结果物化 -0x11(%ebp)（4.1 无死码消除，具名复现）。
    // 技巧21：died 先声明占远槽 -0x11、saveInventory 后声明占近槽 -0x9（ORIG
    // 8635f11/8635f2f 槽号）；`x ^ 1` 具名 char 槽条件出 movzbl+xor$1+test+je
    // （ORIG 8635f32，cmpb 融合形被 ^ 表达式阻断）。
    // [b28c-L26 判词·N82/P190 族] ORIG died 落 -0x11（与 saveInventory -0x9 间留
    // 8B 隙），4.4 对 1 字节局部紧邻分配 -0xa ⇒ 本件唯一残差（2 行）。实测把 died
    // 改 long long 借 8 字节对齐推深槽基 → 55→58 反劣（store/load 变 8B 两段），
    // 已回退 bool；volatile/double framePad（P190）不适用（本函数无 double 实体）。
    // ORIG 0x08635eeb/0x08635f08：isSaveInven 先调（byte 槽 -0x9），
    // CheckDiedCreature 后调（-0x11，4.1 两 byte 槽间留隙）
    const bool saveInventory = user->isSaveInven();   // b28c-L26 档①：callee bool 化
    died = user->getCurCharacInvenW()->GetCreatureMgrW()->CheckDiedCreature();
    (void)died;
    // ORIG 8635f32 `movzbl -0x9,%eax; xor $1,%eax; test %al; je`：
    // N96/P0：绑定形全域禁用后无合法源形可阻 fold-const（探针 /tmp/l7_m.cpp
    // m1/m3 恒折 `cmpb $1,-0x9; je`；register-hint n1 形溢入 %ebx 改帧）——
    // setcc-fold 负例在册，取最短 gap 折叠形，残差 3 行封顶。
    // P168a 负例（setcc-fold）：ORIG 8635f32 `movzbl -0x9,%eax; xor $1,%eax;
    // test %al; je` 寄存器形不可造——表达式形折 cmpb $1（53 条），
    // X2 `v ^= 1` 两语句形生 xorb 栈槽物化（54 条，反劣），取折叠形封顶。
    if ((unsigned char)(saveInventory ^ 1))
    {
        user->disableSaveInven();
    }
    return 1;
}