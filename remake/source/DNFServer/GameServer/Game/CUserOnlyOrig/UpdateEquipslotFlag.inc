unsigned char CUser::UpdateEquipslotFlag(char flag)   // P9：ORIG 两处 getCur
                                                      // call 后无 movzbl ⇒ uchar
{
    // ORIG 0x0868aa6b：`cmp $0x2c,%eax; setg %al; test %al,%al; je ae3(mov$0尾岛)`。
    // [b28d-solo-CUserOnlyOrig P0/P226] ORIG 无 hi 槽（旧具名 bool 生 -0x9 槽 +
    // cmpb 回读、帧 0x38 vs ORIG 0x28）⇒ 内联条件。
    if (get_charac_level() > 0x2c) {
        if (flag == 0)
        {
            // ORIG 0x0868aa86：`cmp $0x2,%al` 字节宽比较 ⇒ 截断操作数
            if ((unsigned char)getCurEquipslotFlag() == 2)
                return 2;
            setCurEquipslotFlag(1);
            return getCurEquipslotFlag();
        }
        if (flag == 2)
        {
            setCurEquipslotFlag(2);
            return getCurEquipslotFlag();
        }
        return 0x13;
    }
    return 0;
