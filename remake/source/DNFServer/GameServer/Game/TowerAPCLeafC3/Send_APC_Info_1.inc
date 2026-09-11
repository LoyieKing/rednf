// ---------------------------------------------------------------------------
// TowerOfDespair_APC_Mgr::Send_APC_Info —— ORIG T 0x085feee4
//                                          [0x085feee4, 0x085ff01f]（本批次目标）
// find(layer)；未命中 → cMyTrace("...Send_APC_Info(const TOD_Layer&, CUser*)",
// 0xa1, 5)("TOD:invalid layer %d", GetLayer)；
// 命中 → it->second.Send_APC_Info(layer,user) → get_user_charac_no()
// → cMyTraceNoop 构造后输出
// "TOD:sending [%d] toUser, layer [%d] : clone : [%d]"(getCurCharacNo,layer,no)。
// 注：运行期 mgr 对象由 CtorProducers_1.cpp 的 new 创建（空 map 内存映像与
// 真实 map<TOD_Layer,SendingAPC_Info> 一致）；填充方 Set_APC_Info 后续批次
// 接入时须按本真实容器模型实现。
// ---------------------------------------------------------------------------
// [b28d-32 N(P75): 装载序/寄存器差（90/87） | ORIG 0x085feee4]
void TowerOfDespair_APC_Mgr::Send_APC_Info(TOD_Layer const& layer, CUser* user)
{
    // ORIG 形态：默认构造 it → end() 先求值 → find 赋值 → operator!= 判断；
    // 日志三参为内联 getter 表达式（右到左求值，寄存器临时跨 noop 构造存活）。
    // [b28e-M1S2] 反转声明序（endIt 先）实测 verdict NEAR→DIFF（90/90 不变，槽映射
    // 未如预期互换且 end() 发射点提前）⇒ 回滚。残留 = 两个迭代器槽互换：
    // ORIG it=-0x34/endIt=-0x30（4.1 先声明深），我方 it=-0x30/endIt=-0x34
    //（4.4 先声明浅，与本件实测一致）⇒ P339 方向相反、无可造形，判 N(P75 槽序)。
    // [P6AuditOldN-S1 2026-09-11 负结论] P339 第三形「声明序 endIt→it + 赋值分离」实测
    // 反劣：our 90→95（gap 0→−5），多出 endIt 默认构造 call + 追加拷贝
    //（`lea -0x30; call it-ctor` ⇒ `mov -0x3c,%eax; mov %eax,-0x30`）。
    // 4.1/4.4 槽方向相反在此件无源码可造形（带初始化器反转亦为负例，前波已证）⇒ 封 N。
    std::map<TOD_Layer, SendingAPC_Info>::iterator it;
    std::map<TOD_Layer, SendingAPC_Info>::iterator endIt = m_APCInfoMap.end();
    it = m_APCInfoMap.find(layer);
    if (it != endIt)
    {
        it->second.Send_APC_Info(layer, user);
        // [b28e-M1S2] ORIG 0x085ff2xx：noop 的 ctor 在三个 getter 求值**之后**
        // （`call get_user_charac_no; mov %eax,%esi; call GetLayer; movzwl %ax,%edi;
        //  call getCurCharacNo; mov %eax,%ebx; lea -0x29(%ebp),%eax;
        //  call cMyTraceNoop::C1Ev`）⇒ 源码为**临时对象直接调用**形（this 实参
        // 最后求值），非具名 `cMyTraceNoop noop; noop(...)`（4.4 会在声明处提前
        // 构造 ctor）。同因改写 else 支的 cMyTrace。
        cMyTraceNoop()("TOD:sending [%d] toUser, layer [%d] : clone : [%d]",
                       user->getCurCharacNo(),
                       layer.GetLayer(),
                       it->second.get_user_charac_no());
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa1, 5)(
            "TOD:invalid layer %d", (unsigned int)(layer.GetLayer() & 0xffff));
    }
}