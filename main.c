#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#define TILE_SIZE 16
#define RECTANGLE_BK \
	(Rectangle) { 0, 0, TILE_SIZE, TILE_SIZE }
#define RECTANGLE_BALL \
	(Rectangle) { TILE_SIZE, 0, TILE_SIZE, TILE_SIZE }
#define RECTANGLE_PLAYER \
	(Rectangle){0, TILE_SIZE, TILE_SIZE, TILE_SIZE * 2}

// -- global state --
uint8_t score[2];
Camera2D camera;
Texture2D atlas;

float screen_width;
float screen_height;
float virtual_width;
float virtual_height;


typedef struct {
	float ticks;
	float seconds;
	bool finished;
} ctimer_t;
void ctimer_init(ctimer_t* timer, float seconds);
void ctimer_tick(ctimer_t* timer, float dt);
bool ctimer_finished(ctimer_t* timer);
void ctimer_restart(ctimer_t* timer);

void ctimer_init(ctimer_t* timer, float seconds){
	timer->finished=false;
	timer->seconds=seconds;
	timer->ticks=0.0f;
}
void ctimer_tick(ctimer_t* timer, float dt){
	if(timer->ticks >= timer->seconds)
	return;

	timer->ticks=+dt;
	if(timer->ticks >= timer->seconds)
		timer->finished = true;
}

bool ctimer_finished(ctimer_t* timer){
	return timer->finished;
}

void ctimer_restart(ctimer_t* timer){
	timer->finished=false;
	timer->ticks = 0.0f;
}



typedef struct
{
	Vector2 min;
	Vector2 max;
} aabb_t;
void aabb_init(aabb_t *bb, Vector2 min, Vector2 max);
void aabb_render(aabb_t bb);
bool aabb_collide(aabb_t bb1, aabb_t bb2); // 8 bytes per struct, no need to pass as ref
void aabb_offset(aabb_t *bb, Vector2 offset);

#define PLAYERS_MAX 2
#define BALLS_MAX 500

aabb_t players[PLAYERS_MAX];
void players_init();
void players_input_handler();

uint16_t balls_offset = 0;
uint16_t balls_capacity = BALLS_MAX; // no need in this case. addition for the future
aabb_t balls[BALLS_MAX];
Vector2 velocity[BALLS_MAX];

void balls_init();
void balls_append();
void balls_delete();
void balls_update();

void draw_background()
{
	for (uint16_t i = 0; i < virtual_width / TILE_SIZE; i++)
		for (uint16_t j = 0; j < virtual_height / TILE_SIZE; j++)
			DrawTextureRec(atlas, RECTANGLE_BK,(Vector2){i * TILE_SIZE, j * TILE_SIZE},WHITE);
}
void draw_players()
{
	for (uint8_t i = 0; i < PLAYERS_MAX; i++)
	{
		DrawTextureRec(
		atlas,
		RECTANGLE_PLAYER,
		players[i].min,
		WHITE);
	}	
}
void draw_balls()
{
	for (uint16_t i = balls_offset; i-- > 0;)
	{
		DrawTextureRec(
		atlas,
		RECTANGLE_BALL,
		balls[i].min,
		WHITE);
	}	
}
void draw_colliders(){
	for (uint8_t i = 0; i < PLAYERS_MAX; i++)
		aabb_render(players[i]);
	for (uint16_t i = balls_offset; i-- > 0;)
		aabb_render(balls[i]);
}

void aabb_init(aabb_t *bb, Vector2 min, Vector2 max){
	bb->min=min;
	bb->max=max;
}
void aabb_render(aabb_t bb){
	DrawRectangleLines(bb.min.x, bb.min.y, bb.max.x - bb.min.x, bb.max.y - bb.min.y, BLUE);
}
bool aabb_collide(aabb_t bb1, aabb_t bb2){
	return 
	bb1.min.x <= bb2.max.x &&
	bb1.min.y <= bb2.max.y &&
	bb1.max.x >= bb2.min.x &&
	bb1.max.y >= bb2.min.y \
	;
}
void aabb_offset(aabb_t *bb, Vector2 offset){
	bb->min = Vector2Add(bb->min, offset);
	bb->max = Vector2Add(bb->max, offset);
}

void players_init(){
	score[0] = 0;
	score[1] = 0;

	aabb_init(players, Vector2Zero(), (Vector2){TILE_SIZE, TILE_SIZE*2}); 
	aabb_init(players + 1, (Vector2){virtual_width-TILE_SIZE, 0.0f},  (Vector2){virtual_width, TILE_SIZE*2});
};
void _input_handler(uint8_t index, int key_up, int key_down){
	float movement_y = 0.0f; // only vertical movement...
	float dt = GetFrameTime();
	float speed = 50.0f;

	if (IsKeyDown(key_up))
		movement_y = -1.0f;
	if (IsKeyDown(key_down))
		movement_y = 1.0f;
	
	float step_y =  movement_y * dt * speed;
	
	players[index].min.y = Clamp(players[index].min.y + step_y, 0,virtual_height - TILE_SIZE*2);
	players[index].max.y = players[index].min.y + TILE_SIZE*2;
	
}
void players_input_handler(){
	_input_handler(0, KEY_W, KEY_S);
	_input_handler(1, KEY_O, KEY_L);
}

void balls_append(){
	Vector2 md = (Vector2) {virtual_width/2 - TILE_SIZE/2,virtual_height/2 - TILE_SIZE/2};
	aabb_init(balls + balls_offset, md, Vector2AddValue(md, TILE_SIZE));
	velocity[balls_offset]= (Vector2){-50, 0.0f};
	balls_offset++;
};


void balls_delete(){
	if(balls_offset == 1){
		balls_offset--;
		return;
	}
}

void balls_update(){
	float dt = GetFrameTime();
	
	for (uint16_t i = balls_offset; i-- > 0;)
	{

		// check for player colide
		for (size_t p = 0; p < PLAYERS_MAX; p++)
		{
			if(aabb_collide(players[p], balls[i])){
				Vector2 ball_center   = {0};
				Vector2 player_center = {0};
				float max_ang = 50;

				
				ball_center.x = balls[i].min.x + (balls[i].max.x - balls[i].min.x) / 2.0f;
				ball_center.y = balls[i].min.y + (balls[i].max.y - balls[i].min.y) / 2.0f;


				player_center.x = players[p].min.x + (players[p].max.x - players[p].min.x) / 2.0f;
				player_center.y = players[p].min.y + (players[p].max.y - players[p].min.y) / 2.0f;

				float overlap_x = fmin(balls[i].max.x, players[p].max.x) - fmax(balls[i].min.x, players[p].min.x);
				float overlap_y = fmin(balls[i].max.y, players[p].max.y) - fmax(balls[i].min.y, players[p].min.y);
				
				bool top_kick = false;
				bool bottom_kick = false;
				Vector2 n = {0};
				if(overlap_x > overlap_y){
					
					// arriba o abajo
					if(ball_center.y > player_center.y){
						//arriba
						top_kick = true;
						n = (Vector2){0.0, 1.0f};
						
					}else{
						//abajo
						bottom_kick=true;
						n = (Vector2){0.0, -1.0f};

					}
				}else{
					if(ball_center.x < virtual_width /2 ){
						// mas cerca del p1
						n = (Vector2){1.0, 0.0f};

					}else{
						n = (Vector2){-1.0, 0.0f};

					}
				}
				
				float push = (n.x != 0.0f) ? overlap_x : overlap_y;
				aabb_offset(balls + i, Vector2Scale(n, push));
				float speed = Vector2Length(velocity[i]);
				
				// tilesize is half size of player
				float dy = Vector2Subtract(ball_center, player_center).y / TILE_SIZE;
				dy = Clamp(dy, -1, 1);
				float new_ang = dy * max_ang;
				
				if(top_kick || bottom_kick){
					//float dir = (ball_center.x < virtual_width/2) ?  -1.0f:1.0f;
					//velocity[i].x = dir * velocity[i].x;
					//velocity[i].y = bottom_kick ? -velocity[i].y : velocity[i].y;

				}else{	
					velocity[i].x = n.x * speed * cosf(new_ang * (PI/180));
					velocity[i].y =  speed * sinf(new_ang * (PI/180));	
				}
			}
		}	

		// collide con bordes
		if(balls[i].min.y < 0) {
			velocity[i] = Vector2Reflect(velocity[i], (Vector2){0.0,-1.0});
		}
		if(balls[i].max.y > virtual_height) {
			velocity[i] = Vector2Reflect(velocity[i], (Vector2){0.0,1.0});
		}
		
		if(balls[i].min.x <= 0) {
			score[1] +=1;
			balls_delete();
			balls_append();
		}
		if(balls[i].max.x >= virtual_width) {
			score[0] +=1;
			balls_delete();
			balls_append();
		}

		// update
		Vector2 v = {0};
		v.x  +=  velocity[i].x * dt;
		v.y  +=  velocity[i].y * dt;
		aabb_offset(balls + i, v);
	}
}

int main(void)
{
	const char *w_title = "PONG!";

	screen_width = 640;
	screen_height = 576;
	virtual_width = 160;
	virtual_height = 144;

	InitWindow(screen_width, screen_height, w_title);
	
	atlas = LoadTexture("./pong_sprite_sheet.png");
	RenderTexture2D target = LoadRenderTexture(virtual_width, virtual_height);
	float scale = fminf(screen_width / virtual_width, screen_height / virtual_height);

	// no resize window.
	Vector2 offset = {(GetScreenWidth() - target.texture.width * scale) * 0.5, (GetScreenHeight() - target.texture.height * scale) * 0.5};
	Rectangle src = {0.0f, 0.0f, target.texture.width, -target.texture.height};
	Rectangle dst = {offset.x, offset.y, target.texture.width * scale - offset.x, target.texture.height * scale - offset.y};
	SetTargetFPS(60);
	camera.zoom = 1.0;
	
	players_init();
	balls_append();
	while (!WindowShouldClose())
	{
		players_input_handler();

		balls_update();
		BeginTextureMode(target);
		
		BeginMode2D(camera);

			draw_background();
			draw_players();
			draw_balls();
			
			//draw_colliders();
						

		EndMode2D();
		DrawText(TextFormat("score1 %i, score2 %i,", score[0],score[1]), virtual_width/2 - 30, virtual_height - 10, 1, BLACK);
		EndTextureMode();

		BeginDrawing();
		// ClearBackground(RAYWHITE);
		DrawTexturePro(target.texture, src, dst, offset, 0.0f, WHITE);
		// DrawText("Do you realize?", screen_resolution_x/2, screen_resolution_y/2, 12, BLACK);
		EndDrawing();
	}
	UnloadRenderTexture(target);
	CloseWindow();

	return 0;
}
