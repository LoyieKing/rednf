// ----------------------------------------------------------------------------
// ORIG T 0x081300f6（docs/class_func_reports/advancealtar__StageControl/
// sendStageInfo.md 全文）：getUser 判空直接返回；put_header(0,0x209) +
//   put_binary({stageIndex, stageDifficulty}, 8) + finalize(true) → Send。
// ----------------------------------------------------------------------------
// [B24D判词 DONE_AE] 语义正确：getUser 判空返回 + _NotiAdvanceAltarStageInfo(8B)+
//   put_header(0,0x209)+put_binary(&body,8)+finalize(true)+Send。已改用与 ORIG 同源
//   struct 写法（去 body[]+memcpy 二次拷贝，68/68 指令对齐）；仅剩 call/符号地址
//   AE 占位差异。
void StageControl::sendStageInfo()
{
    CUser* user = m_characData.getUser();   // [BigP3D 成员化] 真实成员直调（等价 this+0x98）
    if (user == 0)
    {
        return;
    }

    // ORIG 用 _NotiAdvanceAltarStageInfo（memset 0,8 的 ctor + 两 int 直赋），
    // 栈上两 int 连续成 8B payload；P48 去 body[]+memcpy 的二次拷贝形。
    NOTIPacketStruct::_NotiAdvanceAltarStageInfo body;
    body.m_stageIndex =
        m_procStage.getStageIndex();        // [BigP3D 成员化] 真实成员直调（等价 this+0x04）
    body.m_stageDifficulty =
        m_procStage.getStageDifficulty();

    PacketGuard guard;
    guard.put_header(0, 0x209);
    guard.put_binary(reinterpret_cast<char*>(&body), 8);
    guard.finalize(true);
    user->Send(guard);
}