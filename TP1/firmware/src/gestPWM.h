#ifndef GestPWM_H
#define GestPWM_H
/*--------------------------------------------------------*/
// GestPWM.h
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

#include <stdint.h>
#include"Mc32DriverAdc.h"
#include "app.h"
/*--------------------------------------------------------*/
// D�finition des fonctions prototypes
/*--------------------------------------------------------*/

#define TAILLE_FILTRE 10                        // Taille du filtre coulissant
#define RESOLUTION_ADC 1023                     // Resolution de l'ADC
#define VITESSE_MAX 198                         // Vitesse maximum 
#define ANGLE_MAX 180                           // Angle maximum

#define MAX_SERVO 0.0024                        // Temps max du servomoteur
#define MIN_SERVO 0.0006                        // Temps min du servomoteur


typedef struct {
    uint8_t absSpeed;    // vitesse 0 � 99
    uint8_t absAngle;    // Angle  0 � 180
    int8_t SpeedSetting; // consigne vitesse -99 � +99
    int8_t AngleSetting; // consigne angle  -90 � +90
} S_pwmSettings;

extern S_pwmSettings PWMData; 

void GPWM_Initialize(S_pwmSettings *pData);

// Ces 3 fonctions ont pour param�tre un pointeur sur la structure S_pwmSettings.
void GPWM_GetSettings(S_pwmSettings *pData);	// Obtention vitesse et angle
void GPWM_DispSettings(S_pwmSettings *pData);	// Affichage
void GPWM_ExecPWM(S_pwmSettings *pData);		// Execution PWM et gestion moteur.
void GPWM_ExecPWMSoft(S_pwmSettings *pData);		// Execution PWM software.


#endif
