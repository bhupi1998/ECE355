// Bhupinder Singh Assignment 2 ECE 355
//Part 1
#define BIT0 0x0
#define BIT1 0x1
#define BIT2 0x2
#define BIT3 0x4
#define BIT4 0x8
#define BIT5 0x10
#define BIT6 0x20
#define BIT7 0x40

#define switch BIT0
#define displayBitsB 0xF0 // PortB
#define displayBitsA 0x0F // PortA
#define LED1 BIT6
#define LED2 BIT5

// IO ports
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



char countValueA = 0;
char countValueB = 0;
char currentPortBVal;
char currentPortAVal;
void main(){
    unsigned int counter_value=100000000; // interrupt every second @ 100MHz
    *CTCON = BIT1; //stop timer if running
    *CTCON |= BIT7; //set in timer mode
    *CNTM = counter_value; //loading initial counter value
    //initialize port B (1 output 0 input)

    // set PB10 as input
    *PBDIR &= ~switch;
    
    // set 7 segment display output bits
    *PBDIR |= displayBitsB;
    *PADIR |= displayBitsA;
    // set PA5 and PA6 as led outputs
    *PADIR |= LED1|LED2;
    // Set Leds to intial state
    *PAOUT |= LED1; //led 1 is initially on
    *PAOUT &= ~LED2; // ensuring led is off
    
    // set up interrupt vector
    *IVECT = (unsigned int) &intserv;
    //Initialize display as 0;
    *PBOUT &= ~0xF0;
    *PAOUT &= ~0x0F;

    //Enable interrupt PSR[6]
    asm("MoveControl PSR, #0x20");
    // Enable counter interrupt
    *CTCON |= BIT4;

    // Enable timer
    *CTCON &= ~BIT1; //clear stop bit
    *CTCON |= BIT0; //enable start bit
    while(1){
        // PBIN will read a 0 when on
        if(~*PBIN & switch != 0){
            // waiting for release
            while(~*PBIN & switch != 0);
            // temporary variable to ensure LED are never on or off at the same time.
            char temp = *PAOUT;
            // check if LED1 is on
            if(*PAOUT & LED1 != 0){ //meaning LED1 is on
                //Turn LED2 on and LED1 off
                temp |= LED2; //turn led 2 on
                temp &= ~LED1; //turn led 1 off
                *PAOUT = temp; // update PAOUT with new LED config 
            }else{
                //Turn LED1 on and LED2 off
                temp |= LED1; //turn led 1 on
                temp &= ~LED2; //turn led 2 off
                *PAOUT = temp; // update PAOUT with new LED config 
            }
        }    
    }  

}

// interrupt for timer
interrupt void intserv()
{
    // if led 1 is on, digit 1 is decremented
    if(*PAOUT & LED1 != 0){
        // if value is 0, set it to 10
        countValueA = countValueA >= 0 ? 10 : countValueA;
        countValueA--;
        // save port B value so new value can be loaded in one go
        currentPortAVal = *PAOUT;
        // clear current value
        currentPortAVal &= ~displayBits;
        // update with new values and upload to port B
        currentPortAVal |= countValueB;
        *PBOUT = currentPortAVal;  
    }
    else if(*PAOUT & LED2 != 0){ // decrement digit 2
        countValueB = countValueB >= 0 ? 10 : countValueB;
        countValueB--;

        // load value into counter
        //shift value by 4 bits
        countValueB= countValueB << 4;
        // save port B value so new value can be loaded in one go
        currentPortBVal = *PBOUT;
        // clear current value
        currentPortBVal &= ~displayBitsB;
        // update with new values and upload to port B
        currentPortBVal |= countValueB;
        *PBOUT = currentPortBVal;  
    }
}