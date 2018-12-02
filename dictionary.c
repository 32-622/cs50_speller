#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"

#define TABLE_SIZE 150000 // size of hash table

// linked list node structure
typedef struct sll_list
{
    char word[LENGTH + 1];
    struct sll_list *next;
}
sll_node;

// hash table item structure
typedef struct hash_item
{
    sll_node *next;
}
item;

item table[TABLE_SIZE]; // data structure - hash table declaration
int track_size; // tracking words added to dictionary

// hash function / I used Bob Jenkins one_at_a_time hash, found here: http://www.burtleburtle.net/bob/hash/doobs.html
unsigned oaat_hash(char *x) // input: word_cpy
{
    unsigned hash = 0;

    for (int i = 0; i < strlen(x); i++)
    {
        hash += x[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    hash = hash % TABLE_SIZE;
    return hash;
}

// add node to linked list in hash table /function
item sll_add(char *y, item z) // input: word from dictionary, hash table number
{
    sll_node *sll_new = malloc(sizeof(sll_node)); // allocate memory for a new node
    if (sll_new == NULL)
    {
        fprintf(stderr, "Memory allocation error");
        // what to return?
    }

    strcpy(sll_new->word, y); // copy word to new string
    sll_new->next = z.next;
    z.next = sll_new;
    return z;
};

// destroy list /function
void sll_destroy(sll_node *x)
{
    sll_node *current = x;
    if (current->next != NULL)
    {
        sll_destroy(current->next);
    }
    free(current);
};

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    // Const char can't be 'tolower'ed, so create a copy of word, than change its case and hash it.
    char word_cpy[strlen(word) + 1]; // strlen does not include \0, so +1 is needed
    strcpy(word_cpy, word);

    for (int i = 0; i < strlen(word_cpy); i++)
    {
        word_cpy[i] = tolower(word_cpy[i]);
    }

    // hash a word_cpy
    int hashed = oaat_hash(word_cpy);

    if (table[hashed].next == NULL) // if there are no nodes (words) in table
    {
        return false;
    }

    sll_node *current = table[hashed].next;

    while (strcmp(word_cpy, current->word) != 0) // compare words
    {
        if (current->next == NULL)
        {
            return false;
        }
        current = current->next;
    }
    return true;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // open dictionary file
    FILE *file = fopen(dictionary, "r");
    if (file == NULL)
    {
        printf("Could not open %s.\n", dictionary);
        return false;
    }

    char temp_word[LENGTH + 1];
    int index = 0;

    // read dictionary file character by character
    for (int c = fgetc(file); c != EOF; c = fgetc(file))
    {
        if (c != '\n')
        {
            temp_word[index] = c;
            index++;
        }

        // end of line
        if (c == '\n')
        {
            temp_word[index] = '\0'; // we have a word
            index = 0; // prepare for a new word

            // hash a word
            int hashed = oaat_hash(temp_word);

            // add word to table
            table[hashed] = sll_add(temp_word, table[hashed]);
            track_size++;
        }
    }
    fclose(file);
    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return track_size;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    // for number of buckets
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (table[i].next != NULL)
        {
            sll_destroy(table[i].next);
        }
    }
    return true;
}
