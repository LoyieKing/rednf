// [b28d-solo-CGuildServerProxy N98: 53/53 NEAR] P316 翻案（DIFF→NEAR）：pGuild
//   声明为 `STGuildProxy* volatile` 后，4.4 把 memset/memcpy 的 dst 装载提前到压栈
//   之前（ORIG @0x0846ead7 `mov -0xc(%ebp),%eax` 先于 `movl $0x17,0x8(%esp)`），
//   memcpy src 自然落 %edx（ORIG `mov 0x10(%ebp),%edx` 同形）⇒ 求值序/寄存器全对齐。
//   残余仅槽号互换：4.4 给 volatile pGuild=-0x10、end() 内联临时=-0xc，ORIG 反之
//   （pGuild=-0xc、end 临时=-0x10）。已实测不闭合：①声明移至函数头最前（it 得
//   -0x14 ✓ 但 pGuild/end 仍互换）；②具名 endIt（it/endIt 互换）；③
//   `*(STGuildProxy* volatile*)&pGuild` 读形（+2 条）。均属 N98 槽分配族 ⇒ 封顶
//   | ORIG 0x0846ea3e。b28d-solo2 复验追加：④`end()==it` 操作数互换（P74补②）；
//   ⑤`*&m_guildInfoMap.end()`（P316 sret 折叠抑制）；⑥pGuild 声明移到 it 之后——
//   三形 end() sret 仍恒 -0xc（it -0x14 / pGuild -0x10 或反向），槽号互换不变 ⇒ 定案。
//   ⑦pGuild 移入 else 块内声明（P314 块级旋钮）⇒ pGuild -0x14 / it -0x10 更劣，已回滚。
//   b28d-PoolDBLogin 复验（Main P326｜N93 翻案形，一次性）：同形实测 ndiff 10→19、
//   ours 53→58、NEAR→DIFF（TU 三数 near 1→0 / diff 5→6 **变差**）⇒ 本件 ORIG 比较
//   调点同为**非 const** iterator::eq @0x0846ea88，三级槽 = -0x14(it) / -0x10(end) /
//   -0xc(pGuild 指针，非迭代器临时)，无转换临时可落 ⇒ P326 形不适用，已回滚；
//   N98 槽号互换维持封顶。
void CGuildServerProxy::ChangeGuildNameInProxy(unsigned int guildId, char* name)
{
    STGuildProxy* volatile pGuild;
    // 槽序：find 结果 -0x14（与 ORIG 一致）；end() 临时与 pGuild 互换（N98 残差）。
    std::map<unsigned int, STGuildProxy>::iterator it = m_guildInfoMap.find(guildId);
    // [b28d-solo3 一次性旋钮·否] P326 非 const 内联临时
    // `it == iterator(m_guildInfoMap.end())` 实测逐条同形（槽仍 pGuild -0x10 /
    // end 临时 -0xc，5 行残差不变）⇒ 无第三级临时可落，已回滚。
    if (it == m_guildInfoMap.end())
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp", __PRETTY_FUNCTION__, 0x860,
                              "Find fail from guild info proxy(guild_id:%d)", guildId);
    }
    else
    {
        pGuild = &it->second;
        memset(pGuild, 0, 0x17);
        memcpy(pGuild, name, 0x16);
    }
}