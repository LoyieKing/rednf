int CCharacExpertJob::SendExpertJobInfo(CUser* user)
{
    PacketGuard packet;
    packet.put_header(0, 0xcd);
    packet.put_byte(m_giveupCount);
    // [b23S3C] P48 去缓存 + P0 律：ORIG 0x0832b44d/0x61/0xbb 三处独立 call
    // GetCurCharacExpertJobType（put_byte 实参、switch 判定、GetExpertJobScript
    // 实参），判定比较一律走 %eax 无 -off 槽 ⇒ 原码无 jobType 具名局部；
    // 决策树 `cmp $3;je / cmp $3;jg / cmp $1;jl / cmp $4;jne` 为 switch(1,2,3,4)
    // 分派形（旧 `>=1 && <=4` 范围测形不同）。
    packet.put_byte(user->GetCurCharacExpertJobType());
    switch (user->GetCurCharacExpertJobType())
    {
    case 1:
    case 2:
    case 4:
    {
        int count = 0;
        int countIndex = packet.get_index();
        packet.put_byte(count);   // 先占位写 0（ORIG 0xa3 读 -0x18 槽，非立即数）
        // [b23S3C] 第三处独立重取（ORIG 0x0832b4bb→0xc7 结果入 %ebx 作第 2 实参）。
        STExpertJobScript* script = (STExpertJobScript*)G_CDataManager()
                                        ->GetExpertJobScript(
                                            user->GetCurCharacExpertJobType());
        if (script != 0)
        {
            for (int i = 0; i < 0xf0; ++i)
            {
                // b28：清 register asm（N96/P0 全域禁）。ORIG `xor $1/test %al`
                // 为 4.1 !int 直测 lowering（4.4 恒折 test/je），值域 0/1 等价改写。
                // b28c-1：P168a X2 实测占槽（帧 0x30→0x40，ndiff 7→78）⇒ 不可翻。
                if (!get_recipe(i))
                    continue;
                // [b23S3C] P197③/P198①：ORIG 0x0832b556 先 `movl $0x0,-0xc(%ebp)`
                // 再 GetRecipeInfo ⇒ 两句式。
                stRecipeInfo* info = 0;
                info = script->GetRecipeInfo(i);
                if (info == 0)
                    continue;
                // b28d-solo [R2 适配]：STExpertJobScript.h 将 m_pad 拆为真实字段
                // m_field0(+0x00, ulong)；本 TU 按 ORIG 0x0832b57d/0x586 的
                // `mov (%eax),%eax; test %eax,%eax` 4 字节读语义直取该字段。
                if (info->m_field0 == 0)
                    continue;
                packet.put_int(static_cast<int>(info->m_field0));
                ++count;
            }
        }
        packet.put_byte(countIndex, count);   // 回填 recipe 计数
        break;
    }
    case 3:
        packet.put_int(m_enchantCount + 1);
        packet.put_int(m_endurance);
        break;
    default:
        break;
    }
    packet.finalize(true);
    user->Send(packet);
    return 1;
