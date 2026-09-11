itemGloballyUniqueIdentifier_t* CItemGloballyUniqueIdentifierGenerator::generate(
    itemGloballyUniqueIdentifier_t* id, int field)
{
    id->SetServerId(field);
    id->SetDatetime(static_cast<int>(time(0)));
    id->SetSequenceNo(getSeq());
    return id;   // ORIG 0x88947e2 附近末尾 mov 0xc(%ebp),%eax
}