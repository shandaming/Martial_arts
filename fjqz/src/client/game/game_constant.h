//
// Created by mylw on 22-9-17.
//

#ifndef FJQZ_GAME_CONSTANT_H
#define FJQZ_GAME_CONSTANT_H

#define MMAP ""

#define MAIN_MAP_COORD_COUNT 480
#define MAIN_MAP_COORD_COUNT_SQUARED (MAIN_MAP_COORD_COUNT * MAIN_MAP_COORD_COUNT)
#define MAIN_MAP_DATA_LENGTH (MAIN_MAP_COORD_COUNT * MAIN_MAP_COORD_COUNT * sizeof(int16_t))

#define BATTLE_MAP_COORD_COUNT 64
#define BATTLE_MAP_SAVE_LAYER_COUNT 2		// 数据文件存储地图数据层数
#define BATTLE_ENEMY_COUNT 20
#define TEAMMATE_COUNT 6          //最大队伍人员数

#endif //FJQZ_GAME_CONSTANT_H
