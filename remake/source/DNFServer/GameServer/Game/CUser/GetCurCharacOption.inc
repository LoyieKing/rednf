// ORIG 0x0868f03c _ZNK5CUser18GetCurCharacOptionER15CharacterOption
// 0x200 字节 **结构体赋值** 自 _Charac_info+0x12b7（CharacterOption::m_pad[0x200]）：
// ORIG 4.1 内联为「对齐处理 + rep movsl + 尾块」序列（无 memcpy call），
// 且 getCurCharacR() 调两次（+0x0f 判空、+0x27 取址），不落具名 cur 缓存（P183）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868f03c | _ZNK5CUser18GetCurCharacOptionER15CharacterOption
void CUser::GetCurCharacOption(CharacterOption& option) const
{
    // b28d-lane5 复测（P319 整型加法形 + P222 正向早退）：
    //   ORIG 0x0868f050 `test %eax,%eax; sete %al; test %al,%al; jne <尾>`（极性取反的
    //   失败早退）与 0x0868f068 `mov %eax,%edx; add $0x12b7,%edx`（两段地址算术）。
    if (getCurCharacR() == 0)
        return;
    // 2026-09-11 L9GrpF（P83 结构整体赋值 + P183 去裸偏移）：ORIG 0x0868f068
    //   `mov %eax,%edx; mov 0xc(%ebp),%eax; add $0x12b7,%edx`（目标基址驻 %eax、源以
    //   成员偏移在 %edx 上算）；`reinterpret_cast<unsigned int>` 算术形产
    //   `add $0x12b7,%eax; mov %eax,%edx` 反序。`_Charac_info` 已具名建模
    //   `CharacterOption m_characterOption;`（CUserCharacInfo.h:593，+0x12b7）
    //   ⇒ 用具名成员整体拷贝（同 GetAuraAvatarOption 已 AE 的 m_auraAvatarOption 先例）。
    //   探针 /tmp/L9/F/ph1.cpp h1：与 ORIG 逐条同形（h0=现形不一致）。
    option = getCurCharacR()->m_characterOption;
}