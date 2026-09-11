// 0x086802b8（reqHumanCertify4ClearMap.md 反编译全文转写）：
// flag 为真时按 mcap 点数概率（get_rand_int(10000) < point）触发；点数 ≥100.0
// 时按 (100-point/100) 与 0 取大降 stdDropRate（ServiceRestrict(3,1) 受限时）
// 并 DB_SaveUserHandicap 上报（DB 域待接线，暂略）；随后 reqHumanCertify()
// 通过则取消人证并 InsertTimer(0,uid,9,0x3c,key++,−1)。
// b28d-lane5 N封顶: N85 x87比较lowering; N82/P112 槽号分配 | ORIG 0x086802b8 | _ZN5CUser24reqHumanCertify4ClearMapEb
// b28d-PoolCUser P230③ -march=i486 实验读数（2026-09-09，同 _processSpeical… 条）：
//   (a) 现路由 ndiff **97**（140/149）；(b) +i486+mtune=generic ndiff **92**（140/150
//   x87 段改善但未归零，残差主体在 N82 槽阶梯）；(c) 仅 i486 整 TU 崩不可用。
// [IG1 2026-09-10 语义轴落地·删发明防御] ORIG 0x08680395..0x086803ad 调
//   ServiceRestrictManager::isRestricted 前**无** srm!=0 判空（`call Singleton::Get;
//   mov %eax,(%esp); call isRestricted` 直调）⇒ 我方 `srm != 0 &&` 系发明防御，已删；
//   同时去掉具名 srm 局部（ORIG 无该槽，P0/P183）。改后 our_insns 142（ORIG 141）、
//   gap -1（原 -10）、ndiff 94→95（difflib 噪声）、TU 四数不变 806/1684/4/101。
//   余量：N85 x87 比较 lowering（fucompp/fnstsw/sahf vs fucomip/fstp）+ N82 槽阶。
void CUser::reqHumanCertify4ClearMap(bool flag)
{
    // ORIG 0x086802c6：`movzbl -0x1c(%ebp),%eax; xor $0x1,%eax; test %al,%al;
    //   jne <mov $1 岛>` ⇒ 非具名 bool 局部，而是短路链首项取反；且短路求值序为
    //   getPoint() 先（%ebx ← 0x086802e1）、get_rand_int() 后（0x086802ef cmp）。
    //   正向包裹复现 fall-through 主体（P214①/P226）。
    if (!flag || m_mcap.getPoint() > (unsigned int)get_rand_int(0x2710))
    {
        // b28d-solo-CUser p167：ORIG 0x08680312 调 getPoint 3 次（条件
        //   `(double)getPoint() >= 100.0` 与体内 `point = getPoint()` 分开）⇒ 条件直调。
        if ((double)m_mcap.getPoint() >= 100.0)
        {
            // ORIG 0x08680350..0x08680364：`mul $0x51eb851f`(无符号 /100) → `mov $0x64;
            // sub` → 结果直入 max 首实参槽（-0x14），**无额外 (int) 中转槽** ⇒ cast 外移。
            // b28d-CuE 2026-09-11（P0 槽-局部律直证）：ORIG 0x08680337 `mov %eax,-0x2c(%ebp)`
            //   后直接 `mul $0x51eb851f`，**无 -0x10 槽**（point 的具名局部），且
            //   0x0868033x 只有一次 getPoint 调用 ⇒ 原码 point 内联在除法表达式里；
            //   我方旧具名 `unsigned int point` 多占 -0x10/-0x18 并引入 store-load 往返
            //   （我方 -0x10 存 point / -0x18 存 0 / 读回 -0x2c）⇒ 删除具名局部。
            // b28d-CuE 2026-09-11（实参序直证）：ORIG 0x08680367 `movl $0x0,-0x10(%ebp)`
            //   后 0x0868036e `lea -0x14; mov 0x4(%esp)` / 0x08680375 `lea -0x10;
            //   mov (%esp)` ⇒ arg1=&(-0x10)=**0 的临时**、arg2=&(-0x14)=值
            //   ⇒ 原码实参序为 max(**0**, 100 - point/100)（Ghidra `max<int>(local_18+1,
            //   local_18)` 同证）。max 对称 ⇒ 语义等价。
            int rate = std::max(0, (int)(100 - m_mcap.getPoint() / 100));
            if (rate < getStdDropRate()
                && ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                       RestrictCategory::Enum_3, 1))
            {
                    setStdDropRate(rate);
                    // ORIG 0x086803d8..0x08680400：getStdDropRate 结果留 %ebx 直跨
                    // get_acc_id 调用后入实参槽（**无具名 nowRate 槽**）⇒ P183 内联。
                    // 4.4 右→左实参求值顺序使 getStdDropRate 先算，与 ORIG 一致。
                    DB_SaveUserHandicap::makeRequest(get_acc_id(), 0x9c41, 0x65,
                                                     getStdDropRate(), 1, 0);
            }
        }

        if (reqHumanCertify())
        {
            setHumanCertified(false);
            // [L9-GrpE 2026-09-11 语义修复·符号性轴⑤] ORIG 0x0868044e `cmp $0x7ffffffe,%eax`
            //   + 0x08680453 `jbe 08680462` 直证该比较按 **unsigned** 进行（Ghidra C 亦为
            //   `if (0x7ffffffe < *(uint *)(this + 0x8e0f8))`）。重建成员为 int 时 4.4 折成
            //   `cmp $0x7fffffff; jne`（仅 x==0x7fffffff 清零），对 x∈[0x80000000,0xffffffff]
            //   （高位/负位模式）行为与 ORIG 相反 ⇒ 显式 unsigned 比较。
            //   同口径探针 /tmp/L9/E/p_rc.cpp：fA（现形）产 `cmp [..],0x7fffffff; jne`；
            //   fB/fC（本形）产 `mov %eax,..; cmp $0x7ffffffe,%eax; jbe` 与 ORIG 逐条同形。
            if (0x7ffffffe < (unsigned int)m_humanCertifyTimerKey)
                m_humanCertifyTimerKey = 0;
            m_humanCertifyTimerKey += 1;
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, GetUID(),
                                        (TIMER_MESSAGE)9, 0x3c,
                                        m_humanCertifyTimerKey, (unsigned int)-1);
        }
    }
}