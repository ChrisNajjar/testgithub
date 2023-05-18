
#include <allegro.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FPS 60
#define TIMER_TICKS (1000 / FPS)

typedef struct {
    int x, y;
    bool is_caught;
    int speed;
    bool move_right;
    bool is_dragging;

} Duck;



BITMAP *sourcebackground;
BITMAP *duck_image;
BITMAP *cursor_image;
BITMAP *panier_image;


Duck ducks[10];
int score;
int remaining_time;
bool game_over;
int message_timer = 0;


void initialiser_allegro() {
    allegro_init();
    install_keyboard();
    install_mouse();
    install_timer();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    srand(time(NULL));
}
// Les images bmp
void charger_ressources() {
    sourcebackground = load_bitmap("../IMAGES/lacbmp.bmp", NULL);
    if (!sourcebackground) {
        allegro_message("Error loading waterbmp.bmp");
        exit(1);
    }
    BITMAP* background= create_bitmap(SCREEN_WIDTH,SCREEN_HEIGHT);
    stretch_blit(sourcebackground,background,0,0,sourcebackground->w,sourcebackground->h,0,0,background->w,background->h);

    duck_image = load_bitmap("../IMAGES/canard1.bmp", NULL);
    if (!duck_image) {
        allegro_message("Error loading canard1.bmp");
        exit(1);
    }

    cursor_image = load_bitmap("../IMAGES/caneapechebmp.bmp", NULL);
    if (!cursor_image) {
        allegro_message("Error loading caneapechebmp.bmp");
        exit(1);
    }
}

BITMAP *charger_ressources_panier() {
    BITMAP *panier_image = load_bitmap("../IMAGES/panierbmp.bmp", NULL);
    if (!panier_image) {
        allegro_message("Erreur lors du chargement de l'image du panier");
        exit(EXIT_FAILURE);
    }
    return panier_image;
}
// Fonctions

void draw_game(BITMAP *buffer, int panier_x, int panier_y, BITMAP *panier_image) {
    draw_sprite(buffer, panier_image, 625, 225);
    for (int i = 0; i < score; i++) {
        stretch_sprite(buffer, duck_image, 10 + i * 50, SCREEN_HEIGHT - panier_image->h - 50, 50, 50);
    }
}

void dessiner_canard(BITMAP *buffer, Duck *duck) {
    if (!duck->is_caught) {
        draw_sprite(buffer, duck_image, duck->x, duck->y);
    }
}

void dessiner_jeu(BITMAP *buffer, BITMAP *panier_image) {
    draw_sprite(buffer, sourcebackground, 0, 0);
    for (int i = 0; i < 10; i++) {
        dessiner_canard(buffer, &ducks[i]);
    }

    draw_sprite(buffer, panier_image, 625, 225);
    draw_sprite(buffer, cursor_image, mouse_x, mouse_y - cursor_image->h);

    textprintf_ex(buffer, font, 10, 10, makecol(0, 0, 255), -1, "Score: %d", score);
    textprintf_ex(buffer, font, 10, 30, makecol(247, 6, 151), -1, "Temps restant: %d", remaining_time / FPS);

    if (message_timer > 0) {
        textprintf_ex(buffer, font, 160, 30, makecol(255, 0, 0), -1, "! Vous avez perdu 5 secondes !");
        message_timer--;
    }

    if (game_over) {
        textout_centre_ex(buffer, font, "Game Over", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, makecol(255, 0, 0), -1);
        textprintf_centre_ex(buffer, font, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 20, makecol(255, 255, 255), -1, "Score: %d", score);
        textout_centre_ex(buffer, font, "Appuyez sur n'importe quelle touche pour recommencer", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 40, makecol(255, 255, 255), -1);
    }
}

void creer_canard(Duck *duck) {
    duck->x = rand() % (SCREEN_WIDTH - duck_image->w);
    duck->y = rand() % (600 - 413 + 1) + 413-duck_image->h;
    duck->is_caught = false;
    duck->speed = 1 + rand() % 3;
    duck->move_right = rand() % 2;
}

void attraper_canard(Duck *duck, int x, int y, bool is_mouse_clicked) {
    if (!duck->is_caught && is_mouse_clicked &&
        x >= duck->x && x <= duck->x + duck_image->w &&
        y >= duck->y && y <= duck->y + duck_image->h) {
        duck->is_dragging = true; // Le canard est maintenant en train d'être déplacé
    }
}

void mise_a_jour_jeu(BITMAP *buffer, BITMAP *panier_image) {
    int canards_peches = 0;

    for (int i = 0; i < 10; i++) {
        if (!ducks[i].is_caught) {
            attraper_canard(&ducks[i], mouse_x, mouse_y, (mouse_b & 1));

            ducks[i].x += ducks[i].speed;

            if (ducks[i].x > SCREEN_WIDTH) {
                ducks[i].x = 0;
            }

            if (ducks[i].is_caught) {
                if (duck_image->w > 50 || duck_image->h > 50) {
                    stretch_sprite(buffer, duck_image, 625 + i * 50, 225 - 50, 50, 50);
                } else {
                    draw_sprite(buffer, duck_image, 625 + i * 50, 225 - duck_image->h);
                }
            }

            // Déplacement du canard avec la souris
            if (ducks[i].is_dragging && mouse_b & 1) {
                // Le canard est en train d'être déplacé et le bouton de la souris est toujours enfoncé
                ducks[i].x = mouse_x - duck_image->w / 2;
                ducks[i].y = mouse_y - duck_image->h / 2;
            }
            else if (ducks[i].is_dragging && !(mouse_b & 1)) {
                // Le canard est en train d'être déplacé mais le bouton de la souris a été relâché
                ducks[i].is_dragging = false;
                // Vérifiez si le canard est dans le panier
                if (ducks[i].x >= 625 && ducks[i].y >= 225 && ducks[i].x <= 625 + panier_image->w && ducks[i].y <= 225 + panier_image->h) {
                    ducks[i].is_caught = true;
                    score++;
                } else {
                    ducks[i].y = rand() % (600 - 413 + 1) + 413 - duck_image->h;
                    remaining_time -= FPS * 5;
                    message_timer = FPS * 10;
                    if (remaining_time < 0) {
                        remaining_time = 0;
                    }
                }
            }

        } else {
            canards_peches++;
        }
    }

    if (canards_peches == 10) {
        game_over = true;
    }

    if (remaining_time > 0) {
        remaining_time--;
    } else {
        game_over = true;
    }
}
    volatile int timer_counter = 0;

    void timer_callback() {
        timer_counter++;
    }
    END_OF_FUNCTION(timer_callback)

void boucle_de_jeu() {
    BITMAP *buffer;
    BITMAP *panier_image;
    bool restart = false;

    do {
        clear_keybuf(); // Effacer le tampon de clavier ici

        buffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
        panier_image = charger_ressources_panier();

        for (int i = 0; i < 10; i++) {
            creer_canard(&ducks[i]);
        }
        score = 0;
        remaining_time = FPS * 60; // Réinitialise le temps restant ici
        game_over = false;

        install_int_ex(timer_callback, MSEC_TO_TIMER(TIMER_TICKS));

        while (!game_over) {
            if(key[KEY_ESC]) {
                game_over = true;
                break;
            }
            while (timer_counter > 0 && !key[KEY_ESC]) {
                mise_a_jour_jeu(buffer, panier_image);
                timer_counter--;
            }

            dessiner_jeu(buffer, panier_image);
            blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            draw_game(buffer, 10, SCREEN_HEIGHT - panier_image->h, panier_image);
        }

        double temps_total = (FPS * 60 - remaining_time) / FPS; // Calculer la durée totale

        if (game_over) {
            clear(screen);
            textout_centre_ex(screen, font, "Game Over", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, makecol(255, 0, 0), -1);
            textprintf_centre_ex(screen, font, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 20, makecol(255, 255, 255), -1, "Score: %d", score);
            textprintf_centre_ex(screen, font, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 40, makecol(255, 255, 255), -1, "Temps: %.2f secondes", temps_total);
            textout_centre_ex(screen, font, "Appuyez sur ESPACE pour rejouer ou ESC pour quitter", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 60, makecol(255, 255, 255), -1);

            while (!keypressed()) {
                if (key[KEY_SPACE]) {
                    restart = true;
                    break;
                }
                else if (key[KEY_ESC]) {
                    restart = false;
                    break;
                }
            }

            clear_keybuf();
        }

        destroy_bitmap(buffer);
        destroy_bitmap(panier_image);
    } while (restart);
}

void liberer_ressources() {
        destroy_bitmap(sourcebackground);
        destroy_bitmap(duck_image);
        destroy_bitmap(cursor_image);
    }

    int main() {
        initialiser_allegro();
        charger_ressources();
        boucle_de_jeu();
        liberer_ressources();

        return 0;
    }
    END_OF_MAIN()











