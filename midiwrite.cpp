//
//  midiwrite.cpp
//  Bakalářka
//
//  Created by Vojtěch Matoušek on 02.03.18.
//  Copyright © 2018 Vojtěch Matoušek. All rights reserved.
//

#include "midiwrite.h"

//****************************************
//*************KONSTRUKTOR****************
//****************************************

midiwrite::midiwrite(const char* fileName,CQT& nota,wavread& data)
{
    
    
    FILE *fp=nullptr;
    uprava_not(nota, data);
    ladeni(nota);
    fp = fopen(fileName,"wb");
    
    if(!fp)
        cout << "Error: Open file" << endl;
    
    ChunkSize_MThD = Reverse32(ChunkSize_MThD);
    FormatType = Reverse16(FormatType);
    NumofTrucks = Reverse16(NumofTrucks);
    TimeDivision = Reverse16(TimeDivision);
    ChunkSize_MTrK = unsigned(pocet_not_novych * 6 + 8);
    ChunkSize_MTrK = Reverse32(ChunkSize_MTrK);
    
    fwrite(ChunkDescriptor, sizeof(char), 4, fp);
    fwrite(&ChunkSize_MThD, sizeof(unsigned int), 1, fp);
    fwrite(&FormatType, sizeof(short), 1, fp);
    fwrite(&NumofTrucks, sizeof(short), 1, fp);
    fwrite(&TimeDivision, sizeof(short), 1, fp);
    fwrite(&ChunkHeader, sizeof(char), 4, fp);
    
    fwrite(&ChunkSize_MTrK, sizeof(unsigned int), 1, fp);
    
    uint8_t Delta_time,Instrument_change, Instrument_no, vol, Note_on,Note_on_channel;
    uint32_t EndofTrack =16723712;
    
    EndofTrack = Reverse32(EndofTrack);
    
    Delta_time = 1;
    Instrument_change = 195;
    Instrument_no = 2;
    
    
    fwrite(&Delta_time, sizeof(uint8_t), 1, fp);
    fwrite(&Instrument_change, sizeof(uint8_t), 1, fp);
    fwrite(&Instrument_no, sizeof(uint8_t), 1, fp);
    fwrite(&Delta_time, sizeof(uint8_t), 1, fp);
    Note_on_channel = 147;
    fwrite(&Note_on_channel, sizeof(uint8_t), 1, fp);
    int i=-1,j=-1,pom=0;
    do {
       
     do{                                                //zapinani not
        if(i==-1)
        {
        ++i;
        Note_on = uint8_t(noty_nove[0][i]);
        fwrite(&Note_on, sizeof(uint8_t), 1, fp);
        vol = 100;
        fwrite(&vol, sizeof(uint8_t), 1, fp);
        
        }
        else
        {
            ++i;
            uint8_t notyy=uint8_t(noty_nove[1][i]);
            Delta_time = uint8_t(notyy);
            fwrite(&Delta_time, sizeof(uint8_t), 1, fp);
            Note_on = uint8_t(noty_nove[0][i]);
            fwrite(&Note_on, sizeof(uint8_t), 1, fp);
            vol = 100;
            fwrite(&vol, sizeof(uint8_t), 1, fp);
           
        }
        pom=0;
    }while (noty_nove[3][i]==noty_nove[3][i+1] && i<pocet_not_novych-1);
        
        do {
            if(pom==0)
            {
                ++j;
                Delta_time = uint8_t(((noty_nove[2][j])));
                
                fwrite(&Delta_time, sizeof(uint8_t), 1, fp);
                Note_on = uint8_t(noty_nove[0][j]);
                fwrite(&Note_on, sizeof(uint8_t), 1, fp);
                vol = 0;
            fwrite(&vol, sizeof(uint8_t), 1, fp);
           
            pom=1;
            }
            else{
                ++j;
                Delta_time = 0;
                fwrite(&Delta_time, sizeof(uint8_t), 1, fp);
                Note_on = uint8_t(noty_nove[0][j]);
                fwrite(&Note_on, sizeof(uint8_t), 1, fp);
                vol = 0;
                fwrite(&vol, sizeof(uint8_t), 1, fp);
                
            }
        } while (noty_nove[3][j]==noty_nove[3][j+1] && j<pocet_not_novych-1);
        
    } while (i<pocet_not_novych-1);
    
    fwrite(&EndofTrack, sizeof(uint32_t), 1, fp);
    
    fclose(fp);
    
}/** midiwrite(fileName) **/

//******************************************
//***************FUNKCE*********************
//******************************************

uint16_t midiwrite::Reverse16(uint16_t value)
{
    return (((value & 0x00FF) << 8) |
            ((value & 0xFF00) >> 8));
}/** SaveToFile(file,datablock) **/

uint32_t midiwrite::Reverse32(uint32_t value)
{
    return (((value & 0x000000FF) << 24) |
            ((value & 0x0000FF00) <<  8) |
            ((value & 0x00FF0000) >>  8) |
            ((value & 0xFF000000) >> 24));
}

void midiwrite::uprava_not(CQT &nota,wavread &data)
{
    noty_upravene = new double *[5];
    for (unsigned i = 0; i<5; ++i)
        noty_upravene[i]= new double [nota.pocet_not];
    unsigned j=0;
    

    int maximum_index=0;
    unsigned long maximum = 0;
    odchylka = new unsigned long [nota.pocet_rytmus];
    
    
    for (unsigned i=0; i<nota.pocet_rytmus; ++i) {
        if (i==(nota.pocet_rytmus-1))
        {
            odchylka[i] = data.samples - nota.index[i];
            if (maximum < odchylka[i]) {
                maximum = odchylka[i];
                maximum_index = i;
            }
        }
        else
        {
            odchylka[i] = nota.index[i+1] - nota.index[i];
            if (maximum < odchylka[i]) {
                maximum = odchylka[i];
                maximum_index = i;
            }
        }
    }
    for (unsigned i=0; i<nota.pocet_not; ++i)
    {
        for (unsigned k = 0; k<nota.pocet_rytmus; ++k)
        {
            if(nota.nota[2][i] > nota.index[k] && nota.nota[2][i] < nota.index[k+1])
            {
                if (odchylka[k] > (maximum - (data.sampleRate/10)) && (odchylka[k] < (maximum + (data.sampleRate/10)))) {
                    noty_upravene[0][j] = nota.nota[0][i];
                    noty_upravene[1][j] = 0;
                    noty_upravene[2][j] = 80;
                    noty_upravene[3][j] = k;
                    noty_upravene[4][j] = nota.nota[3][i];
                    ++j;
                }
                else if (odchylka[k] > (maximum/2 - (data.sampleRate/10)) && (odchylka[k] < (maximum/2 + (data.sampleRate/10)))) {
                    noty_upravene[0][j] = nota.nota[0][i];
                    noty_upravene[1][j] = 0;
                    noty_upravene[2][j] = 40;
                    noty_upravene[3][j] = k;
                    noty_upravene[4][j] = nota.nota[3][i];
                    ++j;
                }
                else if (odchylka[k] > (maximum/4 - (data.sampleRate/10)) && (odchylka[k] < (maximum/4 + (data.sampleRate/10)))) {
                    noty_upravene[0][j] = nota.nota[0][i];
                    noty_upravene[1][j] = 0;
                    noty_upravene[2][j] = 20;
                    noty_upravene[3][j] = k;
                    noty_upravene[4][j] = nota.nota[3][i];
                    ++j;
                }
                else
                {   unsigned long cela,pulova,ctvrtova;
                    cela = abs(int(odchylka[k] - maximum));
                    pulova = abs(int(odchylka[k] - maximum/2));
                    ctvrtova = abs(int(odchylka[k] - maximum/4));
                    
                    if(cela < pulova && cela < ctvrtova )
                    {
                        noty_upravene[0][j] = nota.nota[0][i];
                        noty_upravene[1][j] = 0;
                        noty_upravene[2][j] = 80;
                        noty_upravene[3][j] = k;
                        noty_upravene[4][j] = nota.nota[3][i];
                        ++j;
                    }
                    else if(pulova < cela && pulova < ctvrtova )
                    {
                        noty_upravene[0][j] = nota.nota[0][i];
                        noty_upravene[1][j] = 0;
                        noty_upravene[2][j] = 40;
                        noty_upravene[3][j] = k;
                        noty_upravene[4][j] = nota.nota[3][i];
                        ++j;
                    }
                    else if(ctvrtova < cela && ctvrtova < pulova )
                    {
                        noty_upravene[0][j] = nota.nota[0][i];
                        noty_upravene[1][j] = 0;
                        noty_upravene[2][j] = 20;
                        noty_upravene[3][j] = k;
                        noty_upravene[4][j] = nota.nota[3][i];
                        ++j;
                    }
                    
                }
            }
            else if(nota.nota[2][i] > nota.index[nota.pocet_rytmus-1] && nota.nota[2][i] < data.samples && k == nota.pocet_rytmus-1)
                {
                    if (odchylka[k] == maximum) {
                        noty_upravene[0][j] = nota.nota[0][i];
                        noty_upravene[1][j] = 0;
                        noty_upravene[2][j] = 80;
                        noty_upravene[3][j] = k;
                        noty_upravene[4][j] = nota.nota[3][i];
                        ++j;
                    }
                    else if (odchylka[k] > (maximum/2 - (data.sampleRate/10)) && (odchylka[k] < (maximum/2 + (data.sampleRate/10)))) {
                        noty_upravene[0][j] = nota.nota[0][i];
                        noty_upravene[1][j] = 0;
                        noty_upravene[2][j] = 40;
                        noty_upravene[3][j] = k;
                        noty_upravene[4][j] = nota.nota[3][i];
                        ++j;
                    }
                    else if (odchylka[k] > (maximum/4 - (data.sampleRate/10)) && (odchylka[k] < (maximum/4 + (data.sampleRate/10)))) {
                        noty_upravene[0][j] = nota.nota[0][i];
                        noty_upravene[1][j] = 0;
                        noty_upravene[2][j] = 20;
                        noty_upravene[3][j] = k;
                        noty_upravene[4][j] = nota.nota[3][i];
                        ++j;
                    }
                    else if (odchylka[k] > (maximum/8 - (data.sampleRate/10)) && (odchylka[k] < (maximum/8 + (data.sampleRate/10)))) {
                        noty_upravene[0][j] = nota.nota[0][i];
                        noty_upravene[1][j] = 0;
                        noty_upravene[2][j] = 20;
                        noty_upravene[3][j] = k;
                        noty_upravene[4][j] = nota.nota[3][i];
                        ++j;
                    }
                    else
                    {   unsigned long cela,pulova,ctvrtova,osminova;
                        cela = abs(int(odchylka[k] - maximum));
                        pulova = abs(int(odchylka[k] - maximum/2));
                        ctvrtova = abs(int(odchylka[k] - maximum/4));
                        osminova = abs(int(odchylka[k] - maximum/8));
                        
                        if(cela < pulova && cela < ctvrtova && cela < osminova)
                        {
                            noty_upravene[0][j] = nota.nota[0][i];
                            noty_upravene[1][j] = 0;
                            noty_upravene[2][j] = 80;
                            noty_upravene[3][j] = k;
                            noty_upravene[4][j] = nota.nota[3][i];
                            ++j;
                        }
                        else if(pulova < cela && pulova < ctvrtova && pulova < osminova)
                        {
                            noty_upravene[0][j] = nota.nota[0][i];
                            noty_upravene[1][j] = 0;
                            noty_upravene[2][j] = 40;
                            noty_upravene[3][j] = k;
                            noty_upravene[4][j] = nota.nota[3][i];
                            ++j;
                        }
                        else if(ctvrtova < cela && ctvrtova < pulova && ctvrtova < osminova)
                        {
                            noty_upravene[0][j] = nota.nota[0][i];
                            noty_upravene[1][j] = 0;
                            noty_upravene[2][j] = 20;
                            noty_upravene[3][j] = k;
                            noty_upravene[4][j] = nota.nota[3][i];
                            ++j;
                        }
                        else if(osminova < cela && osminova < pulova && osminova < ctvrtova)
                        {
                            noty_upravene[0][j] = nota.nota[0][i];
                            noty_upravene[1][j] = 0;
                            noty_upravene[2][j] = 20;
                            noty_upravene[3][j] = k;
                            noty_upravene[4][j] = nota.nota[3][i];
                            ++j;
                        }
                    }
                }
            }
        }

    for (unsigned i=0; i<nota.pocet_not; ++i) {
        if (noty_upravene[0][i] > 25.856 && noty_upravene[0][i] < 26.056)
            noty_upravene[0][i] = 20;
        
        else if (noty_upravene[0][i] > 27.4 && noty_upravene[0][i] < 27.6)
            noty_upravene[0][i] = 21;
        
        else if (noty_upravene[0][i] >29.035 && noty_upravene[0][i] < 29.235)
            noty_upravene[0][i] = 22;
        
        else if (noty_upravene[0][i] >30.768 && noty_upravene[0][i] < 30.968)
            noty_upravene[0][i] = 23;
        
        else if (noty_upravene[0][i] >32.603 && noty_upravene[0][i] < 32.803)
            noty_upravene[0][i] = 24;
        
        else if (noty_upravene[0][i] >34.548 && noty_upravene[0][i] < 34.748)
            noty_upravene[0][i] = 25;
        
        else if (noty_upravene[0][i] >36.608 && noty_upravene[0][i] < 36.808)
            noty_upravene[0][i] = 26;
        
        else if (noty_upravene[0][i] >38.791 && noty_upravene[0][i] < 38.991)
            noty_upravene[0][i] = 27;
        
        else if (noty_upravene[0][i] >41.103 && noty_upravene[0][i] < 41.303)
            noty_upravene[0][i] = 28;
        
        else if (noty_upravene[0][i] >43.554 && noty_upravene[0][i] < 43.754)
            noty_upravene[0][i] = 29;
        
        else if (noty_upravene[0][i] >46.149 && noty_upravene[0][i] < 46.349)
            noty_upravene[0][i] = 30;
        
        else if (noty_upravene[0][i] >48.899 && noty_upravene[0][i] < 49.099)
            noty_upravene[0][i] = 31;
        
        else if (noty_upravene[0][i] >51.813 && noty_upravene[0][i] < 52.013)
            noty_upravene[0][i] = 32;
        
        else if (noty_upravene[0][i] >54.9 && noty_upravene[0][i] < 55.1)
            noty_upravene[0][i] = 33;
        
        else if (noty_upravene[0][i] >58.17 && noty_upravene[0][i] < 58.37)
            noty_upravene[0][i] = 34;
        
        else if (noty_upravene[0][i] >61.635 && noty_upravene[0][i] < 63.5704)
            noty_upravene[0][i] = 35;
        
        else if (noty_upravene[0][i] >63.5705 && noty_upravene[0][i] < 67.3505)
            noty_upravene[0][i] = 36;
        
        else if (noty_upravene[0][i] >67.3506 && noty_upravene[0][i] < 71.3553)
            noty_upravene[0][i] = 37;
        
        else if (noty_upravene[0][i] >71.3554 && noty_upravene[0][i] < 75.5984)
            noty_upravene[0][i] = 38;
        
        else if (noty_upravene[0][i] >75.5985 && noty_upravene[0][i] < 80.0937)
            noty_upravene[0][i] = 39;
        
        else if (noty_upravene[0][i] >80.0938 && noty_upravene[0][i] < 84.8563)
            noty_upravene[0][i] = 40;
        
        else if (noty_upravene[0][i] >84.8564 && noty_upravene[0][i] < 89.9021)
            noty_upravene[0][i] = 41;
        
        else if (noty_upravene[0][i] >89.9022 && noty_upravene[0][i] < 95.2480)
            noty_upravene[0][i] = 42;
        
        else if (noty_upravene[0][i] >95.2481 && noty_upravene[0][i] < 100.911)
            noty_upravene[0][i] = 43;
        
        else if (noty_upravene[0][i] >100.912 && noty_upravene[0][i] < 106.911)
            noty_upravene[0][i] = 44;
        
        else if (noty_upravene[0][i] >106.912 && noty_upravene[0][i] < 113.26)
            noty_upravene[0][i] = 45;
        
        else if (noty_upravene[0][i] >113.27 && noty_upravene[0][i] < 120.004)
            noty_upravene[0][i] = 46;
        
        else if (noty_upravene[0][i] >120.005 && noty_upravene[0][i] < 127.140)
            noty_upravene[0][i] = 47;
        
        else if (noty_upravene[0][i] >127.141 && noty_upravene[0][i] < 134.700)
            noty_upravene[0][i] = 48;
        
        else if (noty_upravene[0][i] >134.701 && noty_upravene[0][i] < 142.710)
            noty_upravene[0][i] = 49;
        
        else if (noty_upravene[0][i] >142.711 && noty_upravene[0][i] < 151.196)
            noty_upravene[0][i] = 50;
        
        else if (noty_upravene[0][i] >151.197 && noty_upravene[0][i] < 160.187)
            noty_upravene[0][i] = 51;
        
        else if (noty_upravene[0][i] >160.188 && noty_upravene[0][i] < 169.712)
            noty_upravene[0][i] = 52;
        
        else if (noty_upravene[0][i] >169.713 && noty_upravene[0][i] < 179.803)
            noty_upravene[0][i] = 53;
        
        else if (noty_upravene[0][i] >179.804 && noty_upravene[0][i] < 190.495)
            noty_upravene[0][i] = 54;
        
        else if (noty_upravene[0][i] >190.496 && noty_upravene[0][i] < 201.823)
            noty_upravene[0][i] = 55;
        
        else if (noty_upravene[0][i] >201.824 && noty_upravene[0][i] < 213.824)
            noty_upravene[0][i] = 56;
        
        else if (noty_upravene[0][i] >213.825 && noty_upravene[0][i] < 226.538)
            noty_upravene[0][i] = 57;
        
        else if (noty_upravene[0][i] >226.539 && noty_upravene[0][i] < 240.00)
            noty_upravene[0][i] = 58;
        
        else if (noty_upravene[0][i] >240.01 && noty_upravene[0][i] < 254.281)
            noty_upravene[0][i] = 59;
        
        else if (noty_upravene[0][i] >254.282 && noty_upravene[0][i] < 269.401)
            noty_upravene[0][i] = 60;
        
        else if (noty_upravene[0][i] >269.402 && noty_upravene[0][i] < 285.421)
            noty_upravene[0][i] = 61;
        
        else if (noty_upravene[0][i] >285.422 && noty_upravene[0][i] < 302.393)
            noty_upravene[0][i] = 62;
        
        else if (noty_upravene[0][i] >302.394 && noty_upravene[0][i] < 320.374)
            noty_upravene[0][i] = 63;
        
        else if (noty_upravene[0][i] >320.375 && noty_upravene[0][i] < 339.425)
            noty_upravene[0][i] = 64;
        
        else if (noty_upravene[0][i] >339.426 && noty_upravene[0][i] < 359.608)
            noty_upravene[0][i] = 65;
        
        else if (noty_upravene[0][i] >359.609 && noty_upravene[0][i] < 380.991)
            noty_upravene[0][i] = 66;
        
        else if (noty_upravene[0][i] >380.992 && noty_upravene[0][i] < 403.646)
            noty_upravene[0][i] = 67;
        
        else if (noty_upravene[0][i] >403.647 && noty_upravene[0][i] < 427.648)
            noty_upravene[0][i] = 68;
        
        else if (noty_upravene[0][i] >427.649 && noty_upravene[0][i] < 453.078)
            noty_upravene[0][i] = 69;
        
        else if (noty_upravene[0][i] >453.079 && noty_upravene[0][i] < 480.01)
            noty_upravene[0][i] = 70;
        
        else if (noty_upravene[0][i] >480.02 && noty_upravene[0][i] < 508.563)
            noty_upravene[0][i] = 71;
        
        else if (noty_upravene[0][i] >508.564 && noty_upravene[0][i] < 538.804)
            noty_upravene[0][i] = 72;
        
        else if (noty_upravene[0][i] >538.805 && noty_upravene[0][i] < 570.843)
            noty_upravene[0][i] = 73;
        
        else if (noty_upravene[0][i] >570.844 && noty_upravene[0][i] < 604.787)
            noty_upravene[0][i] = 74;
        
        else if (noty_upravene[0][i] >604.788 && noty_upravene[0][i] < 640.74)
            noty_upravene[0][i] = 75;
        
        else if (noty_upravene[0][i] >640.75 && noty_upravene[0][i] < 678.850)
            noty_upravene[0][i] = 76;
        
        else if (noty_upravene[0][i] >678.851 && noty_upravene[0][i] < 719.217)
            noty_upravene[0][i] = 77;
        
        else if (noty_upravene[0][i] >719.218 && noty_upravene[0][i] < 761.984)
            noty_upravene[0][i] = 78;
        
        else if (noty_upravene[0][i] >761.985 && noty_upravene[0][i] < 807.294)
            noty_upravene[0][i] = 79;
        
        else if (noty_upravene[0][i] >807.295 && noty_upravene[0][i] < 855.298)
            noty_upravene[0][i] = 80;
        
        else if (noty_upravene[0][i] >855.299 && noty_upravene[0][i] < 906.157)
            noty_upravene[0][i] = 81;
        
        else if (noty_upravene[0][i] >906.158 && noty_upravene[0][i] < 960.040)
            noty_upravene[0][i] = 82;
        
        else if (noty_upravene[0][i] >960.041 && noty_upravene[0][i] < 1017.12)
            noty_upravene[0][i] = 83;
        
        else if (noty_upravene[0][i] >1017.13 && noty_upravene[0][i] < 1077.60)
            noty_upravene[0][i] = 84;
        
        else if (noty_upravene[0][i] >1077.61 && noty_upravene[0][i] < 1141.68)
            noty_upravene[0][i] = 85;
        
        else if (noty_upravene[0][i] >1141.69 && noty_upravene[0][i] < 1209.57)
            noty_upravene[0][i] = 86;
        
        else if (noty_upravene[0][i] >1209.58 && noty_upravene[0][i] < 1281.49)
            noty_upravene[0][i] = 87;
        
        else if (noty_upravene[0][i] > 1281.5 && noty_upravene[0][i] < 1357.69)
            noty_upravene[0][i] = 88;
        
        else if (noty_upravene[0][i] >1357.7 && noty_upravene[0][i] < 1438.43)
            noty_upravene[0][i] = 89;
        
        else if (noty_upravene[0][i] >1438.44 && noty_upravene[0][i] < 1523.96)
            noty_upravene[0][i] = 90;
        
        else if (noty_upravene[0][i] >1523.97 && noty_upravene[0][i] < 1614.58)
            noty_upravene[0][i] = 91;
        
        else if (noty_upravene[0][i] >1614.59 && noty_upravene[0][i] < 1710.59)
            noty_upravene[0][i] = 92;
        
        else if (noty_upravene[0][i] >1710.6 && noty_upravene[0][i] < 1812.31)
            noty_upravene[0][i] = 93;
        
        else if (noty_upravene[0][i] >1812.32 && noty_upravene[0][i] < 1920.07)
            noty_upravene[0][i] = 94;
        
        else if (noty_upravene[0][i] >1920.08 && noty_upravene[0][i] < 2034.25)
            noty_upravene[0][i] = 95;
        
        else if (noty_upravene[0][i] >2034.26 && noty_upravene[0][i] < 2155.21)
            noty_upravene[0][i] = 96;
        
        else if (noty_upravene[0][i] >2155.22 && noty_upravene[0][i] < 2283.36)
            noty_upravene[0][i] = 97;
        
        else if (noty_upravene[0][i] >2283.37 && noty_upravene[0][i] < 2419.14)
            noty_upravene[0][i] = 98;
        
        else if (noty_upravene[0][i] >2419.15 && noty_upravene[0][i] < 2562.99)
            noty_upravene[0][i] = 99;
        
        else if (noty_upravene[0][i] >2563 && noty_upravene[0][i] < 2715.40)
            noty_upravene[0][i] = 100;
        
        else if (noty_upravene[0][i] >2715.41 && noty_upravene[0][i] < 2876.86)
            noty_upravene[0][i] = 101;
        
        else if (noty_upravene[0][i] >2876.87 && noty_upravene[0][i] < 3047.93)
            noty_upravene[0][i] = 102;
        
        else if (noty_upravene[0][i] >3047.94 && noty_upravene[0][i] < 3229.17)
            noty_upravene[0][i] = 103;
        
        else if (noty_upravene[0][i] >3229.18 && noty_upravene[0][i] < 3421.19)
            noty_upravene[0][i] = 104;
        
        else if (noty_upravene[0][i] >3421.2 && noty_upravene[0][i] < 3624.63)
            noty_upravene[0][i] = 105;
        
        else if (noty_upravene[0][i] >3624.62 && noty_upravene[0][i] < 3840.15)
            noty_upravene[0][i] = 106;
        
        else if (noty_upravene[0][i] >3840.16 && noty_upravene[0][i] < 4068.50)
            noty_upravene[0][i] = 107;
        
        else if (noty_upravene[0][i] >4068.51 && noty_upravene[0][i] < 4310.43)
            noty_upravene[0][i] = 108;
        
        else if (noty_upravene[0][i] >4310.44 && noty_upravene[0][i] < 4566.773)
            noty_upravene[0][i] = 109;
        
        else if (noty_upravene[0][i] >4566.774 && noty_upravene[0][i] < 4838.324)
            noty_upravene[0][i] = 110;
        
    }
    
}

void midiwrite::ladeni(CQT &nota)
{
    unsigned i=0;
    unsigned k=0,d=0,l=0;
    do {
       
        do {
            ++k;
        } while (noty_upravene[3][k]==noty_upravene[3][k+1] && k!=nota.pocet_not);
            ++d;
    } while (k<nota.pocet_not-1);
    
    int *pocet_k=new int[d];
    k=-1;
    d=0;
    l=0;
    do {
        l=0;
        pocet_k[d]=k+1;
        do {
            ++l;
            ++k;
        } while (noty_upravene[3][k]==noty_upravene[3][k+1] && k!=nota.pocet_not);
        
        
        ++d;
    } while (k<nota.pocet_not-1);
    
    k=0;
    i=0;
    d=0;
    l=0;
    double maximum_j = 0,maximum_i = 0;
    
    for (unsigned j =0; j<nota.pocet_not; ++j)
    {
        i = pocet_k[int(noty_upravene[3][j])];
        do {
            maximum_j = 0.0;
            maximum_i = 0.0;
            if (noty_upravene[0][j] + 1 == noty_upravene[0][i] || noty_upravene[0][j] - 1 == noty_upravene[0][i] || noty_upravene[0][j] +2 == noty_upravene[0][i] || noty_upravene[0][j] - 2 == noty_upravene[0][i]) {
                k = pocet_k[int(noty_upravene[3][j])];
                do {
                    if (noty_upravene[0][j] == noty_upravene[0][k]) {
                        if(maximum_j < noty_upravene[4][k])
                        {
                            maximum_j = noty_upravene[4][k];
                        }
                    }
                    
                    if (noty_upravene[0][i] == noty_upravene[0][k]) {
                        if(maximum_i < noty_upravene[4][k])
                        {
                            maximum_i = noty_upravene[4][k];
                        }
                    }
                    ++k;
                } while (noty_upravene[3][k]==noty_upravene[3][k+1] && k!=nota.pocet_not);
                if (maximum_j>maximum_i)
                {
                    ++l;
                }
                else if(maximum_i > maximum_j)
                {
                    ++l;
                }
            }
            ++i;
        } while (noty_upravene[3][i]==noty_upravene[3][i+1] && i<nota.pocet_not);
        
    }
    
    noty_ladeni = new double *[2];
    for (unsigned i = 0; i<2; ++i)
        noty_ladeni[i]= new double [l];
    i = 0;
    l = 0;
    k = 0;
    d = 0;
    for (unsigned j =0; j<nota.pocet_not; ++j)
    {
        i = pocet_k[int(noty_upravene[3][j])];
        do {
            
            maximum_j = 0.0;
            maximum_i = 0.0;
            if (noty_upravene[0][j] + 1 == noty_upravene[0][i] || noty_upravene[0][j] - 1 == noty_upravene[0][i] || noty_upravene[0][j] +2 == noty_upravene[0][i] || noty_upravene[0][j] - 2 == noty_upravene[0][i]) {
                k = pocet_k[int(noty_upravene[3][j])];
                    do {
                        if (noty_upravene[0][j] == noty_upravene[0][k]) {
                            if(maximum_j < noty_upravene[4][k])
                            {
                                maximum_j = noty_upravene[4][k];
                            }
                        }
                        
                        if (noty_upravene[0][i] == noty_upravene[0][k]) {
                            if(maximum_i < noty_upravene[4][k])
                            {
                                maximum_i = noty_upravene[4][k];
                            }
                        }
                        ++k;
                    } while (noty_upravene[3][k]==noty_upravene[3][k+1] && k!=nota.pocet_not);
                if (maximum_j>maximum_i)
                {
                    noty_ladeni[0][d] = noty_upravene[0][i];
                    noty_ladeni[1][d] = noty_upravene[3][i];
                    ++d;
                }
                 else if(maximum_i > maximum_j)
                {
                    noty_ladeni[0][d] = noty_upravene[0][j];
                    noty_ladeni[1][d] = noty_upravene[3][j];
                    ++d;
                }
            }
            ++i;
        } while (noty_upravene[3][i]==noty_upravene[3][i+1] && i!=nota.pocet_not);
        
    }
    unsigned pocet = 0,pocet_plus = 0,pom=0;
    for (unsigned j=0; j<nota.pocet_not; ++j) {
        pom = 0;
        for (unsigned i=0; i<d; ++i) {
            if (noty_upravene[0][j] == noty_ladeni[0][i] && noty_upravene[3][j] == noty_ladeni[1][i] ) {
                ++pocet;
                pom=1;
            }
        }
        if (pom!=1) {
            ++pocet_plus;
        }
    }
    noty_nove = new double *[4];
    for (unsigned i = 0; i<4; ++i)
        noty_nove[i]= new double [pocet_plus];
    
    pocet = 0;
    pocet_plus = 0;
    pom=0;
    for (unsigned j=0; j<nota.pocet_not; ++j) {
        pom=0;
        for (unsigned i=0; i<d; ++i) {
            if (noty_upravene[0][j] == noty_ladeni[0][i] && noty_upravene[3][j] == noty_ladeni[1][i] ) {
                ++pocet;
                pom=1;
            }
            
        }
        if (pom!=1) {
            noty_nove[0][pocet_plus]=noty_upravene[0][j];
            noty_nove[1][pocet_plus]=noty_upravene[1][j];
            noty_nove[2][pocet_plus]=noty_upravene[2][j];
            noty_nove[3][pocet_plus]=noty_upravene[3][j];
            ++pocet_plus;
        }
    }
    pocet_not_novych = pocet_plus;
    
}

void midiwrite::zapis_not()
{
    
    double **noty_nove1 = new double*[3];
    for (unsigned i = 0; i<3; ++i)
        noty_nove1[i]= new double [pocet_not_novych];
    
    for (int i =0; i<pocet_not_novych; ++i) {
        noty_nove1[0][i] = noty_nove[0][i];
        noty_nove1[1][i] = noty_nove[1][i];
        noty_nove1[2][i] = noty_nove[2][i];
        
    }
    double tmp1,tmp2,tmp3;
    for (int i = 0; i<pocet_not_novych-1; ++i) {
        for (int l = 0; l<(pocet_not_novych-i-1); ++l)
        {
            if (noty_nove1[0][l+1]<noty_nove1[0][l])
            {
                tmp1 = noty_nove1[0][l+1];
                tmp2 = noty_nove1[1][l+1];
                tmp3 = noty_nove1[2][l+1];
                
                noty_nove1[0][l+1]=noty_nove1[0][l];
                noty_nove1[1][l+1]=noty_nove1[1][l];
                noty_nove1[2][l+1]=noty_nove1[2][l];
                
                noty_nove1[0][l] = tmp1;
                noty_nove1[1][l] = tmp2;
                noty_nove1[2][l] = tmp3;
            }
        }
    }
    
    double pred=0;
    for (int i=0; i<pocet_not_novych; ++i) {
        {
            if (i!=0 && pred!=noty_nove1[0][i]) {
                pred = noty_nove1[0][i];
            }
            else if(i == 0)
            {
                pred = noty_nove1[0][i];
            }
            
        }
        
    }
}
