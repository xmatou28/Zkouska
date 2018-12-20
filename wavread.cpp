//
//  openwave.cpp
//  Bakalářka
//
//  Created by Vojtěch Matoušek on 01.03.18.
//  Copyright © 2018 Vojtěch Matoušek. All rights reserved.
//
#include "wavread.h"

void wavread::cteni(string nahravka)
{
    FILE *fp = NULL;
    const char *nahravka1 = nahravka.c_str();
    fp = fopen(nahravka1, "rb");
    if(!fp)
        throw runtime_error("Soubor se vstupní nahrávkou se nepodarilo otevrit");

    
    fread(type, sizeof(char), 4, fp);
    type[4] = '\0';
    if((strcmp(type, "RIFF")) != 0)
        cout << "Error: RIFF" << endl;
    
    
    fread(&size, sizeof(unsigned int), 1,fp);
    
    fread(type, sizeof(char), 4,fp);
    type[4] = '\0';
    if(strcmp(type, "WAVE") != 0 )
        throw runtime_error("Nejedna se o soubor WAVE");
    
    char c;
    unsigned pom1=0,pom2=0,pom3=0,pom4=0;
    do {
        fread(&c, sizeof(char), 1,fp);
        if (c == 'f') {
            pom1=1;
        }
        if (c == 'm' && pom1 == 1) {
            pom2=1;
        }
        if (c == 't' && pom1 == 1 && pom2 ==1) {
            pom3=1;
        }
        if (c == ' ' && pom1 == 1 && pom2 ==1 && pom3 ==1) {
            pom4=1;
        }
    } while (pom4==0);
   
    
    fread(&ChunkSize, sizeof(unsigned int), 1,fp);
    fread(&audioFormat, sizeof(short), 1, fp);
    if(audioFormat != 1)
        throw runtime_error("Soubor WAVE musi byt nekomprimovany");
        
    fread(&channels, sizeof(short), 1, fp);
    fread(&sampleRate, sizeof(unsigned int), 1, fp);
    fread(&byteRate, sizeof(unsigned int), 1, fp);
    fread(&blockAlign, sizeof(short), 1, fp);
    fread(&bitsPerSample, sizeof(short), 1, fp);
    
    pom1 = 0;
    pom2 = 0;
    pom3 = 0;
    pom4 = 0;
    do {
        fread(&c, sizeof(char), 1,fp);
        if (c == 'd') {
            pom1=1;
        }
        if (c == 'a' && pom1 == 1) {
            pom2=1;
        }
        if (c == 't' && pom1 == 1 && pom2 ==1) {
            pom3=1;
        }
        if (c == 'a' && pom1 == 1 && pom2 ==1 && pom3 ==1) {
            pom4=1;
        }
    } while (pom4==0);
    

    fread(&dataSize, sizeof(unsigned int), 1, fp);
    
    unsigned long size_data;
    size_data = dataSize/sizeof(float);
    data = new float[dataSize];
    data_l = new short[dataSize];
    data_r = new short[dataSize];
    unsigned j=0,k=0,l=0;
    if(channels == 2)
    {
        unsigned pom = 0;
        for (unsigned i=0; i<size_data; ++i)
        {
            if (i % 2==0)
            {
                fread(&data[j], sizeof(short), 1,fp);
                ++j;
            }
            else
            {
                fread(&data_l[k], sizeof(short), 1,fp);
                ++k;
                pom = 1;
            }
            
            if (pom == 1) {
                data_r[l] = (data[l] + data_l[l])/2;
                ++l;
                pom = 0;
            }
            
        }
        samples = size_data/2;
    }
    else
    {
        fread(data, sizeof(float), size_data,fp);
        samples = size_data;
    }
    fclose(fp);
}/** wavread(char) **/

wavread::~wavread()
{
    delete[] data;
    data = nullptr;
}
void wavread::SaveToFile(const char* fileName)
{
    ofstream outfile;
    outfile.open(fileName);
    
    for(unsigned i=0; i<(samples);++i)
        outfile << data[i] << endl;
    outfile.close();
}/** SaveToFile(file,datablock) **/

void wavread::WavHeaderOut()
{
    cout << "Chunk Size: " << ChunkSize << endl;
    cout << "Audio format: " << audioFormat << endl;
    cout << "Channels: " << channels << endl;
    cout << "Sample Rate: " << sampleRate << endl;
    cout << "Byte rate: " << byteRate << endl;
    cout << "Block align: " << blockAlign << endl;
    cout << "Bits Per Sample: " << bitsPerSample << endl;
    cout << "Data Size: " << dataSize << endl;
    cout << "Data: " << data << endl;
}/** WavHeaderOut() **/
