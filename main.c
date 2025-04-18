#include <stdio.h>
#include <pthread.h>
#include <string.h>     //memcpy
#include <stdlib.h>     //rand()
#include <time.h>       //seeding for rand()

int INT_CHIPS_IN_BAG = 50;
int INT_BAGS_EATEN = 0;
int END_GAME = 0; //0 = false, 1 = true (end game)

//checking if I pushed to working-branch

enum Suit
{
    SPADES, HEARTS, CLUBS, DIAMONDS
};

struct Card
{
    enum Suit suit;
    size_t rank;
};

//generic linked list
struct Node
{
    void *data;
    struct Node *next;
};

struct Hand
{
    struct Card card;
    int bool_is_empty;
};

struct Player
{
    struct Hand left_hand;
    struct Hand right_hand;
    int int_chips_to_eat;
    int int_player_number;
    int bool_is_dealer;
};

//global declarations
struct Card greasy_card;
struct Node *stack_deck = NULL;                 //much easier to deal with a deck as a linked list/stack

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
void pop(struct Node **top)
{
    //exit if top is empty
    if(*top == NULL)
    {
        return;
    }

    struct Node * temp = *top;
    *top = (*top)->next;
    
    //frees up memory
    free(temp->data);
    free(temp);
}

//accesses top element
void top(struct Node *top, void *data_out, size_t data_size)
{
    //exit if top is empty
    if(top == NULL)
    {
        return;
    }

    memcpy(data_out , top->data, data_size);
}

void intialize_deck(struct Card deck[], int int_max_suit, int int_max_rank)
{
    int int_card_count = 0;
    for(int i = 0; i < int_max_suit; i++)
    {
        for(int j = 1; j < int_max_rank+1; j++)
        {
            deck[int_card_count].suit = (enum Suit) i;
            deck[int_card_count].rank = j;
            int_card_count++;
        }
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
void stack_card_deck(struct Card deck[], int int_max_deck_size)
{
    for(int i = 0; i < int_max_deck_size; i++)
    {
        push(&stack_deck, &deck[i], sizeof(struct Card));
    }
}

void print_stack_card_deck()
{
    struct Card card_current;
       
    while(&stack_deck != NULL)
    {
        top(stack_deck, &card_current, sizeof(struct Card));
        printf("%d of %zu\n", card_current.suit, card_current.rank);
        pop(&stack_deck);
    }
}

void declare_greasy_card()
{
    top(stack_deck, &greasy_card, sizeof(struct Card));
    pop(&stack_deck);
    printf("The Greasy card is %d of %zu", greasy_card.suit, greasy_card.rank);
}

void draw_card(struct Player player)
{
    if(player.left_hand.bool_is_empty == 0)
    {
        //top
    }
}

//n players, m chips per bag, o seed for randomizer
int main()
{
    int players = 1;
    int int_max_deck_size = 4 * 13;
    struct Card card_deck[int_max_deck_size];       //much easier to shuffle while an array
    

    //seed's randomizer, NULL for the mean time
    srand(time(NULL));

    intialize_deck(card_deck, 4, 13);
    shuffle_deck(card_deck, int_max_deck_size);
    stack_card_deck(card_deck, int_max_deck_size);
    declare_greasy_card();
    
    while(END_GAME)
    {

    }
    
    
    
    //print_stack_card_deck(&stack_deck);

    return 0;
}