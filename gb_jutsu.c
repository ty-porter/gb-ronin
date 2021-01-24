#include <gb/gb.h>

#include <stdio.h>
#include <rand.h>
#include <gb/console.h>

#include "dirt.c"
#include "samurai.c"
#include "slash.c"
#include "shuriken.c"

#define START_POS_X     64
#define START_POS_Y     64
#define WIN_X_RIGHT    152
#define WIN_X_LEFT       8
#define SHURIKEN_MIN_Y  20u
#define SHURIKEN_MAX_Y  80u
#define FLOOR_Y         80
#define GRAVITY          1
#define JUMP_VELOCITY   10
#define MOVE_SPEED       2
#define RUN_ANIM_LENGTH  4
#define ANIM_DELAY       4
#define ANIM_IDLE_START 13
#define ANIM_FALL_START  3
#define MAX_SHURIKEN     5
#define SHURIKEN_VEL     2

const unsigned char samurai_tile_offsets[] = {
    1,0,
    2,3
};

const unsigned char map_tiles[] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,

    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,

    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

void player_anim_idle() {
    for (INT8 i = 0; i < 4; i++) {
        set_sprite_tile(i + 1, ANIM_IDLE_START + samurai_tile_offsets[i]);
    }
}

void player_anim_run(INT8 offset) {
    INT8 start_tile_index = offset * 3;

    for (INT8 i = 0; i < 4; i++) {
        if (offset != -1 && samurai_tile_offsets[i] > 0) {
            set_sprite_tile(i + 1, start_tile_index + samurai_tile_offsets[i]);
        }
        else {
            set_sprite_tile(i + 1, samurai_tile_offsets[i]);
        }
    }
}

void player_anim_fall() {
    for (INT8 i = 0; i < 4; i++) {
        if (samurai_tile_offsets[i] > 0) {
            set_sprite_tile(i + 1, ANIM_FALL_START + samurai_tile_offsets[i]);
        }
        else {
            set_sprite_tile(i + 1, samurai_tile_offsets[i]);
        }
    }
}

void player_anim_slash(INT8 pos_x, INT8 pos_y) {
   INT8 offset_x;

    // Read the props from Sprite 1 (main character) to determine direction
    offset_x = get_sprite_prop(1) & S_FLIPX ? -8 : 16;

    set_sprite_prop(5, get_sprite_prop(1) & S_FLIPX);
    set_sprite_prop(6, get_sprite_prop(1) & S_FLIPX);

    set_sprite_prop(6, get_sprite_prop(6) | S_FLIPY);

    move_sprite(5, pos_x + offset_x, pos_y);
    move_sprite(6, pos_x + offset_x, pos_y + 8);

    set_sprite_tile(5, 17);
    set_sprite_tile(6, 17);
}

void player_anim_slash_cancel() {
    set_sprite_tile(5, 18);
    set_sprite_tile(6, 18);
}

void projectile_anim_spin(INT8 n, INT8 start, INT8 tick) {
    for (INT8 i = 0; i < n; i++) {
        set_sprite_tile(start + i, 19 + tick % 2);
    }
}

// Initialize the player character and weapons
void init_character() {
    // Samurai -- Sprites 0 - 16
    set_sprite_data(0, 17, samurai);

    // Slash -- Sprite 17, 18 (18 is blank)
    set_sprite_data(17, 2, slash);

    player_anim_idle(); // Set character tiles to idle animation
    
    set_sprite_tile(5, 17); // slash top
    set_sprite_tile(6, 17); // slash bottom
}

void move_character(INT8 x, INT8 y) {
    INT8 direction, left_offset, right_offset;

    direction = get_sprite_prop(1);

    if (direction & S_FLIPX) {
        left_offset = 0;
        right_offset = 8;
    }
    else {
        left_offset = 8;
        right_offset = 0;
    }

    move_sprite(1, x + right_offset, y);
    move_sprite(2, x + left_offset, y);
    move_sprite(3, x + right_offset, y + 8);
    move_sprite(4, x + left_offset, y + 8);
}

void set_character_direction(INT8 d_x) {
    INT8 props;

    // Read the props from Sprite 1 (main character)
    props = get_sprite_prop(1);

    if (!(props & S_FLIPX) && d_x < 0) {
        set_sprite_prop(1, props | S_FLIPX);
        set_sprite_prop(2, props | S_FLIPX);
        set_sprite_prop(3, props | S_FLIPX);
        set_sprite_prop(4, props | S_FLIPX);
    }
    else if ((props & S_FLIPX) && d_x > 0) {
        set_sprite_prop(1, props & ~S_FLIPX);
        set_sprite_prop(2, props & ~S_FLIPX);
        set_sprite_prop(3, props & ~S_FLIPX);
        set_sprite_prop(4, props & ~S_FLIPX);
    }
}

void init_dirt() {
    set_bkg_data(0x0, 1, dirt_data);
    set_bkg_tiles(0, 0, 20, 16, map_tiles);
}

// Initialize projectiles on screen
// Begins with sprite # (start)
// Needs props to init 
void init_projectiles(INT8 start, unsigned char props[MAX_SHURIKEN][3]) {
    // Sprites 19, 20
    set_sprite_data(19, 2, shuriken);

    for (INT8 i = 0; i < MAX_SHURIKEN; i++) {
        set_sprite_tile(start + i, 19);

        move_sprite(start + i, props[i][1], props[i][2]);
    }
}

void move_projectiles(INT8 start, unsigned char props[MAX_SHURIKEN][3]) {
    for (INT8 i = 0; i < MAX_SHURIKEN; i++) {
        INT8 d_x = props[i][0] ? SHURIKEN_VEL : -SHURIKEN_VEL;

        props[i][1] = props[i][1] + d_x;

        move_sprite(start + i, props[i][1], props[i][2]);
    }
}

INT8 is_falling(INT8 y) {
    if (y < FLOOR_Y) {
        return 1;
    } else {
        return 0;
    }
}

INT8 tick_gravity(INT8 d_y) {
    return d_y + GRAVITY;
}

INT8 debounced_input(INT8 target, INT8 new_key, INT8 old_key) {
    INT8 new_press, old_press;

    new_press = new_key & target;
    old_press = old_key & target;

    return new_press && !old_press;
}

UINT8 rand_uint_range(UINT8 min, UINT8 max) {
    return (UINT8) (rand() % (max - min + 1)) + min;
}

void main() {
    BGP_REG = OBP0_REG = OBP1_REG = 0xE4;

    SPRITES_8x8;

    // Input
    INT8 key1  = 0;
    INT8 key2  = 0;

    // Movement
    UINT8 pos_x = START_POS_X;
    UINT8 pos_y = START_POS_Y;
    INT8  d_x   = 0;
    INT8  d_y   = 0;

    // Animation
    INT8  run_frame = -1; // Not running
    UINT8 tick      =  0;
    UINT8 anim_tick =  0; // Last animation tick
    UINT8 slashing  =  0; // Not slashing

    /* Projectile data

    2D array of size MAX_SHURIKEN x 3

    Each row holds props about projectiles: { direction, x_pos, y_pos }
    */
    unsigned char projectile_props[MAX_SHURIKEN][3];

    for (UINT8 i = 0; i < MAX_SHURIKEN; i++) {
        if (rand() % 2) {
            projectile_props[i][0] = 0x00; // Moving right
            projectile_props[i][1] = WIN_X_LEFT;
        }
        else {
            projectile_props[i][0] = 0x01; // Moving left
            projectile_props[i][1] = WIN_X_RIGHT;
        }

        projectile_props[i][2] = rand_uint_range(SHURIKEN_MIN_Y, SHURIKEN_MAX_Y);
    }

    init_character();
    init_dirt();
    init_projectiles(7, projectile_props);

    SHOW_BKG; SHOW_SPRITES;
    
    while(1) {
        // Check if falling
        if (is_falling(pos_y) == 1) {
            d_y = tick_gravity(d_y);
        }
        else {
            d_y = 0;
        }

        // Read the keys
        key2 = key1;
        key1 = joypad();

        // Jump
        if (debounced_input(J_A, key1, key2) && d_y == 0) {
            d_y = -JUMP_VELOCITY;
        }

        // Control the jump if button released
        if (!(key1 & J_A) && d_y < 0) {
            d_y = 0;
        }

        // Move
        if (key1 & J_LEFT) {
            set_character_direction(d_x);
            d_x = -MOVE_SPEED;
        }
        else if (key1 & J_RIGHT) {
            set_character_direction(d_x);
            d_x = MOVE_SPEED;
        }
        else {
            d_x = 0; // Set movement speed to zero
            run_frame = -1; // Stop the run animation
        }

        if (d_x != 0) {
            if (tick - anim_tick >= ANIM_DELAY) {
                run_frame += 1; // Set the running animation frame
                anim_tick = tick;
            }

            if (run_frame >= RUN_ANIM_LENGTH) {
                run_frame = 0;
            }
        }

        // Set x, y position based on velocity
        pos_x = pos_x + d_x;
        pos_y = pos_y + d_y;

        // Clamp the x position to the window
        if (pos_x > WIN_X_RIGHT) {
            pos_x = WIN_X_RIGHT;
            d_x = 0;
            run_frame = -1;
        }
        else if (pos_x < WIN_X_LEFT) {
            pos_x = WIN_X_LEFT;
            d_x = 0;
            run_frame = -1;
        }

        // Clamp the y position to the floor
        if (pos_y > FLOOR_Y) {
            pos_y = FLOOR_Y;  // place on top of floor
            d_y = 0; // Set y velocity to 0 just in case
        }

        // Slash
        if (debounced_input(J_B, key1, key2)) {
            player_anim_slash(pos_x, pos_y);
            slashing = 1;
        }
        else {
            player_anim_slash_cancel();
            slashing = 0;
        }

        // Handle animations now that everything is calculated...
        if (is_falling(pos_y) == 1) {
            player_anim_fall();
        }
        else if (run_frame != -1) {
            player_anim_run(run_frame);
        }
        else {
            player_anim_idle();
        }
        
        projectile_anim_spin(MAX_SHURIKEN, 7, tick / ANIM_DELAY);
        move_projectiles(7, projectile_props);

        move_character(pos_x, pos_y);

        wait_vbl_done();

        tick += 1; // Increment tick counter
        if (anim_tick > tick) {
            anim_tick = tick - ANIM_DELAY; // Handle overflows for the game counter
        }
    }
}