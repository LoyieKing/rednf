// ORIG getItemRarity T 0x088bfd97（报告全文转写）：未命中 6（RARITY_MAX）。
int getItemRarity(const char* name)
{
    if (std::strcmp(name, "common") == 0) return 0;
    if (std::strcmp(name, "uncommon") == 0) return 1;
    if (std::strcmp(name, "rare") == 0) return 2;
    if (std::strcmp(name, "unique") == 0) return 3;
    if (std::strcmp(name, "epic") == 0) return 4;
    if (std::strcmp(name, "chronicle") == 0) return 5;
    return 6;
}