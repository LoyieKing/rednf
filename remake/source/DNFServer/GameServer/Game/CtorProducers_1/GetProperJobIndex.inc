int CLevelRewardTable::GetProperJobIndex(char job)
{
    // ORIG 0x08687e78（83 字节）逐条：0x08687e84 `cmpb $0,-0xc; jns` → -1 岛；
    // 0x08687e9c `xor $1,%eax; test; je` → 0 岛（技巧 1）；
    // 0x08687eaa `movsbl -0xc,%eax; cmp $7,%eax; ja 0x08687ef4` +
    // 0x08687eb3 `mov 0x8cf10cc(,%eax,4),%eax; jmp *%eax` —— **8 路跳表**
    // （case 0..7 依次 mov $1..$8; jmp end，default 落 0x08687ef4 `mov $-1`）。
    // ⇒ 直写 switch 才能复现跳表（技巧 34）；`return job + 1` 的算式形不会生成
    //   `jmp *%eax` 与被调表项（原 ndiff 27 全在此）。
    if (job < 0)
        return -1;
    if (!GetJobAffection())
        return 0;
    switch (job)
    {
    case 0:  return 1;
    case 1:  return 2;
    case 2:  return 3;
    case 3:  return 4;
    case 4:  return 5;
    case 5:  return 6;
    case 6:  return 7;
    case 7:  return 8;
    default: return -1;
    }
