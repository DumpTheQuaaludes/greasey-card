#include <stdio.h>
#include <pthread.h>
#include <string.h>     //memcpy
#include <stdlib.h>     //rand()
#include <time.h>       //seeding for rand()


//bool macros so I dont lose my mind
#define true 1
#define false 0

int INT_CHIPS_IN_BAG = 50;
int INT_BAGS_EATEN = 0;
int END_GAME = false; 

//checking if I pushed to working-branch

enum Suit
{
    //0   1       2       3      4
    NONE, SPADES, HEARTS, CLUBS, DIAMONDS
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
    struct Hand hand_left;
    struct Hand hand_right;
    int int_chips_to_eat;
    int int_player_number;
    int bool_is_dealer;
};

//global declarations
struct Card greasy_card;
struct Node *stack_deck = NULL;     //much easier to deal with a deck as a linked list/stack
struct Card card_deck[52];            //much easier to shuffle while an array

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
    for(int i = 1; i < int_max_suit+1; i++)
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

void declare_greasy_card()
{
    top(stack_deck, &greasy_card, sizeof(struct Card));
    pop(&stack_deck);
    printf("The Greasy card is %zu of %d\n", greasy_card.rank, greasy_card.suit);
}

void draw_card(struct Player *player)
{
    //choose a hand, left or right hand
    //give that hand a card
    int int_random_hand = rand() % 2; // 0 = left, 1 = right
    printf("The player has drawn a card... ");

    //random declares left
    if(int_random_hand == 0)
    {
        //if left is empty, give card
        if(player->hand_left.bool_is_empty)
        {
            top(stack_deck, &player->hand_left.card, sizeof(struct Card));
            pop(&stack_deck);
            player->hand_left.bool_is_empty = false;
            printf("and they chose to place their card in their left hand, they placed the %zu of %d...\n", player->hand_left.card.rank, player->hand_left.card.suit);
        }

        //otherwise, that means right is empty, so we give them the card
        else
        {
            top(stack_deck, &player->hand_right.card, sizeof(struct Card));
            pop(&stack_deck);
            player->hand_right.bool_is_empty = false;
            printf("and wanted to place their card in their left hand, but it was full, so they placed that card in their right hand, they placed the %zu of %d\n", player->hand_right.card.rank, player->hand_right.card.suit);
        }
    }

    //random declares right
    else if(int_random_hand == 1)
    {
        //if right is empty, give card
        if(player->hand_right.bool_is_empty)
        {
            top(stack_deck, &player->hand_right.card, sizeof(struct Card));
            pop(&stack_deck);
            player->hand_right.bool_is_empty = false;
            printf("and they chose to place their card in their right hand, they placed the %zu of %d...\n", player->hand_right.card.rank, player->hand_right.card.suit);
        }

        //otherwise, that means right is empty, so we give them the card
        else
        {
            top(stack_deck, &player->hand_left.card, sizeof(struct Card));
            pop(&stack_deck);
            player->hand_left.bool_is_empty = false;
            printf("and wanted to place their card in their right hand, but it was full, so they placed that card in their left hand, they placed the %zu of %d\n", player->hand_left.card.rank, player->hand_left.card.suit);
        }
    }
}

void discard_card(struct Player *player)
{
    //choose a hand, left or right hand
    //discard that card, from hand
    int int_random_hand = rand() % 2; // 0 = left, 1 = right

    struct Card temp = {SPADES, 0};

    //discard the left hand
    if(int_random_hand == 0 && !player->hand_left.bool_is_empty)
    {
        //place the card at the bottom of the deck
        //empty the player's left hand
        printf("The Player chose their left hand and discarded the %zu of %d\n", player->hand_left.card.rank, player->hand_left.card.suit);
        player->hand_left.bool_is_empty = true;
        player->hand_left.card = temp;
    }

    if(int_random_hand && !player->hand_right.bool_is_empty)
    {
        //place the card at the bottom of the deck
        //empty the player's right hand
        printf("The Player chose their right hand and discarded the %zu of %d\n", player->hand_right.card.rank, player->hand_right.card.suit);
        player->hand_right.bool_is_empty = true;
        player->hand_right.card = temp;
    }

    
}

void compare_cards(struct Player *player)
{
    printf("The Player is looking at their hand\n");
    //check left hand matches rank with greasy card
    if(player->hand_left.card.rank == greasy_card.rank)
    {
        END_GAME = true;
        printf("The match card was the player's %zu of %d and the greasy card was %zu of %d\n", player->hand_left.card.rank, player->hand_left.card.suit, greasy_card.rank, greasy_card.suit);
        return;
    }

    //check right hand mataches rank with greasy card
    if(player->hand_right.card.rank == greasy_card.rank)
    {
        END_GAME = true;
        printf("The match card was the player's %zu of %d and the greasy card was %zu of %d\n", player->hand_right.card.rank, player->hand_right.card.suit, greasy_card.rank, greasy_card.suit);
        return;
    }

    printf("The player has checked, there was no matches\n");
}

//n players, m chips per bag, o seed for randomizer
int main()
{
    struct Player players[1];

    //player intialization
    players[0].hand_left.bool_is_empty = true;
    players[0].hand_right.bool_is_empty = true;
    players[0].bool_is_dealer = 1;

    int int_max_deck_size = 4 * 13;

    //seed's randomizer, NULL for the mean time
    srand(time(NULL));

    intialize_deck(card_deck, 4, 13);
    shuffle_deck(card_deck, int_max_deck_size);
    stack_card_deck(card_deck, int_max_deck_size);
    declare_greasy_card();
    
    //dealt a card
    draw_card(&players[0]);
    

    while(!END_GAME)
    {
        draw_card(&players[0]);
        compare_cards(&players[0]);
        discard_card(&players[0]);
    }
    
    return 0;
}