
#include <allegro.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FPS 60
#define TIMER_TICKS (1000 / FPS)
#define LIMITE_HAUTE_EAU 100
#define LIMITE_BASSE_EAU 500

// Structures
typedef struct {
    int x, y;
    bool is_caught;
    int speed;
    bool move_right;
} Duck;

BITMAP *sourcebackground;
BITMAP *duck_image;
BITMAP *cursor_image;

Duck ducks[10];
int score;
int remaining_time;
bool game_over;

void initialiser_allegro() {
    allegro_init();
    install_keyboard();
    install_mouse();
    install_timer();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    srand(time(NULL));
}

//Les images bmp
void charger_ressources() {
    sourcebackground = load_bitmap("../lacbmp.bmp", NULL);
    if (!sourcebackground) {
        allegro_message("Error loading waterbmp.bmp");
        exit(1);
    }
    BITMAP* background= create_bitmap(SCREEN_WIDTH,SCREEN_H);
    stretch_blit(sourcebackground,background,0,0,sourcebackground->w,sourcebackground->h,0,0,background->w,background->h);





    duck_image = load_bitmap("../canard4.bmp", NULL);
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
void creer_canard(Duck *duck) {//met le canard dans l ecran et bouge
    duck->x = rand() % (SCREEN_WIDTH - duck_image->w);
    duck->y = rand() % (600 - 413 + 1) + 413-duck_image->h;
    duck->is_caught = false;
    duck->speed = 1 + rand() % 3; // Set a random speed between 1 and 3
    duck->move_right = rand() % 2; // Set a random initial direction (true for right, false for left)
}

void dessiner_canard(BITMAP *buffer, Duck *duck) {
    if (!duck->is_caught) {
        draw_sprite(buffer, duck_image, duck->x, duck->y);
    }
}

void attraper_canard(Duck *duck, int x, int y, bool is_mouse_clicked) {
    if (!duck->is_caught && is_mouse_clicked &&
        x >= duck->x && x <= duck->x + duck_image->w &&
        y >= duck->y && y <= duck->y + duck_image->h) {
        duck->is_caught = true;
        score++;
    }
}

// Mise à jour du jeu
void mise_a_jour_jeu() {
    int canards_peches = 0; // Compteur de canards attrapés

    for (int i = 0; i < 10; i++) {
        if (!ducks[i].is_caught) {
            attraper_canard(&ducks[i], mouse_x, mouse_y, (mouse_b & 1));

            // mouvement canard
            if (ducks[i].move_right) {
                ducks[i].x += ducks[i].speed;
            } else {
                ducks[i].x -= ducks[i].speed;
            }

            // collision mur
            if (ducks[i].x < 0 || ducks[i].x > SCREEN_WIDTH - duck_image->w) {
                ducks[i].move_right = !ducks[i].move_right;
            }
        } else {
            canards_peches++; // Augmente le compteur de canards attrapés
        }
    }

    // Si tous les canards ont été attrapés, terminez le jeu
    if (canards_peches == 10) {
        game_over = true;
    }

    if (remaining_time > 0) {
        remaining_time--;
    } else {
        game_over = true;
    }
}

void dessiner_jeu(BITMAP *buffer) {
    draw_sprite(buffer, sourcebackground, 0, 0);
    for (int i = 0; i < 10; i++) {
        dessiner_canard(buffer, &ducks[i]);
    }

    draw_sprite(buffer, cursor_image, mouse_x, mouse_y-cursor_image->h);

    textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), -1, "Score: %d", score);
    textprintf_ex(buffer, font, 10, 30, makecol(255, 255, 255), -1, "Time: %d", remaining_time / FPS);

    if (game_over) {
        textout_centre_ex(buffer, font, "Game Over", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, makecol(255, 0, 0), -1);
    }
}

volatile int timer_counter;
void timer_callback();

void boucle_de_jeu() {
    BITMAP *buffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
    LOCK_VARIABLE(timer_counter);
    LOCK_FUNCTION(timer_callback);
    install_int_ex(timer_callback, MSEC_TO_TIMER(TIMER_TICKS));
    // Initialiser le jeu
    for (int i = 0; i < 10; i++) {
        creer_canard(&ducks[i]);
    }
    score = 0;
    remaining_time = FPS * 60; // Durée du jeu en secondes (exemple : 60 secondes)
    game_over = false;

    while (!key[KEY_ESC] && !game_over) {
        while (timer_counter > 0) {
            mise_a_jour_jeu();
            timer_counter--;
        }

        dessiner_jeu(buffer);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    destroy_bitmap(buffer);
// Initialiser le jeu
    for (int i = 0; i < 10; i++) {
        creer_canard(&ducks[i]);
    }
    score = 0;
    remaining_time = FPS * 60; // Durée du jeu en secondes (exemple : 60 secondes)
    game_over = false;

    while (!key[KEY_ESC] && !game_over) {
        while (timer_counter > 0) {
            mise_a_jour_jeu();
            timer_counter--;
        }

        dessiner_jeu(buffer);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    destroy_bitmap(buffer);
}

// Terminer et libérer les ressources
void nettoyer() {
    destroy_bitmap(sourcebackground);
    destroy_bitmap(duck_image);
    destroy_bitmap(cursor_image);

}

int main() {
    initialiser_allegro();
    charger_ressources();
    boucle_de_jeu();
    nettoyer();
    return 0;
}
END_OF_MAIN()

// Fonction de rappel du timer
volatile int timer_counter = 0;

void timer_callback() {
    timer_counter++;
}
END_OF_FUNCTION(timer_callback)
