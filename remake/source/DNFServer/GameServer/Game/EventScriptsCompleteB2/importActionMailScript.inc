// ============================================================================
// C. importActionMailScript —— T 0x0897f147（1656 字节）
//    [objdump·报告缺失] 转写。标签：[inven]/[job](8 职业)/[detail]/[/send mail]。
// ============================================================================
// ---- importActionMailScript T 0x0897f147（1656B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importActionMailScript.md（双段
// 完整阅读）。此函数主循环用 token 串比较链（非 __lookup 表——ORIG 无
// 惰性建表段，实证汇编 +0x4a 起即 operator== 链）。[job] 节内循环：
// ScanType 失败跳回主循环头（+0x0bb）；每轮先默认构造 pair<int,int> kv
// （-0xbc/-0xb8 跨职业共享，+0x0c9 在比较链之前），命中职业则
// job(-0xb4)=立即数 + kv.first/kv.second = ScanInt(&ok)×2 +
// value_type(job,kv) 引用实参构造 + map::insert（P134 嵌套形）。
// [detail] coin 校验为 signed char <0 || 15< 双段（js/jle，+0x5e6/+0x5f1）。
// 残差归因（N 类）：[job] 8 职业分支已按 ORIG 物理复制+具名 entry/res 独立
// 槽（计数 378/380），但 4.4 栈槽分配器按 SSA 序落位（token 在浅端 -0xc，
// ORIG 4.1 在深端 -0xac），全函数槽号文本错位不可由声明序翻转复现（P112）；
// 比较链 je/jne 朝向为 setcc-fold 族。
bool importActionMailScript(Action_SendMail* mail)
{
    // b28d-solo-EventScriptsCompleteB2 复扫（2026-09-09）：原判 DIFF/N82+N95。
    // 本轮修复：外层 `while (ScanType(...))` 改 `for(;;)+if(!ScanType) return
    // false`，复现 ORIG 否定早退形（+0x1a jmp / +0x23 xor;test;je body）⇒
    // gap 2→0、ORIG/OURS 助记符差集清零、DIFF→NEAR（p167 CLEAR(59/59) 不变）。
    // 残差仅全帧槽号镜像（ORIG token -0xac 先声明深、8 组 value_type/
    // insert-sret 临时 -0x14..-0xa8；4.4 token -0xc 先声明浅且按 lifetime
    // 复用块内临时）⇒ N82/N95；语义逐分支与 ORIG 汇编一致。
    // 2026-09-10 复验：clean 380/380（**逐条长度全等**，仅槽号文本差），
    // 判据① 全 TU 分支方向差 = 0（本函数无方向/落点差）⇒ 判 NEAR(N82)。
    std::string token;
    bool ok = false;

    for (;;)
    {
        if (!ScanType(token, true))
            return false;

        if (token == "[inven]")
        {
            // +0x064 ScanInt(&ok)，字节存 +0x1c。
            mail->m_field1c = static_cast<unsigned char>(ScanInt(&ok));
        }
        else if (token == "[job]")
        {
            for (;;)
            {
                if (!ScanType(token, true))
                    break;                      // +0x0bb → 主循环头
                // P160 逐指令：ORIG 8 职业分支物理复制，每分支 value_type 临时与
                // insert sret 各占独立槽（frame 0xd0；-0xa0/-0x8c/../-0x14 与
                // -0xa8/-0x94/../-0x1c）。4.4 只为具名变量固定分槽，故逐分支具名。
                std::pair<int, int> kv;         // +0x0c9 每轮默认构造（槽共享）
                int jobKey;                     // -0xb4（各分支共写同槽）
                typedef std::map<int, std::pair<int, int> > JobMap;
                if (token == "[swordman]")          // +0x0df
                {
                    jobKey = 0;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry0(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res0 =
                        mail->m_jobMap.insert(entry0);
                    (void)res0;
                }
                else
                if (token == "[fighter]")          // +0x174
                {
                    jobKey = 1;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry1(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res1 =
                        mail->m_jobMap.insert(entry1);
                    (void)res1;
                }
                else
                if (token == "[gunner]")          // +0x209
                {
                    jobKey = 2;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry2(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res2 =
                        mail->m_jobMap.insert(entry2);
                    (void)res2;
                }
                else
                if (token == "[mage]")          // +0x295
                {
                    jobKey = 3;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry3(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res3 =
                        mail->m_jobMap.insert(entry3);
                    (void)res3;
                }
                else
                if (token == "[priest]")          // +0x321
                {
                    jobKey = 4;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry4(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res4 =
                        mail->m_jobMap.insert(entry4);
                    (void)res4;
                }
                else
                if (token == "[atgunner]")          // +0x3ad
                {
                    jobKey = 5;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry5(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res5 =
                        mail->m_jobMap.insert(entry5);
                    (void)res5;
                }
                else
                if (token == "[thief]")          // +0x439
                {
                    jobKey = 6;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry6(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res6 =
                        mail->m_jobMap.insert(entry6);
                    (void)res6;
                }
                else
                if (token == "[atfighter]")          // +0x4c5
                {
                    jobKey = 7;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry7(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res7 =
                        mail->m_jobMap.insert(entry7);
                    (void)res7;
                }
                else
                {
                    if (token == "[/job]")
                        break;                  // +0x55e -> 主循环头
                    // 未知 token：自然回节内头
                }
            }
        }
        else if (token == "[detail]")
        {
            mail->m_field18 = 0;                                   // +0x584
            mail->m_itemIdx = ScanInt(&ok);                        // +0x20
            mail->m_addInfo = ScanInt(&ok);                        // +0x24
            mail->m_gold    = ScanInt(&ok);                        // +0x28
            mail->m_coin    = static_cast<unsigned char>(ScanInt(&ok));  // +0x2c
            // +0x5e6 js / +0x5ef cmp 0xf jle：signed char 出界清零。
            if (static_cast<char>(mail->m_coin) < 0 ||
                static_cast<char>(mail->m_coin) > 15)
            {
                mail->m_coin = 0;
            }
            ScanStr(&mail->m_field30);                             // 标题 key
            ScanStr(&mail->m_field34);                             // 正文 key
        }
        else if (token == "[/send mail]")
        {
            return true;                        // +0x639 mov $1,%ebx
        }
    }
    return false;                               // ScanType EOF：+0x40 mov $0,%ebx
}