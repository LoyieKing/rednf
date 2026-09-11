// ORIG 0x0818b638：ORIG 本体即恒返 -1（纯 ret #0xffffffff，保留语义勿"修复"）。
int CREATE_QUERY::arad_sp_create_dungeon_point(MySQL*, SIG_CREATE_CHARAC*)
{
    return -1;
}