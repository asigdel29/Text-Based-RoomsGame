/*
Homework: 1
Author:Anubhav Sigdel
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
struct room;
struct creature;

typedef struct {
    int id;
    int type;
    struct room* location;
} creature;

typedef struct {
    int state;
    int id;
    struct room* north;
    struct room* south;
    struct room* east;
    struct room* west;
    creature* creatures[10];
} room;


void reaction(creature* c, int v);
int leave(creature* c);
void cleanDirty(creature*currentcreature, char* action);

creature* pc = NULL;
int respect = 40;

room* newRoom() {
    room* r = (room*)malloc(sizeof(room));
    r->north = NULL;
    r->south = NULL;
    r->east = NULL;
    r->west = NULL;
    for (int i = 0; i < 10; i++)
        r->creatures[i] = NULL;
    return r;
}

creature* newCreature() {
    creature* c = (creature*)malloc(sizeof(creature));
    c->location = NULL;
    return c;
}

char* getType(creature* c) {
    if (c->type == 0)
        return "PC";
    if (c->type == 1)
        return "Animal";
    if (c->type == 2)
        return "human";
}

int getInteger(char* s, int numCharacters) {
    if (numCharacters == 0) return -2;
    char num[numCharacters];
    int i = 0;
    if (s[i] == '-')
        num[i] = s[i++];
    for (; i < numCharacters; i++) {
        if (!isdigit(s[i]))
            return -2;
        else
            num[i] = s[i];
    }
    return atoi(num);
}

int ValidCreature(char* s, int numberOfRooms) {
    int length = strlen(s);
    char str[length];
    for (int i = 0; i < length; i++)
        str[i] = s[i];
    char* token = strtok(str," ");
    int x = getInteger(token, strlen(token));
    if (!(x == 0 || x == 1 || x == 2))
        return 0;
    token = strtok(NULL," \n\0\t");
    x = getInteger(token, strlen(token));
    if (x < -1 || x > numberOfRooms - 1)
        return 0;
    return 1;
}

int roomNotFull(room* r) {
    for (int i = 0; i < 10; i++) {
        if (!r->creatures[i])
            return 1;
    }
    return 0;
}

int addCreature(room* r, creature* c) {
    for (int i = 0; i < 10; i++) {
        if (!r->creatures[i]) {
            r->creatures[i] = (creature*)c;
            c->location = (struct room*)r;
            return 1;
        }
    }
    return 0;
}

int removeCreature(room* r, creature* c) {
    for (int i = 0; i < 10; i++) {
        creature* creat = (creature*)r->creatures[i];
        if (creat == c) {
            r->creatures[i] = NULL;
            return 1;
        }
    }
    return 0;
}

char* getState(room* r) {
    int state = r->state;
    switch(state) {
        case 0:
            return "clean";
        case 1:
            return "half-dirty";
        case 2:
            return "dirty";
        default:
            return NULL;
    }
}

void Contents(room* r) {
    printf("\nRoom %d,", r->id);
    printf("%s", getState(r));
    printf("\nneighbors:\n");
    if (r->north)
        printf("\n%d towards the north\n",((room*)r->north)->id);
    if (r->south)
        printf("%d towards the south\n",((room*)r->south)->id);
    if (r->east)
        printf("%d towards the east\n",((room*)r->east)->id);
    if (r->west)
        printf("%d towards the west\n",((room*)r->west)->id);
    printf("contains:");
    for (int i = 0; i < 10; i++) {
        creature* c = (creature*)r->creatures[i];
        if (c) {
            if (c == pc)
                printf("%s\n",getType(c));
            else
                printf("%s %d\n",getType(c),c->id);
        }
    }
}

void cleanDirty(creature* currentcreature, char* action) {
    room* r = (room*)currentcreature->location;
    if (strcmp(action,"dirty") == 0)
        r->state++;
    else
        r->state--;
    for (int i = 0; i < 10; i++) {
        creature* c = (creature*)r->creatures[i];
        if (c) {
            if (c != pc) {
                if (c == currentcreature) {
                    if (strcmp(getType(c),"Animal") == 0) {
                        if (strcmp(action,"dirty") == 0) {
                            respect -= 3;
                            printf("%d growls a lot,", c->id);
                            printf("Respect is now %d\n",respect);
                            if (r->state == 2)
                                leave(c);
                        } else { // must be clean
                            respect += 3;
                            printf("%d licks your face a lot,", c->id);
                            printf("Respect is now %d\n",respect);
                        }
                    } else { // must be NPC
                        if (strcmp(action,"dirty") == 0) {
                            respect += 3;
                            printf("%d smiles a lot at you,", c->id);
                            printf("Respect is now %d\n",respect);
                        } else { // must be clean
                            respect -= 3;
                            printf("%d grumbles a lot,", c->id);
                            printf("Respect is now %d\n",respect);
                            if (r->state == 0)
                                leave(c);
                        }
                    }
                } else {
                    if (strcmp(getType(c),"Animal") == 0) {
                        if (strcmp(action,"dirty") == 0) {
                            reaction(c,-1);
                            if (r->state == 2)
                                leave(c);
                        } else {
                            reaction(c,1);
                        }
                    } else {
                        if (strcmp(action,"dirty") == 0) {
                            reaction(c,1);
                        } else {
                            reaction(c,-1);
                            if (r->state == 0)
                                leave(c);
                        }
                    }
                }
            }
        }
    }
}

void reaction(creature* c, int v) {
    if (strcmp(getType(c),"Animal") == 0) {
        if (v < 0) {
            respect--;
            printf("%d growls at you,", c->id);
            printf("Respect is now %d\n",respect);
        } else {
            respect++;
            printf("%d licks your face,", c->id);
            printf("Respect is now %d\n",respect);
        }
    } else { //must be npc
        if (v < 0) {
            respect--;
            printf("%d grumbles,", c->id);
            printf("Respect is now %d\n",respect);
        } else {
            respect++;
            printf("%d smiles at you,", c->id);
            printf("Respect is now %d\n",respect);
        }
    }
}

int leave(creature* c) {
    room* current = (room*)c->location;
    removeCreature(current, c);
    room* north = (room*)current->north;
    room* east = (room*)current->east;
    room* south = (room*)current->south;
    room* west = (room*)current->west;
    int numberAvailable = 0;
    room* available[4];
    if (north && roomNotFull(north))
        available[numberAvailable++] = north;
    if (east && roomNotFull(east))
        available[numberAvailable++] = east;
    if (south && roomNotFull(south))
        available[numberAvailable++] = south;
    if (west && roomNotFull(west))
        available[numberAvailable++] = west;
    if (numberAvailable > 0) {
        int direction = rand() % numberAvailable;
        room* relocation = available[direction];
        addCreature(available[direction], c);
        if (strcmp(getType(c),"Animal") == 0) {
            if (relocation->state == 2)
                relocation->state = 1;
        } else {
            if (relocation->state == 0)
                relocation->state = 1;
        }
        if (available[direction] == north) {
            printf("%d leaves towards the north\n",c->id);
        } else if (available[direction] == east) {
            printf("%d leaves towards the east\n", c->id);
        } else if (available[direction] == south) {
            printf("%d leaves towards the south\n", c->id);
        } else if (available[direction] == west) {
            printf("%d leaves towards the west\n",c->id);
        }
    } else {
        printf("\n%d has nowhere to go, That means it's out of the game!\n",c->id);
        for (int i = 0; i < 10; i++) {
            creature* c = (creature*)current->creatures[i];
            if (c && c != pc) {
                printf("  ");
                reaction(c,-1);
            }
        }
    }
}

int stringContainsChar(char* str, char c) {
    int length = strlen(str);
    for (int i = 0; i < length; i++) {
        if (str[i] == c)
            return 1;
    }
    return 0;
}

creature* getCreature(room* r, char* strid) {
    int id = getInteger(strid, strlen(strid));
    if (id == -2)
        return NULL;
    for (int i = 0; i < 10; i++) {
        creature* c = (creature*)r->creatures[i];
        if (c && c->id == id)
            return c;
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    char input[20];
    int digits ;
    int numberOfRooms ;
    do {
        for (int i = 0; i < 20; i++) input[i] = 'a';
        printf("Enter n (number of rooms)\n");
        fgets(input, 3, stdin);
        if (isdigit(input[0]))
            if (isdigit(input[1]))
                digits = 2;
            else
                digits = 1;
        else
            digits = 0;
        numberOfRooms = getInteger(input, digits);
    } while (numberOfRooms < 1 || numberOfRooms > 99);
    room* rooms[numberOfRooms];
    int neighbors[numberOfRooms][4];
    printf("\nEnter five integers signifying the cleanliness of the room and the numbers of the respective neighbors.");
    printf("\nState can be 0 for clean, 1 for half-dirty, 2 for dirty. -1 signifies no neighbor");
    printf("\nEnter number block for all %d rooms:\n", numberOfRooms);
   for (int i = 0; i < numberOfRooms; i++) {
        rooms[i] = newRoom();
            for (int j = 0; j < 20; j++) input[j] = 'a';
            fgets(input, 20, stdin);
        char* token = strtok(input," ");
        int x = getInteger(token, strlen(token));
        rooms[i]->state = x;
        rooms[i]->id = i;
        for (int j = 0; j < 3; j++) {
            token = strtok(NULL," ");
            x = getInteger(token, strlen(token));
            neighbors[i][j] = x;
        }
        token = strtok(NULL," ");
        x = getInteger(token, strlen(token)-1);
        neighbors[i][3] = x;
    }

    for (int i = 0; i < numberOfRooms; i++) {
        if (neighbors[i][0] > -1)
            rooms[i]->north = (struct room*)rooms[neighbors[i][0]];
        if (neighbors[i][1] > -1)
            rooms[i]->south = (struct room*)rooms[neighbors[i][1]];
        if (neighbors[i][2] > -1)
            rooms[i]->east = (struct room*)rooms[neighbors[i][2]];
        if (neighbors[i][3] > -1)
            rooms[i]->west = (struct room*)rooms[neighbors[i][3]];
    }

    digits = 0;
    int maxCreatures;
    if (numberOfRooms < 10)
        maxCreatures = numberOfRooms*10;
    int numberOfCreatures;
   do {
        for (int i = 0; i < 20; i++) input[i] = 'a';
        printf("\nEnter m (number of creatures)\n");
        fgets(input, 3, stdin);
        if (isdigit(input[0]))
            if (isdigit(input[1]))
                digits = 2;
            else
                digits = 1;
        else
            digits = 0;
        numberOfCreatures = getInteger(input, digits);
    } while (numberOfCreatures < 1 || numberOfCreatures > 99);
    creature* creatures[numberOfCreatures];

    printf("\nLocation is the number of the room the creature is in. Numbering starts at 0");
    printf("\n0 for the PC, 1 for an animal, 2 for an NPC.");
    printf("\nEnter Location for all %d creatures:\n", numberOfCreatures);
    for (int i = 0; i < numberOfCreatures; i++) {
        creatures[i] = newCreature();
        int creatureInstalled = 0;
        while (!creatureInstalled) {
            do {
                for (int j = 0; j < 20; j++) input[j] = 'a';
                fgets(input, 20, stdin);
            } while (!ValidCreature(input, numberOfRooms));
            char* token = strtok(input," ");
            int typeNum = getInteger(token, strlen(token));
            token = strtok(NULL," ");
            int roomNum = getInteger(token, strlen(token)-1);
            if (typeNum != 0) {
                if (roomNotFull(rooms[roomNum])) {
                    creatures[i]->type = typeNum;
                    creatures[i]->id = i;
                    addCreature(rooms[roomNum], creatures[i]);
                    creatureInstalled = 1;
                } else {
                    printf("\n%d is full\n", roomNum);
                }
            } else if (!pc) {
                if (roomNotFull(rooms[roomNum])) {
                    creatures[i]->type = 0;
                    addCreature(rooms[roomNum], creatures[i]);
                    pc = creatures[i];
                    pc->id = i;
                    creatureInstalled = 1;
                } else {
                    printf("\n%d is full\n", roomNum);
                }
            }
        }
    }
    if (!pc) {
        printf("\nPC has not been created");
        return 0;
    }
    char* command = NULL;
    while ( command == NULL) {
        printf("\nEnter a valid command:\n");
        for (int j = 0; j < 20; j++) input[j] = 'a';
        fgets(input, 20, stdin);
        command = strtok(input," \n\0");
    }
    while (strcmp(command, "exit") != 0) {
        int validCommand = 1;
        if (strcmp(command, "look") == 0) {
            Contents((room*)pc->location);
        } else if (strcmp(command, "dirty") == 0) {
            if (((room*)pc->location)->state == 2)
                printf("\nThe room is already dirty.\n");
            else
                cleanDirty(pc,"dirty");
        } else if (strcmp(command, "clean") == 0) {
            if (((room*)pc->location)->state == 0)
                printf("\nThe room is already clean.\n");
            else
                cleanDirty(pc, "clean");
        } else if (strcmp(command, "north") == 0) {
            room* location = (room*)pc->location;
            if (location->north) {
                if (roomNotFull((room*)location->north)) {
                    removeCreature(location, pc);
                    addCreature((room*)location->north, pc);
                    printf("\nYou leave towards the north\n");
                } else {
                    printf("\nThe room is already full!\n");
                }
            } else {
                printf("\nThere exists no room.\n");
            }
        } else if (strcmp(command, "east") == 0) {
            room* location = (room*)pc->location;
            if (location->east) {
                if (roomNotFull((room*)location->east)) {
                    removeCreature(location, pc);
                    addCreature((room*)location->east, pc);
                    printf("\nYou leave towards the east\n");
                } else {
                    printf("\nThe room is already full\n");
                }
            } else {
                printf("\nThere exists no room.\n");
            }
        } else if (strcmp(command, "south") == 0) {
            room* location = (room*)pc->location;
            if (location->south) {
                if (roomNotFull((room*)location->south)) {
                    removeCreature(location, pc);
                    addCreature((room*)location->south, pc);
                    printf("\nYou leave towards the south\n");
                } else {
                    printf("\nThe room is already full\n");
                }
            } else {
                printf("\nThere exists no room.\n");
            }
        } else if (strcmp(command, "west") == 0) {
            room* location = (room*)pc->location;
            if (location->west) {
                if (roomNotFull((room*)location->west)) {
                    removeCreature(location, pc);
                    addCreature((room*)location->west, pc);
                    printf("\nYou leave towards the west");
                } else {
                    printf("\nThe room is already full\n");
                }
            } else {
                printf("\nThere exists no room.\n");
            }
        } else if (stringContainsChar(command, ':')) {
            char* leftCommand = strtok(command,": \n\0");
            char* rightCommand = strtok(NULL, " \n\0");
            creature* creatCommand = getCreature((room*)pc->location, leftCommand);
            if (creatCommand) {
                if (strcmp(rightCommand, "dirty") == 0) {
                    if (((room*)pc->location)->state == 2)
                        printf("\nThe room is already dirty.\n");
                    else
                        cleanDirty(creatCommand,"dirty");
                } else if (strcmp(rightCommand, "clean") == 0) {
                    if (((room*)pc->location)->state == 0)
                        printf("\nThe room is already clean.\n");
                    else
                        cleanDirty(creatCommand,"clean");
                } else if (strcmp(rightCommand, "north") == 0) {
                    room* location = (room*)pc->location;
                    if (location->north) {
                        if (roomNotFull((room*)location->north)) {
                            removeCreature(location, creatCommand);
                            addCreature((room*)location->north, creatCommand);
                            printf("\n%s %d move towards the north\n", getType(creatCommand), creatCommand->id);
                        } else {
                            printf("\nThe room is already full\n");
                        }
                    } else {
                        printf("\nThere exists no room.\n");
                    }
                } else if (strcmp(rightCommand, "east") == 0) {
                    room* location = (room*)pc->location;
                    if (location->east) {
                        if (roomNotFull((room*)location->east)) {
                            removeCreature(location, creatCommand);
                            addCreature((room*)location->east, creatCommand);
                            printf("\n%s %d move towards the east\n", getType(creatCommand), creatCommand->id);
                        } else {
                            printf("\nThe room is already full\n");
                        }
                    } else {
                        printf("\nThere exists no room.\n");
                    }
                } else if (strcmp(rightCommand, "south") == 0) {
                    room* location = (room*)pc->location;
                    if (location->south) {
                        if (roomNotFull((room*)location->south)) {
                            removeCreature(location, creatCommand);
                            addCreature((room*)location->south, creatCommand);
                            printf("\n%s %d move towards the south\n", getType(creatCommand), creatCommand->id);
                        } else {
                            printf("\nThe room is already full\n");
                        }
                    } else {
                        printf("\nThere exists no room.\n");
                    }
                } else if (strcmp(rightCommand, "west") == 0) {
                    room* location = (room*)pc->location;
                    if (location->west) {
                        if (roomNotFull((room*)location->west)) {
                            removeCreature(location, creatCommand);
                            addCreature((room*)location->west, creatCommand);
                            printf("\n%s %d move towards the west\n", getType(creatCommand), creatCommand->id);
                        } else {
                            printf("\nThe room is already full\n");
                        }
                    } else {
                        printf("\nThere exists no room.\n");
                    }
                }
            } else {
                printf("\nERROR 404 (Creature not found)\n");
            }
        } else {
            printf("\nERROR 404 (Command not found)\n");
            validCommand = 0;
        }
        if (respect > 79 || respect < 1)
            break;
        command = NULL;
        while (command == NULL) {
            printf("\nEnter a valid command:\n");
            for (int j = 0; j < 20; j++) input[j] = 'a';
            fgets(input, 20, stdin);
            command = strtok(input," \n\0");
        }
    }

    if (respect > 79)
        printf("\nYou won!");
    else if (respect < 1)
        printf("\nYou lost");
    else
        printf("\nGoodbye!\n");

    for (int i = 0; i < numberOfRooms; i++)
        free(rooms[i]);
    for (int i = 0; i < numberOfCreatures; i++)
        free(creatures[i]);
    return 0;
}