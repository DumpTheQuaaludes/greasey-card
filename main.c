#include <stdio.h>
#include <pthread.h>
#include <string.h>     //memcpy
#include <stdlib.h>     //rand()
#include <time.h>       //seeding for rand()

int INT_CHIPS_IN_BAG = 50;
int INT_BAGS_EATEN = 0;

//checking if I pushed to working-branch

//suits = 4
//0=spades, 1=heart, 2=clubs, 3=diamonds
//ranks = 13
// a=1,2,3,4,5,6,7,8,9,10, j=11, q=12,k=13
struct card
{
    unsigned int suit;
    unsigned int rank;
};

void intialize_deck(struct card deck[], int int_max_suit, int int_max_rank)
{
    int int_card_count = 0;
    for(int i = 0; i < int_max_suit; i++)
    {
        for(int j = 1; j < int_max_rank+1; j++)
        {
            deck[int_card_count].suit = i;
            deck[int_card_count].rank = j;
            int_card_count++;
        }
    }
}

void print_deck(struct card deck[])
{
    for(int i = 0; i < 52; i++)
    {
        printf("%u of %u\n", deck[i].suit, deck[i].rank);
    }
}

//generic swap function
void swap(void *initial_position, void *new_position, size_t size)
{
    char temp[size];
    memcpy(temp, initial_position, size);
    memcpy(initial_position, new_position, size);
    memcpy(new_position, temp, size);
}

void shuffle_deck(struct card deck[], int int_deck_size)
{
    for(int i=0; i < int_deck_size; i++)
    {
        int int_new_position = i + (rand() % (int_deck_size - i));
        
        swap(&deck[i], &deck[int_new_position], sizeof(struct card));
    }
}

//n players, m chips per bag, o seed randomizer
int main()
{
    int int_max_deck_size = 4 * 13;
    struct card card_deck[int_max_deck_size];

    //seed's randomizer
    srand(time(NULL));

    printf("Before Shuffle\n");
    intialize_deck(card_deck, 4,13);
    print_deck(card_deck);

    printf("\n");
    printf("\n");
    printf("After Shuffle\n");
    shuffle_deck(card_deck, int_max_deck_size);
    print_deck(card_deck);

    return 0;
}