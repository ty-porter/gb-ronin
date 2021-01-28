#include <gb/gb.h>
#include <gb/console.h>

#include <stdio.h>
#include <rand.h>

#include "background.h"

#include "dirt.c"
#include "samurai.c"
#include "slash.c"
#include "shuriken.c"

// Coordinates
#define START_POS_X     64
#define START_POS_Y     64
#define WIN_X_RIGHT    152
#define WIN_X_LEFT       8
#define SHURIKEN_MIN_Y  20u
#define SHURIKEN_MAX_Y  80u
#define FLOOR_Y        100

// Physics
#define GRAVITY          1
#define JUMP_VELOCITY   10
#define MOVE_SPEED       2
#define MAX_Y_SPEED      5

// Animation
#define RUN_ANIM_LENGTH  4
#define SLSH_ANIM_LENGTH 4
#define ANIM_DELAY       4
#define ANIM_IDLE_START 13
#define ANIM_FALL_START  3
#define SLSH_ANIM_SPEED  2

// Projectiles
#define MAX_SHURIKEN     5
#define SHURIKEN_VEL     2
#define SHRKN_DIR     0x01 // Flag for shuriken direction (left is 0x01)
#define SHRKN_MOVING  0x04 // Flag to determine if shuriken is moving
#define SHRKN_SLSHED  0x08 // Flag for shurikens that have been slashed
#define SHRKN_CHANCE  0.5f // Chance to throw a new projectile

// Sprites
#define SAMURAI          1 // to 4
#define SLASH            5 // to 8
#define SHURIKEN         9 // to MAX_SHURIKEN

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

void player_anim_slash(INT8 pos_x, INT8 pos_y, INT8 frame_nb) {

    /*
        chr_offset_x: offset from player character (-16 or 16)
        top_mod:      modifies x offset of top row (8 or 0)
        bot_mod:      modifies x offset of bottom row (8 or 0)
    */
    INT8 chr_offset_x, top_mod, bot_mod;

    // Read the props from Sprite 1 (main character) to determine direction
    if (get_sprite_prop(1) & S_FLIPX) {
        // Facing left
        chr_offset_x = -16;
        top_mod = 8;
        bot_mod = 0;
    }
    else {
        // Facing right
        chr_offset_x = 16;
        top_mod = 0;
        bot_mod = 8;
    }

    for (INT8 i = 0; i < 4; i++) {
        set_sprite_prop(5 + i, get_sprite_prop(1) & S_FLIPX);
    }

    set_sprite_prop(7, get_sprite_prop(7) | S_FLIPY);
    set_sprite_prop(8, get_sprite_prop(8) | S_FLIPY);

    move_sprite(5, pos_x + chr_offset_x + top_mod, pos_y);
    move_sprite(6, pos_x + chr_offset_x + bot_mod, pos_y);
    move_sprite(7, pos_x + chr_offset_x + top_mod, pos_y + 8);
    move_sprite(8, pos_x + chr_offset_x + bot_mod, pos_y + 8);
    

    INT8 frame_offset = (frame_nb % 2) * 2;

    if (frame_nb < 2) {
        set_sprite_tile(5, 17 + frame_offset);
        set_sprite_tile(6, 18 + frame_offset);

        set_sprite_tile(7, 21); // blank
        set_sprite_tile(8, 21); // blank
    } else {
        set_sprite_tile(7, 19 - frame_offset);
        set_sprite_tile(8, 20 - frame_offset);

        set_sprite_tile(5, 21); // blank
        set_sprite_tile(6, 21); // blank
    }
}

void player_anim_slash_cancel() {
    for (INT8 i = 0; i < 4; i++) {
        set_sprite_tile(5 + i, 21);
    }
}

void projectile_anim_spin(INT8 tick) {
    for (INT8 i = 0; i < MAX_SHURIKEN; i++) {
        set_sprite_tile(SHURIKEN + i, 22 + tick % 2);
    }
}

void play_sound_player_jump() {
    NR41_REG = 0x04;
    NR42_REG = 0x17;
    NR43_REG = 0x51;
    NR44_REG = 0xC0;
}

void play_sound_player_attack(INT8 frame_nb) {
    NR41_REG = 0x01;
    NR42_REG = 0x47;
    NR43_REG = 0x11 + (0x10 * frame_nb);
    NR44_REG = 0xC0;
}

void play_sound_projectile_slashed() {
    NR10_REG = 0x0F;
    NR11_REG = 0xBF;
    NR12_REG = 0x65;
    NR13_REG = 0x6C;
    NR14_REG = 0x87;
}

void play_queued_sounds() {
    NR50_REG = 0x77;
    NR51_REG = 0xFF;
    NR52_REG = 0x80;
}

INT8 slashed_projectile(INT8 x, INT8 y, unsigned char projectiles[MAX_SHURIKEN][3]) {
    INT8 x_offset = (get_sprite_prop(1) & S_FLIPX) ? -16 : 16;
    UINT8 y_offset = 8; // small allowance for hitbox
    UINT8 slashed = 0;

    for (INT8 i = 0; i < MAX_SHURIKEN; i++) {
        // If within slash width
        if (projectiles[i][1] >= x + x_offset && projectiles[i][1] <= x + x_offset + 16) {
            // If within slash height
            if (projectiles[i][2] >= y - y_offset && projectiles[i][2] <= y + 16 + y_offset) {
                // Not already slashed
                if (!(projectiles[i][0] & SHRKN_SLSHED)) {
                    slashed = 1;
                }

                // Set these to slashed even if they've already been slashed
                projectiles[i][0] = projectiles[i][0] | SHRKN_SLSHED;

                // Set their move flag to false
                projectiles[i][0] = projectiles[i][0] & ~SHRKN_MOVING;
            }
        }
    }

    return slashed;
}

// Initialize the player character and weapons
void init_character() {
    // Samurai -- Sprites 0 - 16
    set_sprite_data(0, 17, samurai);

    // Slash -- Sprite 17 - 21 (21 is blank)
    set_sprite_data(17, 5, slash);

    player_anim_idle(); // Set character tiles to idle animation
    
    set_sprite_tile(5, 17); // slash top left
    set_sprite_tile(6, 17); // slash top right
    set_sprite_tile(7, 17); // slash bottom left
    set_sprite_tile(8, 17); // slash bottom right

    set_sprite_prop(7, get_sprite_prop(7) | S_FLIPY);
    set_sprite_prop(8, get_sprite_prop(8) | S_FLIPY);
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

void init_background() {
    set_bkg_data(0x0, background_tile_count, background_tile_data);
    set_bkg_tiles(0, 0, background_tile_map_width, background_tile_map_height, background_map_data);
}

// Initialize projectiles on screen
// Begins with sprite # (start)
// Needs props to init 
void init_projectiles(unsigned char props[MAX_SHURIKEN][3]) {
    // Sprites 22, 23
    set_sprite_data(22, 2, shuriken);

    for (INT8 i = 0; i < MAX_SHURIKEN; i++) {
        set_sprite_tile(SHURIKEN + i, 22);

        move_sprite(SHURIKEN + i, props[i][1], props[i][2]);
    }
}

void init_new_projectile_props(INT8 nb, unsigned char props[MAX_SHURIKEN][3]) {
    props[nb][0] = 0x00; // Reset everything

    if (rand() % 2) {
        props[nb][0] = props[nb][0] & ~SHRKN_DIR; // Moving right
        props[nb][1] = 0;                         // Offscreen to left
    }
    else {
        props[nb][0] = props[nb][0] | SHRKN_DIR;  // Moving left
        props[nb][1] = 168;                       // Offscreen to right
    }

    props[nb][2] = (rand() % (SHURIKEN_MAX_Y - SHURIKEN_MIN_Y + 1)) + SHURIKEN_MIN_Y;

    move_sprite(nb + SHURIKEN, props[nb][1], props[nb][2]);
}

void move_projectiles(unsigned char props[MAX_SHURIKEN][3]) {
    for (INT8 i = 0; i < MAX_SHURIKEN; i++) {
        // If not already moving, and not falling
        if (!(props[i][0] & SHRKN_MOVING) && !(props[i][0] & SHRKN_SLSHED)) {
            // Check if it can start moving
            if (((UINT8) rand()) / 255 > SHRKN_CHANCE) {
                props[i][0] = props[i][0] | SHRKN_MOVING;
            }
        }

        // Else if moving, but not slashed (falling)
        if (props[i][0] & SHRKN_MOVING && !(props[i][0] & SHRKN_SLSHED)) {
            INT8 d_x_mov = (props[i][0] & SHRKN_DIR) ? -SHURIKEN_VEL : SHURIKEN_VEL;

            props[i][1] = props[i][1] + d_x_mov;

            move_sprite(SHURIKEN + i, props[i][1], props[i][2]);
        }

        // Else if slashed (falling)
        if (props[i][0] & SHRKN_SLSHED) {
            props[i][0] = props[i][0] & ~SHRKN_MOVING; // Explicitly make sure it's not moving, just in case

            // Note, this is the negative of the normal movement divided by 2
            INT8 d_x_fall = -( (props[i][0] & SHRKN_DIR) ? -SHURIKEN_VEL : SHURIKEN_VEL) / 2;

            props[i][1] = props[i][1] + d_x_fall;
            props[i][2] = props[i][2] + SHURIKEN_VEL;

            move_sprite(SHURIKEN + i, props[i][1], props[i][2]);
        }

        // If offscreen, reinitialize
        if (!(props[i][0] & SHRKN_DIR) && props[i][1] > 160) { // X right
            init_new_projectile_props(i, props);
        }
        else if (props[i][0] & SHRKN_DIR && props[i][1] < 4) { // X left
            init_new_projectile_props(i, props);
        }
        else if (props[i][2] > 144) { // Y, only need to check one because shurikens only fall
            init_new_projectile_props(i, props);
        }
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
    if (d_y + GRAVITY > MAX_Y_SPEED) {
        return MAX_Y_SPEED;
    }

    return d_y + GRAVITY;
}

INT8 debounced_input(INT8 target, INT8 new_key, INT8 old_key) {
    INT8 new_press, old_press;

    new_press = new_key & target;
    old_press = old_key & target;

    return new_press && !old_press;
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
    INT8  run_frame   = -1; // Not running
    INT8  slash_frame = -1; // Not slashing
    UINT8 tick        =  0;
    UINT8 anim_tick   =  0; // Last animation tick

    /* Projectile data

    2D array of size MAX_SHURIKEN x 3

    Each row holds props about projectiles: { direction, x_pos, y_pos }
    */
    unsigned char projectile_props[MAX_SHURIKEN][3];

    for (UINT8 i = 0; i < MAX_SHURIKEN; i++) {
        init_new_projectile_props(i, projectile_props);
    }

    init_background();
    init_character();
    init_projectiles(projectile_props);

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
            play_sound_player_jump();
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
        if (debounced_input(J_B, key1, key2) && slash_frame == -1) {
            slash_frame = 0; // Start slash animation
        }
        else if (slash_frame >= SLSH_ANIM_LENGTH) {
            slash_frame = -1; // Stop the slash
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

        if (slash_frame != -1) {
            player_anim_slash(pos_x, pos_y, slash_frame);
            if (tick % SLSH_ANIM_SPEED == 1) {
                slash_frame += 1;
            }
            play_sound_player_attack(slash_frame);
            
            if (slashed_projectile(pos_x, pos_y, projectile_props) == 1) {             
                play_sound_projectile_slashed();
            }
        }
        else {
            player_anim_slash_cancel();
        }
        
        projectile_anim_spin(tick / ANIM_DELAY);
        move_projectiles(projectile_props);

        move_character(pos_x, pos_y);

        wait_vbl_done();

        tick += 1; // Increment tick counter
        if (anim_tick > tick) {
            anim_tick = tick - ANIM_DELAY; // Handle overflows for the game counter
        }

        // Play the sounds!
        play_queued_sounds();
    }
}