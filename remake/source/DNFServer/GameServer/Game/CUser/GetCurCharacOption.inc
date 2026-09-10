void CUser::GetCurCharacOption(CharacterOption& option) const
{
    // b28d-lane5 复测（P319 整型加法形 + P222 正向早退）：
    //   ORIG 0x0868f050 `test %eax,%eax; sete %al; test %al,%al; jne <尾>`（极性取反的
    //   失败早退）与 0x0868f068 `mov %eax,%edx; add $0x12b7,%edx`（两段地址算术）。
    if (getCurCharacR() == 0)
        return;
    option = *reinterpret_cast<const CharacterOption*>(
        reinterpret_cast<unsigned int>(getCurCharacR()) + 0x12b7);
