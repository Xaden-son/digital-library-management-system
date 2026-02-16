#include <stdio.h>
#include <string.h>

// Maximum number of books that can be stored
#define MAX_BOOKS 100

// File used for saving and loading library data
#define DATA_FILE "library.txt"

// Book status constants (avoid magic numbers) 
#define STATUS_READ     1
#define STATUS_OWNED    2                                           
#define STATUS_WISHLIST 3

// Structure that represents a single book
typedef struct {
    int id;                 // Unique book ID
    char title[60];         // Book title
    char author[40];        // Book author
    int year;               // Publication year
    int status;             // Book status (read, owned, wishlist)
} Book;

// Global storage for all books
Book books[MAX_BOOKS];      // Array to store books
int bookCount = 0;          // Number of books currently stored
int nextId = 1;             // Auto-increment ID generator

//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Function prototypes (declared before main for readability)
void printMainMenu(void); 
void clearInputBuffer(void);
void readLine(const char* prompt, char* out, int maxLen);
int  readInt(const char* prompt);
int  readIntInRange(const char* prompt, int min, int max);

void addBook(void);
void listReadBooks(void);
void listOwnedBooks(void);                                  
void listWishlist(void);
void updateBookStatus(void);
void deleteBook(void);
void showStats(void);

// File operations for persistence
void saveLibrary(void);
void loadLibrary(void);

///////////////////////////////////////////////////////////////////////////////////////////////////////////

// Prints the main menu options
void printMainMenu(void) {
    printf("\n=== DIGITAL LIBRARY ===\n");
    printf("1) Show my read books\n");
    printf("2) Show my owned (unread) books\n");
    printf("3) Show my wishlist\n");
    printf("4) Add a book\n");
    printf("5) Update book status\n");
    printf("6) Delete a book\n");
    printf("7) Show stats\n");
    printf("0) Exit\n");
    printf("Select: ");
}

// Clears leftover characters from input buffer
void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { } 
}

// Safely reads a line of text and removes newline character
void readLine(const char* prompt, char* out, int maxLen) {

    printf("%s", prompt);
    if (fgets(out, maxLen, stdin) == NULL) { 
        out[0] = '\0';
        return;
    }

    size_t len = strlen(out);
    if (len > 0 && out[len - 1] == '\n')
        out[len - 1] = '\0';                   
}

// Reads a valid integer from user
int readInt(const char* prompt) {
    
    int x;
    printf("%s", prompt);

    while (scanf("%d", &x) != 1) {
        printf("Invalid input.\nTry again:");
        clearInputBuffer();
    }
    clearInputBuffer();
    return x;
}

// Reads an integer within a specific range
int readIntInRange(const char* prompt, int min, int max) {
    int x = readInt(prompt);
    while (x < min || x > max) {
        printf("Please enter a number between %d and %d.\n", min, max);
        x = readInt(prompt);
    }
    return x;
}

// Adds a new book to the library
void addBook(void) {
    if (bookCount >= MAX_BOOKS) {
        printf("Library is full.\n");
        return;
    }

    Book b;
    b.id = nextId++;   

    readLine("Title: ", b.title, sizeof(b.title));
    readLine("Author: ", b.author, sizeof(b.author));
    b.year = readInt("Year: ");
    b.status = readIntInRange("Status (1=Read, 2=Owned, 3=Wishlist): ", 1, 3);

    books[bookCount++] = b;   
    printf("Book added. (ID=%d).\n", b.id);
}

// Helper function to list books by status (internal use only)
static void listByStatus(int status) {
    int found = 0;
    for (int i = 0; i < bookCount; i++) {
        if (books[i].status == status) {
            printf("ID:%d | %s | %s | %d\n",
                   books[i].id,
                   books[i].title,
                   books[i].author,
                   books[i].year);
            found = 1;
        }
    }
    if (!found)
        printf("No books found.\n");
}

// Lists read books
void listReadBooks(void) {
    listByStatus(STATUS_READ);
}

// Lists owned but unread books
void listOwnedBooks(void) {
    listByStatus(STATUS_OWNED);
}

// Lists wishlist books
void listWishlist(void) {
    listByStatus(STATUS_WISHLIST);
}

// Updates the status of a book using its ID
void updateBookStatus(void) {

    int enterId = readInt("Enter book ID: ");
    int newStatus = readIntInRange("Enter new status (1-3): ",1,3);

    int found=0;

    for (int i = 0; i < bookCount; i++) {
        if (books[i].id == enterId) 
        {
            books[i].status = newStatus;
            found = 1;
            printf("Book status updated successfully.\n");
            break;
        }
    }

    if (!found)
        printf("Book not found.\n");
}

// Deletes a book by shifting array elements
void deleteBook(void) {
    int deleteId = readInt("Enter a book ID to delete: ");
    int found = 0;

    for (int i = 0; i < bookCount; i++) {
        if (books[i].id == deleteId) {
            for (int j = i; j < bookCount - 1; j++) {
                books[j] = books[j + 1];
            }
            bookCount--;
            found = 1;
            printf("Book deleted.\n");
            break;
        }
    }

    if (!found)
        printf("Invalid book ID.\n");
}

// Shows statistics about the library
void showStats(void) {
    int read = 0, owned = 0, wish = 0;

    for (int i = 0; i < bookCount; i++) {
        if (books[i].status == STATUS_READ) read++;
        else if (books[i].status == STATUS_OWNED) owned++;
        else if (books[i].status == STATUS_WISHLIST) wish++;
    }

    printf("\n--- STATS ---\n");
    printf("\nTotal books: %d\n", bookCount);
    printf("Read: %d\nOwned: %d\nWishlist: %d\n", read, owned, wish);
}

// Saves library data to file
void saveLibrary(void) {

    FILE* f = fopen(DATA_FILE, "w");
    if (!f) return;

    for (int i = 0; i < bookCount; i++) {           
        fprintf(f, "%d|%s|%s|%d|%d\n",
                books[i].id,
                books[i].title,
                books[i].author,
                books[i].year,
                books[i].status);
    }
    fclose(f);
}

// Loads library data from file
void loadLibrary(void) {
    FILE* f = fopen(DATA_FILE, "r");
    if (!f) return;

    bookCount = 0;   
    nextId = 1;

    char line[256];                             
    while (fgets(line, sizeof(line), f)) {
        Book b;
        char title[60], author[40];

        if (sscanf(line, "%d|%59[^|]|%39[^|]|%d|%d",&b.id, title, author, &b.year, &b.status) == 5)
        {
            strcpy(b.title, title);                                         
            strcpy(b.author, author);
            books[bookCount++] = b;
            if (b.id >= nextId) nextId = b.id + 1;          
        }
    }
    fclose(f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

// Program entry point
int main(void) {
    int choice;

    loadLibrary();   // Load saved data at startup

    do {                                    
        printMainMenu();

        if (scanf("%d", &choice) != 1) {
            printf("Enter an integer, not a letter or word.\n");
            clearInputBuffer();
            choice = -1;      // ensure loop does NOT exit
            continue;         // show menu again
        }
        clearInputBuffer();

        switch (choice) {
            case 1: listReadBooks(); break;
            case 2: listOwnedBooks(); break;
            case 3: listWishlist(); break;
            case 4: addBook(); break;
            case 5: updateBookStatus(); break;
            case 6: deleteBook(); break;
            case 7: showStats(); break;
            case 0:
                saveLibrary();   // Save data before exit
                printf("Goodbye.\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);

    return 0;   
}
