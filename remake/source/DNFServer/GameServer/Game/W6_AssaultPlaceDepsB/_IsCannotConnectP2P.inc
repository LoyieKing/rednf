// ORIG T 0x82e927e _IsCannotConnectP2P(int, int)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=AE 26/26（p167 1/1 CLEAR）
//   修法：bool 返回 + 合并短路（byte!=0 && !Empty()）+ 嵌套 if 复现两 je 共享尾岛；
//   P311 成员数组视图 `m_assaulters[a].m_connectIp[b]` 复现 ORIG 装载序（ndiff 11→1，
//   唯一残差 +0x15 常量折叠由 AE 口径归一）。
// [b28c-L7] 返回类型 int→bool（判词见 CAssaultMgr.h）：ORIG 0x82e927e 体尾
// `mov $0x0/$0x1,%eax` 与 bool 归一形一致；改型使两处调用点复现
// `xor $1,%eax` / `test %al,%al` 直测形。
bool CAssaultPlace::_IsCannotConnectP2P(int a, int b)
{
    // asm：offset = this + a*0x20 + b + 0x15 的单字节（a<<5 后按字节加 b，
    //   并非第二个槽基址）；该字节非零且 m_assaulters[b] 非空 → 判"无法连接"。
    // [b28d-26] P183：去 p 局部缓存（ORIG 0x82e927e 全表达式直算，无 -0xc 槽，
    //   帧 sub $0x18 vs 我们 $0x28）。
    // ORIG 0x82e927e 偏移拆分：`add $0x10,%eax; movzbl 0x5(%eax)`。
    // [b28d-solo-W6_AssaultPlaceDepsB] P133/P168a①：ORIG 为「byte!=0 && !Empty()」
    // 合并短路——两条件各以 je 直跳共享 mov $1 尾岛（call 后 xor $1,%eax）。
    // 独立早返形（if(byte==0)return true; if(Empty)return false;）反产 jne+早岛。
    // [b28d-solo-W6_AssaultPlaceDepsB] P311 成员数组视图：CAssaulter 的
    //   m_connectIp[9] 位于 +0x15 ⇒ 直写 `m_assaulters[a].m_connectIp[b]`
    //   复现 ORIG 装载序（a→%ecx / b→%eax / this→%edx / shl / add / lea）。
    //   残差：ORIG 把 +0x15 拆成 `add $0x10` + disp `0x5`，4.4 恒折单条
    //   `add $0x15`（P30/N4/N102 常量折叠族，源形不可择）。
    // [b28d-solo-W6_AssaultPlaceDepsB] P225 推广：`&&` 合并形被 4.4 折成
    //   bool 归一（mov $1/test/je/mov $0 尾段，+5 条）；拆嵌套 if 复现 ORIG
    //   两条件各自 je 直跳共享 mov $1 尾岛形。
    if (static_cast<unsigned char>(m_assaulters[a].m_connectIp[b]) != 0) {
        if (!m_assaulters[b].Empty())
            return false;
    }
    return true;
}