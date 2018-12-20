//
//  CQT.cpp
//  Bakalářka
//
//  Created by Vojtěch Matoušek on 09.03.18.
//  Copyright © 2018 Vojtěch Matoušek. All rights reserved.
//

#include "CQT.h"

CQT::~CQT()
{
    delete [] Ak;
    Ak = nullptr;
    delete [] w;
    w = nullptr;
    delete [] fk;
    fk = nullptr;
    delete [] x;
    x = nullptr;
    delete [] XCQ;
    XCQ = nullptr;
    delete [] y;
    y = nullptr;
    delete [] z;
    z = nullptr;
    delete [] zz;
    zz = nullptr;
    delete [] index;
    index = nullptr;
    delete [] nota;
    nota = nullptr;
    
}
void CQT::poleAk(double freq,unsigned int krok,wavread &data)
{
    Ak = new double[krok];
    for (unsigned i = 0;i<krok;++i)
    {
        Ak[i] = w[i] * cos(2*PI*freq*i/data.sampleRate);
    }
    
}/** poleAk(freq) **/

void CQT::poleFk()
{
    kmax = unsigned(floor(log(fmax/fmin)/log(pow(2.0,1/(double)bins))));
    fk = new double[kmax+1];
    
    for (unsigned k=0; k<kmax+1; ++k)
    {
        fk[k] = pow(pow(2.0,1/(double)bins),k)*fmin;
    }
   
}/** poleFk() **/

void CQT::windowHaming(unsigned int krok)
{
    double a0 = 7938/(double)18608, a1 = 9240/(double)18608, a2 = 1430/(double)18608;

    w = new double[krok];
    
    for (unsigned i=0; i<krok; ++i)
        w[i]=a0-a1*cos((2*PI*i)/(krok-1))+a2*cos((4*PI*i)/(krok-1));
    
}/** windowHaming(krok) **/

void CQT::rectangular(unsigned int krok)
{
    w = new double[krok];
    
    for (unsigned i=0; i<krok; ++i)
        w[i]=1;
}/** rectangular(krok) **/

void CQT::poleXCQ(wavread &data)
{
    unsigned int krok = unsigned(floor(zmena * data.sampleRate));   //krok DFT
    poleFk();														//vytvoreni pole frekvenci
    //windowHaming(krok);											//Hamingovo okno
    rectangular(krok);												//pravouhle okno
    int krok_pocet = int(floor(data.samples/krok));					//celkovy pocet kroku pres celou nahravku
    XCQ = new double *[kmax+1];										//Matice XCQ radky
    for (unsigned i = 0; i<kmax+1; ++i)								//Matice XCQ sloupce
        XCQ[i]=new double [krok_pocet];
    
    
    y = new double *[kmax+1];
    for (unsigned i = 0; i<kmax+1; ++i)
        y[i]= new double [krok_pocet*krok];
    
    double Kk;
    
    for (unsigned k=0; k<kmax+1; ++k)
    {
        poleAk(fk[k],krok,data);
        
        for (unsigned long n = 0; n < (data.samples-krok); n=n+krok) {
            double S = 0;
            unsigned i = 0;
            for (unsigned long j = n; j<(n+krok); ++j) {
                S=S+data.data[j]*Ak[i];
                ++i;
            }
            double pom = floor(n/krok);
            XCQ[k][int(pom)]= S;
        }
        for (unsigned q=0; q<(data.samples - krok-1); q=q+krok) {
            Kk = XCQ[k][q/krok];
            for (unsigned l=0; l<krok; ++l) {
                y[k][q+l]=Kk*cos(2*PI*l*fk[k]/data.sampleRate);
                
            }
        }
		
    }

    vyhodnoceni(krok_pocet, krok, data.sampleRate);
}/** XCQ() **/

void CQT::vyhodnoceni(int krok_pocet,unsigned int krok,unsigned int fs)
{
    double Q_max = 0.0;
    for(unsigned i=0;i<kmax+1;++i)
    {
        for(unsigned j=0;j<krok_pocet*krok-krok;++j)
        {
            if(Q_max < abs(y[i][j]))
            {
                Q_max = abs(y[i][j]);
            }
        }
    }
    
    z = new double *[kmax+1];
    for (unsigned i = 0; i<kmax+1; ++i)
        z[i]= new double [krok_pocet*krok];
    
    for (unsigned i = 0; i<kmax+1; ++i)
    {
        for(unsigned j=0;j<krok_pocet*krok-krok;++j)
        {
            z[i][j] = y[i][j]/Q_max;
        }
    }
	
    double m;
    m = 0.2;
    unsigned j = 0;
	
    for (unsigned k=0; k<kmax+1; ++k)
    {
        int lab = 0;
        for (unsigned i=0; i < (krok_pocet*krok-krok-floor(1.5*fs/fk[k])); ++i)
        {
            if (((abs(z[k][i]))>m) && (lab == 0))
            {
                lab = 1;
            }
            if (lab == 1)
            {
                lab=0;
                for (unsigned ii=i; ii<i+floor(1.5*fs/fk[k]); ++ii)
                {
                    if ((abs(z[k][ii]))>m)
                    {
                        lab = 1;
                    }
                }
                if (lab==1)
                {
                    i=i+unsigned(floor(1.5*fs/fk[k]));
                }
                else
                {
                    ++j;
                    lab=0;
                }
            }
        }
    }
    nota = new double *[4];
    for (unsigned i = 0; i<4; ++i)
        nota[i]= new double [j];
    
    j=0;
    unsigned ii;
    double maximum_z = 0.0;
    for (unsigned k=0; k<kmax+1; ++k)
    {
        int lab = 0;
        for (unsigned i=0; i < (krok_pocet*krok-krok-floor(1.5*fs/fk[k])); ++i)
        {
            if (((abs(z[k][i]))>m) && (lab == 0))
            {
                nota[0][j]=fk[k];
                nota[1][j]=i;
                nota[3][j]=abs(z[k][i]);
                lab = 1;
                maximum_z = abs(z[k][i]);
            }
            if (lab == 1)
            {
                lab=0;
                for (ii=i; ii<i+floor(1.5*fs/fk[k]); ++ii)
                {
                    if ((abs(z[k][ii]))>m)
                    {
                        if (maximum_z < abs(z[k][ii])) {
                            nota[3][j]=abs(z[k][ii]);
                            maximum_z = abs(z[k][ii]);
                        }
                        lab = 1;
                    }
                }
                if (lab==1)
                {
                    i=i+unsigned(floor(1.5*fs/fk[k]));
                }
                else
                {
                    nota[2][j]=ii;
                    ++j;
                    lab=0;
                }
            }
        }
    }
    
    double tmp1,tmp2,tmp3,tmp4;
    for (unsigned i = 0; i<j-1; ++i) {
        for (unsigned l = 0; l<(j-i-1); ++l)
        {
            if (nota[1][l+1]<nota[1][l])
            {
            tmp1 = nota[0][l+1];
            tmp2 = nota[1][l+1];
            tmp3 = nota[2][l+1];
            tmp4 = nota[3][l+1];
                
            
            nota[0][l+1]=nota[0][l];
            nota[1][l+1]=nota[1][l];
            nota[2][l+1]=nota[2][l];
            nota[3][l+1]=nota[3][l];
            
            nota[0][l] = tmp1;
            nota[1][l] = tmp2;
            nota[2][l] = tmp3;
            nota[3][l] = tmp4;
            }
        }
    }
    
    pocet_not = j;
    
}/** vyhodnoceni() **/

void CQT::rytmus(wavread &data)
{
    zz = new double [data.samples];
    
    
    float m=5500;
    for (unsigned i=0; i<data.samples; ++i) {
        if (abs(data.data[i])>m) {
            zz[i] = 1;
        }
    }
    unsigned lab=0;
    unsigned j=0;
    
    for (unsigned i = 0; i<(data.samples - (data.sampleRate/10)); ++i) {
        if ((zz[i]==1) && (lab == 0)) {
            lab = 1;
            ++j;
        }
        if (lab==1)
        {
            lab =0;
            for (unsigned ii=i; ii<(i + (data.sampleRate/10)); ++ii) {
                if (zz[ii]==1) {
                    lab = 1;
                }
            }
            if (lab==1) {
                i=i + (data.sampleRate/10);
            }
            else
            {
                lab=0;
            }
            
        }
    }
    index = new unsigned [j];
    
    lab=0;
    j=0;
    
    for (unsigned i = 0; i<(data.samples - (data.sampleRate/10)); ++i) {
        if ((zz[i]==1) && (lab == 0)) {
            index[j] = i;
            lab = 1;
            ++j;
        }
        if (lab==1)
        {
            lab =0;
            for (unsigned ii=i; ii<(i + (data.sampleRate/10)); ++ii) {
                if (zz[ii]==1) {
                    lab = 1;
                }
            }
            if (lab==1) {
                i=i + (data.sampleRate/10);
            }
            else
            {
                lab=0;
            }
            
        }
    }
    pocet_rytmus = j;
    
}/** vyhodnoceni(data) **/
