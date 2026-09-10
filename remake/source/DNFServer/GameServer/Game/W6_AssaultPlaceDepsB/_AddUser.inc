void CAssaultPlace::_AddUser(CUser* user, ENUM_ASSAULT_TEAM team)
{
    // [b28d-26] ORIG 0x82e7018：`je <循环增量>` 跳过主体、主体以 ret 早返，
    //   throw 块在循环尾之后（无循环内 slot==7 判定）；SetAssaultPlace 实参直读
    //   this+0x104（无 GetAssaultPlaceId call）。
    for (int slot = 0; slot <= 7; ++slot) {
        if (m_assaulters[slot].IsAttachAble())
        {
            m_assaulters[slot].AttachUser(user, team);
            user->SetAssaultPlace(m_field104);   // ORIG 直读 +0x104 → CUserCharacInfo::SetAssaultPlace
            m_field100++;                        // GetAssaulterCount 计数域
            return;
        }
    }
    throw (ENUM_ERROR)0x04;                      // ORIG __cxa_throw ENUM_ERROR(4)
