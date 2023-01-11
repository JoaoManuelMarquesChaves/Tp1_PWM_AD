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
#include "Mc32DriverAdcAlt.h"
#include "peripheral/oc/plib_oc.h"
#include "app.h"

S_pwmSettings PWMData;      // pour les settings

void GPWM_Initialize(S_pwmSettings *pData)
{
   // Init les data 
    
   // Init état du pont en H
   BSP_EnableHbrige();
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
    static uint16_t Memory_Angle[10]={};
    static uint16_t Memory_Speed[10]={};
    static uint16_t Moyenne_Angle = 0;
    static uint16_t Moyenne_Speed = 0;
    static uint16_t i,i2=0;
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
    Moyenne_Speed =(Memory_Speed[0]+Memory_Speed[1]
            +Memory_Speed[2]+Memory_Speed[3]
            +Memory_Speed[4]+Memory_Speed[5]
            +Memory_Speed[6]+Memory_Speed[7]
            +Memory_Speed[8]+Memory_Speed[9])/10;
    
    Moyenne_Angle = (Memory_Angle[0]+Memory_Angle[1]
            +Memory_Angle[2]+Memory_Angle[3]
            +Memory_Angle[4]+Memory_Angle[5]
            +Memory_Angle[6]+Memory_Angle[7]
            +Memory_Angle[8]+Memory_Angle[9])/10;
    
    pData->absSpeed = ((Moyenne_Speed*198)/1023);
    pData->absAngle = ((Moyenne_Angle*180)/1023);

    // conversion 
    pData->SpeedSetting = pData->absSpeed -99;
    pData->AngleSetting = pData->absAngle -90;
    
    if(pData->SpeedSetting < 0 )
    {
        pData->absSpeed = 0-pData->SpeedSetting; 
    }
    else
    {
        pData->absSpeed = pData->SpeedSetting;
    }
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
 float Control_Data;
 static uint16_t Cpt_Speeed;
 static uint16_t Cpt_Angle; 
 // vitesse
 // gestion direction pont H
    // sens horaire
    if(pData->SpeedSetting <= 1)
    {
        AIN1_HBRIDGE_W = 1; 
        AIN2_HBRIDGE_W = 0; 
        STBY_HBRIDGE_W = 1; 
    }
    // sens anti horaire
    if (pData->SpeedSetting >= 1)
    {
        AIN1_HBRIDGE_W = 0; 
        AIN2_HBRIDGE_W = 1; 
        STBY_HBRIDGE_W = 1; 
    }
    // stop
    else 
    {
        STBY_HBRIDGE_W = 0;      
    }
    //calcul implusion por OC2
    Control_Data = pData->absSpeed; 
    if ( Control_Data != pData->absSpeed)
    {
        Cpt_Speeed ++;
    }
    
    PLIB_OC_PulseWidth16BitSet(Cpt_Speeed,pData->absSpeed);
    
// Angle
    //calcul implusion por OC3
    Control_Data = pData->AngleSetting; 
    if ( Control_Data != pData->AngleSetting)
    {
        Cpt_Angle ++;
    }

    PLIB_OC_PulseWidth16BitSet(Cpt_Angle,pData->AngleSetting);
}

// Execution PWM software
void GPWM_ExecPWMSoft(S_pwmSettings *pData)
{
    pData->absSpeed;
}
