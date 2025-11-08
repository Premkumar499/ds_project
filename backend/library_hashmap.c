#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 10

typedef struct Book {
    int id;
    char title[50];
    char author[50];
    char genre[30];
    struct Book* next;
} Book;

Book* hashTable[TABLE_SIZE];

// Hash function (simple modulo)
int hashFunction(int id) {
    return id % TABLE_SIZE;
}

// Insert a book into hash table
void insertBook(int id, char* title, char* author, char* genre) {
    int index = hashFunction(id);
    Book* newBook = (Book*)malloc(sizeof(Book));
    newBook->id = id;
    strcpy(newBook->title, title);
    strcpy(newBook->author, author);
    strcpy(newBook->genre, genre);
    newBook->next = hashTable[index];
    hashTable[index] = newBook;
}

// Search book by ID
Book* searchBook(int id) {
    int index = hashFunction(id);
    Book* temp = hashTable[index];
    while (temp != NULL) {
        if (temp->id == id)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

// Export data to JSON file
void exportToJSON() {
    FILE* fp = fopen("books.json", "w");
    if (!fp) return;

    fprintf(fp, "[\n");
    int first = 1;
    for (int i = 0; i < TABLE_SIZE; i++) {
        Book* temp = hashTable[i];
        while (temp != NULL) {
            if (!first) fprintf(fp, ",\n");
            fprintf(fp, "  {\"id\": %d, \"title\": \"%s\", \"author\": \"%s\", \"genre\": \"%s\"}",
                    temp->id, temp->title, temp->author, temp->genre);
            first = 0;
            temp = temp->next;
        }
    }
    fprintf(fp, "\n]");
    fclose(fp);
}

int main() {
    insertBook(101, "C Programming", "Dennis Ritchie", "Programming");
    insertBook(202, "Data Structures", "Mark Allen Weiss", "Computer Science");
    insertBook(303, "Artificial Intelligence", "Stuart Russell", "AI");
    insertBook(404, "Python Basics", "Guido van Rossum", "Programming");

    printf("Enter Book ID to search: ");
    int id;
    scanf("%d", &id);

    Book* found = searchBook(id);
    if (found)
        printf("Book Found: %s by %s (%s)\n", found->title, found->author, found->genre);
    else
        printf("Book Not Found.\n");

    exportToJSON();
    printf("Book data exported to books.json\n");
    return 0;
}
