#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAX_USERS 100
#define MAX_NAME 50
#define MAX_POSTS 100
#define MAX_FRIENDS 50
#define MAX_MESSAGES 100
#define MAX_MESSAGE_LENGTH 200
#define DATA_FILE "social_network_data.dat"

typedef struct {
    char sender[MAX_NAME];
    char receiver[MAX_NAME];
    char content[MAX_MESSAGE_LENGTH];
} Message;

typedef struct {
    char content[280];
} Post;

typedef struct {
    char username[MAX_NAME];
    char password[MAX_NAME];
    char friends[MAX_FRIENDS][MAX_NAME];
    int friendCount;
    Post posts[MAX_POSTS];
    int postCount;
    Message messages[MAX_MESSAGES];
    int messageCount;
} User;

User users[MAX_USERS];
int userCount = 0;
User* loggedInUser = NULL;

void saveData();
void loadData();
User* findUser(const char* username);
void getPassword(char *password, size_t maxLen);
void registerUser();
void loginUser();
void addFriend();
void viewFriends();
void createPost();
void viewPosts();
void sendMessage();
void viewMessages();
void menu();

void loadData() {
    FILE *file = fopen(DATA_FILE, "rb");
    if (file != NULL) {
        fread(&userCount, sizeof(int), 1, file);
        fread(users, sizeof(User), userCount, file);
        fclose(file);
    }
}

void saveData() {
    FILE *file = fopen(DATA_FILE, "wb");
    if (file != NULL) {
        fwrite(&userCount, sizeof(int), 1, file);
        fwrite(users, sizeof(User), userCount, file);
        fclose(file);
    }
}

User* findUser(const char* username) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return &users[i];
        }
    }
    return NULL;
}

void getPassword(char *password, size_t maxLen){
    int i = 0;
    char ch;

    while (i < maxLen - 1) {
        ch = getch();
        if (ch == '\r') {
            break;
        } else if (ch == '\b') { 
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
    printf("\n");
}

void registerUser() {
    if (userCount >= MAX_USERS) {
        printf("USER LIMIT REACHED.\n");
        return;
    }
    char username[MAX_NAME], password[MAX_NAME];
    getchar();
    printf("ENTER NEW USERNAME: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';
    
    if (findUser(username)) {
        printf("USERNAME ALREADY EXISTS.\n");
        return;
    }

    while(1) {
        printf("ENTER PASSWORD (exactly 8 characters): ");
        getPassword(password, sizeof(password));
        if(strlen(password) == 8) {
            break;
        } else {
            printf("THE LENGTH OF THE PASSWORD SHOULD BE EQUAL TO 8. TRY AGAIN.\n");
        }
    }
    
    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    users[userCount].friendCount = 0;
    users[userCount].postCount = 0;
    users[userCount].messageCount = 0;
    userCount++;
    saveData();
    printf("USER REGISTERED SUCCESSFULLY.\n");
}

void loginUser() {
    char username[MAX_NAME], password[MAX_NAME];
    getchar();
    
    printf("ENTER USERNAME: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';
    
    while(1) {
        printf("\nENTER PASSWORD (exactly 8 characters): ");
        getPassword(password, sizeof(password));
        if(strlen(password) == 8) {
            break;
        } else {
            printf("\nTHE LENGTH OF THE PASSWORD SHOULD BE EQUAL TO 8.\n");
        }
    }
    
    User* user = findUser(username);
    if (user && strcmp(user->password, password) == 0) {
        loggedInUser = user;
        printf("LOGIN SUCCESSFUL.\n");
    } else {
        printf("INVALID CREDENTIALS.\n");
    }
}

void addFriend() {
    if (!loggedInUser) {
        printf("PLEASE LOGIN TO ADD FRIENDS.\n");
        return;
    }
    char friendName[MAX_NAME];
    getchar();
    printf("ENTER FRIEND's USERNAME: ");
    fgets(friendName, sizeof(friendName), stdin);
    friendName[strcspn(friendName, "\n")] = '\0';
    
    User* friendUser = findUser(friendName);
    if (!friendUser || strcmp(friendName, loggedInUser->username) == 0) {
        printf("\nINVALID USER.\n");
        return;
    }
    for (int i = 0; i < loggedInUser->friendCount; i++) {
        if (strcmp(loggedInUser->friends[i], friendName) == 0) {
            printf("ALREADY FRIENDS.\n");
            return;
        }
    }
    strcpy(loggedInUser->friends[loggedInUser->friendCount++], friendName);
    strcpy(friendUser->friends[friendUser->friendCount++], loggedInUser->username);
    saveData();
    printf("FRIEND ADDED SUCCESSFULLY.\n");
}

void viewFriends() {
    if (!loggedInUser) {
        printf("PLEASE LOGIN TO FIND YOUR FRIENDS.\n");
        return;
    }
    if (loggedInUser->friendCount == 0) {
        printf("YOU ARE NOT FRIENDS WITH ANYONE.\n");
        return;
    }
    printf("YOUR FRIENDS:\n");
    for (int i = 0; i < loggedInUser->friendCount; i++) {
        printf("%s\n", loggedInUser->friends[i]);
    }
}

void createPost() {
    if (!loggedInUser) {
        printf("PLEASE LOGIN TO CREATE A POST.\n");
        return;
    }
    if (loggedInUser->postCount >= MAX_POSTS) {
        printf("POST LIMIT REACHED.\n");
        return;
    }
    printf("ENTER YOUR STATUS: ");
    getchar();
    fgets(loggedInUser->posts[loggedInUser->postCount++].content, 280, stdin);
    saveData();
    printf("POST CREATED.\n");
}

void viewPosts() {
    if (!loggedInUser) {
        printf("PLEASE LOGIN TO SEE POSTS.\n");
        return;
    }
    if (loggedInUser->postCount == 0) {
        printf("NO POSTS YET.\n");
        return;
    }
    printf("YOUR POSTS:\n");
    for (int i = 0; i < loggedInUser->postCount; i++) {
        printf("- %s", loggedInUser->posts[i].content);
    }
}

void sendMessage() {
    if (!loggedInUser) return;
    char recipient[MAX_NAME];
    printf("ENTER RECIPIENT's USERNAME: ");
    scanf("%s", recipient);
    User* receiver = findUser(recipient);
    if (!receiver) {
        printf("USER NOT FOUND.\n");
        return;
    }
    if (loggedInUser->messageCount >= MAX_MESSAGES || receiver->messageCount >= MAX_MESSAGES) {
        printf("MESSAGE LIMIT REACHED.\n");
        return;
    }
    printf("ENTER YOUR MESSAGE: ");
    getchar();
    char msgText[MAX_MESSAGE_LENGTH];
    fgets(msgText, MAX_MESSAGE_LENGTH, stdin);
    
    Message *senderMsg = &loggedInUser->messages[loggedInUser->messageCount++];
    strcpy(senderMsg->sender, loggedInUser->username);
    strcpy(senderMsg->receiver, recipient);
    strcpy(senderMsg->content, msgText);
    
    Message *receiverMsg = &receiver->messages[receiver->messageCount++];
    strcpy(receiverMsg->sender, loggedInUser->username);
    strcpy(receiverMsg->receiver, recipient);
    strcpy(receiverMsg->content, msgText);
    
    saveData();
    printf("MESSAGE SENT.\n");
}

void viewMessages() {
    if (!loggedInUser) return;
    printf("RECEIVED MESSAGES: \n");
    int found = 0;
    for (int i = 0; i < loggedInUser->messageCount; i++) {
        if (strcmp(loggedInUser->messages[i].receiver, loggedInUser->username) == 0) {
            printf("FROM %s: %s\n", loggedInUser->messages[i].sender, loggedInUser->messages[i].content);
            found = 1;
        }
    }
    if (!found) {
        printf("NO RECEIVED MESSAGE.\n");
    }
    
    printf("SENT MESSAGES: \n");
    int sentFound = 0;
    for (int i = 0; i < loggedInUser->messageCount; i++) {
        if (strcmp(loggedInUser->messages[i].sender, loggedInUser->username) == 0) {
            printf("TO %s: %s\n", loggedInUser->messages[i].receiver, loggedInUser->messages[i].content);
            sentFound = 1;
        }
    }
    if (!sentFound) {
        printf("NO SENT MESSAGE.\n");
    }
}

void menu() {
    int choice;
    printf("\n************SOCIAL NETWORK STIMULATION****************\n");
    while (1) {
        if (!loggedInUser) {
            printf("\n1. REGISTER\n2. LOGIN\n3. EXIT\nENTER YOUR CHOICE:\n");
            scanf("%d", &choice);
            switch (choice) {
                case 1:
                    registerUser();
                    break;
                case 2:
                    if (userCount == 0) {
                        printf("PLEASE REGISTER FIRST.\n");
                    } else {
                        loginUser();
                    }
                    break;
                case 3:
                    printf("EXITING\n");
                    exit(0);
                default:
                    printf("INVALID CHOICE\n");
            }
        } 
        else {
            printf("\n1. ADD FRIEND\n2. VIEW FRIENDS\n3. CREATE POST\n4. VIEW POSTS\n5. SEND MESSAGE\n6. VIEW MESSAGES\n7. LOGOUT\n8. EXIT\nENTER YOUR CHOICE:\n");
            scanf("%d", &choice);
            switch (choice) {
                case 1:
                    addFriend();
                    break;
                case 2:
                    viewFriends();
                    break;
                case 3:
                    createPost();
                    break;
                case 4:
                    viewPosts();
                    break;
                case 5:
                    sendMessage();
                    break;
                case 6:
                    viewMessages();
                    break;
                case 7:
                    loggedInUser = NULL;
                    printf("LOGGED OUT\n");
                    break;
                case 8:
                    exit(0);
                default:
                    printf("INVALID CHOICE.\n");
            }
        }
    }
}

int main() {
    loadData();
    menu();
    return 0;
}
