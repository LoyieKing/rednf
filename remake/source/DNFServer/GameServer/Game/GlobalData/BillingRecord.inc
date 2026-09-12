// ----------------------------------------------------------------------------
// BillingRecord —— ORIG 0x0817991e（0x58f 字节，报告 BillingRecord.md + objdump 转写）。
// 打包 0x10e 字节计费记录（SIG 结构以文件内局部缓冲按 ORIG 偏移写入）后
// put_binary/入 DB 队列；PuUser 标志异常时补 DB_SavePuUserCheck。
// ----------------------------------------------------------------------------
// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x0817991e
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR；残差=N82 帧/槽 + N89 STL 展开。
void ItemVendingMachine::BillingRecord(CUser* user, int a3, int a4, int a5,
                                       const char* itemName, const char* accName,
                                       int priceLabel, unsigned long long uid,
                                       char payFlag)
{
    // ORIG 0x08179944..0x08179973：Acquire 结果直接作 CStreamGuard ctor 实参
    // （`call Acquire; movl $0x1,0x8(%esp); mov %eax,0x4(%esp); lea -0x14; ...`），
    // 无 stream 具名局部（P259：缓存局部 +1 store/+1 load 且槽整体平移）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("localjapan/AppendVendingMachine.cpp",
                                           0x730),
        true);
    // ORIG BillingRecord.md：operator*() 各 << 前重取 + operator<<(int)（P48/CStreamGuard 形态）
    (*guard.operator*()) << (int)0xb0;           // SIG opcode（operator*() 取 this）
    (*guard.operator*()) << (int)user->GetUID();

    // 0x10e 字节记录（B = ORIG 帧 -0x13b(%ebp)，memset 0x10e 实证；
    // 以下偏移全部按 ORIG 汇编 0x0817991e 逐条核对）
    SIG_BILLING_RECORD rec;
    memset(&rec, 0, sizeof(rec));
    // ORIG 0x081799d5：strcpy(B+0x00, 0x1c(%ebp)) —— 第 5 实参 itemName；
    // 账号名（get_acc_name）写 B+0x35，被举报/支付名 accName 写 B+0x69。
    strcpy(rec.m_agencyNo, itemName);                             // B+0x00
    strcpy(rec.m_userId, user->get_acc_name());                   // B+0x35（char[0x19]）
    rec.m_sealFlag = 1;                                           // ORIG -0x126
    rec.m_saleTime = a5;                                          // ORIG -0x122 = 0x18(%ebp)
    rec.m_price = 0;                                              // ORIG -0x11e
    rec.m_slotPartType = 1;                                       // ORIG -0x11a
    rec.m_slotPartCode = a3;                                      // ORIG -0x116 = 0x10(%ebp)
    rec.m_ipgNo = a4;                                             // ORIG -0x112 = 0x14(%ebp)
    rec.m_mId = user->get_acc_id();                               // ORIG -0x10e
    rec.m_characNo = user->getCurCharacNo();                      // ORIG -0x10a
    rec.m_job = user->get_charac_job();                           // ORIG -0xed

    // SSN 拆分 → 生日串（B+0x56 char[11]）与世纪类型（B+0x52）
    // [FL7GlobalData 2026-09-10 · 语义轴/P0] ORIG 0x08179a91..0x08179aae：
    //   `mov 0xc(%ebp),%eax; mov %eax,(%esp); call GetSsnString;
    //    movl $0xe,0x8(%esp); mov %eax,0x4(%esp); lea -0x22; call strncpy`
    // ⇒ 原码**无 ssn 具名局部**（返回值 %eax 直传实参位，零槽往返）、
    // 且**无 test/je 判空**。我方旧版两处均发明（`const char* ssn = ...` 占 -0x10 槽
    // 致全帧 -4 平移 + null 检查多 2 条）⇒ 删除（P140/P171 无防御照抄 + P0 右值直传）。
    char ssnPart[0xe];
    memset(ssnPart, 0, sizeof(ssnPart));
    strncpy(ssnPart, user->GetSsnString(), 0xe);
    // ORIG 汇编：yy/mm/dd 为 char[3]（memset 3 字节保证 %s 终止符），cent 为 char[2]。
    // [FL7GlobalData 2026-09-10 · P212 槽序/语句序] ORIG 0x08179ab3/ace/ae9/b04 的 memset
    // 执行序为 centBuf(-0x24)→yy(-0x27)→mm(-0x2a)→dd(-0x2d)（槽由浅到深）、
    // strncpy 序 0x08179b1f/b3c/b56/b73 = centBuf(ssn+6,1)→yy(ssn+0,2)→mm(ssn+2,2)→dd(ssn+4,2)
    // ⇒ 原码声明序 centBuf,yy,mm,dd（4.1/4.4 同向：先声明=更浅）。旧序把 centBuf 放最后
    // 致 yy/mm/dd 槽整体 +2 且 memset/strncpy 块序反（ndiff 108 主因）。
    char centBuf[2];
    memset(centBuf, 0, 2);
    char yy[3], mm[3], dd[3];
    memset(yy, 0, 3); memset(mm, 0, 3); memset(dd, 0, 3);
    // 汇编 strncpy 布局：yy←ssn+0(2)、mm←ssn+2(2)、dd←ssn+4(2)、cent←ssn+6(1)
    // ORIG strncpy（非 memcpy）拆分 centBuf/yy/mm/dd
    strncpy(centBuf, ssnPart + 6, 1);
    strncpy(yy, ssnPart + 0, 2);
    strncpy(mm, ssnPart + 2, 2);
    strncpy(dd, ssnPart + 4, 2);
    // [L8 2026-09-10 判据⑫/T1 直证] ORIG 0x08179bc0 段用 `movsbl -0x9(%ebp),%eax`
    // 读该局部（1 字节 + 符号扩展）后 `cmp $0x2/%0x1/%0x3/%0x4`，而我方 int 版产
    // 4 条 `cmpl $K,-0xc(%ebp)`（widthscan：cmp_l orig=0 our=5、ext_movsbl orig=2 our=0）
    // ⇒ ORIG 的 cent 是 **char**（atoi 返回值按 char 存储）。
    char cent = atoi(centBuf);
    // ORIG 0x08179bcd 起：sprintf 直接写入记录 B+0x56（无独立局部缓冲），
    // 世纪类型写 B+0x52。
    // [FL7GlobalData 2026-09-10 · P46/P286] ORIG 0x08179b9e 段是 **switch(cent)** 决策树：
    //   `movsbl -0x9(%ebp),%eax`（char 形参整型提升到 int）`cmp $0x2,%eax; je L2;
    //    cmp $0x2,%eax; jg Lhi; cmp $0x1,%eax; je L1; jmp default; Lhi: cmp $0x3/%0x4 ...`
    // 我方 if/else 链产 4×`cmpb $K,-0x9(%ebp); jne`（无提升、无决策树）⇒ 按 P324
    // （叶块物理序=书写序：ORIG 叶块地址 L1<L2<L3<L4）以升序 case 改写。
    switch (cent)
    {
    case 1:
        sprintf(rec.m_birthday, "19%s-%s-%s", yy, mm, dd);
        rec.m_sex = 1;
        break;
    case 2:
        sprintf(rec.m_birthday, "19%s-%s-%s", yy, mm, dd);
        rec.m_sex = 2;
        break;
    case 3:
        sprintf(rec.m_birthday, "20%s-%s-%s", yy, mm, dd);
        rec.m_sex = 1;
        break;
    case 4:
        sprintf(rec.m_birthday, "20%s-%s-%s", yy, mm, dd);
        rec.m_sex = 2;
        break;
    }

    rec.m_prizeFlag = 1;                                   // ORIG -0xda
    rec.m_resultCode = 0;                                  // ORIG -0xd6
    // ORIG 0x08179cca：strcpy(B+0x69, 0x20(%ebp)) —— 第 6 实参 accName。
    strcpy(rec.m_resultMessage, accName);
    rec.m_serverId = G_CEnvironment()->m_serverEnvir.m_serverGroup;  // -0x6e
    // [H7PartyGlobal 2026-09-11 · P11/T2 调用点收窄] ORIG 0x08179cfb `movsbl %al,%eax;
    // mov %eax,-0x6a(%ebp)`——返回值按 **signed char 符号扩展**后 4B store；旧形
    // `movzbl` 在 growType>0x7f 时给 0x80..0xff 正值，与 ORIG 负值语义不同（真语义差）。
    // 只收窄本消费点，不动共享头返回类型（② 依 ① 教训改走窄修复，避免全 TU 劣化）。
    rec.m_growType = (signed char)user->getCurCharacGrowType();   // -0x6a
    rec.m_level = user->get_charac_level();                // -0x66
    rec.m_giftServerInfo = rec.m_serverId;                 // -0x62
    rec.m_giftGetCharacNo = 0;                             // -0x5e
    rec.m_chargePrice = rec.m_saleTime;                    // -0x5a = a5
    rec.m_freePrice = 0;                                   // -0x56
    rec.m_countNo = priceLabel;                            // -0x52 = 0x24(%ebp)
    rec.m_eachPrice = rec.m_saleTime;                      // -0x4e = a5
    // strcmp(accName,"mileage item"/"cera point")——ORIG 0x8179d49 源=0x20(%ebp) 实证
    if (strcmp(accName, "mileage item") == 0)
        rec.m_priceType = 2;
    else if (strcmp(accName, "cera point") == 0)
        rec.m_priceType = 3;
    else
        rec.m_priceType = 1;

    // ORIG 0x08179d82：memset(B+0xf5,0,0x10) → GetPeerIP2(B+0xf5,0x10)，
    // 失败（返回 0）时 strncpy(B+0xf5,"NONE",5)。
    memset(rec.m_userIp, 0, 0x10);
    if (user->m_network.GetPeerIP2(rec.m_userIp, 0x10) == 0)
        memcpy(rec.m_userIp, "NONE", 5);
    // [FL7GlobalData 2026-09-10 · u32 双字视图实测否定] 试
    // `(int)((const unsigned int*)&uid)[0/1]` 后我方反多 `lea`×2 + `add`（ndiff 12→14）
    // ⇒ 取址视图不可用；ORIG 0x08179e2x 的两段直读
    // （`mov -0x150(%ebp),%eax; mov -0x14c(%ebp),%edx; mov %eax,-0x36; mov %edx,-0x32`）
    // 对 4.4 的 `uid >> 32`（多 `mov %edx,%eax; xor %edx,%edx`）不可复现，维持原形记 N92 族。
    // [K8GlobalData 2026-09-11 · 负结论，勿重烧] ORIG 0x08179e2x 的 8 字节直拷
    // （`mov -0x150,%eax; mov -0x14c,%edx; mov %eax,-0x36; mov %edx,-0x32`）实测
    // **不可由本 TU 源码形复现**：`memcpy(&m_tranNoLow,&uid,8)` 在 4.4 -O0 下
    // 退化为 **call memcpy**（`movl $0x8,0x8(%esp); lea -0x150; lea -0x13b+0x105;
    // call`）⇒ ndiff 10→**14**（328 条）。结合已作废的 `((const unsigned int*)&uid)[i]`
    // 视图（反多 lea×2+add），维持两段 int 赋值原形（ndiff 10：`uid>>32` 多
    // `mov %edx,%eax; xor %edx,%edx` 两条 + GetPuUser 处 1 条 sete）⇒ 记 N92/P22。
    rec.m_tranNoLow = (int)(unsigned int)uid;                    // ORIG -0x36 = 0x28(%ebp)
    rec.m_tranNoHigh = (int)(unsigned int)(uid >> 32);           // ORIG -0x32 = 0x2c(%ebp)
    rec.m_buyType = payFlag;                                     // ORIG -0x2e = 0x30(%ebp)

    guard.operator->()->put_binary(reinterpret_cast<char*>(&rec), 0x10e);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);

    // PuUser 标志：ORIG 08179e56..08179e73（BillingRecord.md）`call GetPuUser;
    // xor $0x1,%eax; test %al,%al; je skip` → 非 1（未认证）时补存（P22 bool 取反
    // xor 形态；原实现判定极性相反，按报告修正）。
    // [K8GlobalData 2026-09-11 · 类型轴 N（K4CUser 裁决）] 本条残差 1 条 sete
    // （我方 `test %al,%al; sete %al` vs ORIG `xor $0x1,%eax`）**禁止**通过改
    // `CUser::GetPuUser()` 返回类型消除：ORIG 本体 0x08692b74 为
    // `mov 0x8(%ebp),%eax; movzbl 0x796d8(%eax),%eax; ret`（无 testb/setne，生产端
    // 形同 uchar 直返），且探针（4.4.7 -m32 -O0 /tmp/pu_probe.cpp）证 `bool g(S*)
    // { return s->f; }`（f=unsigned char）恒 +2 条 ⇒ 改 bool 反使 identical 掉级；
    // 字段亦不可 bool 化（InterDispatcher.cpp:18163 可写 >1）。
    // [R4a01 2026-09-12 · 第四轮 SEMANTIC-DIFF 修复] 旧写 `!user->GetPuUser()` 在
    // 返回类型 uchar 下降为 `test %al,%al; sete %al; test %al,%al; je` = **v==0 才补存**；
    // ORIG 0x08179e56..0x08179e66 为 `call; xor $0x1,%eax; test %al,%al; je 0x8179e98`
    // = **v!=1 才补存**（定性 (b)：ORIG 本体 0x08692b74 为 uchar 直传无归一化 ⇒ 值域 0..255，
    // 由 Inter_LoadPuUserCheck 0x084e7ff1 `movzbl (%eax),%edx; mov %dl,0x796d8(%eax)` 直写任意值）
    // ⇒ v∈2..255 时 ORIG 发 DB_SavePuUserCheck::makeRequest 而旧形跳过（行为分叉）。
    // 按 ORIG 可观察语义改写为 `!= 1`（Main 第四轮裁定：只改消费点，不动 GetPuUser 本体）。
    if (user->GetPuUser() != 1)
        DB_SavePuUserCheck::makeRequest(user->get_acc_id());
}