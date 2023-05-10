//test git

/*code plusieur canard bouge cane a peche mouse score et tps ...*/

#include <allegro.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FPS 60
#define TIMER_TICKS (1000 / FPS)

// Structures
typedef struct {
    int x, y;
    bool is_caught;
    int speed;
    bool move_right;
} Duck;

BITMAP *background;
BITMAP *duck_image;
BITMAP *cursor_image;

Duck ducks[10];
int score;
int remaining_time;
bool game_over;

void init_allegro() {
    allegro_init();
    install_keyboard();
    install_mouse();
    install_timer();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    srand(time(NULL));
}

//Les images bmp
void load_resources() {
    background = load_bitmap("../waterbmp.bmp", NULL);
    if (!background) {
        allegro_message("Error loading waterbmp.bmp");
        exit(1);
    }

    duck_image = load_bitmap("../canard3.bmp", NULL);
    if (!duck_image) {
        allegro_message("Error loading canard1.bmp");
        exit(1);
    }

    cursor_image = load_bitmap("../caneapechebmp.bmp", NULL);


    if (!cursor_image) {
        allegro_message("Error loading caneapechebmp.bmp");
        exit(1);
    }
}
// Fonctions
void create_duck(Duck *duck) {//met le canard dans l ecran et bouge
    duck->x = rand() % (SCREEN_WIDTH - duck_image->w);
    duck->y = rand() % (SCREEN_HEIGHT - duck_image->h);
    duck->is_caught = false;
    duck->speed = 1 + rand() % 3; // Set a random speed between 1 and 3
    duck->move_right = rand() % 2; // Set a random initial direction (true for right, false for left)
}

void draw_duck(BITMAP *buffer, Duck *duck) {
    if (!duck->is_caught) {
        draw_sprite(buffer, duck_image, duck->x, duck->y);
    }
}

void catch_duck(Duck *duck, int x, int y) {
    if (!duck->is_caught &&
        x >= duck->x && x <= duck->x + duck_image->w &&
        y >= duck->y && y <= duck->y + duck_image->h) {
        duck->is_caught = true;
        score++;

    }
}

void update_game() {
    for (int i = 0; i < 10; i++) {
        if (!ducks[i].is_caught) {
            catch_duck(&ducks[i], mouse_x, mouse_y);

            // Update the duck's position
            if (ducks[i].move_right) {
                ducks[i].x += ducks[i].speed;
            } else {
                ducks[i].x -= ducks[i].speed;
            }

            // Change the direction if the duck hits the screen boundaries
            if (ducks[i].x < 0 || ducks[i].x > SCREEN_WIDTH - duck_image->w) {
                ducks[i].move_right = !ducks[i].move_right;
            }
        }
    }

    if (remaining_time > 0) {
        remaining_time--;
    } else {
        game_over = true;
    }
}

void draw_game(BITMAP *buffer) {
    draw_sprite(buffer, background, 0, 0);

    for (int i = 0; i < 10; i++) {
        draw_duck(buffer, &ducks[i]);
    }

    draw_sprite(buffer, cursor_image, mouse_x, mouse_y);

    textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), -1, "Score: %d", score);
    textprintf_ex(buffer, font, 10, 30, makecol(255, 255, 255), -1, "Time: %d", remaining_time / FPS);

    if (game_over) {
        textout_centre_ex(buffer, font, "Game Over", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, makecol(255, 0, 0), -1);
    }
}

volatile int timer_counter;
void timer_callback();

void game_loop() {
    BITMAP *buffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
    LOCK_VARIABLE(timer_counter);
    LOCK_FUNCTION(timer_callback);
    install_int_ex(timer_callback, MSEC_TO_TIMER(TIMER_TICKS));



    // Initialiser le jeu
    for (int i = 0; i < 10; i++) {
        create_duck(&ducks[i]);
    }
    score = 0;
    remaining_time = FPS * 60; // Durée du jeu en secondes (exemple : 60 secondes)
    game_over = false;

    while (!key[KEY_ESC] && !game_over) {
        while (timer_counter > 0) {
            update_game();
            timer_counter--;
        }

        draw_game(buffer);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    destroy_bitmap(buffer);
}

// Terminer et libérer les ressources
void clean_up() {
    destroy_bitmap(background);
    destroy_bitmap(duck_image);
    destroy_bitmap(cursor_image);

}


int main() {
    init_allegro();
    load_resources();
    game_loop();
    clean_up();

    return 0;
}
END_OF_MAIN()

// Fonction de rappel du timer
volatile int timer_counter = 0;

void timer_callback() {
    timer_counter++;
}
END_OF_FUNCTION(timer_callback)





/*code background eau avec canard qui s affcihe*/


//code avec background et mini personnages qui bouge
/*
#include <allegro.h>

typedef struct personnage
{
    BITMAP* bas[3];
    BITMAP* gauche[3];
    BITMAP* droite[3];
    BITMAP* haut[3];
    int x, y;
    int direction;
    int frame;
    int en_deplacement;
}t_personnage;


void separer_bitmap_personnage(t_personnage* personnage, BITMAP* bitmap)
{
    int taille_w = 78;
    int taille_h = 92;

    for (int j = 0; j < 3; j++) {
        personnage->bas[j] = create_sub_bitmap(bitmap, j * taille_w, 0 * taille_h, taille_w, taille_h);
        personnage->gauche[j] = create_sub_bitmap(bitmap, j * taille_w, 1 * taille_h, taille_w, taille_h);
        personnage->droite[j] = create_sub_bitmap(bitmap, j * taille_w, 2 * taille_h, taille_w, taille_h);
        personnage->haut[j] = create_sub_bitmap(bitmap, j * taille_w, 3 * taille_h, taille_w, taille_h);
    }
}

void dessiner_personnage(t_personnage* personnage, BITMAP* buffer)
{
    BITMAP* frame;
    switch (personnage->direction) {
        case 0:
            frame = personnage->bas[personnage->frame];
            break;
        case 1:
            frame = personnage->gauche[personnage->frame];
            break;
        case 2:
            frame = personnage->droite[personnage->frame];
            break;
        case 3:
            frame = personnage->haut[personnage->frame];
            break;
    }
    masked_blit(frame, buffer, 0, 0, personnage->x, personnage->y, frame->w, frame->h);
}


void maj_personnage(t_personnage* personnage)
{
    int vitesse = 1;
    personnage->en_deplacement = 0;
    if (key[KEY_UP]) {
        personnage->y -= vitesse;
        personnage->direction = 3;//colonne sur le sprite
        personnage->en_deplacement = 1;
    }
    if (key[KEY_DOWN]) {
        personnage->y += vitesse;
        personnage->direction = 0;//colonne sur le sprite
        personnage->en_deplacement = 1;
    }
    if (key[KEY_LEFT]) {
        personnage->x -= vitesse;
        personnage->direction = 1;//colonne sur le sprite
        personnage->en_deplacement = 1;
    }
    if (key[KEY_RIGHT]) {
        personnage->x += vitesse;
        personnage->direction = 2;//colonne sur le sprite
        personnage->en_deplacement = 1;
    }
}

int main() {
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);

    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);

    BITMAP *background = load_bitmap("../background_mario.bmp", NULL);
    if (!background) {
        allegro_message("Erreur lors du chargement de 'waterbmp.bmp'\n");
        return 1;
    }

    BITMAP *sprite = load_bitmap("../canardbmp.bmp", NULL);
    if (!sprite) {
        allegro_message("Erreur lors du chargement de 'canardbmp.bmp'\n");
        destroy_bitmap(background);
        destroy_bitmap(buffer);
        return 1;
    }



    t_personnage personnage;
    personnage.direction = 0;
    personnage.frame = 0;
    personnage.x = SCREEN_W/2;
    personnage.y = SCREEN_H/2;

    separer_bitmap_personnage(&personnage, sprite);

    int frame_counter = 0;

    while (!key[KEY_ESC]) {

        maj_personnage(&personnage);

        stretch_blit(background, buffer, 0, 0, background->w, background->h, 0, 0, buffer->w, buffer->h);

        dessiner_personnage(&personnage, buffer);

        blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);

        frame_counter++;
        if (frame_counter >= 10) {
            if (personnage.en_deplacement) {
                personnage.frame = (personnage.frame + 1) % 3;
            } else {
                personnage.frame = 0;
            }
            frame_counter = 0;
        }

        rest(10);
    }

    for (int i = 0; i < 3; i++) {
        destroy_bitmap(personnage.bas[i]);
        destroy_bitmap(personnage.gauche[i]);
        destroy_bitmap(personnage.droite[i]);
        destroy_bitmap(personnage.haut[i]);
    }
    destroy_bitmap(sprite);
    destroy_bitmap(background);
    destroy_bitmap(buffer);

    return 0;
}
END_OF_MAIN();
*/

