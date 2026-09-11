// [b28d-32 N(P75): 1 条装载序/寄存器差（202/201） | ORIG 0x085fe906]
// 【b28e-M1S2 本波复核】202/201。P331 核过：残差为**多段不同常数**平移
//   （strlen 结果槽 ORIG -0x20 vs 我方 -0x28（差 8）；6 个 int 局部组
//   ORIG -0x50..-0x3c vs 我方 -0x24..-0x10（差 0x2c））⇒ 非「恒定平移」⇒ 封 N。
//   %edi 保活差：ORIG `lea 0x4(%eax),%edi`（this+4 跨 cMyTrace 调用存活）vs
//   我方 %esi；帧 0x7c vs 0x70（= ORIG 多 push %edi + 槽深差）。
//   语义轴：cMyTrace 格式串 `@TOD : setting layer:%d, name:%s, lv:%d, job:%d,
//   grow_type:%d, guild_name:%s, pvp_grade:%d, creature_name:%s, creature_id:%d`
//   与 ORIG 0x8cd6904 逐字节一致（objdump -s 核对），9 个实参装载段
//   （0x28..0x10(%esp)）与 ORIG 逐条对应。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N3,N82,N95）→ 试=本波 N3 两形已留痕（临时对象
// 直呼 ⇒ 203/槽映射劣化；叠加 P339 仍 203）+ 本轮复核 202/201 同形（strlen 结果槽差 8、
// 6×int trace 实参组差 0x2c、ORIG 多 `push %edi` 保活 this+4）+ strall CLEAR ⇒ 维持
// （新=DIFF gap=+1, blocks=27）。
void SendingAPC_Info::Set_APC_Info(TOD_Layer const& layer, _APCInfo const& info)
{
    // b21 verdict=DIFF（N 残余）：202/201（gap=1）。调用点全等（clear/put_header/put_byte×/
    //   GetLayer/strlen×3/put_int×/put_str×3/finalize/cMyTraceC1+cMyTracecl）。包字段序
    //   （userCharacNo/name/lv/job/grow/guild/pvp/abilities[0x16]/creature/creatureId）逐位核。
    //   trace 实参物化两批局部槽且顺序差(N82)、GetLayer 结果寄存器、帧 push edi vs 栈。
    //   语义完成，byte 层 N 封顶（近 identical）。
    // [N3 复核 2026-09-11] 两旋钮实测均不过闸门（基线 our=201/gap=+1）：
    // ① 临时对象直接调用形 `cMyTrace(...)(...)`（src/TowerAPCLeafC3.cpp 由具名 trace 改造）
    //    ⇒ ORIG 的 ctor 发射点（0x085feb00 实参物化**之后**）与 prologue `push %edi` 复现，
    //    但 our 201→203（gap +1→−1）、残差转为全帧槽映射差（trace 实参槽 ORIG
    //    -0x50/-0x4c/-0x48/-0x44/-0x40/-0x3c、ctor -0x38；我方 -0x40/-0x3c/-0x38/-0x34…、ctor -0x50）
    //    ⇒ 判负例。
    // ② P339 全头部声明（6 个 trace 实参局部提头、声明序=ORIG 槽深→浅）叠加① ⇒ our 203 不变、
    //    槽仅整体深移 0x10 仍不对齐 ⇒ 判负例。
    //    ⇒ 保留原形（具名 trace + 块内声明），按 N82/N95 槽族封顶。
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
}