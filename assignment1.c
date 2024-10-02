// Bhupinder Singh Assignment 1 ECE 355
//Part 1
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
#define CNTM (volatile unsigned int *) 0xFFFFFFD0 // timer inital value
#define CTCON (volatile unsigned char *) 0xFFFFFFD8 // timer control register 
#define CTSTAT (volatile unsigned char *) 0xFFFFFFD9 // timer status register
#define IVECT (volatile unsigned int *) (0x20) //ISR vector address


#define switchE BIT0
#define switchD BIT1

char countValue = 0;
char currentPortBVal;
void main(){
    unsigned int counter_value=100000000; // interrupt every second @ 100MHz
    *CTCON = BIT1; //stop timer if running
    *CTCON |= BIT7; //set in timer mode
    *CNTM = counter_value; //loading initial counter value
    //initialize port B (1 output 0 input)
    // set switches as inputs
    *PBDIR &= ~(switchE|switchD);
    // set 7 segment display output bits
    *PBDIR |= BIT7+BIT6+BIT5+BIT4;

    // set up interrupt vector
    *IVECT = (unsigned int) &intserv;
    //Initialize dispaly as 0;
    *PBOUT &= (BIT7|BIT6|BIT5|BIT4);

    //Enable interrupt PSR[6]
    asm("MoveControl PSR, #0x20");
    // Enable counter interrupt
    *CTCON |= BIT4;
    while(1){
        if(*PBIN & switchD != 0){
            //stop timer
            *CTCON |= BIT1; // set Stop bit
            *CTCON &= ~BIT0; // clear Start bit just in case
        }    
        if(*PBIN & switchE != 0){
            // This will start the counter from where it was left off
            // Assignment does not specify 
            //start timer
            *CTCON |= BIT0; // set Start bit
            *CTCON &= ~BIT1; // clear Stop bit just in case
        }
    }  

}

// interrupt for timer
interrupt void intserv()
{
    if(countValue == 9){
        countValue = 0;
    } else{
        countValue++;
    }
    // load value into counter
    //shift value by 4 bits
    countValue= countValue <<4;
    // save port B value so new value can be loaded in one go
    currentPortBVal = *PBOUT;
    currentPortBVal &= ~displayBits;
    currentPortBVal |= countValue;
    *PBOUT = currentPortBVal;  
}