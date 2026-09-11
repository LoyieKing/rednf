// ORIG T 0x08100a2e (0x10f)。
// [b28d-solo-W6_IPGDeps] verdict=AE（74=74）。修法（三处）：① 删 accStr 具名局部，
// NumberToString 结果内联作 snprintf 第 4 变参（ORIG 0x08100a56 无槽往返，P0/P134）；
// ② SIPGData::m_uid unsigned→int ⇒ ORIG 0x08100b22 `call GetUID; mov %eax,%edx;
// mov -0xc(%ebp),%eax; mov %edx,0x20(%eax)` 的 RHS-first+中转（P211 档③/BL2）；
// ③ m_notifyFlag unsigned char→bool + 源 `(unsigned char)flag` ⇒ ORIG 0x08100b31
// LHS-first（P300）。P167 CLEAR（9/9）。
SIPGData* CIPGHelper::_SetIPGData(CUser* user, IPG_DATA_TYPE type, bool flag)
{
    SIPGData* data = m_mgr->AllocIPGData();
    // [b28d-solo-W6_IPGDeps] ORIG 0x08100a56 段：NumberToString 结果无槽往返
    // （直接 `mov %eax,0xc(%esp)` 作 snprintf 第 4 变参）⇒ 内联实参（P0/P134）。
    OS_API::snprintf(data->m_account, 0x14, "%s",
                     NumberToString(user->get_acc_id(), 0));   // _Z14NumberToStringji
    strncpy(data->m_accName, user->get_acc_name(), 0x28);      // ORIG n=0x28 > m_accName[0x14]【越界写】
    memcpy(data->m_gameCode, "DUNF", 5);                       // DAT_08b41459 = "DUNF\0"
    data->m_recordLen = m_dataTableA[type];
    strncpy(data->m_envCode, m_str24, 0x14);
    data->m_productCode = (unsigned short)m_dataTableB[type];
    data->m_uid = user->GetUID();
    data->m_type = type;
    // [b28d-solo-W6_IPGDeps] ORIG 0x08100b31 段 `mov -0xc(%ebp),%eax;
    // movzbl -0x1c(%ebp),%edx; mov %dl,0x24(%eax)` = LHS-first；按 P300 以
    // uchar 源形（cast）翻转 4.4 的 RHS-first。
    data->m_notifyFlag = (unsigned char)flag;   // ORIG mov %dl,0x24 直存参数字节
    return data;
}
