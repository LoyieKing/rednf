// ORIG 0x0868ddc2 _ZNK5CUser19GetAuraAvatarOptionER18stAuraAvatarOption
// 读当前角色 _Charac_info+0x1281..+0x1289 三 int → option。ORIG 无 cur 局部：
// getCurCharacR() 判定(0x868ddce)与拷贝(0x868dde2)各重调一次（P48，调用点差集归零）。
// 记 N（跨版本 regmove，N18a 族）：4.1 O0 将 option 基址驻 %edx 单装载 +
// `0x1281(%eax)` 索引三连；4.4 O0 对引用形参逐条重装载，register-asm 绑定反产
// 冗余 regmove（probe 四形 21/23/26/28/30 均不达 24）——语义+调用点全对，残 +4。
void CUser::GetAuraAvatarOption(stAuraAvatarOption& option) const
{
    // b28c-L5：清 `register const _Charac_info* asm("eax")` 存量绑定（P0/N96）。
    //   判词（布局视图缺口，非源形问题）：ORIG 拷贝段是
    //     `mov 0xc(%ebp),%edx`（**目标** option 基址驻 %edx）+
    //     `mov 0x1281(%eax),%ecx …`（**源**以 this 相对 disp32 直读）的 12 字节
    //   POD 结构整体赋值形。当前 `_Charac_info` 只有 `struct{int×3}`/`int[3]`
    //   两视图、无 stAuraAvatarOption 成员 ⇒ 任何写法都只能先 `add $0x1281`
    //   物化源地址（4.4 把源基址留给 callee-saved、目标挤到 %eax，装载方向反）。
    //   已按 §5.3 申请在 CUser.h:692 union 内补具名结构视图（Main 已代落
    //   `stAuraAvatarOption m_auraAvatarOption;`，+0x1281，0xc 字节，零布局影响）。
    // ORIG 0x0868ddc2 拷贝段 =
    //   `mov 0xc(%ebp),%edx / mov 0x1281(%eax),%ecx / mov %ecx,(%edx) /
    //    mov 0x1285(%eax),%ecx / mov %ecx,0x4(%edx) / mov 0x1289(%eax),%eax /
    //    mov %eax,0x8(%edx)`
    // —— 目标基址**单装载驻 %edx** + 源基址驻 %eax 连续三 dword，正是 12 字节
    // POD 结构整体赋值的 4.1 展开形（P83：结构整体赋值 vs 逐字段/ memcpy 三形互斥；
    // 逐字段写 4.4 恒每字段重装载两侧基址 ⇒ 多 4 条，原绑定期靠钉 %eax 勉强凑）。
    // 源子对象已由 CUserCharacInfo.h:499 union 具名建模（m_auraAvatarOption @+0x1281）
    // ⇒ 用具名结构成员整体拷贝，无需指针缓存亦无裸偏移（§4.3）。
    if (getCurCharacR() == 0)
        return;
    option = getCurCharacR()->m_auraAvatarOption;   // +0x1281 具名结构整体拷贝
}