bool DB_SaveGameOption_2::dispatch(int userIdx, int slot, Stream* stream)
{
    // 声明序照 ORIG 栈槽（低→高）：buf(-0x111) bufLen(-0x24) db(-0x20)
    // sig(-0x1c)。key_type 每次从 sig+4 重读（ORIG movzbl 形态）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_B,
                                                  E_SERVER_GROUP_NONE);
    SIG_LOAD_CHARAC_HOTKEY_OPTION* sig =
        stream->GetOutBuffer<SIG_LOAD_CHARAC_HOTKEY_OPTION>();
    static const int compressedBufferSize = 0xed;  // ORIG _ZZ…DB_SaveGameOption_2…E20compressedBufferSize
    char buf[0xed] = { '\0' };
    unsigned int bufLen = compressedBufferSize;
    if (!compress_zip(buf, bufLen, (const char*)sig + 5, 0xbc))
        return 0;
    db->set_query(
        "upDate member_key_option set key_option='%s' where m_id=%s and key_type=%d",
        db->blob_to_str(0, buf, (int)bufLen),
        NumberToString(((DB_SaveGameOption2View*)sig)->m_id, 0),
        ((DB_SaveGameOption2View*)sig)->m_keyType);
    // ORIG 0x0842fd42：choose 结果（mov$1/mov$0 于 eax）+ test/je，无
    // register-bool 汇聚块 ⇒ UpdateLogin 直判形。
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into member_key_option(m_id, key_type, key_option) values(%s,%d,'%s')",
            NumberToString(((DB_SaveGameOption2View*)sig)->m_id, 0),
            ((DB_SaveGameOption2View*)sig)->m_keyType,
            db->blob_to_str(0, buf, (int)bufLen));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
