#include"distance.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <string>
#include <opencv/cv.h>
#include <assert.h>

using namespace cv;
using namespace std;


void distance(vector<vector<Point2f> > &coinsOuvrant)
{
    vector<Point2f> coinsMur(4);

    cout<<"Entrer coins mur :"<<endl;
    for (int i(0);i<4;i++)
    {
        cout<<i+1<<"  x :";
        cin>>coinsMur[i].x;
        cout<<i+1<<"  y :";
        cin>>coinsMur[i].y;
    }
    /***cout<<"nombre d'ouvrants : ";
    int n;
    cin>>n;
    for(int j(0);j<n;j++)
    {
        coinsOuvrant.push_back(vector<Point2f>(4));
        cout<<"Entrer coins ouvrant "<<j+1<<" :"<<endl;
        for (int i(0);i<4;i++)
        {
            cout<<i+1<<"  x :";
            cin>>coinsOuvrant[j][i].x;
            cout<<i+1<<"  y :";
            cin>>coinsOuvrant[j][i].y;
        }
    }*/

    int n(coinsOuvrant.size());
    cout<<n;

    /// Ordonner les coins du mur et ceux des ouvrant au sein des tableaux coinsMur et coinsOuvrant
    ordonnerCoinsMur(coinsMur);
    ordonnerCoinsOuvrants(coinsOuvrant);

    /// Affichage des résultats dans la console
    for(int j(0);j<n;j++)
    {
        float dist[12],dimRel[2],posRel[4];
        position(coinsOuvrant[j],coinsMur,dist);
        dimRelatives(dist,dimRel,posRel);
        cout<<"############ Ouvrant "<<j+1<<" ############"<<endl<<endl;

        cout<<"hauteur relative : "<<dimRel[0]*100<<" %"<<endl;
        cout<<"largeur relative : "<<dimRel[1]*100<<" %"<<endl<<endl;

        cout<<"distance relative par rapport au mur de gauche : "<<posRel[0]*100<<" %"<<endl;
        cout<<"distance relative par rapport au mur de droite : "<<posRel[1]*100<<" %"<<endl;
        cout<<"distance relative par rapport au plafond : "<<posRel[2]*100<<" %"<<endl;
        cout<<"distance relative par rapport au sol : "<<posRel[3]*100<<" %"<<endl<<endl;
    }
}

void echanger(Point2f& x, Point2f& y)
{
    Point temp = x;
    x = y;
    y = temp;
}

void ordonnerCoinsMur(vector<Point2f>& coins)
/*** On ordonne les coins du tableau coins dans l'ordre suivant : haut-gauche, haut-droite, bas-droite, bas-gauche*/
{
    try
    {
        if (coins.size()!=4)
        {
            throw string("Pas assez de coins");
        }
        else
        {
            for(int j(0);j<3;j++)
            {
                for(int l(0);l<3-j;l++)
                {
                    if (coins[l+1].y<coins[l].y)
                    {
                        echanger(coins[l],coins[l+1]);
                    }
                }
            }
            if(coins[0].x>coins[1].x)
            {
                echanger(coins[0],coins[1]);
            }
            if(coins[2].x<coins[3].x)
            {
                echanger(coins[2],coins[3]);
            }
        }
    }
    catch(string const& chaine)
    {
        cerr << chaine << endl;
    }
}

void ordonnerCoinsOuvrants(vector<vector<Point2f> >& coins)
{
    for(unsigned int i(0);i<coins.size();i++)
    {
        ordonnerCoinsMur(coins[i]);
    }
}

bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f &r)
/*** Cette fonction donne l'intersection r entre la droite passant par o1 et p1 et a droite passant par o2 et p2*/
{
    Point2f x = o2 - o1;
    Point2f d1 = p1 - o1;
    Point2f d2 = p2 - o2;

    float cross = d1.x*d2.y - d1.y*d2.x;
    if (abs(cross) < 1e-8){return false;}

    double t1 = (x.x * d2.y - x.y * d2.x)/cross;
    r = o1 + d1 * t1;
    return true;
}

void position(vector<Point2f> ouvrant, vector<Point2f> mur, float dist[12])
/*** Cette fonction calcule les 12 distances caractéristiques de la géométrie du problème
(Voir schéma sur rapport final pour mieux comprendre)*/
{
    Point2f p1(ouvrant[0]),p2(ouvrant[1]),p3(ouvrant[2]),p4(ouvrant[3]);
    Point2f p5(mur[0]),p6(mur[1]),p7(mur[2]),p8(mur[3]);

    Point2f a,b,c,d,e,f,g,h;
    try
    {
        if(!intersection(p3,p4,p8,p5,a) or !intersection(p1,p2,p5,p8,b)
        or !intersection(p1,p4,p5,p6,c) or !intersection(p2,p3,p5,p6,d)
        or !intersection(p1,p2,p6,p7,e) or !intersection(p4,p3,p6,p7,f)
        or !intersection(p2,p3,p7,p8,g) or !intersection(p1,p4,p7,p8,h))
        {
            throw string("Contour trop applati : lignes parrallèles");
        }
    }
    catch(string const& chaine)
    {
        cerr << chaine << endl;
    }

    dist[0] = madistance(p4,p1);
    dist[1] = madistance(p1,p2);
    dist[2] = madistance(p2,p3);
    dist[3] = madistance(p3,p4);
    dist[4] = madistance(p4,a);
    dist[5] = madistance(p1,b);
    dist[6] = madistance(p1,c);
    dist[7] = madistance(p2,d);
    dist[8] = madistance(p2,e);
    dist[9] = madistance(p3,f);
    dist[10] = madistance(p3,g);
    dist[11] = madistance(p4,h);
}

float madistance(Point2f& p, Point2f& q)
/// Trouve la distance en pixels entre le point p et le point q
{
    Point diff = p - q;
    return sqrt(diff.x*diff.x + diff.y*diff.y);
}

void dimRelatives(float dist[12],float dimRel[2], float posRel[4])
/*** A partir des 12 distances caractéritiques du tableau dist, on trouve :
- les deux dimensions relatives de l'ouvrant (hauteur et largeur)
- les quatres distances relatives permettant de placer l'ouvrant sur le mur (gauche, droite, haut, bas)*/
{
    ///voir schéma pour visualiser
    float d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12;
    d1 = dist[0];
    d2 = dist[1];
    d3 = dist[2];
    d4 = dist[3];
    d5 = dist[4];
    d6 = dist[5];
    d7 = dist[6];
    d8 = dist[7];
    d9 = dist[8];
    d10 = dist[9];
    d11 = dist[10];
    d12 = dist[11];

    float h1(d12+d1+d7);
    float h2(d11+d3+d8);
    float l1(d6+d2+d9);
    float l2(d5+d4+d10);
    float dR1,dR2,dR3,dR4,dR5,dR6,dR7,dR8,dR9,dR10,dR11,dR12;
    dR1 = d1/h1;
    dR2 = d2/l1;
    dR3 = d3/h2;
    dR4 = d4/l2;
    dR5 = d5/l2;
    dR6 = d6/l1;
    dR7 = d7/h1;
    dR8 = d8/h2;
    dR9 = d9/l1;
    dR10 = d10/l2;
    dR11= d11/h2;
    dR12 = d12/h1;

    dimRel[0] = (dR1 + dR3)/2; ///hauteur relative ouvrant
    dimRel[1] = (dR2 + dR4)/2;  ///largeur relative ouvrant

    posRel[0] = (dR6 + dR5)/2;  ///distance à gauche entre l'ouvrant et le mur de gauche
    posRel[1] = (dR10 + dR9)/2;  ///distance à droite entre l'ouvrant et le mur de droite
    posRel[2] = (dR7 + dR8)/2;  ///distance en haut entre l'ouvrant et le plafond
    posRel[3] = (dR11 + dR12)/2;  ///distance en bas entre l'ouvrant et le sol
}
