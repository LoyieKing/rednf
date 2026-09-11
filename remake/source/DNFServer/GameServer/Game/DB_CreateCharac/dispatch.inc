// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x08400d2c 213=213。
// 修法：P102 同一 bool ok（声明在 sig 前取 -0xd 深槽）+ if(!ok) goto 三岛标签串（P223）
// + P248 档① SendResult int→bool（消 test/setne）+ SigGiftInvenView 具名成员取址复现
// ORIG `lea 0x3a9(%eax),%edx`（数组名退化恒折 add+mov）。
bool DB_CreateCharac::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
    {
        return false;
    }

    // P102/P0：ORIG 全程复用同一 bool 槽 -0xd；声明在 sig 之前使 4.4 给 ok 更深槽。
    bool ok;
    SIG_CREATE_CHARAC* sig = stream->GetOutBuffer<SIG_CREATE_CHARAC>();
    if (sig == NULL)
    {
        return false;
    }

    sig->m_resultCode = 0;

    // P102/P0：ORIG 全程复用同一 bool 槽 -0xd（每次 `mov %al,-0xd; movzbl -0xd; xor $1;
    // test; je/jne`）⇒ 所有子调用结果赋给同一具名 bool ok，逐个判断；失败路径照 ORIG
    //（CheckLimit/CreateNewCharac/_CreateNewCharacStat 失败直落 SendResult，其余 rc=2）。
    ok = CheckLimitCreateNewCharac(sig, true);              // call 08401922
    if (!ok)
    {
        goto island1;                                       // ORIG jne nop+jmp 岛
    }
    ok = CreateNewCharac(sig);                              // call 08401dcc
    if (!ok)
    {
        goto island2;                                       // ORIG jne nop+jmp 岛
    }
    ok = GetCharacNo(sig);                                  // call 0840275c
    if (!ok)
    {
        sig->m_resultCode = 2;
        goto send_result;
    }
    ok = _CreateNewCharacStat(sig);                         // call 0840215e
    if (!ok)
    {
        goto island3;                                       // ORIG jne nop 岛（fall-through）
    }
    if (sig->m_giftFindOk != 0)                             // byte +0x533f
    {
        ok = CreateInventory(sig->m_characNo,
                             &reinterpret_cast<SigGiftInvenView*>(sig)->m_inven);
    }
    else
    {
        ok = CreateInventory(sig->m_characNo, NULL);
    }
    if (sig->m_equipFindOk != 0)                            // byte +0x5340
    {
        // ORIG lea 0x4e01(%eax)：&m_giftItem 起点，压缩长度 0x2dc。
        ok = CreateEquip(sig->m_characNo, &sig->m_giftItem);
    }
    if (!ok)
    {
        sig->m_resultCode = 2;
    }
    ok = CreateSkill(sig);
    if (!ok)
    {
        sig->m_resultCode = 2;
    }
    ok = CreatePvP(sig->m_characNo);
    if (!ok)
    {
        sig->m_resultCode = 2;
    }
    ok = _createCharacDungeon(sig->m_accId);
    if (!ok)
    {
        sig->m_resultCode = 2;
    }
    else
    {
        ok = _createCharacQuest(sig->m_characNo);
        if (!ok)
        {
            sig->m_resultCode = 2;
        }
        else
        {
            ok = _createCharacNPCRelationship(sig->m_characNo);
            if (!ok)
            {
                sig->m_resultCode = 2;
            }
            else
            {
                ok = InsertLimitCreateNewCharac(sig, true);
            }
        }
    }

island3:;
island2:;
island1:;
send_result:
    // P248 档①：ORIG 只有 `mov %al,-0xd`（无 test/setne）⇒ SendResult 返回类型改 bool。
    ok = SendResult(userIdx, slot, sig);
    if (!ok)
    {
        return false;
    }
    return true;
}