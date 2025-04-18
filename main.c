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
struct Card
{
    unsigned int suit;
    unsigned int rank;
};

//generic linked list
struct Node
{
    void *data;
    struct Node *next;
};

//insert element into list
void push(struct Node **top, void *data, size_t data_size)
{
    //create a new node with memory size a Node
    struct Node *new_node = (struct Node *) malloc(sizeof(struct Node));
    
    //allocate new_nodes data's memory equal to memory size of Card
    new_node->data = malloc(data_size);

    //copy the new nodes data to the 
    memcpy(new_node->data, data, data_size);

    //updates stack by moving the value down
    new_node->next = *top;
    *top = new_node;
}

//remove top element
void pop(struct Node **top, void *data_out, size_t data_size)
{
    //check if the top is empty, exit if empty
    if(*top == NULL)
    {
        return;
    }

    memcpy(data_out, (*top)->data, data_size);
    struct Node * temp = *top;
    *top = (*top)->next;
    
    //frees up memory
    free(temp->data);
    free(temp);
}

//accesses top element
void top(struct Node **top, void *data, size_t data_size)
{

}

void intialize_deck(struct Card deck[], int int_max_suit, int int_max_rank)
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

void print_deck(struct Card deck[])
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

void shuffle_deck(struct Card deck[], int int_deck_size)
{
    for(int i=0; i < int_deck_size; i++)
    {
        int int_new_position = i + (rand() % (int_deck_size - i));
        
        swap(&deck[i], &deck[int_new_position], sizeof(struct Card));
    }
}

//uses stack functions to 
void stack_card_deck(struct card deck[])
{

}

//n players, m chips per bag, o seed randomizer
int main()
{
    int int_max_deck_size = 4 * 13;
    struct Card card_deck[int_max_deck_size];

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