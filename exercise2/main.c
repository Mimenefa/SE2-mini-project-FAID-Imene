#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

// ---- shared variables ----

// counts how many players are currently playing each game
int nb_football = 0;
int nb_mario = 0;
int nb_nfs = 0;

// mutex to protect the counters and game switching logic
sem_t mutex;

// semaphores to block players when they cant enter
sem_t football_sem;
sem_t mario_sem;
sem_t nfs_sem;

// waiting queues count
int wait_football = 0;
int wait_mario = 0;
int wait_nfs = 0;

// max players per game
#define MAX_F  4
#define MAX_SM 2
#define MAX_NFS 1

// which game is currently on the playstation (0 = none, 1=football, 2=mario, 3=nfs)
int current_game = 0;


// ---- enter/quit functions ----

void enter_F_game(int id){
    sem_wait(&mutex);

    // can enter if no one is playing OR football is already being played and slots left
    if((current_game == 0 || current_game == 1) && nb_football < MAX_F){
        current_game = 1;
        nb_football++;
        printf("[Football #%d] entering - players now: %d\n", id, nb_football);
        sem_post(&mutex);
    } else {
        wait_football++;
        sem_post(&mutex);
        sem_wait(&football_sem); // block until allowed
        sem_wait(&mutex);
        current_game = 1;
        nb_football++;
        printf("[Football #%d] entering - players now: %d\n", id, nb_football);
        sem_post(&mutex);
    }
}

void quit_F_game(int id){
    sem_wait(&mutex);
    nb_football--;
    printf("[Football #%d] leaving - players now: %d\n", id, nb_football);

    if(nb_football == 0){
        current_game = 0;
        // wake up waiting players, priority equal so check all
        if(wait_football > 0){
            wait_football--;
            current_game = 1;
            sem_post(&football_sem);
        } else if(wait_mario > 0){
            wait_mario--;
            current_game = 2;
            sem_post(&mario_sem);
        } else if(wait_nfs > 0){
            wait_nfs--;
            current_game = 3;
            sem_post(&nfs_sem);
        }
    } else if(wait_football > 0 && nb_football < MAX_F){
        // still football playing, let another football player in
        wait_football--;
        sem_post(&football_sem);
    }

    sem_post(&mutex);
}


void enter_SM_game(int id){
    sem_wait(&mutex);

    if((current_game == 0 || current_game == 2) && nb_mario < MAX_SM){
        current_game = 2;
        nb_mario++;
        printf("[SuperMario #%d] entering - players now: %d\n", id, nb_mario);
        sem_post(&mutex);
    } else {
        wait_mario++;
        sem_post(&mutex);
        sem_wait(&mario_sem);
        sem_wait(&mutex);
        current_game = 2;
        nb_mario++;
        printf("[SuperMario #%d] entering - players now: %d\n", id, nb_mario);
        sem_post(&mutex);
    }
}

void quit_SM_game(int id){
    sem_wait(&mutex);
    nb_mario--;
    printf("[SuperMario #%d] leaving - players now: %d\n", id, nb_mario);

    if(nb_mario == 0){
        current_game = 0;
        if(wait_mario > 0){
            wait_mario--;
            current_game = 2;
            sem_post(&mario_sem);
        } else if(wait_football > 0){
            wait_football--;
            current_game = 1;
            sem_post(&football_sem);
        } else if(wait_nfs > 0){
            wait_nfs--;
            current_game = 3;
            sem_post(&nfs_sem);
        }
    } else if(wait_mario > 0 && nb_mario < MAX_SM){
        wait_mario--;
        sem_post(&mario_sem);
    }

    sem_post(&mutex);
}


void enter_NFS_game(int id){
    sem_wait(&mutex);

    if((current_game == 0 || current_game == 3) && nb_nfs < MAX_NFS){
        current_game = 3;
        nb_nfs++;
        printf("[NFS #%d] entering - players now: %d\n", id, nb_nfs);
        sem_post(&mutex);
    } else {
        wait_nfs++;
        sem_post(&mutex);
        sem_wait(&nfs_sem);
        sem_wait(&mutex);
        current_game = 3;
        nb_nfs++;
        printf("[NFS #%d] entering - players now: %d\n", id, nb_nfs);
        sem_post(&mutex);
    }
}

void quit_NFS_game(int id){
    sem_wait(&mutex);
    nb_nfs--;
    printf("[NFS #%d] leaving - players now: %d\n", id, nb_nfs);

    if(nb_nfs == 0){
        current_game = 0;
        if(wait_nfs > 0){
            wait_nfs--;
            current_game = 3;
            sem_post(&nfs_sem);
        } else if(wait_football > 0){
            wait_football--;
            current_game = 1;
            sem_post(&football_sem);
        } else if(wait_mario > 0){
            wait_mario--;
            current_game = 2;
            sem_post(&mario_sem);
        }
    }

    sem_post(&mutex);
}


// ---- thread functions ----

void *football_player(void *arg){
    int id = *((int*)arg);
    free(arg);

    printf("[Football #%d] is waiting to play\n", id);
    enter_F_game(id);

    // simulate playing
    sleep((rand()%3)+1);

    quit_F_game(id);
    printf("[Football #%d] is done playing\n", id);

    return NULL;
}

void *supermario_player(void *arg){
    int id = *((int*)arg);
    free(arg);

    printf("[SuperMario #%d] is waiting to play\n", id);
    enter_SM_game(id);

    sleep((rand()%3)+1);

    quit_SM_game(id);
    printf("[SuperMario #%d] is done playing\n", id);

    return NULL;
}

void *nfs_player(void *arg){
    int id = *((int*)arg);
    free(arg);

    printf("[NFS #%d] is waiting to play\n", id);
    enter_NFS_game(id);

    sleep((rand()%3)+1);

    quit_NFS_game(id);
    printf("[NFS #%d] is done playing\n", id);

    return NULL;
}


// ---- main ----

int main(){
    srand(time(NULL));

    // init semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&football_sem, 0, 0);
    sem_init(&mario_sem, 0, 0);
    sem_init(&nfs_sem, 0, 0);

    pthread_t tids[16];
    int i, idx = 0;
    int *idp;

    for(i = 0; i < 8; i++){
        idp = malloc(sizeof(int));
        *idp = i;
        pthread_create(&tids[idx++], NULL, football_player, idp);
    }

    for(i = 0; i < 4; i++){
        idp = malloc(sizeof(int));
        *idp = i;
        pthread_create(&tids[idx++], NULL, supermario_player, idp);
    }

    for(i = 0; i < 4; i++){
        idp = malloc(sizeof(int));
        *idp = i;
        pthread_create(&tids[idx++], NULL, nfs_player, idp);
    }

    for(i = 0; i < 16; i++)
        pthread_join(tids[i], NULL);

    // cleanup
    sem_destroy(&mutex);
    sem_destroy(&football_sem);
    sem_destroy(&mario_sem);
    sem_destroy(&nfs_sem);

    printf("all players done\n");
    return 0;
}
