#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>



typedef struct {uint8_t size_x; uint8_t size_y;} sprite_t;
typedef struct {Vector2 min; Vector2 max;} aabb_t;
void aabb_init(aabb_t* bb, Vector2 min, Vector2 max);
void aabb_render(aabb_t bb);
bool aabb_collide(aabb_t bb1, aabb_t bb2); // 8 bytes per struct, no need to pass as ref
void aabb_update(aabb_t* bb, Vector2 offset);

Camera2D CAMERA;

int main(void)
{
	char *w_title = "PONG!";

	float screen_width = 640;
	float screen_height = 576;
	float virtual_width = 160;
	float virtual_height = 144;

	InitWindow(screen_width, screen_height, w_title);

	RenderTexture2D target = LoadRenderTexture(virtual_width, virtual_height);
	float scale = fminf(screen_width/virtual_width, screen_height/virtual_height);

	// no resize window.
	Vector2 offset = {(GetScreenWidth() - target.texture.width*scale)*0.5, (GetScreenHeight() - target.texture.height*scale)*0.5};
	Rectangle src  = {0.0f, 0.0f, target.texture.width , -target.texture.height};
	Rectangle dst  = {offset.x, offset.y, target.texture.width*scale - offset.x, target.texture.height*scale - offset.y};
	
	CAMERA.offset = (Vector2){virtual_width/2, virtual_height/2};
	CAMERA.zoom = scale;
	
	while (!WindowShouldClose())
	{
		BeginTextureMode(target);
		ClearBackground(BLACK);	
			BeginMode2D(CAMERA);
			DrawCircleV(Vector2Zero(), 1.0f, MAROON);
			EndMode2D();
		EndTextureMode();
		
		
		
		

		BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawTexturePro(target.texture, src, dst, offset, 0.0f, WHITE);
			//DrawText("Do you realize?", screen_resolution_x/2, screen_resolution_y/2, 12, BLACK);
		EndDrawing();
	}
	UnloadRenderTexture(target);
	CloseWindow();

	return 0;
}
