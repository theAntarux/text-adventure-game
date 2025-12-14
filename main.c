// INCLUDES //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// VARIABLES & DEFS //

#define MAX_ROOMS 6
#define MAX_INVENTORY 10
#define MAX_NAME 50
#define MAX_DESC 200
#define MAX_ITEM 20
#define MAX_MESSAGE 100
#define MAX_LOG 1000
#define MAX_LOG_LINE 256

typedef struct {
    int id;
    char description[MAX_DESC];
    char item[MAX_ITEM];
    int has_npc;
    int npc_defeated;
    int north;
    int south;
    int east;
    int west;
    int locked;
} Room;

typedef struct {
    char name[MAX_NAME];
    int current_room;
    int lives;
    char inventory[MAX_INVENTORY][MAX_ITEM];
    int inventory_count;
    int won;
} Player;

char message_listok_obor[MAX_MESSAGE] = "Obor strazi kluc.";
char message_listok_prsten[MAX_MESSAGE] = "Prsten je na severe od knih.";

char log_lines[MAX_LOG][MAX_LOG_LINE];
char log_filename[256];
int log_count = 0;

Room *rooms;
Player player;

// FUNCTIONS //

// Adds line to the game log
void add_log(const char *text) {
    if (log_count < MAX_LOG) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);

        snprintf(log_lines[log_count], MAX_LOG_LINE, "[%s] %s", timestamp, text);
        log_count++;
    }
}

// Save the entire log to file when the game ends
void save_log() {
    FILE *file = fopen(log_filename, "w");
    if (file == NULL) {
        printf("Failed to save log\n");
        return;
    }

    for (int i = 0; i < log_count; i++) {
        fprintf(file, "%s\n", log_lines[i]);
    }

    fclose(file);
}

// Initializes the rooms
void init_rooms() {
    rooms = (Room *)malloc(MAX_ROOMS * sizeof(Room));
    if (rooms == NULL) {
        printf("Chyba alokacie pamate.\n");
        exit(1);
    }

    // Room 0: Starting room
    rooms[0].id = 0;
    strcpy(rooms[0].description, "Nachadzas sa v prazdnej miestnosti");
    strcpy(rooms[0].item, "");
    rooms[0].has_npc = 0;
    rooms[0].npc_defeated = 0;
    rooms[0].north = 1;
    rooms[0].south = -1;
    rooms[0].east = 2;
    rooms[0].west = 5;
    rooms[0].locked = 0;

    // Room 1: Library with hint note about the ring
    rooms[1].id = 1;
    strcpy(rooms[1].description, "Si v starej kniznici hradu. Na stole lezi pokrceny listok.");
    strcpy(rooms[1].item, "listok_prsten");
    rooms[1].has_npc = 0;
    rooms[1].npc_defeated = 0;
    rooms[1].north = 4;
    rooms[1].south = 0;
    rooms[1].east = -1;
    rooms[1].west = -1;
    rooms[1].locked = 0;

    // Room 2: Room with magic sword
    rooms[2].id = 2;
    strcpy(rooms[2].description, "Vosiel si do miestnosti, kde na starom stole lezi carovny mec.");
    strcpy(rooms[2].item, "mec");
    rooms[2].has_npc = 0;
    rooms[2].npc_defeated = 0;
    rooms[2].north = -1;
    rooms[2].south = -1;
    rooms[2].east = 3;
    rooms[2].west = 0;
    rooms[2].locked = 0;

    // Room 3: Room with the giant guarding the key
    rooms[3].id = 3;
    strcpy(rooms[3].description, "Narazil si na obra!");
    strcpy(rooms[3].item, "kluc");
    rooms[3].has_npc = 1;
    rooms[3].npc_defeated = 0;
    rooms[3].north = -1;
    rooms[3].south = -1;
    rooms[3].east = -1;
    rooms[3].west = 2;
    rooms[3].locked = 0;

    // Room 4: Locked room with the magic ring treasure
    rooms[4].id = 4;
    strcpy(rooms[4].description, "Vosiel si do tmavej komnaty, kde na zlatom podstavci lezi carovny prsten!");
    strcpy(rooms[4].item, "prsten");
    rooms[4].has_npc = 0;
    rooms[4].npc_defeated = 0;
    rooms[4].north = -1;
    rooms[4].south = 1;
    rooms[4].east = -1;
    rooms[4].west = -1;
    rooms[4].locked = 1;

    // Room 5: Secret room with hint note about the giant
    rooms[5].id = 5;
    strcpy(rooms[5].description, "Tato tajna miestnost je plna prachu. Na stole lezi stary listok.");
    strcpy(rooms[5].item, "listok_obor");
    rooms[5].has_npc = 0;
    rooms[5].npc_defeated = 0;
    rooms[5].north = -1;
    rooms[5].south = -1;
    rooms[5].east = 0;
    rooms[5].west = -1;
    rooms[5].locked = 0;
}

// Starts a new game
void new_game(char *name) {
    strcpy(player.name, name);

    player.current_room = 0;
    player.lives = 3;
    player.inventory_count = 0;
    player.won = 0;

    init_rooms();
    snprintf(log_filename, sizeof(log_filename), "logs/%s.log", name);

    log_count = 0;
    add_log("Nova hra zacata.");
}

// Saves the current game
void save_game() {
    char filename[256];
    snprintf(filename, sizeof(filename), "saves/%s.dat", player.name);

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Chyba ukladania hry.\n");
        return;
    }

    fwrite(&player, sizeof(Player), 1, file);
    fwrite(rooms, sizeof(Room) * MAX_ROOMS, 1, file);

    fclose(file);

    save_log();
    add_log("Hra bola ulozena.");
}

// Loads a saved game
void load_game(char *name) {
    char filename[256];
    snprintf(filename, sizeof(filename), "saves/%s.dat", name);

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Chyba nacitania hry.\n");
        exit(1);
    }

    rooms = (Room *)malloc(MAX_ROOMS * sizeof(Room));
    if (rooms == NULL) {
        printf("Chyba alokacie pamate.\n");
        exit(1);
    }

    fread(&player, sizeof(Player), 1, file);
    fread(rooms, sizeof(Room) * MAX_ROOMS, 1, file);

    fclose(file);
    snprintf(log_filename, sizeof(log_filename), "logs/%s.log", name);

    log_count = 0;
    add_log("Hra bola nacitana a pokracuje.");
}

// Checks if the player has a specific item in inventory
int has_item(char *item) {
    for (int i = 0; i < player.inventory_count; i++) {
        if (strcmp(player.inventory[i], item) == 0) {
            return 1;
        }
    }
    return 0;
}

// Prints player statistics and current room info
void print_info() {
    char buf[512];
    snprintf(buf, sizeof(buf), "Info: %s | Zivoty: %d", rooms[player.current_room].description, player.lives);
    add_log(buf);

    printf("%s\n", rooms[player.current_room].description);
    printf("Pocet zivotov: %d\n", player.lives);
    printf("Inventar: ");

    if (player.inventory_count == 0) {
        printf("prazdny");
    } else {
        for (int i = 0; i < player.inventory_count; i++) {
            printf("%s ", player.inventory[i]);
        }
    }

    printf("\n");
}

// Handles player movement
void move(int direction) {
    const char *dir_name[] = {"sever", "juh", "vychod", "zapad"};
    char buf[256];

    int next_room = -1;
    if (direction == 0) next_room = rooms[player.current_room].north;
    else if (direction == 1) next_room = rooms[player.current_room].south;
    else if (direction == 2) next_room = rooms[player.current_room].east;
    else if (direction == 3) next_room = rooms[player.current_room].west;

    if (next_room == -1) {
        snprintf(buf, sizeof(buf), "Pokus o pohyb na %s - cesta neexistuje.", dir_name[direction]);
        add_log(buf);
        printf("Tam sa neda ist.\n");
        return;
    }

    if (rooms[next_room].locked) {
        add_log("Pokus o vstup do zamknutej miestnosti.");
        printf("Dvere su zamknute. Pouzi kluc!\n");
        return;
    }

    player.current_room = next_room;
    snprintf(buf, sizeof(buf), "Pohyb na %s do miestnosti %d.", dir_name[direction], player.current_room);
    add_log(buf);

    if (rooms[player.current_room].has_npc && rooms[player.current_room].npc_defeated == 1) {
        strcpy(rooms[player.current_room].description, "Obor je porazeny a lezi na zemi. Teraz mozes bezpecne preskumat miestnost.");
    }

    if (strcmp(rooms[player.current_room].item, "") == 0) {
        if (player.current_room == 2) {
            strcpy(rooms[player.current_room].description, "Miestnost s velkym stolom je teraz prazdna, ale mas uz mec v rukach.");
        }
        if (player.current_room == 1) {
            strcpy(rooms[player.current_room].description, "Kniznica je prazdna, listok si uz zobral.");
        }
        if (player.current_room == 5) {
            strcpy(rooms[player.current_room].description, "Tajna miestnost je teraz bez listka.");
        }
        if (player.current_room == 3 && rooms[3].npc_defeated == 1) {
            strcpy(rooms[player.current_room].description, "Si v komnate s obrom, ktory lezi zraneny na zemi.");
        }
    }

    printf("%s\n", rooms[player.current_room].description);
}

// Handles item take
void take_item(char *item) {
    char original_item[MAX_ITEM];
    strcpy(original_item, item);

    if (strcmp(item, "listok") == 0) {
        if (strcmp(rooms[player.current_room].item, "listok_prsten") == 0 ||
            strcmp(rooms[player.current_room].item, "listok_obor") == 0) {
            strcpy(item, rooms[player.current_room].item);
        } else {
            add_log("Pokus zobrat listok - ziadny listok tu nie je.");
            printf("Taky predmet tu nie je.\n");
            return;
        }
    } else if (strcmp(rooms[player.current_room].item, item) != 0) {
        char buf[256];
        snprintf(buf, sizeof(buf), "Pokus zobrat %s - predmet tu nie je.", original_item);
        add_log(buf);
        printf("Taky predmet tu nie je.\n");
        return;
    }

    if (player.current_room == 3 && rooms[3].npc_defeated == 0 && strcmp(item, "kluc") == 0) {
        add_log("Pokus zobrat kluc - obor este zije.");
        printf("Najprv poraz obra.\n");
        return;
    }

    if (player.inventory_count >= MAX_INVENTORY) {
        add_log("Pokus zobrat predmet - inventar plny.");
        printf("Inventar je plny.\n");
        return;
    }

    strcpy(player.inventory[player.inventory_count], item);
    player.inventory_count++;
    strcpy(rooms[player.current_room].item, "");

    char buf[256];
    snprintf(buf, sizeof(buf), "Zobral predmet: %s", item);
    add_log(buf);

    printf("Zobral si predmet: %s\n", item);

    if (player.current_room == 4 && strcmp(item, "prsten") == 0) {
        player.won = 1;
        add_log("Hrac nasiel prsten - VYHRA!");
        printf("Gratulujem, uspesne si hru vyhral a nasiel prsten!\n");
        save_game();
        free(rooms);
        exit(0);
    }
}

// Handles item usage
void use_item(char *item) {
    char buf[256];

    if (strcmp(item, "kluc") == 0) {
        if (has_item("kluc")) {
            if (player.current_room == 1 && rooms[4].locked) {
                rooms[4].locked = 0;
                add_log("Hrac odomkol dvere klucem.");
                printf("Odomkol si dvere na do miestnosti.\n");
            } else {
                add_log("Pokus pouzit kluc - nic na odomknutie.");
                printf("Tu nie je co odomknut.\n");
            }
        } else {
            add_log("Pokus pouzit kluc - hrac ho nema.");
            printf("Nemas kluc.\n");
        }
    } else if (strcmp(item, "mec") == 0) {
        if (rooms[player.current_room].has_npc == 0 || rooms[player.current_room].npc_defeated == 1) {
            add_log("Pokus bojovat mec - ziadny nepriatel.");
            printf("Tu nie je s kym bojovat.\n");
            return;
        }

        if (!has_item("mec")) {
            add_log("Pokus bojovat mec - hrac ho nema.");
            printf("Nemas mec.\n");
            return;
        }

        add_log("Boj s obrom zacal.");

        int chance = rand() % 2;
        if (chance == 0) {
            snprintf(buf, sizeof(buf), "Obor prezil utok - hrac stratil zivot (zostava: %d)", player.lives - 1);
            add_log(buf);
            printf("Obor prezil tvoj utok a zautocil naspat. Stracas zivot.\n");
            player.lives--;
            if (player.lives <= 0) {
                add_log("Hrac zomrel - PREHRA.");
                printf("Prehra, nemas zivoty.\n");
                save_game();
                free(rooms);
                exit(0);
            }
        } else {
            add_log("Hrac porazil obra a ziskal kluc.");
            printf("Tvoj utok uspel, obra si zneskodnil, vypadol mu z vrecka kluc.\n");
            rooms[player.current_room].npc_defeated = 1;
        }
    } else if (strcmp(item, "listok_obor") == 0 || strcmp(item, "listok_prsten") == 0) {
        if (has_item(item)) {
            if (strcmp(item, "listok_obor") == 0) {
                add_log("Hrac precital listok_obor.");
                printf("%s\n", message_listok_obor);
            } else {
                add_log("Hrac precital listok_prsten.");
                printf("%s\n", message_listok_prsten);
            }
        } else {
            add_log("Pokus precitat listok - hrac ho nema.");
            printf("Nemas tento listok.\n");
        }
    } else {
        snprintf(buf, sizeof(buf), "Pokus pouzit neznamy predmet: %s", item);
        add_log(buf);
        printf("Nevies pouzit tento predmet.\n");
    }
}

// INITIALIZATION //

int main(int argc, char *argv[]) {
    srand(time(NULL));

    // This check needs to be here as I'm doing .bat tooling and this helps the overall debug of the tooling
    if (argc != 3) {
        printf("Pouzi: game -n Meno alebo -l Meno\n");
        return 1;
    }

    if (strcmp(argv[1], "-n") == 0) {
        new_game(argv[2]);
    } else if (strcmp(argv[1], "-l") == 0) {
        load_game(argv[2]);
    } else {
        printf("Nespravny parameter.\n");
        return 1;
    }

    if (player.won == 1) {
        printf("Carovny prsten je tvoj. Hra je dokoncena.\n");
        free(rooms);
        return 0;
    }

    if (player.lives == 0) {
        printf("Si mrtvy a nemozes pokracovat v hre!");
        free(rooms);
        return 0;
    }

    add_log("Hra spustena.");

    printf("Vitaj v hre. Nachadzas sa na zamku s miestnostami.\n");
    printf("V jednej z nich je ukryty carovny prsten.\n");
    printf("Na odomknutie komnaty s prstenom potrebujes kluc.\n");
    printf("Zoznam prikazov: sever, juh, vychod, zapad, info, zobrat <predmet>, pouzit <predmet>, koniec\n");

    char command[50];
    while (1) {
        printf("> ");
        fgets(command, 50, stdin);
        command[strcspn(command, "\n")] = 0;

        char log_cmd[256];
        snprintf(log_cmd, sizeof(log_cmd), "Prikaz: %s", command);
        add_log(log_cmd);

        if (strcmp(command, "sever") == 0) move(0);
        else if (strcmp(command, "juh") == 0) move(1);
        else if (strcmp(command, "vychod") == 0) move(2);
        else if (strcmp(command, "zapad") == 0) move(3);
        else if (strcmp(command, "info") == 0) print_info();
        else if (strstr(command, "zobrat ") != NULL) {
            char item[MAX_ITEM];
            sscanf(command, "zobrat %s", item);
            take_item(item);
        } else if (strstr(command, "pouzit ") != NULL) {
            char item[MAX_ITEM];
            sscanf(command, "pouzit %s", item);
            use_item(item);
        } else if (strcmp(command, "koniec") == 0) {
            add_log("Hrac ukoncil hru prikazom koniec.");
            printf("Koncis hru predcasne, ukladam jej stav na disk.\n");
            save_game();
            free(rooms);
            return 0;
        } else {
            add_log("Neznamy prikaz.");
            printf("Neznamy prikaz.\n");
        }
    }

    return 0;
}

// END //