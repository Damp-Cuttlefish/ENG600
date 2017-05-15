#ifndef __33FJ128MC802_H
#include "xc.h"
#endif

#include "misc.h"


void init_setup( void )
{
    
    
    /*--------------Configure Ports--------------*/
    TRISBbits.TRISB2 = 1;   //QEI1A in
    TRISBbits.TRISB3 = 1;   //QEI1B in
    
    TRISBbits.TRISB15 = 1;  //QEI2A in
    TRISBbits.TRISB5 = 1;   //QEI2B in
    
    
    TRISBbits.TRISB14 = 0;  //PWM1H1 out
    TRISBbits.TRISB12 = 0;  //PWM1H2 out
    
    TRISAbits.TRISA0 = 0;   //Microstick LED pin
    
    
    
    TRISBbits.TRISB7 = 1;   //Button 1 CN23
    TRISBbits.TRISB6 = 0;   //Motor_Enable  CN24
    Motor_Enable = 0;
    
    CNPU2bits.CN23PUE = 1;  // Enable Internal Pullups on button pins
    //CNPU2bits.CN24PUE = 1;
    
    TRISBbits.TRISB13 = 0;  // L/R 
    TRISBbits.TRISB4 = 0;  // RST
    
    

    AD1PCFGL = 0xFFFF; //Disable analogue pins
    
    // Configure i2c pins as open drain
    ODCBbits.ODCB8 = 1; //SCL
    ODCBbits.ODCB9 = 1; //SDA
    
    

    LEDPin = 1;
    
    
    /*--------------Map Peripherals--------------*/
    //  -TRISx must match mode selected in PPS
    //  -Outputs of remappables selected by pin registers (RPnR<4:0>)
    //  -Inputs to remappables selected by peripheral register (QEA1R, INT1R, etc)
    //  -IOLOCK must be disabled before writes to config registers
    //  -Cont. State Monitoring shadows config registers. Any change whilst locked triggers reset
    //  -Config bit IOL1WAY (set by default) allows only one unlock of IOLOCK per reset    
    
    __builtin_write_OSCCONL(OSCCON & 0xDF);         // Unlock IO config
    // Assign QEI1 phase A and B to RP2 and RP3
    RPINR14bits.QEA1R = 0b0010;
    RPINR14bits.QEB1R = 0b0011;
    //Assign QEI2 Phase A and B to RP15 and RP5
    RPINR16bits.QEA2R = 0b1111;
    RPINR16bits.QEB2R = 0b0101;
    
    // Relock the Pin assignment registers
     __builtin_write_OSCCONL(OSCCON | 0x40);        // Relock IO config
    
    /*--------------Initialise PWM--------------*/
    //PWM1
    P1TCON = 0b1000000000000000; 	//Enable
    PWM1CON1 = 0b0000111100110000; 	//Enable 1H and 2H, all independent
    P1TPER = 900; //Approximately 3kHz
	/*Default oscillator configuration is used, so clock is sourced from the internal
	 *fast RC oscillator with PLL enabled to give Fosc=8MHz.
	 *Processor/timer clock is Fosc/2 ~= 4MHz */

    P1DC1 = 900;    //PWM1 Generator 1 Duty Cycle
    P1DC2 = 900;    //PWM1 Generator 2 Duty Cycle
    
    
    /*--------------Init QEI--------------*/
    QEI1CONbits.QEIM = 0b111; //4x mode no index reset
    QEI1CONbits.PCDOUT = 0;
    
    QEI2CONbits.QEIM = 0b111; //4x mode no index reset
    QEI2CONbits.PCDOUT = 0;
    /* 15   - Count error flag, 1 if error detected
     * 14   - null
     * 13   - If 1, stop when device idle
     * 12   - Index pin status
     * 11   - Direction status, 1=up 0=down
     * 10-8 - QIEM, QEI Mode select
     *         111 = Enabled (x4 mode) with reset by match (MAXxCNT)
     *         110 = Enabled (x4 mode) with index pulse reset
     *         101 = Enabled (x2 mode) with reset by match (MAXxCNT)
     *         100 = Enabled (x2 mode) with index pulse reset
     *         01x = Unused (module disabled)
     *         001 = Starts 16-bit Timer
     *         000 = QEI/Timer off
     * 7    - If 1, Phase A and B inputs are swapped
     * 6    - If 1, UP/DOWN is output to pin
     * 5-3  - Unused in QEI mode, timer only
     * 2    - If 1, index pulse resets count (If in mode 1x0)
     * 1-0  - Unused in QEI mode, timer only
     */

    
    DFLT1CON = 0b0000000000100000; //Filter control register
    DFLT2CON = 0b0000000000100000; //Filter control register

    /* 15-11 - null
     * 10-9  - Index Match Value (Not using Index pin)
     * 8     - If 1, count error interrupts are disabled
     * 7     - If 1, inputs are digitally filtered. Else no filtering.
     * 6-4   - Filter clock div select
     *          111 = 1:156
     *          110 = 1:128
     *             <...>
     *          000 = 1:1
     * 3-0   - null
     *
     */
   
    POS1CNT = 4000;                   //Initial position
    MAX1CNT = 10000;               //Counts per rev
    
    POS2CNT = 4000;                   //Initial position
    MAX2CNT = 10000;               //Counts per rev
    
     /*------------Start I2C------------*/    
    //SDA and SCL are tri stated on init, TRISx is overridden by direction control
    // Must be configured as open drain in pin setup (ODCx)
    //  SDA => RB9
    //  SCL => RB8
    
    //I2C1CON = 0b1000000000000000; //I2C1 control register    
    /* 15   - Enable
     * 14   - null
     * 13   - Stop in idle mode
     * 12   - Clock release in slave mode
     * 11   - IPMI enable
     * 10   - If 1, 10 bit slave address, else 7 bit
     * 9    - Disable slew rate control
     * 8    - If 1, use SMbus input levels. Else normal IO levels.
     * 7    - General call enable (Slave mode only)
     * 6    - Clock stretch, slave mode only
     * 5    - ACK Data bit. 1=NACK (For requesting final data), 0=ACK
     * 4    - Acknowledge sequence enable
     * 3    - Receive sequence enable (In master mode)
     * 2    - Stop condition enable
     * 1    - Repeated start condition enable bit
     * 0    - Start condition enable bit
     */
    I2C1CONbits.I2CSIDL = 0;    // Don't stop in idle mode
    I2C1CONbits.SCLREL=1;       // Release clock in slave mode
    I2C1CONbits.IPMIEN = 0;     // Disable IPMI
    I2C1CONbits.A10M = 0;       // use 7 bit slave address
    I2C1CONbits.DISSLW = 0;     // Slew rate control enabled???
    I2C1CONbits.SMEN = 0;       // Don't use SMBus thresholds
    I2C1CONbits.GCEN = 0;       // No general call support (Slave mode only)
    I2C1CONbits.STREN = 0;      // No clock stretching (Slave only)
    IEC1bits.MI2C1IE = 0;       //Disable I2C interrupts
    
   
    
    
    /*  In a 7-Bit Addressing mode, each node using the I2C protocol should be configured
        with a unique address that is stored in the I2CxADD register.
        While transmitting the address byte, the master must shift the address bits<7:0>,
        left by 1 bit, and configure bit 0 as the R/W bit.
     * 
     * 0xB0 >> 1 is the 7 bit address, 0xB0w/0xB1r the 8 bit
     */
    
    I2C1ADD = master_addr;
    
    
    I2C1MSK = 0;
    /*15-10     null
     *9-0
     * For 10-Bit Address:
        1 = Enables masking for bit Ax of the incoming message address; bit match is not required in this position
        0 = Disables masking for bit Ax; bit match is required in this position
       For 7-Bit Address (I2CxMSK<6:0> only):
        1 = Enables masking for bit Ax + 1 of the incoming message address; bit match is not required in this position
        0 = Disables masking for bit Ax + 1; bit match is required in this position
     */
    
    //No ISRCCON
    
    //Baud rate config
    I2C1BRG = 35; // Recalculate using formula in i2c manual 98
    /*Fosc/2 ~= 4MHz
     * 
     */
    
    I2C1RCV = 0x0000; //Clear TX/RX Buffers
	I2C1TRN = 0x0000;
    
    I2C1STATbits.RBF = 0;
    
    I2C1CONbits.I2CEN = 1;      //Enable
    
    /*------------Set up Timer Interrupt------------*/
    //Prescaler count is cleared on any timer config change or reset
    T1CON = 0b0000000000000000; //Timer mode, 1:8 prescaler
    /* 15   Run
     * 14   null
     * 13   Stop when idle
     * 12-7 null
     * 6    When TCS = 0:
     *          1 = Gated time accumulation enabled
     *          0 = Gated time accumulation disabled
     * 5-4  Prescaler
     *          11 = 1:256
     *          10 = 1:64
     *          01 = 1:8
     *          00 = 1:1
     * 3    null
     * 2    External clock sync (Unused when TCS = 0)
     * 1    TCS - Timer clock select
     *          1 = Ext clock from TxCK pin
     *          0 = Int clock (Fosc/2)
     * 0    null
     */
    //Fosc/2 ~= 4MHz
    
    TMR1 = 0; 	//Timer count register
    PR1 = 3688;	//Timer period register, compared against TMR1
    
    IPC0bits.T1IP = 0x01;   //Interrupt priority level 1
    IFS0bits.T1IF = 0;      //Clear timer 1 interrupt flag
    IEC0bits.T1IE = 1;      //Enable timer 1 interrupt
    
    T1CONbits.TON = 1;      //Start timer
    

}