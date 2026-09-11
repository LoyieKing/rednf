// ORIG MakeNotipacketCreatureState @ 0x08340918：clear → put_header(0,0x67)
// → put_int(uid) → put_int(state) → finalize(true)。
void CPacketRespondent::MakeNotipacketCreatureState(int uid, int state)
{
    clear();
    put_header(0, 0x67);
    put_int(uid);
    put_int(state);
    finalize(true);
}