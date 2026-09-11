// [b28d-solo-CGuildServerProxy N107/N7: 63/62] 本件已修两处真缺口：
//   ① 删具名 endIt 改内联 end()（ORIG find 落 -0x14、end 落 -0x10；具名 endIt 使
//      4.4 反排成 -0xc/-0x10，与同 TU ChangeGuildNameInProxy 的 ORIG 槽序对齐）；
//   ② 补 pGuild 死槽复现 ORIG @0x0846e9eb/0x0846e9fc 的两次 ptEv（第一次 +0x4 写
//      -0xc 后全程未读，P171 死写入照抄；单语句 `it->second = info;` 只发一次 ptEv）。
//   残余仅逐字段拷贝段：ORIG `lea 0x4(%eax),%edx`（dst 物化 %edx、src 装 %eax），
//   4.4 恒 dst 留 %eax、src 装 %edx 且 +4 折 disp（N107/N18a）。实测 9 形
//   （显式 operator-> / (*it) / *(&it->second) / RHS cast 族 / 具名 q）均不可翻。
//   b28d 复验追加：`*(volatile STGuildProxy*)&it->second`（编译失败：volatile 对象无
//   非 volatile operator=）、`register STGuildProxy* pDst=&it->second; *pDst=info;`
//   （63→65 劣化）均不可用 | ORIG 0x0846e95e
//   b28d-solo2 复验追加：`it->second.operator=(info);` 显式限定调用实测与 `=` 逐条
//   同形（4.4 仍 dst=eax+disp / src=edx），N107/N76 族封顶不变 | ORIG 0x0846e95e
//   b28d-PoolDBLogin 复验（Main P326｜N93 翻案形，一次性）：
//   `it == std::map<unsigned int, STGuildProxy>::const_iterator(m_guildInfoMap.end())`
//   实测 ndiff 31→42、ours 62→67（+5 条 = const_iterator 转换 ctor + 转换后比较调点）
//   ⇒ 本件 ORIG 比较调点是**非 const** `_ZNKSt17_Rb_tree_iteratorISt4pairIKj12STGuildProxyEEeqERKS4_`
//   @0x0846e9a8，仅两槽 -0x14(it)/-0x10(end)，无第三级转换临时 ⇒ P326 形结构性不适用，
//   已回滚（回滚后 fresh 复测 = 基线 63/62 ndiff 31）。
bool CGuildServerProxy::ModifyGuildInfoProxy(unsigned int guildId, STGuildProxy& info)
{
    std::map<unsigned int, STGuildProxy>::iterator it = m_guildInfoMap.find(guildId);
    if (it == m_guildInfoMap.end())
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp", __PRETTY_FUNCTION__, 0x852,
                              "Find fail from guild info proxy(guild_id:%d)", guildId);
        return false;
    }
    else
    {
        // [b28d-solo-CGuildServerProxy P171 死写入照抄] ORIG @0x0846e9eb/0x0846e9fc
        // 对 it->second 连发两次 _Rb_tree_iterator::ptEv：第一次 `add $0x4` 结果
        // 写入 -0xc(%ebp) 后全程未读（死槽），第二次结果 +0x4 作为逐字段拷贝 dst。
        // 该死槽只能由「具名 pGuild 取址 + 另写 it->second」两语句复现（探针
        // /tmp/probe_mgi.cpp f vs f2：单语句只发一次 ptEv）。
        STGuildProxy* pGuild = &it->second;
        // [b28d-solo3 一次性旋钮] 取址形：诱导 4.4 先算 dst 地址到寄存器
        // （ORIG 0x0846ea07 `lea 0x4(%eax),%edx` + 逐字段 store (%edx)）。
        *reinterpret_cast<STGuildProxy*>(&it->second) = info;
        (void)pGuild;
        return true;
    }
}