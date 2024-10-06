// Bhupinder Singh Assignment 1 ECE 355
//Part 2
#define BIT0 0x0
#define BIT1 0x1
#define BIT2 0x2
#define BIT3 0x4
#define BIT4 0x8
#define BIT5 0x10
#define BIT6 0x20
#define BIT7 0x40


// IO ports
#define PCONT (volatile unsigned char *) 0xFFFFFFF7
// Port A
#define PAIN (volatile unsigned char *) 0xFFFFFFF0
#define PAOUT (volatile unsigned char * ) 0xFFFFFFF1
#define PADIR (volatile unsigned char *) 0xFFFFFFF2
// Port B
#define PBIN (volatile unsigned char *) 0xFFFFFFF3
#define PBOUT (volatile unsigned char * ) 0xFFFFFFF4
#define PBDIR (volatile unsigned char *) 0xFFFFFFF5

// timer 
#define CNTM (volatile unsigned int *) 0xFFFFFFD0 // timer inital value
#define CTCON (volatile unsigned char *) 0xFFFFFFD8 // timer control register 
#define CTSTAT (volatile unsigned char *) 0xFFFFFFD9 // timer status register
#define IVECT (volatile unsigned int *) (0x20) //ISR vector address


#define SWITCH BIT7
#define DIGIT1 0x78 // PORT A
#define DIGIT2 0xF0 // PORT B
#define LED1 BIT2
#define LED2 BIT0

char countValue1 = 0;
char countValue2 = 0;
char currentPortBVal;
char currentPortAVal;


interrupt void intserv();
// bit used to enable the increment
char countEnable = 0;
char switchPressed = 0;
void main(){
    unsigned int counter_value=100000000; // interrupt every second @ 100MHz
    *CTCON = BIT1; //stop timer if running
    *CTCON &= ~BIT7; //set in counter mode
    *CNTM = counter_value; //loading initial counter value

    //initialize port B (1 output 0 input)
    // set port B outputs
    *PBDIR |= DIGIT2+LED1+LED2;

    // set Port A outputs bits
    *PADIR |= DIGIT1;
    // set Port A input
    *PADIR &= ~(SWITCH);

    // set up interrupt vector
    *IVECT = (unsigned int) &intserv;

    //Initialize displays as 0;
    *PBOUT &= ~DIGIT2;
    *PAOUT &= ~DIGIT1;

    //Initialize LEDs
    *PBOUT |= LED1;
    *PBOUT &= ~LED2;

    //Enable interrupt PSR[6]
    asm("MoveControl PSR, #0x20");
    //Enable port A in interrupt
    *PCONT |= BIT4;

    // Enable timer
    *CTCON &= ~BIT1; //clear stop bit
    *CTCON |= BIT0; //enable start bit
    char timerVal = 0;
    while(1){
        while(((*CTSTAT & BIT0) == 0)){}; //wait for count to be done
            // if LED1 is on, increment digit1
            if((*PBOUT & LED1) == 1){
                countValue1++;
                countValue1 = countValue1 >= 10 ? 0 : countValue1;
                char temp;
                temp = countValue1 << 3;
                currentPortAVal = *PAOUT;
                currentPortAVal &= ~DIGIT1;
                currentPortAVal |= temp;
                *PAOUT = currentPortAVal; 
            }
            // if LED2 is on, increment digit2
            if((*PBOUT & LED2) == 1){
                countValue2++;
                countValue2 = countValue2 >= 10 ? 0 : countValue2;
                char temp;
                temp = countValue2 << 4;
                currentPortBVal = *PBOUT;
                currentPortBVal &= ~DIGIT2;
                currentPortBVal |= temp;
                *PBOUT = currentPortBVal; 
            }
            // Counter reloads automatically based on page 398 of textbook

    // if switchD is pressed, countEnable will be 0 and the counter won't be polled    
    }  

}

// interrupt for timer
interrupt void intserv()
{
    // if switch is pressed 
    if(((*PAIN & SWITCH) == 0) && ~switchPressed){
        switchPressed = 1;
    }else if(((*PAIN & SWITCH) != 0) && switchPressed){
        switchPressed = 0;
        char temp;
        // LED1 is on
            if(*PAOUT & LED1 != 0){ //meaning LED1 is on
                temp = *PAOUT;
                //Turn LED2 on and LED1 off
                temp |= LED2; //turn led 2 on
                temp &= ~LED1; //turn led 1 off
                *PAOUT = temp; // update PAOUT with new LED config 
            }else{
                temp = *PAOUT;
                //Turn LED1 on and LED2 off
                temp |= LED1; //turn led 1 on
                temp &= ~LED2; //turn led 2 off
                *PAOUT = temp; // update PAOUT with new LED config 
            }
    }
}