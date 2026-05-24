#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

// thread functions for each game type

void *football_player(void *arg){
    int id = *((int*)arg);
    free(arg);

    printf("Football player %d is waiting to play\n", id);
    int t = (rand() % 3) + 1;
    sleep(t);
    printf("Football player %d is done playing\n", id);

    return NULL;
}

void *supermario_player(void *arg){
    int id = *((int*)arg);
    free(arg);

    printf("SuperMario player %d is waiting to play\n", id);
    sleep((rand()%3)+1);
    printf("SuperMario player %d is done playing\n", id);

    return NULL;
}

void *nfs_player(void *arg){
    int id = *((int*)arg);
    free(arg);

    printf("NFS player %d is waiting to play\n", id);
    sleep((rand()%3)+1);
    printf("NFS player %d is done playing\n", id);

    return NULL;
}


int main(){

    srand(time(NULL));

    // total 16 threads
    pthread_t tids[16];
    int i, idx = 0;
    int *idp;

    // create 8 football threads
    for(i = 0; i < 8; i++){
        idp = malloc(sizeof(int));
        *idp = i;
        pthread_create(&tids[idx], NULL, football_player, idp);
        idx++;
    }

    // 4 super mario
    for(i = 0; i < 4; i++){
        idp = malloc(sizeof(int));
        *idp = i;
        pthread_create(&tids[idx], NULL, supermario_player, idp);
        idx++;
    }

    // 4 nfs
    for(i = 0; i < 4; i++){
        idp = malloc(sizeof(int));
        *idp = i;
        pthread_create(&tids[idx], NULL, nfs_player, idp);
        idx++;
    }

    // join all of them
    for(i = 0; i < 16; i++){
        pthread_join(tids[i], NULL);
    }

    printf("done\n");
    return 0;
}
