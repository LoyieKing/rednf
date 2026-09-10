void CUser::sendBingoAddData()
{
    // ORIG 0x0869148b/0x086914ae：GetRepeatEvent 与 getBingoData 结果均免槽直用
    // （`mov (%eax),%edx;...; mov %eax,(%esp); call *%edx` / `movzbl 0x5(%eax),%eax`，
    // 技巧37）；具名 event/data 指针各 spill 一槽收紧帧 0x18→0x28。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x6a)->IsEventing(0)) {
        if (static_cast<BingoData*>(getBingoData())->m_isWin) {
            sendDecorationObjectCnt(1);
        }
    }
