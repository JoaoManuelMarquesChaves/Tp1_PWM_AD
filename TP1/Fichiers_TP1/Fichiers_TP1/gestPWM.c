/*--------------------------------------------------------*/
// GestPWM.c
/*--------------------------------------------------------*/
//	Description :	Gestion des PWM 
//			        pour TP1 2016-2017
//
//	Auteur 		: 	C. HUBER
//
//	Version		:	V1.1
//	Compilateur	:	XC32 V1.42 + Harmony 1.08
//
/*--------------------------------------------------------*/



#include "GestPWM.h"
#include "Mc32DriverAdc.h"
#include "peripheral/oc/plib_oc.h"

S_pwmSettings PWMData;      // pour les settings

void GPWM_Initialize(S_pwmSettings *pData)
{
   // Init les data 
    
   // Init état du pont en H
    
   // lance les timers et OC
   DRV_OC0_Start();
   DRV_OC1_Start();
   DRV_TMR0_Start();
   DRV_TMR1_Start();
   DRV_TMR2_Start();
   DRV_TMR3_Start();
    
}

// Obtention vitesse et angle (mise a jour des 4 champs de la structure)
void GPWM_GetSettings(S_pwmSettings *pData )	
{
    static uint8_t Memory_Angle[10]={};
    static uint8_t Memory_Speed[10]={};
    static uint8_t Moyenne_Angle = 0;
    static uint8_t Moyenne_Speed = 0;
    static uint8_t i,i2=0;
    S_ADCResults ReadData;
    // Lecture du convertisseur AD
    ReadData = BSP_ReadAllADC();
    Memory_Speed[i]= ReadData.Chan0;
    Memory_Angle[i]= ReadData.Chan1;
    i++;
    // valeur moyenne glisante
    if(i>= 10)
    {
        i=0;
    }
    for ( i2 = 0; i2 > 9; i2++ )
    {
        Moyenne_Angle = Moyenne_Angle+Memory_Angle[i2];
        Moyenne_Speed = Moyenne_Speed+Memory_Speed[i2];
    }
    pData->absSpeed = (Moyenne_Speed/10)*(198/1023);
    pData->absAngle = (Moyenne_Angle/10) *(180/1023);
    // conversion
    pData->SpeedSetting = pData->absSpeed -99;
    pData->AngleSetting = pData->absAngle -90;
}

// Affichage des information en exploitant la structure
void GPWM_DispSettings(S_pwmSettings *pData)
{
    lcd_gotoxy(1,1 );
    printf_lcd("Tp1 PWM 2022-23");
    lcd_gotoxy(1,2 );      // ecrire sur la deuxieme ligne
    printf_lcd("SpeedSetting %3d",pData->SpeedSetting);
    lcd_gotoxy(1,3 );      // ecrire sur la deuxieme ligne
    printf_lcd("absSpeed %2d",pData->absSpeed);
    lcd_gotoxy(1,4 );      // ecrire sur la deuxieme ligne
    printf_lcd("Angle %3d",pData->AngleSetting);
}

// Execution PWM et gestion moteur à partir des info dans structure
void GPWM_ExecPWM(S_pwmSettings *pData)
{
   
}

// Execution PWM software
void GPWM_ExecPWMSoft(S_pwmSettings *pData)
{
    
}


