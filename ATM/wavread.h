//
//  wavdata.h
//  Bakalářka
//
//  Created by Vojtěch Matoušek on 01.03.18.
//  Copyright © 2018 Vojtěch Matoušek. All rights reserved.
//

#ifndef wavdata_h
#define wavdata_h

#include<iostream>
#include<fstream>
#include<string>
using namespace std;

class wavread
{
    public:
    //promenne
    char type[5],type2[14];
    unsigned int size, ChunkSize;
    short audioFormat, channels;
    unsigned int sampleRate, byteRate;
    short blockAlign, bitsPerSample;
    unsigned int ListSize, ISFTSize, dataSize;
    short *data_l,*data_r;
    unsigned long samples;
    float *data;

    //konstruktory
    wavread():data(nullptr){sampleRate = 0;}
    
    //destruktor
    ~wavread();
    
    //ulozeni vzorku nahravky do souboru
    void SaveToFile(const char* fileName);
    
    //vypsani hlavicky wav souboru do konzole
    void WavHeaderOut();
    
    //nacteni hlavicky z wave
    void cteni(string nahravka);
};

#endif /* wavdata_h */
