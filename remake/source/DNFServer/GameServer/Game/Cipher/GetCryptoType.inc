// [P327] ORIG `_GLOBAL__I__Z13GetCryptoTypei` @0x08097644：本 TU 静态初始化
// 入口键名 = 第一个「强符号·外部链接·本 TU 定义」的实体名，故 GetCryptoType
// 必须定义在本文件最前（ORIG 0x08097160 亦位于 TU 最前）。
int GetCryptoType(int key)
{
    return key % CRYPTO_GRAPH_MAX;
}