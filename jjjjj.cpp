//
// Created by Alexandru Marusteri on 4/9/25.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 200

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

typedef struct {
    char title[50];
    float price;
    char description[100];
} Listing;

void header();
Listing readListingFromInput();
void writeListingToFile(Listing l, const char* path);
void readListingsFromFile(const char* path);
int deleteListing(const char* title, const char* path);
void readAndSortListingsFromFile(const char* path);
int compareListingsByPrice(const void* a, const void* b);
void menu(int op, const char* path);

void header() {
    system("clear");
    printf("--------------\n");
    printf(GREEN "ReMax Real Estate\n" RESET "\n");
    printf("Main menu:\n");
    printf(YELLOW "1" RESET " - Add new listing\n");
    printf(YELLOW "2" RESET " - Display current listings\n");
    printf(YELLOW "3" RESET " - Delete listing\n");
    printf(YELLOW "4" RESET " - Display listings sorted by price\n");
    printf(YELLOW "0" RESET " - Exit\n");
    printf("--------------\n");
}

Listing readListingFromInput() {
    Listing l;
    printf("--------------\n");
    printf(GREEN "Add New Listing\n" RESET);
    printf("--------------\n");

    getchar();
    do {
        printf("✅ Property Title (max 50 characters || eg: Long Beach villa): ");
        fgets(l.title, sizeof(l.title), stdin);
        l.title[strcspn(l.title, "\n")] = '\0';
        if (strlen(l.title) == 0) {
            printf(RED "🚫 Title cannot be empty.\n" RESET);
        }
    } while(strlen(l.title) == 0);

    int validPrice = 0;
    while (!validPrice) {
        printf("✅ Property Price (e.g., 123456.78): ");
        if (scanf("%f", &l.price) != 1 || l.price <= 0) {
            printf(RED "🚫 Invalid price. Enter a positive number.\n" RESET);
            while (getchar() != '\n');
        } else {
            validPrice = 1;
        }
    }
    getchar();

    do {
        printf("✅ Property Description (max 100 characters): ");
        fgets(l.description, sizeof(l.description), stdin);
        l.description[strcspn(l.description, "\n")] = '\0';
        if (strlen(l.description) == 0) {
            printf(RED "🚫 Description cannot be empty.\n" RESET);
        }
    } while(strlen(l.description) == 0);

    return l;
}

void writeListingToFile(Listing l, const char* path) {
    FILE *fp = fopen(path, "a");
    if (fp == NULL) {
        printf(RED "🚫 Unable to open file.\n" RESET);
        return;
    }
    fprintf(fp, "%s,%.2f,%s\n", l.title, l.price, l.description);
    fclose(fp);
    printf(GREEN "✅ Listing added successfully!\n" RESET);
}

void readListingsFromFile(const char* path) {
    system("clear");
    FILE *fp = fopen(path, "r");
    char line[MAX_LINE];

    printf("--------------\n");
    printf(GREEN "Current Listings\n" RESET);
    printf("--------------\n");

    if (fp == NULL) {
        printf(RED "🚫 No listings available.\n" RESET);
    } else {
        int count = 0;
        while (fgets(line, MAX_LINE, fp)) {
            if (line[0] != '\n') {
                Listing l;
                line[strcspn(line, "\n")] = '\0';
                sscanf(line, "%[^,],%f,%[^\n]", l.title, &l.price, l.description);

                printf(YELLOW "Title:" RESET " %s%s%s\n", CYAN, l.title, RESET);
                printf(YELLOW "Price:" RESET " %s$%.2f%s\n", GREEN, l.price, RESET);
                printf(YELLOW "Description:" RESET " %s%s%s\n", CYAN, l.description, RESET);
                printf("--------------\n");
                count++;
            }
        }
        if (count == 0) {
            printf(RED "🚫 No listings found.\n" RESET);
        }
        fclose(fp);
    }

    printf("Press Enter to continue...");
    getchar();
    getchar();
}

int deleteListing(const char* titleToDelete, const char* path) {
    FILE *fp = fopen(path, "r");
    FILE *temp = fopen("temp.txt", "w");
    char line[MAX_LINE];
    int found = 0;

    if (fp == NULL || temp == NULL) {
        printf(RED "🚫 Error accessing files.\n" RESET);
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return 0;
    }

    while (fgets(line, MAX_LINE, fp)) {
        Listing l;
        sscanf(line, "%[^,],%f,%[^\n]", l.title, &l.price, l.description);
        if (strcmp(l.title, titleToDelete) != 0) {
            fputs(line, temp);
        } else {
            found = 1;
        }
    }

    fclose(fp);
    fclose(temp);
    remove(path);
    rename("temp.txt", path);

    return found;
}

int compareListingsByPrice(const void* a, const void* b) {
    Listing* l1 = (Listing*)a;
    Listing* l2 = (Listing*)b;
    if (l1->price < l2->price) return -1;
    else if (l1->price > l2->price) return 1;
    else return 0;
}

void readAndSortListingsFromFile(const char* path) {
    system("clear");
    FILE *fp = fopen(path, "r");
    char line[MAX_LINE];
    Listing listings[100];
    int count = 0;

    printf("--------------\n");
    printf(GREEN "Sorted Listings by Price\n" RESET);
    printf("--------------\n");

    if (fp == NULL) {
        printf(RED "🚫 No listings available.\n" RESET);
    } else {
        while (fgets(line, MAX_LINE, fp)) {
            if (line[0] != '\n') {
                line[strcspn(line, "\n")] = '\0';
                sscanf(line, "%[^,],%f,%[^\n]", listings[count].title, &listings[count].price, listings[count].description);
                count++;
            }
        }
        fclose(fp);

        if (count == 0) {
            printf(RED "🚫 No listings found.\n" RESET);
        } else {
            qsort(listings, count, sizeof(Listing), compareListingsByPrice);
            for (int i = 0; i < count; i++) {
                printf(YELLOW "Title:" RESET " %s%s%s\n", CYAN, listings[i].title, RESET);
                printf(YELLOW "Price:" RESET " %s$%.2f%s\n", GREEN, listings[i].price, RESET);
                printf(YELLOW "Description:" RESET " %s%s%s\n", CYAN, listings[i].description, RESET);
                printf("--------------\n");
            }
        }
    }

    printf("Press Enter to continue...");
    getchar();
    getchar();
}

void menu(int op, const char* path) {
    switch(op) {
        case 1: {
            system("clear");
            Listing l = readListingFromInput();
            writeListingToFile(l, path);
            printf("Press Enter to continue...");
            getchar();
            break;
        }
        case 2:
            readListingsFromFile(path);
            break;
        case 3: {
            system("clear");
            char titleToDelete[50];
            printf("--------------\n");
            printf(RED "Delete Listing\n" RESET);
            printf("--------------\n");

            getchar();
            printf("✅ Title of listing to delete: ");
            fgets(titleToDelete, sizeof(titleToDelete), stdin);
            titleToDelete[strcspn(titleToDelete, "\n")] = '\0';

            if (strlen(titleToDelete) == 0) {
                printf(RED "🚫 Title cannot be empty.\n" RESET);
            } else {
                int result = deleteListing(titleToDelete, path);
                if (result)
                    printf(GREEN "✅ Listing deleted successfully.\n" RESET);
                else
                    printf(RED "🚫 Listing not found.\n" RESET);
            }

            printf("Press Enter to continue...");
            getchar();
            break;
        }
        case 4:
            readAndSortListingsFromFile(path);
            break;
        default:
            system("clear");
            printf(GREEN "✅ EXIT!\n" RESET);
    }
}

int main() {
    char path[] = "listings.txt";
    int option;

    do {
        header();
        printf("Enter option: ");
        if (scanf("%d", &option) != 1) {
            printf(RED "🚫 Invalid input. Please enter a number.\n" RESET);
            while (getchar() != '\n');
            option = -1;
            continue;
        }
        menu(option, path);
    } while(option > 0 && option <= 4);

    return 0;
}
