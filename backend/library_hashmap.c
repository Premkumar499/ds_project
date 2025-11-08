#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cjson/cJSON.h>

#define TABLE_SIZE 101  // Prime number for better distribution
#define PORT 8080

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

// Hash function (better distribution using prime)
int hashFunction(int id) {
    return id % TABLE_SIZE;
}

// Initialize hash table
void initHashTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = NULL;
    }
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
    printf("📚 Loaded: %s (ID: %d)\n", title, id);
}

// Search book by ID using hashmap
Book* searchBook(int id) {
    int index = hashFunction(id);
    Book* temp = hashTable[index];
    printf("🔍 Searching in hash bucket %d for ID %d\n", index, id);
    
    while (temp != NULL) {
        if (temp->id == id) {
            printf("✅ Found book in hashmap!\n");
            return temp;
        }
        temp = temp->next;
    }
    printf("❌ Book not found in hashmap\n");
    return NULL;
}

// Load books from JSON file
int loadBooksFromJSON() {
    FILE* file = fopen("../frontend/books.json", "r");
    if (!file) {
        printf("❌ Error: Cannot open books.json\n");
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* data = malloc(length + 1);
    fread(data, 1, length, file);
    data[length] = '\0';
    fclose(file);

    cJSON* json = cJSON_Parse(data);
    if (!json) {
        printf("❌ Error parsing JSON\n");
        free(data);
        return 0;
    }

    int bookCount = 0;
    cJSON* book = NULL;
    cJSON_ArrayForEach(book, json) {
        cJSON* id = cJSON_GetObjectItem(book, "id");
        cJSON* title = cJSON_GetObjectItem(book, "title");
        cJSON* author = cJSON_GetObjectItem(book, "author");
        cJSON* genre = cJSON_GetObjectItem(book, "genre");
        cJSON* year = cJSON_GetObjectItem(book, "year");
        cJSON* image = cJSON_GetObjectItem(book, "image");
        cJSON* description = cJSON_GetObjectItem(book, "description");

        if (id && title && author && genre && year && image && description) {
            insertBook(
                id->valueint,
                title->valuestring,
                author->valuestring,
                genre->valuestring,
                year->valueint,
                image->valuestring,
                description->valuestring
            );
            bookCount++;
        }
    }

    cJSON_Delete(json);
    free(data);
    printf("✅ Loaded %d books into hashmap\n", bookCount);
    return bookCount;
}

// Send HTTP response
void sendResponse(int client_socket, char* response) {
    char header[] = "HTTP/1.1 200 OK\r\n"
                   "Content-Type: application/json\r\n"
                   "Access-Control-Allow-Origin: *\r\n"
                   "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                   "Access-Control-Allow-Headers: Content-Type\r\n\r\n";
    
    send(client_socket, header, strlen(header), 0);
    send(client_socket, response, strlen(response), 0);
}

// Handle search request
void handleSearchRequest(int client_socket, int book_id) {
    Book* book = searchBook(book_id);
    
    if (book) {
        char response[2048];
        snprintf(response, sizeof(response),
            "{"
            "\"success\": true,"
            "\"book\": {"
            "\"id\": %d,"
            "\"title\": \"%s\","
            "\"author\": \"%s\","
            "\"genre\": \"%s\","
            "\"year\": %d,"
            "\"image\": \"%s\","
            "\"description\": \"%s\""
            "},"
            "\"searchMethod\": \"hashmap\","
            "\"hashBucket\": %d"
            "}",
            book->id, book->title, book->author, book->genre, 
            book->year, book->image, book->description,
            hashFunction(book_id)
        );
        sendResponse(client_socket, response);
    } else {
        char response[] = "{\"success\": false, \"message\": \"Book not found\", \"searchMethod\": \"hashmap\"}";
        sendResponse(client_socket, response);
    }
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
    printf("🚀 Starting Library Management System Backend\n");
    printf("===============================================\n");
    
    // Initialize hash table
    initHashTable();
    
    // Load books from JSON
    int bookCount = loadBooksFromJSON();
    if (bookCount == 0) {
        printf("❌ Failed to load books. Exiting.\n");
        return 1;
    }
    
    printf("\n🌐 Starting API server on port %d...\n", PORT);
    
    // Create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        printf("❌ Error creating socket\n");
        return 1;
    }
    
    // Allow socket reuse
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Configure server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    // Bind socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("❌ Error binding socket\n");
        return 1;
    }
    
    // Listen for connections
    if (listen(server_socket, 10) < 0) {
        printf("❌ Error listening on socket\n");
        return 1;
    }
    
    printf("✅ Backend API server running at http://localhost:%d\n", PORT);
    printf("📖 Hashmap loaded with %d books\n", bookCount);
    printf("🔍 Try: http://localhost:%d/search/101\n", PORT);
    printf("Press Ctrl+C to stop\n\n");
    
    // Handle requests
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket < 0) continue;
        
        char buffer[1024];
        recv(client_socket, buffer, sizeof(buffer), 0);
        
        // Parse HTTP request
        if (strncmp(buffer, "GET /search/", 12) == 0) {
            char* id_str = buffer + 12;
            int book_id = atoi(id_str);
            printf("\n🔍 API Request: Search for book ID %d\n", book_id);
            handleSearchRequest(client_socket, book_id);
        } else if (strncmp(buffer, "GET /health", 11) == 0) {
            char response[] = "{\"status\": \"healthy\", \"books\": \"loaded\", \"backend\": \"hashmap\"}";
            sendResponse(client_socket, response);
        } else {
            char response[] = "{\"error\": \"Invalid endpoint. Use /search/{id} or /health\"}";
            sendResponse(client_socket, response);
        }
        
        close(client_socket);
    }
    
    close(server_socket);
    return 0;
}
