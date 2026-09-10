void SendingAPC_Info::Set_APC_Info(TOD_Layer const& layer, _APCInfo const& info)
{
    // b21 verdict=DIFF（N 残余）：202/201（gap=1）。调用点全等（clear/put_header/put_byte×/
    //   GetLayer/strlen×3/put_int×/put_str×3/finalize/cMyTraceC1+cMyTracecl）。包字段序
    //   （userCharacNo/name/lv/job/grow/guild/pvp/abilities[0x16]/creature/creatureId）逐位核。
    //   trace 实参物化两批局部槽且顺序差(N82)、GetLayer 结果寄存器、帧 push edi vs 栈。
    //   语义完成，byte 层 N 封顶（近 identical）。
    m_buf.clear();
    m_userCharacNo = info.m_userCharacNo;
    m_buf.put_header(0, 0x15a);
    m_buf.put_byte(layer.GetLayer());

    int nameLen = (int)strlen(info.m_userCharacName);
    m_buf.put_int(nameLen);
    m_buf.put_str(info.m_userCharacName, nameLen);

    m_buf.put_byte(info.m_lv);
    m_buf.put_byte(info.m_job);
    m_buf.put_byte(info.m_growType);

    int guildLen = (int)strlen(info.m_guildName);
    m_buf.put_int(guildLen);
    m_buf.put_str(info.m_guildName, guildLen);

    m_buf.put_int(info.m_pvpGrade);
    for (int i = 0; i < 0x16; ++i)                  // ORIG cmpl $0x15/setle
        m_buf.put_int(info.m_abilities[i]);

    int creatureLen = (int)strlen(info.m_creatureName);
    m_buf.put_int(creatureLen);
    m_buf.put_str(info.m_creatureName, creatureLen);

    m_buf.put_int(info.m_creatureId);
    m_buf.finalize(true);

    // ORIG +0x1fb..+0x2b1：trace 实参先物化到局部槽（creature_id→esi、
    // &creature_name/-0x50、pvp/-0x4c、&guild/-0x48、grow/-0x44、job/-0x40、
    // lv/-0x3c、GetLayer→ebx），再反序写入变参栈槽。
    char* creatureName = const_cast<char*>(info.m_creatureName);
    int pvpGrade = info.m_pvpGrade;
    char* guildName = const_cast<char*>(info.m_guildName);
    int growType = info.m_growType;
    int job = info.m_job;
    int lv = info.m_lv;

    cMyTrace trace(__PRETTY_FUNCTION__,
                   0x68, 5);
    trace("@TOD : setting layer:%d, name:%s, lv:%d, job:%d, grow_type:%d, "
          "guild_name:%s, pvp_grade:%d, creature_name:%s, creature_id:%d",
          (unsigned int)(layer.GetLayer() & 0xffff),
          info.m_userCharacName, lv, job, growType,
          guildName, pvpGrade, creatureName,
          info.m_creatureId);
