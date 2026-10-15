// ---- 发信 ----
// [b28d-L8] P75/N82 | 帧差 0x28 vs 0x30 + 实参装载序（strncpy/memcpy 参数，ORIG 0x08555756）
// [b28d-solo-CMailBoxHelper] 修法（DIFF 153→14 行，156→145 insn）：
// ①删 `InvenRandBlobV const&` 引用局部（多指针槽 + 每处解引用）改
//   `InvenItemRandTailV`（0x25 进 pad）直接 cast item 指针 ⇒ disp 折叠
//   （P183/P311）；
// ②m_isCreature 直存（ORIG `mov %al,0x58`）、m_isAvatarSend 写 bool 表达式
//   （ORIG `sete %dl; mov %dl,0x56`），删两处三元 ?1:0（-8 条）。
// 残余 N：4.1 跨语句寄存器缓存（ORIG item 指针载一次 %edx 连续读
// +0x25/+0x29/+0x2d/+0x31、msg 载一次 %eax 连续写；4.4 每句重载，
// N7/P75/P183 族）+ m_upgradeSep 段 msg 地址跨 call 装 %ebx
// （ORIG call 后 `mov 0x8(%ebp),%edx`，N76/P211 档②实测不翻）。
// [本波 2026-09-09 P247] 净修复 gap -14→-4（ORIG 131 / 我方 135）：
//  ①+0x3a..+0x43 三标量 → `stAmplifyGuidView m_guid`，源码
//    `m_guid = item.m_amp.m_guid` ⇒ 结构赋值复现 ORIG 0x085557e0 段
//    「双基址各载一次 + int/int/word 连续 store」（三独立字段 4.4 恒逐条重载）；
//  ②+0x47..+0x54 四标量 → `RandomOption m_rand`，源码
//    `m_rand = item.m_random.m_random` ⇒ 复现 ORIG 0x0855581c 段
//    （探针 /tmp/ms_probe.cpp /tmp/ms_probe2.cpp 逐条全等）；
//  ③std::min 实参序改 `(lens[1], lens[0])`（ORIG 0x085558b3 压栈序；min 交换律等价）。
//  残余 4 条 = 4.4 在 4 处 `msg->f = call()` 统一「call 前装 %ebx 保活 msg 地址」
//  （ORIG 4.1 恒 call 后 `mov 0x8(%ebp),%edx` 重载）；单语句探针
//  /tmp/ebx_probe{,3,4,5,6}.cpp 两侧均无 ebx ⇒ 本函数形状下 4.4 特有分配；
//  字段异型化 char/short（P211 档③/P248 档②）实测 ebx 数量不变 ⇒ N7/P75 封顶。
void CMailBoxHelper::_makeSendMailData(SIG_MAILBOX_SEND_NEW_MAIL* msg,
                                       const char* pSender, int nSenderCharacNo,
                                       Inven_Item const& item, unsigned int nDestCharacNo,
                                       unsigned int nAmount, const char* pTitle,
                                       int nTitleLen, unsigned int nB, int nType)
{
    // ORIG 0x08555756 逐字段转写。不缓存 msg/item 局部（O0 落栈槽导致帧差），
    // 每字段经宏直读参数槽，同 ORIG `mov 0x8(%ebp),%eax` 形态。
    // [b28d-solo-CMailBoxHelper] register 提示试验（/tmp/probe_reg.cpp g 形）：
    // 全 register 版 108 insn（少 ORIG 23 条，ORIG 仅段内 CSE 缓存），已回退。
    // [2026-09-09 Lane20] SIG_MAILBOX_SEND_NEW_MAIL 已在 SigTypes_D.h 字段化
    // （§8.3，sizeof 0x168/偏移/mangled 不变）⇒ 直接用形参具名类型访问，
    // 消掉赋值点的视图 cast（cast 会使 4.4 提前把 msg 装 callee-saved 保活：
    // ORIG 0x085557xx 各点均为 `call; mov 0x8(%ebp),%edx; mov %eax,disp(%edx)`）。
#define MAIL_MSG msg
#define MAIL_IT  (&item)
    std::strncpy(MAIL_MSG->m_senderName, pSender, 0x1d);
    MAIL_MSG->m_amount = nAmount;
    MAIL_MSG->m_senderCharacNo = nSenderCharacNo;
    MAIL_MSG->m_destCharacNo = nDestCharacNo;

    MAIL_MSG->m_seal = *reinterpret_cast<const unsigned char*>(&MAIL_IT->m_seal);
    MAIL_MSG->m_addInfo = MAIL_IT->m_addInfo;
    MAIL_MSG->m_addInfo2 = MAIL_IT->m_addInfo2;
    MAIL_MSG->m_endurance = MAIL_IT->m_endurance;
    MAIL_MSG->m_itemAttr = MAIL_IT->GetItemAttr();
    MAIL_MSG->m_extendInfo = MAIL_IT->m_extendInfo;
    // item+0x15/0x19/0x1d = amp guid（ORIG 0x085557e0 段：双基址各载一次 +
    // int/int/word 连续 store ⇒ 结构赋值，P247；三独立字段赋值 4.4 恒逐条重载）。
    MAIL_MSG->m_guid = MAIL_IT->m_amp.m_guid;
    MAIL_IT->m_amp.getValues(MAIL_MSG->m_ampType, MAIL_MSG->m_ampIdx);
    // RandomOption（item+0x25，0xe 字节）：ORIG 0x0855581c 段同型 ⇒ 结构赋值
    // （探针 /tmp/ms_probe2.cpp 实测 4.4 展开 = int/int/int/word + 双基址各载一次）。
    MAIL_MSG->m_rand = MAIL_IT->m_random.m_random;
    MAIL_MSG->m_upgradeSep = MAIL_IT->m_upgradeSep.GetUpgradeSeparate();
    // ORIG 0x085558xx：m_isCreature 直存（`mov %al,0x58`）、m_isAvatarSend
    // 为 bool 表达式（`sete %dl; mov %dl,0x56`）——三元 ?1:0 会多出 test/je 物化。
    MAIL_MSG->m_isCreature = MAIL_IT->IsCreatureItemType();
    MAIL_MSG->m_isAvatarSend = (MAIL_IT->m_itemType == 8);

    MAIL_MSG->m_titleLen = 0;
    std::memset(MAIL_MSG->m_title, 0, 0x100);
    if (pTitle != 0 && nTitleLen != 0)
    {
        // [2026-09-09 Lane20] ORIG 0x085558b3：`&lens[1]` 直接 `lea
        // -0xa(%ebp),%eax` 单条、`&lens[0]` 为 `lea -0xc(%ebp)`；数组成员
        // 取址在 4.4 恒折 `lea base; add $2` 两步（探针 /tmp/probe_cmh/p4.cpp
        // f1 vs f2）⇒ 改**两个独立标量**（4.4 对独立标量取址单条 disp）。槽位
        // 对齐 ORIG（lens[1]@-0xa 浅 / lens[0]@-0xc 深）⇒ 先声明 lens1；
        // 赋值语句序保持 lens0 先（ORIG `mov %ax,-0xc` 先于 `movw $0xff,-0xa`）。
        unsigned short lens1;
        unsigned short lens0;
        lens0 = (unsigned short)nTitleLen;
        lens1 = 0xff;
        MAIL_MSG->m_titleLen = std::min(lens1, lens0);
        std::strncpy(MAIL_MSG->m_title, pTitle, MAIL_MSG->m_titleLen);
    }
    MAIL_MSG->m_nB = nB;
    MAIL_MSG->m_nType = nType;
    MAIL_MSG->m_flag = 0;
#undef MAIL_MSG
#undef MAIL_IT
}