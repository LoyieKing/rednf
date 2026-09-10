void CPacketRespondent::MakeNotipacketCreatureItemList(
    const std::map<int, CCreatureItem*>* items, int slot)
{
    clear();
    put_header(0, 0x69);

    int nameLen = 0;
    int count = 0;
    int index = get_index();
    put_byte(count);  // 计数占位

    for (std::map<int, CCreatureItem*>::const_iterator it = items->begin();
         it != items->end(); it++)
    {
        CCreatureItem* item = it->second;
        if (item != 0)
        {
            if (item->GetCreatureItemType() == 1)
            {
                if (slot == -1 || item->GetSlotNo() == slot)
                {
                    put_int(item->GetUid());

                    // 同一对象继续按 CCreature 语义读取（ORIG 直呼 CCreature 访问器）。
                    put_byte(reinterpret_cast<CCreature*>(item)->GetStomach());
                    if (reinterpret_cast<CCreature*>(item)->IsGrowCreature())
                    {
                        put_byte(1);
                        // ORIG 0x8340b1f：GetExp 结果全宽落槽 -0x28，movzwl -0x26（u.w[1]）
                        // + movswl 直扩进 %edx 作实参（无独立局部、无写回）⇒ union 拆半
                        // 惯用形（同 GainExp，彼处因 exp= 赋参数槽而呈 cwtl+mov 0xc 形）。
                        union { int all; unsigned short w[2]; unsigned char b[4]; } u;
                        u.all = reinterpret_cast<CCreature*>(item)->GetExp();
                        put_int((int)(short)u.w[1]);
                        put_byte(0xa);
                        put_byte((int)u.b[0]);
                    }
                    else
                    {
                        put_byte(0);
                        put_int(reinterpret_cast<CCreature*>(item)->GetExp());
                    }

                    put_byte(reinterpret_cast<CCreature*>(item)->GetLevel());
                    // ORIG 0x834ba4/0x834bcc：GetName 调用两次（strlen 实参与 put_str 实参
                    // 各一次），无 name 具名局部（P183/P0：具名指针槽 -0xc 会使全帧深 4）。
                    nameLen = (int)strlen(reinterpret_cast<CCreature*>(item)->GetName());
                    put_int(nameLen);
                    put_str(reinterpret_cast<CCreature*>(item)->GetName(), nameLen);
                    put_byte((int)(unsigned char)
                             reinterpret_cast<CCreature*>(item)->IsNoCharge());

                    ++count;
                }
            }
        }
    }

    put_byte(index, count);
    finalize(true);
