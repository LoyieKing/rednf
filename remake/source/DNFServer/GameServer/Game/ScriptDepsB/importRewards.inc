// ============================================================================
// importRewards —— ORIG 0x08aa8b79
// 报告 _lt_global_gt_/importRewards.md。appendage 命中→stRewardChangeStatus；
// 否则 [give emotion]→stRewardGiveEmotion；[send postal]→stRewardPostal；
// 其余令牌无副作用。恒返回 true（eax 流出）。
// 扫描顺序逐条对齐汇编：change=status 先存 appendage 再 Str/ValueType/Float；
// postal 先两个 int(+0x18/+0x1c) 后三个 string(+0xc/+0x10/+0x14)。
// ============================================================================
// [b28d-solo-ScriptDepsB AE-语义 / N(P197④) 形态封顶 | ORIG 0x08aa8b79]
//  修法：else-if 链 + 单一 return 1（typeText 公共尾块析构）；appendage 异型化
//  （unsigned）翻 store RHS-first（P211）；postal 分支照抄 ORIG -0x20 预清死局部。
//  首条件用 while+goto 复现 ORIG 槽比物化（P225 实证）：4.4 对 `if(槽!=K)` 恒
//  直跳（je 1 条），对 **while 条件**才发 `cmpl $K,槽; setne; test; je` 同形；
//  body 末尾 goto 公共尾（等价 break 后跳尾）⇒ 与 ORIG 0x08aa8ba4/0x08aa8c10 逐条
//  一致（探针 /tmp/sdb_probe/p1.o f_while_goto）。
//  D1 虚调目标（旧 N7）已由 CTitleBook.h 指针算术形修复。
int importRewards(std::vector<stTitleRewardBasis*>* out, std::string token)
{
    std::string typeText;
    const unsigned int appendage =
        static_cast<unsigned int>(loadAppendageType(token));

    // ORIG 0x08aa8ba4: cmpl $-1,槽; setne %al; test %al,%al; je <else>
    while (appendage != static_cast<unsigned int>(-1))
    {
        stRewardChangeStatus* reward = new stRewardChangeStatus();
        reward->m_appendage = appendage;                    // +0x0c
        ScanStr(&typeText);
        reward->m_valueType = loadValueType(typeText);      // +0x10
        reward->m_value = ScanFloat((bool*)0);              // +0x14（fstps）
        out->push_back(reward);
        goto rewardsTail;                                   // ORIG 0x08aa8c10 jmp 尾
    }
    if (token == "[give emotion]")
    {
        stRewardGiveEmotion* reward = new stRewardGiveEmotion();
        reward->m_emotionId = ScanInt((bool*)0);            // +0x0c
        out->push_back(reward);
    }
    else if (token == "[send postal]")
    {
        int deadPostal = 0;
        stRewardPostal* reward = new stRewardPostal();
        reward->m_itemIdx = ScanInt((bool*)0);              // +0x18
        reward->m_addInfo = ScanInt((bool*)0);              // +0x1c
        ScanStr(&reward->m_sender);                         // +0x0c
        ScanStr(&reward->m_subject);                        // +0x10
        ScanStr(&reward->m_message);                        // +0x14
        out->push_back(reward);
    }
rewardsTail:
    return 1;
}