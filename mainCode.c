#include<p18f452.h>
#pragma config WDT = OFF

#define size 2
#define voltageMap 1.5
#define max 1023
#define RS_PIN PORTCbits.RC5
#define RW_PIN PORTCbits.RC6
#define EN_PIN	PORTCbits.RC7


typedef union tag
{
	unsigned int result;
	unsigned char bytes[size];
}typePunning;

typedef struct temp
{
	unsigned char oldValue;
	unsigned char newValue;
}tempeRature;


void acquisitionTime(void);
void convertingAdc(void);
void compareTempPwm(unsigned char temp);
void commandInstruction(void);
void dataInstruction(void);
void delay450_us(void);
void delay250_ms(void);
void displayResult(unsigned char temp);
void displayString(void);
void busyFlag(void);
void lcdInitialization(void);
void printLcd(unsigned char value);
void clearDdramAddress(unsigned char dilimiter);
void firstAdress(void);
void addressOfTemp(void);
void addressOfMotor(void);
void speedValue(unsigned char flag);

#pragma interrupt interruptServiceRoutine
void interruptServiceRoutine(void)
{
	if(PIR1bits.ADIF)
	{
		PIR1bits.ADIF = 0;
		convertingAdc();
		acquisitionTime();
		ADCON0bits.GO = 1;
	}
	else if(PIR1bits.TMR2IF)
	{	
		PIR1bits.TMR2IF = 0;
		TMR2 = 0;
	}
}


#pragma code interruptAdress = 0x00008
void interruptAdress(void)
{
	_asm
		GOTO interruptServiceRoutine
	_endasm
}


void main(void)
{
	TRISAbits.TRISA5 = 1;
	TRISD = 0x00;
	TRISC = 0x1B;
	INTCONbits.GIE = 1;
	INTCONbits.PEIE = 1;
	PIE1bits.ADIE = 1;
	PIR1bits.ADIF = 0;
	PIE1bits.TMR2IE = 1;
	PIR1bits.TMR2IF = 0;
	ADCON0 = 0x61;
	ADCON1 = 0x8A;
	PR2 = 187;
	T2CON = 0x03;
	TMR2 = 0;	
	CCPR1L = 0;
	lcdInitialization();
	displayString();
	acquisitionTime();
	ADCON0bits.GO = 1;
	while(1);
}


void acquisitionTime(void)
{
	T0CON = 0x48;
	TMR0L = 0xEC;
	INTCONbits.TMR0IF = 0;
	T0CONbits.TMR0ON = 1;
	while(!INTCONbits.TMR0IF);
	INTCONbits.TMR0IF = 0;
	T0CONbits.TMR0ON = 0;
}
void convertingAdc(void)
{	
	static unsigned char state = 0;
	typePunning adcValue;
	static tempeRature tempValue;
	adcValue.bytes[0] = ADRESL;
	adcValue.bytes[1] = ADRESH;
	if(!state)
	{
		state = 1;
		tempValue.oldValue = ((adcValue.result*voltageMap)/max)*100;
		displayResult(tempValue.oldValue);
		compareTempPwm(tempValue.oldValue);
	}
	else
	{
		tempValue.newValue = ((adcValue.result*voltageMap)/max)*100;
		if(tempValue.newValue != tempValue.oldValue)
		{
			tempValue.oldValue = tempValue.newValue;
			addressOfTemp();
			clearDdramAddress(11);
			displayResult(tempValue.oldValue);
			compareTempPwm(tempValue.oldValue);
		}
	}
}
void addressOfTemp(void)
{
	LATD = 0x85;
	commandInstruction();
	busyFlag();
}
void addressOfMotor(void)
{
	LATD = 0xC6;
	commandInstruction();
	busyFlag();
}
void firstAdress(void)
{
	LATD = 0x80;
	commandInstruction();
	busyFlag();
}
void clearDdramAddress(unsigned char dilimiter)
{
	unsigned char i = 0;
	while(i < dilimiter)
	{
		LATD = 0x20;
		dataInstruction();
		busyFlag();
		++i;
	}	
}
void lcdInitialization(void)
{
	delay250_ms();
	LATD = 0x38;
	commandInstruction();
	delay250_ms();
	LATD = 0x01;
	commandInstruction();
	delay250_ms();
	LATD = 0x0C;
	commandInstruction();
	delay250_ms();
}
void commandInstruction(void)
{		
	RS_PIN = 0;
	RW_PIN = 0;
	EN_PIN = 1;	
	delay450_us();
	EN_PIN = 0;
}
void dataInstruction(void)
{
	RS_PIN = 1;
	RW_PIN = 0;
	EN_PIN = 1;	
	delay450_us();
	EN_PIN = 0;
}
void delay450_us(void)
{
	T0CON = 0x40;
	TMR0L = 31;
	INTCONbits.TMR0IF = 0;
	T0CONbits.TMR0ON = 1;
	while(!INTCONbits.TMR0IF);
	INTCONbits.TMR0IF = 0;
	T0CONbits.TMR0ON = 0;
}
void delay250_ms(void)
{
	T0CON = 0x01;
	TMR0H = 0x0B;
	TMR0L = 0xDC;
	INTCONbits.TMR0IF = 0;
	T0CONbits.TMR0ON = 1;
	while(!INTCONbits.TMR0IF);
	INTCONbits.TMR0IF = 0;
	T0CONbits.TMR0ON = 0;
}
void displayResult(unsigned char temp)
{
	unsigned char divide = 10;
	signed char TOP = -1;
	unsigned char stack[15];
	addressOfTemp();
	stack[++TOP] = 'S';
	stack[++TOP] = 'U';
	stack[++TOP] = 'I';
	stack[++TOP] = 'S';
	stack[++TOP] = 'L';
	stack[++TOP] = 'E';
	stack[++TOP] = 'C';
	stack[++TOP] = ' ';
	while(temp != 0)
	{
		stack[++TOP] = temp%10+0x30;
		temp/=10;	
	}
	while(TOP != -1)
		printLcd(stack[TOP--]);
	firstAdress();
}

void displayString(void)
{
	unsigned char str[] = "TEMP ", i = 0, motor[] = "SPEED ";
	while(str[i] != '\0')
		printLcd(str[i++]);
	i = 0;
	LATD = 0xC0;
	commandInstruction();
	busyFlag();
	while(motor[i] != '\0')
		printLcd(motor[i++]);
	firstAdress();
}
void printLcd(unsigned char value)
{
	LATD = value;
	dataInstruction();		
	busyFlag();
}
void busyFlag(void)
{
	TRISDbits.TRISD7 = 1;
	RS_PIN = 0;
	RW_PIN = 1;
	while(1)	
	{
		EN_PIN = 0;
		delay450_us();
		EN_PIN = 1;
		if(!PORTDbits.RD7)	
		{
			TRISDbits.TRISD7 = 0;
			EN_PIN = 0;
			break;
		}	
	}
}
void compareTempPwm(unsigned char temp)
{
	if(temp > 40 && temp <= 150)
	{
		CCP1CON = 0x0E;	
		CCPR1L = 180;
		T2CONbits.TMR2ON = 1;
		speedValue(0);
	}
	else if(temp > 25 && temp <= 40)
	{
		CCP1CON = 0x0E;
		CCPR1L = 100;
		T2CONbits.TMR2ON = 1;
		speedValue(1);
	}
	else
	{
		CCPR1L = 0;
		T2CONbits.TMR2ON = 0;
		CCP1CON = 0;
		speedValue(2);
	}
	firstAdress();
}
void speedValue(unsigned char flag)
{
	unsigned char i = 0, low[] = "LOW", high[] = "HIGH", medium[] = "MEDIUM";
	addressOfMotor();
	clearDdramAddress(7);
	addressOfMotor();
	if(!flag)
	{	
		while(high[i] != '\0')
			printLcd(high[i++]);
	}
	else if(flag == 1)
	{
		while(medium[i] != '\0')
			printLcd(medium[i++]);
	}
	else
	{
		while(low[i] != '\0')
			printLcd(low[i++]);
	}
}