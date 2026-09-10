void CUser::GetRecord(SIG_LOAD_ETC* sig) const
{
    // b28d-solo-CUser p167：ORIG 0x08677dd8 调 getBattleRecordRefR 3 次
    //   （begin/end/size，无 rec 缓存；我方缓存仅 1 次 ⇒ MISSING ×2），且序列化用
    //   结构整体赋值（rep movsl，报告实证）而非 memcpy call（我方 EXTRA memcpy ×1）
    //   ⇒ 去缓存 + `sig->m_records[index] = it->second;`（P83/P183）。
    int index = 0;
    for (std::map<int, GameResultType>::const_iterator it =
             getBattleRecordRefR()->begin();
         it != getBattleRecordRefR()->end() && index <= 99; ++it, ++index)
    {
        sig->m_records[index] = it->second;
    }
    sig->m_recordCount = (int)getBattleRecordRefR()->size();
