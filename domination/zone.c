#include <stdio.h>
#include <i2c1602.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
/*
OVERVIEW
        GPIO 17 maintains the blue button, and should be set as a pull up resi>
        GPIO 27 maintains the red button, and should be set as a pull up resis>
        other pins may be needed to toggle light panels through relays. For fu>

        Each of these pins are wired to ground through the button.
        pressing the button causes the pin output to go low.
        when the pin output goes low, _____
        once pressed, the GPIO output needs to be disabled or set to low. (???)
       when a zone is captured, the only button that should be illuminated is>
*/

#define BLUEBUTTON 17
#define BLUELIGHT  23
#define REDBUTTON  27
#define REDLIGHT   24
#define LOCKOUT 5
#define CONTESTTIME 5



void blueCapture();  //sets the zone to be oriented to blue team
void redCapture();  //sets the zone to be oriented to red team
int blueContested(); //function that runs when the zone is captured by red team
                        //and begins being contested by blue


int redContested(); // function that runs when the zone is captured by blue te>
                        //and begins being contested by red

void dominationinit();



bool run = true;
bool blueCaptured = false;
bool redCaptured = false;
int blueContestResult = 1;
int redContestResult = 1;


int main(){
	wiringPiSetupPinType(WPI_PIN_BCM);
	dominationinit(); 
	while (run){
		while(blueCaptured == false && redCaptured == false){
			if(digitalRead(BLUEBUTTON) == LOW){
				printf("blue controls the zone!\n");
				blueCapture();
			}
			else if(digitalRead(REDBUTTON) == LOW){
				printf("red controls the zone\n");
				redCapture();
				}
		}//end init while
	while(blueCaptured == true && redCaptured == false){
	//the only button that is able to be depressed now is the red one, which should be active
	//for loop for scoring goes here
	
	//While the zone is controlled by blue, blueContestResult stores the returned value of blueContested(). This makes sure that blueContested() fully executes and doesn't wait to resolve
	//until we come back to the while loop. blueContestResult should be 1 if blue retains control of the zone (hits their button while flashing)
	//and should be 2 if blue fails to retain control. if any ContestResult variable is 3, then it prevents anything else from executing.


		if(digitalRead(REDBUTTON)==LOW){
			blueContestResult=blueContested();
			if(blueContestResult==1){
				blueCapture();
				blueContestResult = 3;
			}
			else if (blueContestResult == 2){
				redCapture();
				blueContestResult = 3;
			}
		}
	}
	while(redCaptured == true && blueCaptured == false){ 
		//logic to execute while red is captured
		if(digitalRead(BLUEBUTTON)==LOW){
                        redContestResult = redContested();
			 if(redContestResult==1){
                        	redContestResult=3;
                                redCapture();
			}
                        else if (redContestResult==2){
				redContestResult=3;
				blueCapture();
                        }

		}
	}
	if(redCaptured == true && blueCaptured == true){
	//CRITICAL ERROR- THIS SHOULD NEVER BE CALLED
	printf("ERROR: both teams have captured the zone, returning to neutral");
	dominationinit();
	}
	}//end while
}// end main


/*
dominationinit() is called when the device gets the signal to start a game of
domination.



*/

void dominationinit(){
	pinMode(BLUEBUTTON, INPUT);
	pullUpDnControl(BLUEBUTTON, PUD_UP); //default state of both pins is high, as
	//digitalWrite(BLUEBUTTON, HIGH);		//we need to detect when this is
					//connected to ground and goes low

	pinMode(REDBUTTON, INPUT);
	pullUpDnControl(REDBUTTON, PUD_UP);
	//digitalWrite(REDBUTTON, HIGH);

	pinMode(BLUELIGHT, OUTPUT);
	//pull up/down resistors are only necessary when a pin has a floating input value
	digitalWrite(BLUELIGHT, HIGH);//default init to on when round starts

	pinMode(REDLIGHT, OUTPUT);
	digitalWrite(REDLIGHT, HIGH);

}


void blueCapture(){ //sets the zone to be oriented to blue team
	//turn off both LED's
	blueCaptured = true;
	redCaptured = false; //this should be redundant but a saftey precaution
	digitalWrite(BLUELIGHT, LOW);
	digitalWrite(REDLIGHT, LOW);
	//should buttons be disabled?

	//for(int i = 0; i<LOCKOUT; i++){
	//	printf("Blue controls the zone- %d seconds remaining until contest available\n", LOCKOUT-i);
	//	delay(1000);
	//}
	//after <LOCKOUT> seconds, zone becomes available to contest
	digitalWrite(REDLIGHT, HIGH);
	//red light comes back on, signaling it is able to be pressed.

	//if using buttons that have integrated LED's, it will be necessary to 
	//re-enable those pins and/or lights
}

void redCapture(){ //sets the zone to be oriented to red team

        redCaptured = true;
	blueCaptured = false; //redundant
        digitalWrite(BLUELIGHT, LOW);
        digitalWrite(REDLIGHT, LOW);

        //should buttons be disabled?

        //for(int i = 0; i<LOCKOUT; i++){
        //        printf("red controls the zone- %d seconds remaining until contest available\n", LOCKOUT-i);
        //        delay(1000);
        //}
        //after <LOCKOUT> seconds, zone becomes available to contest
        digitalWrite(BLUELIGHT, HIGH);

}

int blueContested(){ //function that runs when the zone is captured by blue team
                        //and begins being contested by red
        //red button was hit, turn red light off
        digitalWrite(REDLIGHT, LOW); //turn off the red light
        //this will ideally be exectuted in a new process
	for(int i = 0; i<CONTESTTIME; i++){
		digitalWrite(BLUELIGHT, HIGH);
		delay(500);
		digitalWrite(BLUELIGHT, LOW);
		delay(500);
		printf("Red is attempting to capture the zone! %d seconds remaining\n", CONTESTTIME-i);
			if(digitalRead(BLUEBUTTON)== LOW){
				printf("zone has been recaptured by blue\n");
				return 1;
			}
	}//exit for
	//if the for loop terminates, then the time has run out and we can exit with the zone under red control
	printf("blue has failed to defend the zone and is now under red control\n");
        return 2;
}



int redContested(){ // function that runs when the zone is captured by red team
			//and begins being contested by blue

        digitalWrite(BLUELIGHT, LOW); //turn off the red light
        for(int i = 0; i<CONTESTTIME; i++){
                digitalWrite(REDLIGHT, HIGH);
                delay(500);
                digitalWrite(REDLIGHT, LOW);
                delay(500);
                printf("Blue is attempting to capture the zone! %d seconds remaining\n", CONTESTTIME-i);
                        if(digitalRead(REDBUTTON)== LOW){
                                printf("zone has been recaptured by red\n");
                                return 1;
                        }
        } //exit for
        printf("red has failed to defend the zone and is now under blue control\n");
        return 2;


}




