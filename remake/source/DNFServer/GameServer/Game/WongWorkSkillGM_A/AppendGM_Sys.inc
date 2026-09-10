void CGMAccounts::AppendGM_Sys(unsigned int mid, char type)
{
    // ORIG：POD 直接赋值（stGMInfo_t 无用户 ctor，ORIG nm 无 C1 符号）。
    CGMAccounts::stGMInfo_t info = {};   // ORIG 零填 -0x18/-0x14 两槽后覆写
    info.mid = mid;
    info.type = static_cast<unsigned int>(static_cast<int>(type));
    m_gmList.push_back(info);

    // ORIG 0x08109524 段：`call NumberToString; mov %eax,%ebx`（ctor 前求值 + %ebx
    //   跨 CMyFileLog ctor 保活）⇒ P227 全表达式临时链形（具名 log 对象/语句表达式
    //   两形在 4.4 下把结果落槽 -0xc，多 1 store + 1 reload）。
    // ORIG 0x0810957b 立即数符号为 `_ZZN8WongWork11CGMAccounts12AppendGM_SysEjcE12__FUNCTION__`
    //   （P139：手写字面量不生成该局部静态符号 ⇒ 仅ORIG 差集）⇒ 传 __FUNCTION__。
    CMyFileLog(__FUNCTION__, 0xcd)("./log/Init", "GM List Add mid:%s",
                                   NumberToString(mid, 0));
