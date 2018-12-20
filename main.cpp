//
//  main.cpp
//  Bakalářka
//
//  Created by Vojtěch Matoušek on 01.03.18.
//  Copyright © 2018 Vojtěch Matoušek. All rights reserved.
//
#include <sstream>
#include <string>
#include <iostream>
#include "wavread.h"
#include "midiwrite.h"
#include "CQT.h"

int main(int argc, char** argv)
{
    /*char ano_ne;
    do {
        
    cout << "Vyberte si, zda chcete analyzovat skladbu z databaze nebo vlastni skladbu:" << endl << endl;
    
    cout << "Databaze:" << endl;
    cout << "Stay With Me = 1" << endl;
    cout << "Heart of Courage = 2" << endl;
    cout << "Akord G dur, D dur, e moll a C dur = 3" << endl;
    cout << "Vlozeni cesty k vlastnimu WAVE souboru = 4" << endl << endl;
    
    cout << "Zadejte cislo 1 - 4: ";
            
    unsigned int ktera;
    cin >> ktera;
        cout << endl;
        if (ktera < 1 || ktera > 4) {
            
        do {
            cout << "Zadejte jen cislo 1 - 4: ";
            cin >> ktera;
            } while (ktera < 1 || ktera > 4);
        }
        
    CQT data;
    string str2,str1,str3;
    if (ktera == 1)
    {
        str1 = "Databaze_rytmus/Stay_With_Me.wav";
        str2 = "Databaze_rytmus/Stay_With_Me.txt";
        str3 = "Stay_With_Me.mid";
    }
    else if (ktera == 2)
    {
        str1 = "Databaze_rytmus/Heart_of_Courage.wav";
        str2 = "Databaze_rytmus/Heart_of_Courage.txt";
        str3 = "Heart_of_Courage.mid";
    }
    else if (ktera == 3)
    {
        str1 = "Databaze_rytmus/g_d_e_c.wav";
        str2 = "Databaze_rytmus/g_d_e_c.txt";
        str3 = "g_d_e_c.mid";
    }
    else if (ktera == 4)
    {
        cout << "Zadejte cestu k souboru WAVE: ";
        cin >> str1;
        cout << "Zadejte nazev vysledneho MIDI souboru: ";
        cin >> str3;
        cout << "Zadejte minimalni analyzovanou frekvenci [Hz]: ";
        cin >> data.fmin;
        cout << "Zadejte maximalni analyzovanou frekvenci [Hz]: ";
        cin >> data.fmax;
        cout << "Zadejte pocet pocitanych tonu v jedne oktave: ";
        cin >> data.bins;
        data.zmena = 0.1;
    }
    else
    {
        cout << endl << "Zadejte cislo 1 - 4!" << endl;
        return 1;
    }
    
    wavread dataBlock;
    
    //char str1;*/
     /*ifstream in(str2);
     char znak;
     in >> data.fmax;
     in >> znak;
     in >> data.fmin;
     in >> znak;
     in >> data.bins;
     in >> znak;
     in >> data.zmena;*/
     
    /*data.fmax= 932.328;
    data.fmin= 82.407;
    data.bins = 12;
    data.zmena = 0.1;*/
    /*
    dataBlock.cteni(str1);
        cout << "------------------------------------------------------------------------------------------------------------" << endl;
        cout << "WAVE soubor se skladbou " << str1 << " se nacetl a prave se analyzuje" << endl;
    data.poleXCQ(dataBlock);
    data.rytmus(dataBlock);
        cout << "Analyza skladby " << str1 << " je u konce a vytvari se soubor MIDI s analyzovanymi notami" << endl;
    const char *mid = str3.c_str();
    midiwrite MIDIblock(mid,data,dataBlock);
    cout << "Vytvoreni MIDI souboru "<< str3 <<" je u konce" << endl;
    cout << "------------------------------------------------------------------------------------------------------------" << endl << endl;
    cout << "Chcete analyzovat dalsi skladbu? (Ano (A), Ne (N)): ";
        
    cin >> ano_ne;

        if (ano_ne != 'A' && ano_ne != 'N')
        {
            do {
                cout << "Zadejte pouze Ano (A) nebo Ne (N): ";
                cin >> ano_ne;
            } while (ano_ne != 'A' && ano_ne != 'N');
        }
    } while (ano_ne == 'A');*/
    string str1;
    str1 = "wave.wav";
    wavread dataBlock;
    dataBlock.cteni(str1);
    dataBlock.WavHeaderOut();
    dataBlock.SaveToFile("wave.txt");
    
    return 0;
}
