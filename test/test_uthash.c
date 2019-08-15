#include "include/uthash.h"
#include <stdlib.h> /* malloc */
#include <stdio.h>  /* printf */
#include <time.h>

//gcc test_uthash.c -o uthash -I ./include 
typedef struct example_user_t
{
    int id;
    int cookie;
    UT_hash_handle hh;

} example_user_t;

int main(int argc, char const *argv[])
{

    int i = 0;
    example_user_t *user, *users = NULL;
    srand((unsigned int)time(NULL));
    for (i = 0; i < 10; i++)
    {
        if ((user = (example_user_t *)malloc(sizeof(example_user_t)) == NULL))
        {
            exit(-1);
        }
        user->id = rand() % 100;
        user->cookie = i * i;
        HASH_ADD_INT(users, id, user);
    }

    for (user = users; user != NULL; user = (example_user_t *)users->hh.next)
    {
        printf("user %d, cookie %d\n", user->id, user->cookie);
    }

    return 0;
}
