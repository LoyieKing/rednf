// ---- SendExpertJobInfo（报告，ORIG T 0x0832b454 sz 0x201）----
// b28d-solo-ExpertJobOnlyMgr：N 封顶（P197④/N-88 族，语义等价）。
// ORIG 0x0832b572 `cmpl $0,-0xc; sete %al; test %al,%al; jne <continue 岛>` 系
// 4.1.2 槽比零 setcc 物化；4.4.7 对 if/while 条件恒折直跳 `je`（-3 条）。
// 本波实测旋钮：①P194 具名 bool（落槽 -0x9 ⇒ 全帧重排，劣化）；
// ②P225 while+break（产镜像 setne/je 且少 1 岛，劣化）；③P226 语句表达式
//（回退基线 je）；④内联比较 = 双调用真 bug（P197④ 禁）。其余逐条全等：
// switch(1,2,3,4) 决策树、三 continue 岛（nop;jmp;nop;jmp;nop）、槽位集
//（-0x18/-0x28/-0x14/-0xc/-0x10）与全部调用点。唯一残差 = 该 3 条 setcc 物化。
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
                // [L3Free25to36 2026-09-11 负结论] 本件唯一残差 = ORIG 0x0832b57d
                // `cmpl $0x0,-0xc(%ebp); sete %al; test %al,%al; jne <continue>`（3 条，
                // 指针 ==0 的**未折** lowering）vs 4.4.6 恒 `je <continue>`（1 条）。
                // 试形：`if (!info)`（P168 族）实测同产 `je`，零变化；同 TU 内 ORIG 对
                // `p != 0`（OnEnchantByBead 0x0849ed52 `cmpl $0; jne`）**折**直跳
                // ⇒ 4.1 对 `ptr == 0` 独有的 setcc 物化，无源形可造，记 N96 附。
                stRecipeInfo* info = 0;
                // [R25a06 2026-09-13 · §31 P352 试探] ORIG 0x0832b572 为
                //   `cmpl $0x0,-0xc(%ebp); sete %al; test %al,%al; jne <continue>`
                //   三条 setcc 物化（4.1 对「赋值内嵌条件」的 lowering）；旧两句式
                //   （info=…; if(info==0)）4.4 折成单条 `je`。P352：`if ((lv = f()) == X)`
                //   一句写可令 4.4 复现 setcc 物化（本形 ==0 ⇒ sete）。
                if ((info = script->GetRecipeInfo(i)) == 0)
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
}