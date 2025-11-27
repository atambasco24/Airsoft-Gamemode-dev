
// to compile:
// gcc bomb.c -o bomb -lwiringPi -li2c1602
#include <stdio.h>
#include <i2c1602.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdlib.h>
#include <time.h>


#define LCD_ADDR 0x27 // "i2cdetect -y 1"
I2C16x2 lcd; // i2cdetect init

#define ROWS 4
#define COLS 3
#define R1_PIN 11
#define R2_PIN 13
#define R3_PIN 15
#define R4_PIN 16
#define C1_PIN 29
#define C2_PIN 31
#define C3_PIN 33 


/*                                         //LCD FUNCTIONS
void lcd_setAddr(int fd);
void lcd_init(int addr);

void lcd_byte(int bits, int mode);                // send a byte of data
void lcd_toggle_enable(int bits);                 // toggle Enable bit
void typeInt(int i);                                      // print integer
void typeFloat(float myFloat);                    // print float
void typeString(const char *s);                   // print string
void typeChar(char val);                                  // print a char
void lcdLoc(int line);                                    // move cursor
void ClrLcd(void);                                                // clr LCD return home

typedef struct I2C16x2 {
        int fd;
        int addr;
} I2C16x2;
*/



// Define the pin numbers connected to the microcontroller
int rowPins[ROWS] = {R1_PIN, R2_PIN, R3_PIN, R4_PIN};
int colPins[COLS] = {C1_PIN, C2_PIN, C3_PIN};

// Define the character map for the 4x4 keypad
char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};


//function prototypes
int generateCode();
void arm();
void init_keypad();
char get_key();

int main(){
//	printf("DEBUG: STARTED MAIN\n");
        wiringPiSetup();
//	printf("DEBUG: WIRING PI SETUP\n");
        lcd_init(LCD_ADDR);
//	printf("DEBUG: LCD INITIATED");
	ClrLcd();
//	printf("DEBUG: LCD CLEARED\n");
	init_keypad();
//	printf("DEBUG: KEYPAD INITIATED\n");
	
	lcdLoc(LINE1);
//	printf("SET LOCATION\n");
	typeString("Press * to arm");
	
    while(1) {
        char pressedKey = get_key();
        if (pressedKey != '\0') {
            printf("%c key pressed\n", pressedKey);
	}
	else if (pressedKey == '*'){
	printf("* pressed");
	}
	else {
	printf("no input detected\n");
	}
	//arm();
	//lcdLoc(LINE2 + 0);
	//typeString("Armed!");
		// Process the pressed key (e.g., print to console, use in a program)
            // Add a slight delay to avoid multiple detections for a single press
     	delay(500);

	 }//end while

   return 0;
} //END MAIN


void init_keypad() {
    // Configure column pins as outputs (initially HIGH)
    for (int c = 0; c < COLS; c++) {
        pinMode(colPins[c], OUTPUT);
        digitalWrite(colPins[c], HIGH); 
    }
    // Configure row pins as inputs with pull-ups
    for (int r = 0; r < ROWS; r++) {
        pinMode(rowPins[r], INPUT);
        // Enable internal pull-up resistors for rows
        pullUpDnControl(rowPins[r], PUD_UP); 
    }
}

char get_key() {
    for (int c = 0; c < COLS; c++) {
        // 1. Set the current column to LOW (active state)
        digitalWrite(colPins[c], LOW);

        // 2. Check all rows for a LOW signal
        for (int r = 0; r < ROWS; r++) {
            if (digitalRead(rowPins[r]) == LOW) {
                // 3. A key press is detected
                // Add a small delay for debouncing (e.g., 20ms) in a real application
                // while (digitalRead(rowPins[r]) == LOW); // Wait for key release (optional blocking)
		delay(20);
                // 4. Reset the column to HIGH before returning
                digitalWrite(colPins[c], HIGH);
                return keys[r][c]; // Return the character of the pressed key
            }
        }
        // 5. Deactivate the current column (set back to HIGH)
        digitalWrite(colPins[c], HIGH);
    }
    return '\0'; // Return null character if no key is pressed
}



void arm(){//arms device and sets timer
	int armCode = generateCode();

//generate 8 or 7 digit number
	printf("%d\n",armCode);
	printf("writing to LCD\n");
	lcdLoc(LINE1+0);
	typeInt(armCode);
}

int generateCode(){
	srand(time(NULL));
	int CSEggMin = 1;
	int CSEggMax = 20;
	//7355608
	int eggwuh = (rand() % (CSEggMax - CSEggMin + 1 ) + CSEggMin);
	int eggNum = 2;
	int min = 1000000;
	int max = 9999999;

	if(eggwuh != eggNum){
		printf("rolled %d/%d, generating random code\n", eggwuh, CSEggMax);
	return rand() % (max - min + 1) + min;
	}
	else if(eggwuh == eggNum){
		printf("it do come wit eggwuh\n");
	return 7355608;
	}
}

