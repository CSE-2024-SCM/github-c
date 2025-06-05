// =============================
// Weather-Based Outfit Recommender in C Programming 
// =============================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// =============================
// CONSTANTS AND DEFINITIONS
// =============================

#define MAX_LEN 100
#define NUM_OUTFITS 5
#define NUM_ITEMS 3
#define NUM_ACCESSORIES 5
#define NUM_SHOES 5
#define NUM_JACKETS 5
#define MAX_HISTORY 5
#define MIN_TEMP -50.0
#define MAX_TEMP 50.0

#define GREEN   "\033[1;32m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define MAGENTA "\033[1;35m"
#define RESET   "\033[0m"

// =============================
// STRUCTURE DEFINITIONS
// =============================

typedef struct {
    char title[MAX_LEN];
    char items[NUM_ITEMS][MAX_LEN];
} Outfit;

typedef struct {
    char city[MAX_LEN];
    float temp;
    char condition[MAX_LEN];
} Weather;

typedef struct {
    Outfit outfit;
    char accessory[MAX_LEN];
    char shoe[MAX_LEN];
    char jacket[MAX_LEN];
    Weather weather;
    char user_note[MAX_LEN]; // User note feature
    char mood[MAX_LEN];      // Mood feature
    char favorite[MAX_LEN];  // NEW FEATURE: Mark as favorite (yes/no)
} HistoryEntry;

// =============================
// GLOBAL DATA ARRAYS
// =============================

Outfit cold_outfits[NUM_OUTFITS] = {
    {"Winter Warrior", {"Trench Coat", "Corduroy Pants", "Turtleneck"}},
    {"Arctic Explorer", {"Puffer Jacket", "Thermal Leggings", "Wool Sweater"}},
    {"Cozy Professional", {"Wool Coat", "Dark Jeans", "Cashmere Sweater"}},
    {"Mountain Hiker", {"Down Jacket", "Snow Pants", "Thermal Top"}},
    {"Elegant Chill", {"Peacoat", "Wool Trousers", "Layered Shirt"}}
};

Outfit moderate_outfits[NUM_OUTFITS] = {
    {"Smart Casual", {"Long Sleeve Pullover", "Chinos", "Light Cardigan"}},
    {"Weekend Relaxed", {"Henley Shirt", "Khaki Pants", "Zip-up Hoodie"}},
    {"Urban Explorer", {"Denim Jacket", "Joggers", "Graphic Tee"}},
    {"Business Breeze", {"Blazer", "Slacks", "Oxford Shirt"}},
    {"Neutral Trend", {"Sweatshirt", "Cuffed Pants", "Layered Tee"}}
};

Outfit hot_outfits[NUM_OUTFITS] = {
    {"Summer Cool", {"Linen Shirt", "Cotton Shorts", "Baseball Cap"}},
    {"Beach Ready", {"Tank Top", "Board Shorts", "Sun Hat"}},
    {"City Heat", {"Breathable Tee", "Linen Pants", "Cooling Towel"}},
    {"Tropical Explorer", {"Short Sleeve Shirt", "Cargos", "Sun Bandana"}},
    {"Resort Comfort", {"Sleeveless Top", "Jersey Shorts", "Visor"}}
};

char cold_accessories[NUM_ACCESSORIES][MAX_LEN] = {"Wool Scarf", "Insulated Gloves", "Warm Beanie", "Fleece Headband", "Thermal Socks"};
char moderate_accessories[NUM_ACCESSORIES][MAX_LEN] = {"Baseball Cap", "Stylish Watch", "Leather Belt", "Sunglasses", "Light Scarf"};
char hot_accessories[NUM_ACCESSORIES][MAX_LEN] = {"Wide-Brim Hat", "Cooling Bandana", "UV Wristband", "Portable Fan", "Sweat Towel"};

char cold_shoes[NUM_SHOES][MAX_LEN] = {"Waterproof Boots", "Insulated Sneakers", "Warm Chelsea Boots", "Snow Boots", "Thermal Loafers"};
char moderate_shoes[NUM_SHOES][MAX_LEN] = {"Comfortable Sneakers", "Canvas Shoes", "Casual Loafers", "Walking Boots", "Slip-ons"};
char hot_shoes[NUM_SHOES][MAX_LEN] = {"Breathable Sandals", "Flip-Flops", "Mesh Sneakers", "Water Shoes", "Ventilated Slip-ons"};

char cold_jackets[NUM_JACKETS][MAX_LEN] = {"Thermal Jacket", "Wool Jacket", "Insulated Coat", "Snow Parka", "Thick Hoodie"};
char moderate_jackets[NUM_JACKETS][MAX_LEN] = {"Bomber Jacket", "Fleece Jacket", "Blazer", "Windbreaker", "Thin Hoodie"};
char hot_jackets[NUM_JACKETS][MAX_LEN] = {"Mesh Jacket", "Light Hoodie", "Open Shirt", "Sport Vest", "Cotton Kimono"};

HistoryEntry history[MAX_HISTORY];
int history_count = 0;

// =============================
// FUNCTION DECLARATIONS
// =============================

void print_banner();
void display_greeting();
void strip_newline(char *str);
void wait_for_user();
int get_valid_choice(int max);
void get_weather_input(Weather *weather);
const char* get_category(float temp);
void recommend_outfit(const Weather *weather);
void display_outfits(Outfit outfits[], int size);
void display_options(char options[][MAX_LEN], int count);
void simulate_loading(const char *msg);
void show_weather_tips(const char *condition);
void save_history(Outfit o, Weather w, const char *a, const char *s, const char *j, const char *user_note, const char *mood, const char *favorite); // updated
void show_history();
void print_divider();
void repeat_menu();
void farewell();

void get_user_note(char *note) {
    printf("\n(Optional) Add a note about this outfit (e.g., occasion, mood, etc.): ");
    fgets(note, MAX_LEN, stdin);
    strip_newline(note);
}

void get_user_mood(char *mood) {
    printf("\n(Optional) How are you feeling today? (e.g., happy, energetic, laid-back): ");
    fgets(mood, MAX_LEN, stdin);
    strip_newline(mood);
}

// =============================
// NEW FEATURE: Mark as favorite
// =============================

void get_favorite_response(char *favorite) {
    printf("\nWould you like to mark this outfit as a favorite? (yes/no): ");
    fgets(favorite, MAX_LEN, stdin);
    strip_newline(favorite);
    // Standardize
    for (int i = 0; favorite[i]; i++) favorite[i] = tolower(favorite[i]);
    if (strcmp(favorite, "yes") != 0)
        strcpy(favorite, "no");
}

// =============================
// MAIN FUNCTION
// =============================

void suggest_color_style(const char *condition) {
    printf(MAGENTA "\n--- Style Suggestion ---\n" RESET);
    if (strstr(condition, "Rain") || strstr(condition, "rain"))
        printf("Try earthy tones like olive or brown with waterproof fabrics.\n");
    else if (strstr(condition, "Sunny") || strstr(condition, "sunny"))
        printf("Go for bright colors like yellow or turquoise to complement the sunlight.\n");
    else if (strstr(condition, "Cloud") || strstr(condition, "cloud"))
        printf("Warm colors like orange or coral will cheer you up on cloudy days.\n");
    else if (strstr(condition, "Snow") || strstr(condition, "snow"))
        printf("Whites and blues with reflective accessories look stunning in snow.\n");
    else
        printf("Neutral tones like beige, grey, or navy are safe and elegant.\n");
}

void display_greeting_with_day() {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    int hour = tm_info->tm_hour;
    int wday = tm_info->tm_wday;

    const char *days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    printf(BLUE "\nHappy %s!\n" RESET, days[wday]);

    if (hour < 12)
        printf(GREEN "Good Morning! Start your day with great style!\n" RESET);
    else if (hour < 18)
        printf(YELLOW "Good Afternoon! Keep your outfit cool and comfortable.\n" RESET);
    else
        printf(CYAN "Good Evening! Time for something cozy or classy.\n" RESET);
}

void give_temperature_advice(float temp) {
    printf(MAGENTA "\n--- Temperature Advice ---\n" RESET);

    if (temp < 0)
        printf("Extreme cold! Prioritize thermal wear and insulated layers.\n");
    else if (temp < 10)
        printf("Cold weather. Wear full sleeves, coats, and warm footwear.\n");
    else if (temp < 20)
        printf("Mild chill. Layer up moderately with breathable outerwear.\n");
    else if (temp < 30)
        printf("Comfortable temperature. Dress flexibly.\n");
    else if (temp < 40)
        printf("Warm weather. Wear light, breathable fabrics and stay hydrated.\n");
    else
        printf("Extremely hot! Avoid dark colors and heavy clothing. Stay cool!\n");
}

void secret_feature() {
    printf(CYAN "\nYou've unlocked a secret tip! 🌟\n" RESET);
    printf("Tip: Mix textures! Pair cotton with denim or knits for visual interest.\n");
    wait_for_user();
}

void check_for_secret_code() {
    char input[MAX_LEN];
    printf("\nEnter a secret style code or just press Enter to skip: ");
    fgets(input, MAX_LEN, stdin);
    strip_newline(input);

    if (strcmp(input, "fashion101") == 0) {
        secret_feature();
    }
}

void show_help_section() {
    printf(CYAN "\n--- Help & Tips ---\n" RESET);
    printf("1. Temperature input: Enter in Celsius between -50 and 50.\n");
    printf("2. Condition: Examples - Sunny, Rainy, Cloudy, Snowy.\n");
    printf("3. Choose from multiple outfit options manually.\n");
    printf("4. Your selections are stored in history for review.\n");
    printf("5. Look out for hidden Easter eggs! 🤫\n");
    wait_for_user();
}

void main_menu() {
    printf("\n" CYAN "Main Menu:\n1. Get Outfit Recommendation\n2. View History\n3. Help\n4. Exit\n" RESET);
}

int main() {
    while (1) {
        Weather current_weather;
        print_banner();
        display_greeting_with_day();
        main_menu();
        int choice = get_valid_choice(4);

        if (choice == 4) break;
        else if (choice == 2) show_history();
        else if (choice == 3) show_help_section();
        else {
            get_weather_input(&current_weather);
            check_for_secret_code();
            simulate_loading("Analyzing weather and crafting your stylish fit...");
            recommend_outfit(&current_weather);
        }

        print_divider();
        repeat_menu();
        if (get_valid_choice(2) == 2) break;
    }

    farewell();
    return 0;
}

// =============================
// FINAL UPDATE TO RECOMMENDER
// =============================

void recommend_outfit(const Weather *weather) {
    const char *category = get_category(weather->temp);

    Outfit *outfits;
    char (*accessories)[MAX_LEN];
    char (*shoes)[MAX_LEN];
    char (*jackets)[MAX_LEN];

    if (strcmp(category, "cold") == 0) {
        outfits = cold_outfits;
        accessories = cold_accessories;
        shoes = cold_shoes;
        jackets = cold_jackets;
    }
    else if (strcmp(category, "moderate") == 0) {
        outfits = moderate_outfits;
        accessories = moderate_accessories;
        shoes = moderate_shoes;
        jackets = moderate_jackets;
    }
    else {
        outfits = hot_outfits;
        accessories = hot_accessories;
        shoes = hot_shoes;
        jackets = hot_jackets;
    }

    give_temperature_advice(weather->temp);

    printf("\nChoose an outfit from the list below:\n");
    display_outfits(outfits, NUM_OUTFITS);
    int outfit_choice = get_valid_choice(NUM_OUTFITS) - 1;

    printf("\nChoose an accessory:\n");
    display_options(accessories, NUM_ACCESSORIES);
    int acc_choice = get_valid_choice(NUM_ACCESSORIES) - 1;

    printf("\nChoose a shoe option:\n");
    display_options(shoes, NUM_SHOES);
    int shoe_choice = get_valid_choice(NUM_SHOES) - 1;

    printf("\nChoose a jacket:\n");
    display_options(jackets, NUM_JACKETS);
    int jacket_choice = get_valid_choice(NUM_JACKETS) - 1;

    // User Note
    char user_note[MAX_LEN] = "";
    get_user_note(user_note);

    // Mood
    char mood[MAX_LEN] = "";
    get_user_mood(mood);

    // NEW FEATURE: Mark as favorite
    char favorite[MAX_LEN] = "";
    get_favorite_response(favorite);

    // Final Recommendation
    printf(GREEN "\n--- Your Outfit Recommendation ---\n" RESET);
    Outfit selected = outfits[outfit_choice];
    printf("Outfit: %s\n", selected.title);
    for (int i = 0; i < NUM_ITEMS; i++) {
        printf("- %s\n", selected.items[i]);
    }
    printf("Accessory: %s\n", accessories[acc_choice]);
    printf("Shoes: %s\n", shoes[shoe_choice]);
    printf("Jacket: %s\n", jackets[jacket_choice]);
    if (strlen(user_note) > 0)
        printf("Your note: %s\n", user_note);
    if (strlen(mood) > 0)
        printf("Your mood: %s\n", mood);
    printf("Marked as favorite: %s\n", strcmp(favorite, "yes") == 0 ? "Yes" : "No");

    show_weather_tips(weather->condition);
    suggest_color_style(weather->condition);
    save_history(selected, *weather, accessories[acc_choice], shoes[shoe_choice], jackets[jacket_choice], user_note, mood, favorite);
    wait_for_user();
}

// =============================
// FUNCTION DEFINITIONS
// =============================

void print_banner() {
    printf(MAGENTA "\n==== Weather-Based Outfit Recommender ====\n" RESET);
}

void display_greeting() {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    int hour = tm_info->tm_hour;

    if (hour < 12)
        printf(GREEN "\nGood Morning!\n" RESET);
    else if (hour < 18)
        printf(YELLOW "\nGood Afternoon!\n" RESET);
    else
        printf(CYAN "\nGood Evening!\n" RESET);
}

void strip_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
}

void wait_for_user() {
    printf("\nPress Enter to continue...");
    getchar();
}

int get_valid_choice(int max) {
    int choice;
    while (1) {
        printf("\nEnter your choice (1-%d): ", max);
        if (scanf("%d", &choice) && choice >= 1 && choice <= max) {
            while (getchar() != '\n'); // flush input
            return choice;
        }
        else {
            printf(RED "Invalid input. Try again.\n" RESET);
            while (getchar() != '\n');
        }
    }
}

void get_weather_input(Weather *weather) {
    printf("\nEnter your city name: ");
    fgets(weather->city, MAX_LEN, stdin);
    strip_newline(weather->city);

    while (1) {
        printf("Enter current temperature in Celsius: ");
        if (scanf("%f", &weather->temp) && weather->temp >= MIN_TEMP && weather->temp <= MAX_TEMP) {
            break;
        }
        else {
            printf(RED "Invalid temperature. Try again.\n" RESET);
            while (getchar() != '\n');
        }
    }

    while (getchar() != '\n');
    printf("Enter weather condition (e.g., Sunny, Rainy, Cloudy): ");
    fgets(weather->condition, MAX_LEN, stdin);
    strip_newline(weather->condition);
}

const char* get_category(float temp) {
    if (temp < 15.0) return "cold";
    else if (temp <= 30.0) return "moderate";
    return "hot";
}

void simulate_loading(const char *msg) {
    printf("\n%s", msg);
    for (int i = 0; i < 3; i++) {
        printf(".");
        fflush(stdout);
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }
    printf("\n");
}

void display_outfits(Outfit outfits[], int size) {
    for (int i = 0; i < size; i++) {
        printf(YELLOW "%d. %s\n" RESET, i + 1, outfits[i].title);
        for (int j = 0; j < NUM_ITEMS; j++) {
            printf("   - %s\n", outfits[i].items[j]);
        }
    }
}

void display_options(char options[][MAX_LEN], int count) {
    for (int i = 0; i < count; i++) {
        printf("%d. %s\n", i + 1, options[i]);
    }
}

void show_weather_tips(const char *condition) {
    printf(BLUE "\n--- Weather Tip ---\n" RESET);
    if (strstr(condition, "Rain") || strstr(condition, "rain"))
        printf("Don't forget to carry an umbrella or raincoat!\n");
    else if (strstr(condition, "Sunny") || strstr(condition, "sunny"))
        printf("Apply sunscreen and wear light fabrics.\n");
    else if (strstr(condition, "Cloud") || strstr(condition, "cloud"))
        printf("Might be a gloomy day. Bright colors can lift your mood!\n");
    else
        printf("Stay comfortable and adapt as needed.\n");
}

void save_history(Outfit o, Weather w, const char *a, const char *s, const char *j, const char *user_note, const char *mood, const char *favorite) {
    if (history_count == MAX_HISTORY) history_count = 0; // circular
    history[history_count].outfit = o;
    strcpy(history[history_count].weather.city, w.city);
    history[history_count].weather.temp = w.temp;
    strcpy(history[history_count].weather.condition, w.condition);
    strcpy(history[history_count].accessory, a);
    strcpy(history[history_count].shoe, s);
    strcpy(history[history_count].jacket, j);
    if (user_note)
        strncpy(history[history_count].user_note, user_note, MAX_LEN);
    else
        history[history_count].user_note[0] = '\0';
    if (mood)
        strncpy(history[history_count].mood, mood, MAX_LEN);
    else
        history[history_count].mood[0] = '\0';
    if (favorite)
        strncpy(history[history_count].favorite, favorite, MAX_LEN);
    else
        history[history_count].favorite[0] = '\0';
    history_count++;
}

void show_history() {
    if (history_count == 0) {
        printf(RED "\nNo history found.\n" RESET);
        return;
    }

    printf(CYAN "\n--- Past Recommendations ---\n" RESET);
    for (int i = 0; i < history_count; i++) {
        HistoryEntry h = history[i];
        printf(YELLOW "\nCity: %s | Temp: %.1f°C | Condition: %s\n" RESET,
               h.weather.city, h.weather.temp, h.weather.condition);
        printf("Outfit: %s\n", h.outfit.title);
        for (int j = 0; j < NUM_ITEMS; j++) {
            printf(" - %s\n", h.outfit.items[j]);
        }
        printf("Accessory: %s\n", h.accessory);
        printf("Shoes: %s\n", h.shoe);
        printf("Jacket: %s\n", h.jacket);
        if (strlen(h.user_note) > 0)
            printf("Note: %s\n", h.user_note);
        if (strlen(h.mood) > 0)
            printf("Mood: %s\n", h.mood);
        if (strcmp(h.favorite, "yes") == 0)
            printf(GREEN "★ Favorite!\n" RESET);
    }
    wait_for_user();
}

void print_divider() {
    printf("\n----------------------------------------\n");
}

void repeat_menu() {
    printf("\nWould you like to:\n1. Try Again\n2. Exit\n");
}

void farewell() {
    printf(GREEN "\nThank you for using the Outfit Recommender!\nStay stylish and weather-ready!\n" RESET);
}
