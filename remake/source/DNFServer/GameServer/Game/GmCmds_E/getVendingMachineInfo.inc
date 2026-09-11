// ORIG getVendingMachineInfo 0x084b0444：找 vending 表首个 m_id==1 条目中
// 首个 m_groupNum==1 的 info；找不到返回 0。直调 CDataManager::getVendingMachineTable。
// b28d-solo-GmCmds_E：ORIG 为标准 for 循环（begin 后 `jmp cond`，尾部 ppEv
// 回跳，命中 `sete;test;jne` 物化岛）；原 while(true)+break 形改 for 复现。
// p167 补修（P96 反向）：ORIG 0x084b0452/0x084b0467 调 const 版
// `_ZNKSt6vector...5beginEv/3endEv` + `PK18stVendingMachine_t` const_iterator
// （operator->/operator*/ppEv/ne 全 const 族）⇒ table 改 `const vector*`、
// 两循环迭代器改 const_iterator（返回值 const_cast 为零指令编译期操作）。
stVendingMachineInfo_t* CRandomObjectTest::getVendingMachineInfo()
{
    const std::vector<stVendingMachine_t>* table =
        G_CDataManager()->getVendingMachineTable();

    std::vector<stVendingMachine_t>::const_iterator mit = table->begin();
    for (; mit != table->end(); ++mit)
    {
        if (mit->m_id == 1)
        {
            break;
        }
    }

    // ORIG：外层即便因 end 退出仍取 m_infoList（迭代器失效路径原样保留）。
    std::vector<stVendingMachineInfo_t>::const_iterator iit =
        mit->m_infoList.begin();
    for (; iit != mit->m_infoList.end(); ++iit)
    {
        if (iit->m_groupNum == 1)
        {
            return const_cast<stVendingMachineInfo_t*>(&*iit);
        }
    }
    return 0;
}