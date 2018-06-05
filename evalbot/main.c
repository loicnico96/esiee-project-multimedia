#define TARGET_IS_FIRESTORM_RA2
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "drivers/io.h"
#include "drivers/motor.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"

#define EV_ACTION_IDLE		'0'
#define EV_ACTION_FORWARD	'8'
#define EV_ACTION_BACKWARD	'2'
#define EV_ACTION_LEFT		'4'
#define EV_ACTION_RIGHT	'6'
#define EV_ACTION_LEFT_FAST	'7'
#define EV_ACTION_RIGHT_FAST	'9'

// Action actuelle
int g_action; 
int g_running; 
int g_speed; 

// Change la vitesse de l'Evalbot (entre 0 et 100)
void EvChangeSpeed (int speed) {
	if (g_speed != speed) {
		g_speed = speed; 
		MotorSpeed (LEFT_SIDE, (speed << 8)); 
		MotorSpeed (RIGHT_SIDE, (speed << 8)); 
	}
}

// Ordonne l'Evalbot d'avancer
void EvRun () {
	if (g_running == false) {
		g_running = true; 
		MotorRun (LEFT_SIDE); 
		MotorRun (RIGHT_SIDE); 
		LED_On (LED_1); 
		LED_On (LED_2); 
	}
}

// Ordonne l'Evalbot de reculer
void EvStop () {
	if (g_running == true) {
		g_running = false; 
		MotorStop (LEFT_SIDE); 
		MotorStop (RIGHT_SIDE); 
		LED_Off (LED_1); 
		LED_Off (LED_2); 
	}
}

// Changement de direction
void EvCommand (int action) {
	if (g_action != action) {
		g_action = action; 
		switch (g_action) {
			case EV_ACTION_FORWARD: 
				MotorDir (LEFT_SIDE, FORWARD); 
				MotorDir (RIGHT_SIDE, FORWARD); 
				EvChangeSpeed (60); 
				EvRun (); 
				break; 
			case EV_ACTION_BACKWARD:
				MotorDir (LEFT_SIDE, REVERSE); 
				MotorDir (RIGHT_SIDE, REVERSE); 
				EvChangeSpeed (60); 
				EvRun (); 
				break; 
			case EV_ACTION_LEFT:
				MotorDir (LEFT_SIDE, REVERSE); 
				MotorDir (RIGHT_SIDE, FORWARD); 
				EvChangeSpeed (15); 
				EvRun (); 
				break; 
			case EV_ACTION_RIGHT:
				MotorDir (LEFT_SIDE, FORWARD); 
				MotorDir (RIGHT_SIDE, REVERSE); 
				EvChangeSpeed (15); 
				EvRun (); 
				break; 
			case EV_ACTION_LEFT_FAST:
				MotorDir (LEFT_SIDE, REVERSE); 
				MotorDir (RIGHT_SIDE, FORWARD); 
				EvChangeSpeed (30); 
				EvRun (); 
				break; 
			case EV_ACTION_RIGHT_FAST:
				MotorDir (LEFT_SIDE, FORWARD); 
				MotorDir (RIGHT_SIDE, REVERSE); 
				EvChangeSpeed (30); 
				EvRun (); 
				break; 
			default:
				EvStop (); 
		}
	}
}


// Clignotement des LEDs
void Timer0IntHandler () {
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT); 
	switch (g_action) {
		case EV_ACTION_BACKWARD:
			LED_Toggle (LED_1); 
			LED_Toggle (LED_2); 
			break; 
		case EV_ACTION_LEFT:
		case EV_ACTION_LEFT_FAST:
			LED_Toggle (LED_2); 
			break; 
		case EV_ACTION_RIGHT:
		case EV_ACTION_RIGHT_FAST:
			LED_Toggle (LED_1); 
			break; 
	}
}


int main () {
	int i; 
	char s[] = "Connexion etablie.\n"; 
	
	// Horloge système à 50 MHz
	SysCtlClockSet (SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); 
	
	// Initialisation du port UART1 (liaison RS232)
	SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB); 
	SysCtlPeripheralEnable (SYSCTL_PERIPH_UART1); 
	GPIOPinConfigure (GPIO_PB4_U1RX); 
	GPIOPinConfigure (GPIO_PB5_U1TX); 
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);
	UARTConfigSetExpClk (UART1_BASE, SysCtlClockGet (), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE)); 
	
	// Configuration du timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 8 - 1);
	ROM_IntMasterEnable();
	ROM_IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER0_BASE, TIMER_A);
	
	// Initialisation des moteurs et des LEDs
	MotorsInit(); 
	LEDsInit(); 
	
	// Initialisation des variables globales
	g_action = EV_ACTION_IDLE; 
	g_running = false; 
	g_speed = 0; 
	
	// Envoi d'une réponse à l'emetteur
	for (i = 0 ; s[i] != '\0' ; i++)
		UARTCharPut(UART1_BASE, s[i]); 
	
	// Boucle infinie
	while (1)
		EvCommand (UARTCharGet (UART1_BASE)); 
}
