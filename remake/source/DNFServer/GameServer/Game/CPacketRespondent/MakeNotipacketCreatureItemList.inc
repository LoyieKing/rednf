// ORIG MakeNotipacketCreatureItemList @ 0x083409e0：
//   clear → put_header(0,0x69)；index=get_index() 后 put_byte(count) 占位计数；
//   遍历 map 值（post-increment 迭代）：仅当 item 非空且 GetCreatureItemType()==1
//   且 (slot==-1 || GetSlotNo()==slot) 时依次
//     put_int(GetUid)、put_byte(CCreature::GetStomach)、
//     IsGrowCreature ? [put_byte(1), put_int((int16)(exp>>16)), put_byte(10),
//                       put_byte(exp&0xff)]
//                   : [put_byte(0), put_int(GetExp)]、
//     put_byte(GetLevel)、strlen(name)+put_int+put_str、put_byte(IsNoCharge)，
//     计数 ++；
//   循环结束 put_byte(index, count) 回填占位字节，finalize(true)。
//
// [b28d-solo-CPacketRespondent P183/P0/P296] ORIG 0x083409e0 实证：
// ① 无 creature 具名局部——每处访问均 `mov -0xc(%ebp),%eax` 重读 item 后直呼
//    CCreature 访问器（单继承布局同址；镜像类型 reinterpret_cast 保持同一调用符号）；
// ② nameLen 在函数头预清（`movl $0x0,-0x14(%ebp)`）+ 循环内赋值（P198 两句式）；
// ③ 占位 put_byte 读 count 变量（`mov -0x10(%ebp),%edx`），非立即数 0；
// ④ 三跳过条件为正向包裹：4.1 产 `sete;test;je 尾` / 短路物化 `mov $1/$0;test;je 尾`；
//    写 `continue` 使 4.4 产 setne/jne + nop;jmp 中继岛（P296）。
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
}