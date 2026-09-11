// ---- EnterPrivateStore（ORIG T 0x85c8dd6，报告 EnterPrivateStore.md）----
// ISeller 槽 +0x20 / IBuyer 槽 +0x08。买家围观入店：状态必须为 2，未在
// 围观列表则登记并接收店铺信息包。
bool CPrivateStore::EnterPrivateStore()
{
    m_errorHandler.Set(m_pBuyer, 1, 0x56);

    if (m_pBuyer == 0)
        return false;
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }

    if (m_state != 2)
    {
        m_pParent->FreeStoreBuyer(m_pBuyer);
        m_errorHandler.Do(0x3b);
        return false;
    }

    // ORIG 0x85c8dd6：find() 结果 sret=-0x18 作 operator!= 的 this，第二次
    // end() 结果 -0xc 作实参。裸 `find(...) != end()` 使 4.4 先求实参 end()
    // （P75）；具名迭代器固定 find 先求值（P74 补充②旋钮一），再以正极
    // `found != end()` 命中 this=find/arg=end 的 ORIG 形态。
    std::list<CUser*>::iterator found =
        std::find(m_users.begin(), m_users.end(), m_pBuyer);
    if (found != m_users.end())
    {
        m_errorHandler.Do(0x14);
        return false;
    }

    m_users.push_back(m_pBuyer);

    CPacketBufPrivate packet;
    packet.MakeCmdpacketEnterPrivateStore(m_pSeller, (unsigned short)GetSellerIndex(),
                                          m_items);
    m_pBuyer->Send(packet);
    return true;
}