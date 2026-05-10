#pragma once

struct Game
{
};

struct Game game_init();
void game_update(struct Game *game);
void game_render(struct Game *game);
void game_terminate(struct Game *game);