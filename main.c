#include <msp430g2553.h>
#include <intrinsics.h>

#define CPU_F 1100000
#define delaynms(x) __delay_cycles((long)((double)CPU_F*x/1000.0))
#define delaynus(x) __delay_cycles((long)((double)CPU_F*x/1000000.0))

#define ADDT BIT3
#define ADSK BIT4
#define AD_DIR P2DIR
#define AD_OUT P2OUT
#define AD_IN P2IN
#define ADSK_1 AD_OUT|=ADSK
#define ADSK_0 AD_OUT&=~ADSK
#define ADDO (AD_IN&ADDT)

#define WEIGHT_BASE 34844
unsigned char Data_str[16];


void HX711_Init(void){
	AD_DIR|=ADSK;
	AD_DIR&=~ADDT;
}

unsigned int ReadCount(void){
	unsigned long Count;
	unsigned int ADval;
	unsigned char i;
	ADSK_0;
	Count=0;
	while(ADDO);
	for(i=0;i<24;i++){
		ADSK_1;
		Count=Count<<1;
		ADSK_0;
		if(ADDO)Count++;
	}
	ADSK_1;
	Count=Count^0x800000;
	ADSK_0;
	ADval=(unsigned int)(Count>>8);
	return ADval;
}

unsigned long AD_val;
volatile float weight;


void main(void) {
	WDTCTL=WDTHOLD+WDTPW;
	HX711_Init();
	delaynms(1);
	delaynms(1);
	while(1){
		AD_val=ReadCount();

		// To calcolate the divisor you have to weight 1000 gramm and then you have to get the emase value of the HX711
		// divisor = [value of hx711 (0gramm) - value of hx711 (1000gramm)] / 1000
		static const double divisor = 5.959;

		weight = (WEIGHT_BASE - AD_val) / divisor;

		static int i = 0;
		i++;
	}
}

/* Testmessungen mit Euro Münzen
 *
 * 2Euro   => soll: 8.5g => ist: 28.06
 * 1Euro   => soll: 7.5g => ist: 24.4
 * 50 Cent => soll: 7g   => ist: 25.01
 * 20 Cent => soll: 5.7g => ist: 18.3
 * 10 Cent => soll: 4.1g => ist: 12.2
 * 5  Cent => soll: 3.9g => ist: 11.59
 * 2  Cent => soll: 3g   => ist: 8.54
 * 1  Cent => soll: 2.3g => ist: 6.1
 *
 */

