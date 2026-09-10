void CUser::SendRevengeDungeon()
{
    // ORIG 0x08656acf `sete %al; test; jne END` ⇒ 正向 ==0 早退（技巧6/9）
    if (getCurCharacR() == 0)
        return;
    // ORIG 0x08656aec lea -0x22：10B 记录 {type@0,flag@1,remain@2,reserved@6}（pack 1）
    //（pack(1) ⇒ 4.4 按 align 1 紧排，槽基址 -0x22 对齐 ORIG）
