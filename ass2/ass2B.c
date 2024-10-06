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

char countValueA = 0;
char countValueB = 0;
char currentPortBVal;
char currentPortAVal;


interrupt void intserv();
// bit used to enable the increment
char countEnable = 0;

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

            }
            // if LED2 is on, increment digit2
            if((*PBOUT & LED2) == 1){

            }
            // Counter reloads automatically based on page 398 of textbook

    // if switchD is pressed, countEnable will be 0 and the counter won't be polled    
    }  

}

// interrupt for timer
interrupt void intserv()
{
    if((*PAIN & SWITCH) != 0){

}