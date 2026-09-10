bool CUser::exceptedStateMoneyLog()
{
    // ORIG T 0x868f3ba 全录（报告缺失，objdump 71 insn 转写）：GetParty() 每使用点
    // 重调共 4 次（0x868f3c6/3da/3fd/417，无具名槽，T-CU15）；条件全走 4.1 bool
    // 物化 `test;setne %al;test %al,%al;je`（技巧 22 register bool al）；m_89e
    // uchar `movzbl 0x89e;test %al` 免物化直判；get_state `cmp $0x8,%eax; sete`、
    // getCurCharacVill `cmp $0x7,%al` 8 位形（callee uchar 返回）。bf 读保留
    // CParty+0xb24 视图（§4.3 具名，0xc4c 单级系 4.1 复合折叠，4.4 两段为 N）。
    // b28d-solo-CUser AE: ORIG 0x868f3ba 全段 `setne %al; test %al,%al; je` / `movzbl
    // 0x89e; test %al,%al` 均无槽（sub $0x18）；具名 gp/bfNZ/m89e/st8/vil7 各落 -0x9..-0xd
    // 使帧 +0x10 ⇒ 全部内联条件（probe_bool a3/a5）。GetParty() 每使用点重调（T-CU15）。
    if (GetParty() != 0)
    {
        if ((*(&((CParty*)GetParty())->m_battleField)).m_villageMonsterId != 0)
            return true;
        if (((CParty*)GetParty())->m_battleField.m_pDungeon != 0)
        {
            // b28d-PoolCUser 定性（2026-09-09，Main 裁决：字段**保持 uchar**，驳回
            //   bool 化提案）。决定性 producer 直证：ORIG
            //   _ZN8CDungeon11set_dungeonER15STDungeonScript @0x0834b39b..0x0834b3a5
            //   = `movzx edx,BYTE [eax+0x28d]` → `mov BYTE [eax+0x89e],dl` 裸逐字节
            //   搬运（若 bool，4.1 必产 test+setne 再存）；全二进制 +0x89e 消费普查
            //   15 处 = 14× `movzx BYTE[..+0x89e]` + 1× 上述 store，零处 `test BYTE
            //   PTR` ⇒ ORIG 本调用点 `test %al,%al; je`（0x0868f429 后）只是 movzx
            //   装载后的寄存器直测（uchar 同形），**不构成 bool 证据**（P321 T3 修正）。
            //   残差真实成因 = **跨版本 uchar 测试物化 lowering 差**：4.1 直分支
            //   `movzbl;test;je` 三段，4.4 恒插 `setne %al` 四段（探针 probe_uc2/
            //   uc3/uc4 三形实测不可复现；return 1/!=0/扁平 goto 均无效）——按
            //   N7/N18b 装载-物化族封顶。
            // b28d-solo-CUser 试形：uchar 字段显式 `!= 0`（P292 反向探测；ORIG
            //   `movzbl 0x89e; test %al,%al; je` 无 setne，字段不可 bool 化——
            //   CParty.cpp 多处 `!= 1` 消费依赖 >1 值域）。
            // b28d-solo-CUser 试形（P292 档②裸 if）：ORIG 0x868f429 `movzbl 0x89e; test %al,%al; je`
            // 无 setne ⇒ 源形是裸 if(字段)（非 `!= 0` 显式比较）。
            if (((CParty*)GetParty())->m_battleField.m_pDungeon->m_89e)
                return true;
        }
    }
    if (get_state() == 8)
        return true;
    if (getCurCharacVill() == 7)
        return true;
    return false;
