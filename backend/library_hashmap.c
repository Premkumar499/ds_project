#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 10

typedef struct Book {
    int id;
    char title[100];
    char author[100];
    char genre[50];
    int year;
    char image[200];
    char description[500];
    struct Book* next;
} Book;

Book* hashTable[TABLE_SIZE];

// Hash function (simple modulo)
int hashFunction(int id) {
    return id % TABLE_SIZE;
}

// Insert a book into hash table
void insertBook(int id, char* title, char* author, char* genre, int year, char* image, char* description) {
    int index = hashFunction(id);
    Book* newBook = (Book*)malloc(sizeof(Book));
    newBook->id = id;
    strcpy(newBook->title, title);
    strcpy(newBook->author, author);
    strcpy(newBook->genre, genre);
    newBook->year = year;
    strcpy(newBook->image, image);
    strcpy(newBook->description, description);
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

// Export data to JSON file - Export to frontend directory
void exportToJSON() {
    FILE* fp = fopen("../frontend/books.json", "w");
    if (!fp) return;

    fprintf(fp, "[\n");
    int first = 1;
    for (int i = 0; i < TABLE_SIZE; i++) {
        Book* temp = hashTable[i];
        while (temp != NULL) {
            if (!first) fprintf(fp, ",\n");
            fprintf(fp, "  {\n");
            fprintf(fp, "    \"id\": %d,\n", temp->id);
            fprintf(fp, "    \"title\": \"%s\",\n", temp->title);
            fprintf(fp, "    \"author\": \"%s\",\n", temp->author);
            fprintf(fp, "    \"genre\": \"%s\",\n", temp->genre);
            fprintf(fp, "    \"year\": %d,\n", temp->year);
            fprintf(fp, "    \"image\": \"%s\",\n", temp->image);
            fprintf(fp, "    \"description\": \"%s\"\n", temp->description);
            fprintf(fp, "  }");
            first = 0;
            temp = temp->next;
        }
    }
    fprintf(fp, "\n]");
    fclose(fp);
}

int main() {
    // Load data from the shared frontend JSON file
    insertBook(101, "The C Programming Language", "Brian Kernighan & Dennis Ritchie", "Programming", 
               1988, "https://covers.openlibrary.org/b/isbn/0131103628-L.jpg", 
               "The definitive guide to C programming by the creators of the language. Known as the 'K&R' book, it's the classic reference for learning C.");
    
    insertBook(202, "Data Structures and Algorithm Analysis in C++", "Mark Allen Weiss", "Computer Science", 
               2014, "https://covers.openlibrary.org/b/isbn/9780132847377-L.jpg", 
               "A comprehensive textbook covering data structures and algorithms with C++ implementations. Widely used in computer science courses.");
    
    insertBook(303, "Artificial Intelligence: A Modern Approach", "Stuart Russell & Peter Norvig", "Artificial Intelligence", 
               2020, "https://covers.openlibrary.org/b/isbn/9780134610993-L.jpg", 
               "The leading textbook in artificial intelligence. Comprehensive coverage of AI concepts, algorithms, and applications used worldwide.");
    
    insertBook(404, "Python Crash Course", "Eric Matthes", "Programming", 
               2019, "https://covers.openlibrary.org/b/isbn/9781593279288-L.jpg", 
               "A hands-on introduction to programming with Python. Learn programming fundamentals and build practical projects.");

    printf("Enter Book ID to search: ");
    int id;
    scanf("%d", &id);

    Book* found = searchBook(id);
    if (found)
        printf("Book Found: %s by %s (%s, %d)\n", found->title, found->author, found->genre, found->year);
    else
        printf("Book Not Found.\n");

    exportToJSON();
    printf("Book data exported to shared books.json\n");
    return 0;
}
