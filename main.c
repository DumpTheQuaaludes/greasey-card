#include <stdio.h>
#include <pthread.h>
#include <string.h>     //memcpy
#include <stdlib.h>     //rand()
#include <time.h>       //seeding for rand()


//bool macros so I dont lose my mind
#define true 1
#define false 0

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
    //int bool_is_dealer;
    int bool_is_their_turn;
};

//global declarations
struct Card greasy_card;
struct Node *stack_deck = NULL;     //much easier to deal with a deck as a linked list/stack
struct Card card_deck[52];          //much easier to shuffle while an array
int MAX_CHIPS_PER_BAG;
int INT_CHIPS_IN_BAG;
int INT_BAGS_EATEN = 0;
int END_GAME = false; 
pthread_mutex_t mutex_turn = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_turn = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_chips_bag = PTHREAD_MUTEX_INITIALIZER;
int current_turn = 0;
int NUM_PLAYERS;


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

//this one busted my balls, hard...
void push_back(struct Node **top, void *data, size_t data_size)
{
    //create a new node with memory size a Node
    struct Node *new_node = (struct Node *) malloc(sizeof(struct Node));
    
    //allocate new_nodes data's memory equal to memory size of Card
    new_node->data = malloc(data_size);

    //copy the new nodes data to the 
    memcpy(new_node->data, data, data_size);

    new_node->next = NULL;

    //if list is empty, new node becomes the top
    if (*top == NULL)
    {
        *top = new_node;
        return;
    }

    //traverse tot he end
    struct Node *current = *top;
    while(current->next != NULL)
    {
        current = current->next;
    }

    //append the new node
    current->next = new_node;
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

//generic swap function
void swap(void *initial_position, void *new_position, size_t size)
{
    char temp[size];
    memcpy(temp, initial_position, size);
    memcpy(initial_position, new_position, size);
    memcpy(new_position, temp, size);
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

void declare_greasy_card(struct Player *player)
{
    top(stack_deck, &greasy_card, sizeof(struct Card));
    pop(&stack_deck);
    printf("Player %d has drawn the Greasy card, which is %zu of %d\n", player->int_player_number, greasy_card.rank, greasy_card.suit);
}

void draw_card(struct Player *player)
{
    //choose a hand, left or right hand
    //give that hand a card
    int int_random_hand = rand() % 2; // 0 = left, 1 = right
    printf("Player %d has drawn a card... ", player->int_player_number);

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
        printf("Player %d chose their left hand and discarded the %zu of %d ", player->int_player_number, player->hand_left.card.rank, player->hand_left.card.suit);
        player->hand_left.bool_is_empty = true;
        player->hand_left.card = temp;
    }

    if(int_random_hand && !player->hand_right.bool_is_empty)
    {
        //place the card at the bottom of the deck
        //empty the player's right hand
        printf("Player %d chose their right hand and discarded the %zu of %d ", player->int_player_number, player->hand_right.card.rank, player->hand_right.card.suit);
        player->hand_right.bool_is_empty = true;
        player->hand_right.card = temp;
    }

    printf("at the bottom of the deck\n");
}

void compare_cards(struct Player *player)
{
    printf("Player %d is looking at their hand\n", player->int_player_number);
    //check left hand matches rank with greasy card
    if(player->hand_left.card.rank == greasy_card.rank)
    {
        END_GAME = true;
        printf("Player %d's card, ", player->int_player_number);
        printf("%zu of %d, matches the greasy card, ",  player->hand_left.card.rank, player->hand_left.card.suit);
        printf("%zu of %d\n", greasy_card.rank, greasy_card.suit);
        return;
    }

    //check right hand mataches rank with greasy card
    if(player->hand_right.card.rank == greasy_card.rank)
    {
        END_GAME = true;
        printf("Player %d's card, ", player->int_player_number);
        printf("%zu of %d matches the greasy card, ", player->hand_right.card.rank, player->hand_right.card.suit);
        printf("%zu of %d\n", greasy_card.rank, greasy_card.suit);
        return;
    }

    printf("Player %d has checked, there was no matches\n", player->int_player_number);
}

void eat_chips(struct Player *player)
{
    static pthread_mutex_t chip_mutex = PTHREAD_MUTEX_INITIALIZER;
    int chips_eaten = 0;
    int chips_to_eat = rand() % 5 + 1;

    pthread_mutex_lock(&chip_mutex);
    if(INT_CHIPS_IN_BAG > 0)
    {

        if(chips_to_eat >= INT_CHIPS_IN_BAG)
        {
            chips_to_eat = INT_CHIPS_IN_BAG;
            INT_BAGS_EATEN++;
            INT_CHIPS_IN_BAG = MAX_CHIPS_PER_BAG;
            printf("Player %d tried to eat more chips than what was left in the bag, so he finished the bag and opened a new bag ", player->int_player_number);
            printf("The group has so far eaten %d bags of chip(s)\n", INT_BAGS_EATEN);
        }

        INT_CHIPS_IN_BAG -= chips_to_eat;
        chips_eaten = chips_to_eat;
        player->int_chips_to_eat = chips_eaten;
        printf("Player %d eats %d chip(s). Chips left: %d\n", player->int_player_number, chips_to_eat, INT_CHIPS_IN_BAG);
    }

    pthread_mutex_unlock(&chip_mutex);
}

void *player_turn(void *arg)
{
    struct Player *player = (struct Player *)arg;

    while(!END_GAME)
    {
        //lock the turn mutex
        pthread_mutex_lock(&mutex_turn);


        //wait until it's the player's turn
        while(current_turn != player->int_player_number && !END_GAME)
        {
            pthread_cond_wait(&cond_turn, &mutex_turn);
            eat_chips(player);
            //usleep(500000);
            //pthread_mutex_lock(&mutex_turn);
        }
        
        if(END_GAME)
        {
            pthread_mutex_unlock(&mutex_turn);
            break;
        }

        //its a player's turn!
        draw_card(player);
        compare_cards(player);
        discard_card(player);

        //move to the next player's turn
        current_turn = (current_turn + 1) % NUM_PLAYERS;

        pthread_cond_broadcast(&cond_turn);
        pthread_mutex_unlock(&mutex_turn);
    }

    return NULL;
}



//n players, m chips per bag, o seed for randomizer
int main()
{
    NUM_PLAYERS = 4;
    MAX_CHIPS_PER_BAG = 20;
    pthread_t threads[NUM_PLAYERS];
    struct Player players[NUM_PLAYERS];

    int int_max_deck_size = 4 * 13;

    //seed's randomizer, NULL for the mean time
    srand(time(NULL));


    for(int i = 0; i < NUM_PLAYERS; i++)
    {
        players[i].hand_left.bool_is_empty = true;
        players[i].hand_right.bool_is_empty = true;
        players[i].int_player_number = i;
    }

    //pick a random player to be the dealer
    int dealer_index = rand() % NUM_PLAYERS;
    intialize_deck(card_deck, 4, 13);
    printf("Player %d is the dealer!\n", dealer_index);
    printf("Player %d is opening a bag of chips\n", dealer_index);
    INT_CHIPS_IN_BAG = MAX_CHIPS_PER_BAG;
    printf("Player %d is now shuffling the deck...\n", dealer_index);
    shuffle_deck(card_deck, 52);
    stack_card_deck(card_deck, 52);
    declare_greasy_card(&players[dealer_index]);
    printf("Player %d is now dealing the cards...\n", dealer_index);

    for(int i = 0; i < NUM_PLAYERS; i++)
    {
        draw_card(&players[i]);
        pthread_create(&threads[i], NULL, player_turn, &players[i]);
    }
    
    
    for(int i = 0; i < NUM_PLAYERS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("Game Over!\n");
    
    return 0;
}