//
//  Header.h
//  Bakalářka
//
//  Created by Vojtěch Matoušek on 09.03.18.
//  Copyright © 2018 Vojtěch Matoušek. All rights reserved.
//

#ifndef Header_h
#define Header_h

#include<iostream>
#include<fstream>
#include<string>
#include<cmath>
#include "wavread.h"

using namespace std;
#define PI 3.1415926536
class CQT
{
public:
    
    //promenne
    double *Ak,*w,*Hanning,*fk;
    double fmax,fmin;
	unsigned int kmax;
    int bins;
    double zmena;
    double *x,**XCQ,**y,**z,*zz;
    unsigned *index;
    double **nota;
    unsigned int pocet_not,pocet_rytmus;
    //konstruktor
    CQT():Ak(nullptr),w(nullptr),fk(nullptr),x(nullptr),XCQ(nullptr),y(nullptr),z(nullptr),zz(nullptr),index(nullptr),nota(nullptr){zmena = 0; bins=0;}
    
    //destruktor
    ~CQT();
    
    void poleAk(double freq,unsigned int krok,wavread &data);    //vytvoreni pole Ak pro zadanou frekveci pres cely krok
    void poleFk();                                               //vytvoreni pole vsech zkoumanych frekvenci
    void windowHaming(unsigned int krok);                        //vytvoreni Hamingova okna pres cely krok
    void rectangular(unsigned int krok);                         //vytvoreni pravouhleho okna pres cely krok
    void poleHanning();                                          //vytvoreni Hanningova okna pres cely krok
    void poleXCQ(wavread &data);                                 //Konstantni Q transformace
    void vyhodnoceni(int krok_pocet,unsigned int krok,unsigned int fs);
    void rytmus(wavread &data);
};/* class CQT */

#endif /* Header_h */
