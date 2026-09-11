// ============================================================================
// CUser::UpdateTutorialSkipable() @ 0x08697318 —— 无报告，objdump 逐指令转写：
//   8697324: call 819a870 <CUserCharacInfo::enableSaveCharacStat()>  ; this+0x1 = 1
//   8697337: call 84ecae4 <CUser::SetTutorialSkipable(unsigned char)>; 实参 1
//   （SetTutorialSkipable 写 CUser+0x8e08a 字节；GetTutorialSkipable 同址读取。）
// ============================================================================
void CUser::UpdateTutorialSkipable()
{
    enableSaveCharacStat();
    SetTutorialSkipable(1);
}