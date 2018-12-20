//
//  midiwrite.h
//  Bakalářka
//
//  Created by Vojtěch Matoušek on 02.03.18.
//  Copyright © 2018 Vojtěch Matoušek. All rights reserved.
//

#ifndef midiwrite_h
#define midiwrite_h

#include<iostream>
#include<fstream>
#include<string>
#include "CQT.h"
#include "wavread.h"
using namespace std;

class midiwrite
{
public:
   
    //promenne
    char ChunkDescriptor[5] = "MThd";
    char ChunkHeader[5] = "MTrk";
    unsigned int ChunkSize_MThD = 6, ChunkSize_MTrK;
    short FormatType = 1, NumofTrucks = 1, TimeDivision = 20;
    double **noty_upravene,**noty_ladeni,**noty_nove;
    unsigned long *odchylka;
    int pocet_not_novych;
    string pom;
    
    //konstruktor - zpracovani hlavicky wave souboru a ulozeni do prislusnych promennych
    explicit midiwrite(const char* MIDIfile,CQT& nota,wavread& data);
    
    //prevod 16bit cisla z litle endian na big endian
    inline uint16_t Reverse16(uint16_t value);
    
    //prevod 32bit cisla z litle endian na big endian
    inline uint32_t Reverse32(uint32_t value);
    
    void uprava_not(CQT &nota,wavread &data);
    
    void ladeni(CQT &nota);
    
    void zapis_not();
};/* class midiwrite */

#endif /* midiwrite_h */
