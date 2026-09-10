void CUser::UpdateDimensionInout()
{
    // [A06 2026-09-10 P259] ORIG 0x08651fba 段同形（Acquire 直落实参，无具名 stream 槽）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1bd0), true);
    guard.operator*()->operator<<(0x14f);
    // ORIG 08652030 call operator<<(int)：传参为 int（GetUID 返回 uint，须显式转 int
    // 选中 operator<<(int) 重载——否则误落 operator<<(unsigned int)，const 重载甄别）。
    guard.operator*()->operator<<(static_cast<int>(GetUID()));

    SIG_UPDATE_DIMENSION* sig = guard->GetInBuffer<SIG_UPDATE_DIMENSION>();
    memset(sig, 0, 0x10);
    *reinterpret_cast<unsigned int*>(sig) = getCurCharacNo();

    for (int i = 0; i <= 5; ++i)
    {
        char v = static_cast<char>(getDemensionInoutValue(static_cast<char>(i)));
        bool abnormal =
            (v < 0) ||
            ((G_CDataManager()->get_dimensionInout(i)) + 10 < static_cast<int>(v));
        if (abnormal)
        {
            reinterpret_cast<char*>(sig)[4 + i] = 0;
            cMyTrace trace(__PRETTY_FUNCTION__, 0x1bde, 0);
            trace("DIMENSION DUNGEON INOUT_COUNT ABNORMAL USER!! CHARAC_NO(%d)",
                  *reinterpret_cast<unsigned int*>(sig));
        }
        else
        {
            reinterpret_cast<char*>(sig)[4 + i] = v;
        }
    }

    reinterpret_cast<char*>(sig)[0xc] =
        static_cast<char>(getUltimateInoutValue('\x02'));
    reinterpret_cast<char*>(sig)[0xd] =
        static_cast<char>(IsSaveDemensionInoutValue());
    reinterpret_cast<char*>(sig)[0xe] =
        static_cast<char>(IsSaveBloodInoutValue());
    SetSaveDemensionInoutValue(false);
    SetSaveBloodInoutValue(false);

    GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
