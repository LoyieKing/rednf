#ifndef GAME_BLUEMARBLE_MAPSCRIPT_H_
#define GAME_BLUEMARBLE_MAPSCRIPT_H_

// ============================================================================
// df_game_r BlueMarbleMapScript（G2-4 大富翁地图脚本，尺寸 0xc）
// 布局依据 ORIG 构造（0x088d53d6）：仅一个 vector<BlueMarbleTileScript*>。
// ============================================================================

#include <vector>

class BlueMarbleTileScript;

class BlueMarble;   // 前向：仅 friend（ORIG 直读 m_tiles，无 getter 符号）

class BlueMarbleMapScript
{
public:
    BlueMarbleMapScript();
    ~BlueMarbleMapScript();

    void clear();
    BlueMarbleTileScript* getTile(unsigned int zone);
    // b28d-168a①：ORIG initMapList 调用点同 initTileList（xor $1 取反形）；
    // 定义属主非本泳道（R1）⇒ 保留 int。
    int importScript(const char* path);

    // BlueMarble::setMaxBoardSize（ORIG T 0x080d6e88）把 getMap() 返回值直接作为
    // `std::vector<BlueMarbleTileScript*>::size` 的 this（+0x00 == m_tiles），
    // ORIG 无对应 getter 符号 ⇒ 以 friend 授权具名访问，禁止本地 shadow cast。
    friend class BlueMarble;

private:
    std::vector<BlueMarbleTileScript*> m_tiles;  // +0x00（0xc）
};

#endif  // GAME_BLUEMARBLE_MAPSCRIPT_H_
