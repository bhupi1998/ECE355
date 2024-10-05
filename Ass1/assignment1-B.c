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

#define displayBits 0xF0

#define PBIN (volatile unsigned char *) 0xFFFFFFF3
#define PBOUT (volatile unsigned char * ) 0xFFFFFFF4
#define PBDIR (volatile unsigned char *) 0xFFFFFFF5
#define PCONT (volatile unsigned char *) 0xFFFFFFF7 //port control register
#define PSTAT (volatile unsigned char *) 0xFFFFFFF6 //status register
#define CNTM (volatile unsigned int *) 0xFFFFFFD0 // timer inital value
#define CTCON (volatile unsigned char *) 0xFFFFFFD8 // timer control register 
#define CTSTAT (volatile unsigned char *) 0xFFFFFFD9 // timer status register
#define IVECT (volatile unsigned int *) (0x20) //ISR vector address


#define switchE BIT0
#define switchD BIT1

interrupt void intserv();
// bit used to enable the increment
char countEnable = 0;

void main(){
    unsigned int counter_value=100000000; // interrupt every second @ 100MHz
    *CTCON = BIT1; //stop timer if running
    *CTCON &= ~BIT7; //set in counter mode
    *CNTM = counter_value; //loading initial counter value
    //initialize port B (1 output 0 input)
    // set switches as inputs
    *PBDIR &= ~(switchE|switchD);
    // set 7 segment display output bits
    *PBDIR |= displayBits;

    // set up interrupt vector
    *IVECT = (unsigned int) &intserv;
    //Initialize dispaly as 0;
    *PBOUT &= ~displayBits;

    //Enable interrupt PSR[6]
    asm("MoveControl PSR, #0x20");
    //Enable port B in interrupt
    *PCONT |= BIT6;

    char timerVal = 0;
    while(1){
        // if E was pressed last increment counter
        while(countEnable == 1){
            while(((*CTSTAT & BIT0) == 0) && countEnable){}; //wait for count to be done
                if(timerVal == 9){
                    timerVal = 0;
                }else{
                    timerVal++;
                }
                // update display
                timerVal= timerVal <<4;
                // save port B value so new value can be loaded in one go
                char currentPortBVal = *PBOUT;
                currentPortBVal &= ~displayBits;
                currentPortBVal |= timerVal;
                *PBOUT = currentPortBVal;  

                // Counter reloads automatically based on page 398 of textbook
        }
    // if switchD is pressed, countEnable will be 0 and the counter won't be polled    
    }  

}

// interrupt for timer
interrupt void intserv()
{
    if((*PBIN & switchE) != 0){
        countEnable = 1;
        // Enable timer
        *CTCON &= ~BIT1; //clear stop bit
        *CTCON |= BIT0; //enable start bit
    }else if((*PBIN & switchD) != 0){
        countEnable = 0;
        // Stop timer
        *CTCON |= BIT1; //enable stop bit
        // tecnically no need to do this.
        *CTCON &= ~BIT0; //clear start bit
    }
}