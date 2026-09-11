// [b28d-11b] 修法：具名局部 cur/remain + user 声明前置 + `while (user==NULL){goto L;}`+
// 尾标签 L:;（新范式：while+goto 复现 4.1 setcc 物化 + 尾岛）→ IDENTICAL 73 @0x086c8694
void GameWorld::send_buddy_conn_list(unsigned int accId, BuddyList* list, int count)
{
    CUser* user;
    BuddyList* cur = list;
    int remain = count;
    user = find_user_from_world_byaccid(accId);
    while (user == NULL)
    {
        goto L_end;
    }
    {
        PacketGuard packet;
        packet.put_header(0, 0x38);
        packet.put_byte(count);
        while (remain-- != 0)
        {
            packet.put_binary((char*)cur, 0x2a);
            // §4.3 例外保留地址形式：BuddyList 全仓仅前向声明（GameWorld.h），
            // 无任何字段建模证据；0x2a = ORIG 该记录步长（ORIG 逐条 put_binary
            // 后按 0x2a 递增指针），非普通成员裸偏移访问。
            cur = (BuddyList*)((char*)cur + 0x2a);
        }
        packet.finalize(true);
        user->Send(packet);
    }
L_end:;
}