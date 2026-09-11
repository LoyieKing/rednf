void CRidable::SendSpawnRidableObject(PacketGuard* p, int a, int b, int c, int d)
{
    // ORIG 0x82fece2：连续写 4 个 int。
    p->put_int(a);
    p->put_int(b);
    p->put_int(c);
    p->put_int(d);
}