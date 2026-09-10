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
