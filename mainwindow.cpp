#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <math.h>
#include <QDebug>
#include <algorithm>
#include <stdlib.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Tetris 3D");
    resize(800,500);
    szer = 800;
    wys = 500;

    right = new QPushButton;
    right -> setText("Right");
    connect(right,SIGNAL(clicked()),this,SLOT(moveRight()));

    left = new QPushButton;
    left -> setText("Left");
    connect(left,SIGNAL(clicked()),this,SLOT(moveLeft()));

    down = new QPushButton;
    down -> setText("Down");
    connect(down,SIGNAL(clicked()),this,SLOT(moveDown()));

    up = new QPushButton;
    up -> setText("Up");
    connect(up,SIGNAL(clicked()),this,SLOT(moveUp()));

    drop = new QPushButton;
    drop -> setText("Drop");
    connect(drop,SIGNAL(clicked()),this,SLOT(dropSquare()));

    rotateX = new QPushButton;
    rotateX -> setText("RotateX");
    connect(rotateX,SIGNAL(clicked()),this,SLOT(pomX()));

    rotateY = new QPushButton;
    rotateY -> setText("RotateY");
    connect(rotateY,SIGNAL(clicked()),this,SLOT(pomY()));

    rotateZ = new QPushButton;
    rotateZ -> setText("RotateZ");
    connect(rotateZ,SIGNAL(clicked()),this,SLOT(pomZ()));

    score = new QLabel("Score: ");
    score->setText(QString("Score: ") + QString::number(wynikk));
    score->setStyleSheet("QLabel {color : red; font-size:20pt;}");

    //QVBoxLayout//
    layout = new QVBoxLayout;
    layout->addWidget(up);
    layout->addWidget(right);
    layout->addWidget(left);
    layout->addWidget(down);
    layout->addWidget(drop);
    layout->addWidget(rotateX);
    layout->addWidget(rotateY);
    layout->addWidget(rotateZ);
    layout->addWidget(score);

    //QGroupBox//
    grupa = new QGroupBox(this);
    grupa -> setGeometry(550,10,200,400);
    grupa -> setLayout(layout);

    img = new QImage(szer,wys,QImage::Format_RGB32);
    tlo = new QImage(szer,wys,QImage::Format_RGB32);
    tlo2 = new QImage(szer,wys,QImage::Format_RGB32);

    zBufor = new int[250000];

    planszaStart();
    ustawTlo();
    kopiowaniePlansz(2);
    tworzenieNowegoKlocka(-1);

    timer = new QTimer();
    timer->setInterval(3000);
    timer->setSingleShot(false);
    connect(timer, &QTimer::timeout, this, [=]()
    {
        goDown();
    });
    timer->start();
}

void MainWindow::pomX()
{
    moveRotateX(0);
}

void MainWindow::pomY()
{
    moveRotateY(0);
}

void MainWindow::pomZ()
{
    moveRotateZ(0);
}

//Opadanie w czasie//
void MainWindow::goDown()
{
    bool isValid = true;
    for(int i=0; i<obecnyKlocek->iloscKostek; i++)
    {
        if(obecnyKlocek->kostki[i].z==6)
        {
            isValid = false;
            break;
        }
        else if(plansza[obecnyKlocek->kostki[i].z+1][obecnyKlocek->kostki[i].y][obecnyKlocek->kostki[i].x]==true)
        {
            isValid = false;
            break;
        }
    }

    if(isValid)
    {
        t.tablica[11] = t.tablica[11]+100;
        zmianaPolozeniaKlocka(0);
        for(int i=0; i<obecnyKlocek->iloscKostek; i++)
        {
            obecnyKlocek->kostki[i].setValues(obecnyKlocek->kostki[i].x,obecnyKlocek->kostki[i].y,obecnyKlocek->kostki[i].z+1);
        }
    }
    else
    {
        for(int i=0; i<obecnyKlocek->iloscKostek; i++)
        {
            plansza[obecnyKlocek->kostki[i].z][obecnyKlocek->kostki[i].y][obecnyKlocek->kostki[i].x] = true;
        }
        kopiowaniePlansz(0);
        malujCalyKlocek();
        kopiowaniePlansz(1);
        checkBoard();
        tworzenieNowegoKlocka(-1);
    }
}

//Zmiana wyniku//
void MainWindow::changeScore(int x)
{
    wynikk = wynikk + 10*pow(x,2);
    score->setText(QString("Score: ") + QString::number(wynikk));
}

//Funkcja poczatkowa//
void MainWindow::planszaStart()
{
    for(int z=0; z<7; z++)
    {
        for(int y=0; y<5; y++)
        {
            for(int x=0; x<5; x++)
            {
                plansza[z][y][x] = false;
            }
        }
    }

    unsigned char *ptr;
    ptr = tlo->bits();
    for(int i=0; i<500; i++)
    {
        for(int j=0; j<500; j++)
        {
            ptr[szer*4*i + 4*j]=255; // Skladowa BLUE
            ptr[szer*4*i + 4*j + 1] = 255; // Skladowa GREEN
            ptr[szer*4*i + 4*j + 2] = 255; // Skladowa RED
            zBufor[500*i+j] = 9;
        }
    }
}

//Funkcja od tworzenia nowego klocka//
void MainWindow::tworzenieNowegoKlocka(int pom)
{
    for(int i=0; i<16; i++)
    {
        if(i%5==0)
        {
            t.tablica[i] = 1;
            rX.tablica[i] = 1;
            rY.tablica[i] = 1;
            rZ.tablica[i] = 1;
        }
        else
        {
            t.tablica[i] = 0;
            rX.tablica[i] = 0;
            rY.tablica[i] = 0;
            rZ.tablica[i] = 0;
        }
    }

    losujKlocek(pom);
    int x = -50;
    int y = -50;

    ustawKwadrat(0,x,y,0);
    obecnyKlocek->kostki[0].setValues(2,2,0);

    if(obecnyKlocek->rodzaj==0)
    {

    }
    else if(obecnyKlocek->rodzaj==1)
    {
        ustawKwadrat(1,x+100,y,0);

        obecnyKlocek->kostki[1].setValues(3,2,0);
    }
    else if(obecnyKlocek->rodzaj==2)
    {
        ustawKwadrat(1,x-100,y,0);
        ustawKwadrat(2,x+100,y,0);
        ustawKwadrat(3,x,y-100,0);

        obecnyKlocek->kostki[1].setValues(1,2,0);
        obecnyKlocek->kostki[2].setValues(3,2,0);
        obecnyKlocek->kostki[3].setValues(2,1,0);
    }
    else if(obecnyKlocek->rodzaj==3)
    {
        ustawKwadrat(1,x-100,y,0);
        ustawKwadrat(2,x,y-100,0);
        ustawKwadrat(3,x+100,y-100,0);

        obecnyKlocek->kostki[1].setValues(1,2,0);
        obecnyKlocek->kostki[2].setValues(2,1,0);
        obecnyKlocek->kostki[3].setValues(3,1,0);
    }
    else if(obecnyKlocek->rodzaj==4)
    {
        ustawKwadrat(1,x,y-100,0);
        ustawKwadrat(2,x+100,y-100,0);
        ustawKwadrat(3,x+100,y,0);

        obecnyKlocek->kostki[1].setValues(2,1,0);
        obecnyKlocek->kostki[2].setValues(3,1,0);
        obecnyKlocek->kostki[3].setValues(3,2,0);
    }
    else if(obecnyKlocek->rodzaj==5)
    {
        ustawKwadrat(1,x+100,y,0);
        ustawKwadrat(2,x-100,y,0);
        ustawKwadrat(3,x-100,y-100,0);

        obecnyKlocek->kostki[1].setValues(3,2,0);
        obecnyKlocek->kostki[2].setValues(1,2,0);
        obecnyKlocek->kostki[3].setValues(1,1,0);
    }

    zmianaPolozeniaKlocka(0);
}

//Funkcja losujaca jakiego rodzaju bedzie nasz obecny klocek//
void MainWindow::losujKlocek(int pom)
{
    int number;
    srand (time(NULL));
    number = rand() % 6;
    if(pom>=0)
    {
        number=pom;
    }

    if(number==0)
    {
        obecnyKlocek = new Klocek(1,number);
    }
    else if(number==1)
    {
        obecnyKlocek = new Klocek(2,number);
    }
    else
    {
        obecnyKlocek = new Klocek(4,number);
    }

    //Proba wykonywania obrotow na nowym obiekcie dana ilosc razy dla jednej pozniej drugiej i trzeciej osi//
    if(pom>=0)
    {
        int x = ostatniRazX;
        int y = ostatniRazY;
        int z = ostatniRazZ;

        while(x!=0)
        {
            moveRotateX(1);
            x--;
        }

        while(y!=0)
        {
            moveRotateY(1);
            y--;
        }

        while(z!=0)
        {
            moveRotateZ(1);
            z--;
        }
    }
    else
    {
        zRotation = 0;
        yRotation = 0;
        xRotation = 0;
    }
}

//Funkcja ustawiajaca wartosci wierzcholkow danego szescianu w klocku//
void MainWindow::ustawKwadrat(int i, int x, int y, int z)
{
    obecnyKlocek->kostki[i].wierzcholki[0].setValues(x,y,10+z*100);
    obecnyKlocek->kostki[i].wierzcholki[1].setValues(x+100,y,10+z*100);
    obecnyKlocek->kostki[i].wierzcholki[2].setValues(x,y+100,10+z*100);
    obecnyKlocek->kostki[i].wierzcholki[3].setValues(x+100,y+100,10+z*100);
    obecnyKlocek->kostki[i].wierzcholki[4].setValues(x,y,110+z*100);
    obecnyKlocek->kostki[i].wierzcholki[5].setValues(x+100,y,110+z*100);
    obecnyKlocek->kostki[i].wierzcholki[6].setValues(x,y+100,110+z*100);
    obecnyKlocek->kostki[i].wierzcholki[7].setValues(x+100,y+100,110+z*100);

    wierzcholkiPom[0+i*8].setValues(x,y,10+z*100);
    wierzcholkiPom[1+i*8].setValues(x+100,y,10+z*100);
    wierzcholkiPom[2+i*8].setValues(x,y+100,10+z*100);
    wierzcholkiPom[3+i*8].setValues(x+100,y+100,10+z*100);
    wierzcholkiPom[4+i*8].setValues(x,y,110+z*100);
    wierzcholkiPom[5+i*8].setValues(x+100,y,110+z*100);
    wierzcholkiPom[6+i*8].setValues(x,y+100,110+z*100);
    wierzcholkiPom[7+i*8].setValues(x+100,y+100,110+z*100);
}

//Glowna funkcja odpowiedzialna za zmiane polozenia//
void MainWindow::zmianaPolozeniaKlocka(int mode)
{
    Macierz h;
    h.tablica[3] = 0;
    h.tablica[7] = 0;
    h.tablica[11] = 60;
    Macierz hh;
    hh.tablica[3] = 0;
    hh.tablica[7] = 0;
    hh.tablica[11] = -60;
    Macierz glowna;
    /*Macierz wypadkowegoObrotu;
    wypadkowegoObrotu.tablica[0] = cos(xRotation * M_PI / 180) * cos(zRotation * M_PI / 180) - (sin(xRotation * M_PI / 180) * sin(zRotation * M_PI / 180)* cos(yRotation * M_PI / 180));
    wypadkowegoObrotu.tablica[1] = sin(xRotation * M_PI / 180) * cos(zRotation * M_PI / 180) + (cos(xRotation * M_PI / 180) * sin(zRotation * M_PI / 180)* cos(yRotation * M_PI / 180));
    wypadkowegoObrotu.tablica[2] = sin(zRotation * M_PI / 180) * sin(yRotation * M_PI / 180);
    wypadkowegoObrotu.tablica[4] = -cos(xRotation * M_PI / 180) * sin(zRotation * M_PI / 180) - (sin(xRotation * M_PI / 180) * sin(zRotation * M_PI / 180)* cos(yRotation * M_PI / 180));
    wypadkowegoObrotu.tablica[5] = -sin(xRotation * M_PI / 180) * sin(zRotation * M_PI / 180) + (cos(xRotation * M_PI / 180) * cos(zRotation * M_PI / 180)* cos(yRotation * M_PI / 180));
    wypadkowegoObrotu.tablica[6] = cos(zRotation * M_PI / 180) * sin(yRotation * M_PI / 180);
    wypadkowegoObrotu.tablica[8] = sin(xRotation * M_PI / 180) * sin(yRotation * M_PI / 180);
    wypadkowegoObrotu.tablica[9] = -cos(xRotation * M_PI / 180) * sin(yRotation * M_PI / 180);
    wypadkowegoObrotu.tablica[10] = cos(yRotation * M_PI / 180);
    */

    glowna = t*h*rX*rY*rZ*hh;
    float d = 300;

    for(int j=0; j<obecnyKlocek->iloscKostek; j++)
    {
        for(int i=0; i<8; i++)
        {
            float x = wierzcholkiPom[i+j*8].x;
            float y = wierzcholkiPom[i+j*8].y;
            float z = wierzcholkiPom[i+j*8].z;
            obecnyKlocek->kostki[j].wierzcholki[i].x = glowna.tablica[0]*x + glowna.tablica[1]*y + glowna.tablica[2]*z + glowna.tablica[3];
            obecnyKlocek->kostki[j].wierzcholki[i].y = glowna.tablica[4]*x + glowna.tablica[5]*y + glowna.tablica[6]*z + glowna.tablica[7];
            obecnyKlocek->kostki[j].wierzcholki[i].z = glowna.tablica[8]*x + glowna.tablica[9]*y + glowna.tablica[10]*z + glowna.tablica[11];
            obecnyKlocek->kostki[j].wierzcholki[i].x = ((obecnyKlocek->kostki[j].wierzcholki[i].x*d)/(obecnyKlocek->kostki[j].wierzcholki[i].z+d))+250;
            obecnyKlocek->kostki[j].wierzcholki[i].y = ((obecnyKlocek->kostki[j].wierzcholki[i].y*d)/(obecnyKlocek->kostki[j].wierzcholki[i].z+d))+250;
            obecnyKlocek->kostki[j].wierzcholki[i].z = 0;
        }
    }

    rysujKlocek(mode);
}

//Funkcja odpowiedzialna za rysowanie wszystkich krawedzi//
void MainWindow::rysujKlocek(int mode)
{
    if(mode==0)
    {
        kopiowaniePlansz(0);
    }
    for(int i=0; i<obecnyKlocek->iloscKostek; i++)
    {
        //GORA//
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[0].x,obecnyKlocek->kostki[i].wierzcholki[0].y,obecnyKlocek->kostki[i].wierzcholki[4].x,obecnyKlocek->kostki[i].wierzcholki[4].y,mode,obecnyKlocek->kostki[i].z);
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[1].x,obecnyKlocek->kostki[i].wierzcholki[1].y,obecnyKlocek->kostki[i].wierzcholki[5].x,obecnyKlocek->kostki[i].wierzcholki[5].y,mode,obecnyKlocek->kostki[i].z);
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[0].x,obecnyKlocek->kostki[i].wierzcholki[0].y,obecnyKlocek->kostki[i].wierzcholki[1].x,obecnyKlocek->kostki[i].wierzcholki[1].y,mode,obecnyKlocek->kostki[i].z);
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[4].x,obecnyKlocek->kostki[i].wierzcholki[4].y,obecnyKlocek->kostki[i].wierzcholki[5].x,obecnyKlocek->kostki[i].wierzcholki[5].y,mode,obecnyKlocek->kostki[i].z);

        //DOL//
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[2].x,obecnyKlocek->kostki[i].wierzcholki[2].y,obecnyKlocek->kostki[i].wierzcholki[3].x,obecnyKlocek->kostki[i].wierzcholki[3].y,mode,obecnyKlocek->kostki[i].z);
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[6].x,obecnyKlocek->kostki[i].wierzcholki[6].y,obecnyKlocek->kostki[i].wierzcholki[7].x,obecnyKlocek->kostki[i].wierzcholki[7].y,mode,obecnyKlocek->kostki[i].z);
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[2].x,obecnyKlocek->kostki[i].wierzcholki[2].y,obecnyKlocek->kostki[i].wierzcholki[6].x,obecnyKlocek->kostki[i].wierzcholki[6].y,mode,obecnyKlocek->kostki[i].z);
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[3].x,obecnyKlocek->kostki[i].wierzcholki[3].y,obecnyKlocek->kostki[i].wierzcholki[7].x,obecnyKlocek->kostki[i].wierzcholki[7].y,mode,obecnyKlocek->kostki[i].z);

        //PRAWA//
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[1].x,obecnyKlocek->kostki[i].wierzcholki[1].y,obecnyKlocek->kostki[i].wierzcholki[5].x,obecnyKlocek->kostki[i].wierzcholki[5].y,mode,obecnyKlocek->kostki[i].z);
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[3].x,obecnyKlocek->kostki[i].wierzcholki[3].y,obecnyKlocek->kostki[i].wierzcholki[7].x,obecnyKlocek->kostki[i].wierzcholki[7].y,mode,obecnyKlocek->kostki[i].z);
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[1].x,obecnyKlocek->kostki[i].wierzcholki[1].y,obecnyKlocek->kostki[i].wierzcholki[3].x,obecnyKlocek->kostki[i].wierzcholki[3].y,mode,obecnyKlocek->kostki[i].z);
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[5].x,obecnyKlocek->kostki[i].wierzcholki[5].y,obecnyKlocek->kostki[i].wierzcholki[7].x,obecnyKlocek->kostki[i].wierzcholki[7].y,mode,obecnyKlocek->kostki[i].z);

        //LEWA//
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[0].x,obecnyKlocek->kostki[i].wierzcholki[0].y,obecnyKlocek->kostki[i].wierzcholki[4].x,obecnyKlocek->kostki[i].wierzcholki[4].y,mode,obecnyKlocek->kostki[i].z);
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[2].x,obecnyKlocek->kostki[i].wierzcholki[2].y,obecnyKlocek->kostki[i].wierzcholki[6].x,obecnyKlocek->kostki[i].wierzcholki[6].y,mode,obecnyKlocek->kostki[i].z);
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[0].x,obecnyKlocek->kostki[i].wierzcholki[0].y,obecnyKlocek->kostki[i].wierzcholki[2].x,obecnyKlocek->kostki[i].wierzcholki[2].y,mode,obecnyKlocek->kostki[i].z);
        piszLinie(obecnyKlocek->kostki[i].wierzcholki[4].x,obecnyKlocek->kostki[i].wierzcholki[4].y,obecnyKlocek->kostki[i].wierzcholki[6].x,obecnyKlocek->kostki[i].wierzcholki[6].y,mode,obecnyKlocek->kostki[i].z);
    }
    repaint();
}

//Funkcja kopiujaca plansze//
void MainWindow::kopiowaniePlansz(int x)
{
    if(x==0)
    {
        unsigned char *ptr;
        ptr = img->bits();
        unsigned char *ptr2;
        ptr2 = tlo->bits();
        for(int i=0; i<500; i++)
        {
            for(int j=0; j<500; j++)
            {
                ptr[szer*4*i + 4*j] = ptr2[szer*4*i + 4*j];
                ptr[szer*4*i + 4*j + 1] = ptr2[szer*4*i + 4*j + 1];
                ptr[szer*4*i + 4*j + 2] = ptr2[szer*4*i + 4*j + 2];
            }
        }
    }
    else if(x==1)
    {
        unsigned char *ptr;
        ptr = img->bits();
        unsigned char *ptr2;
        ptr2 = tlo->bits();
        for(int i=0; i<500; i++)
        {
            for(int j=0; j<500; j++)
            {
                ptr2[szer*4*i + 4*j] = ptr[szer*4*i + 4*j];
                ptr2[szer*4*i + 4*j + 1] = ptr[szer*4*i + 4*j + 1];
                ptr2[szer*4*i + 4*j + 2] = ptr[szer*4*i + 4*j + 2];
            }
        }
    }
    else if(x==2)
    {
        unsigned char *ptr;
        ptr = tlo->bits();
        unsigned char *ptr2;
        ptr2 = tlo2->bits();
        for(int i=0; i<500; i++)
        {
            for(int j=0; j<500; j++)
            {
                ptr2[szer*4*i + 4*j] = ptr[szer*4*i + 4*j];
                ptr2[szer*4*i + 4*j + 1] = ptr[szer*4*i + 4*j + 1];
                ptr2[szer*4*i + 4*j + 2] = ptr[szer*4*i + 4*j + 2];
            }
        }
    }
    else if(x==3)
    {
        unsigned char *ptr;
        ptr = tlo2->bits();
        unsigned char *ptr2;
        ptr2 = img->bits();
        for(int i=0; i<500; i++)
        {
            for(int j=0; j<500; j++)
            {
                ptr2[szer*4*i + 4*j] = ptr[szer*4*i + 4*j];
                ptr2[szer*4*i + 4*j + 1] = ptr[szer*4*i + 4*j + 1];
                ptr2[szer*4*i + 4*j + 2] = ptr[szer*4*i + 4*j + 2];
                zBufor[500*i+j] = 9;
            }
        }
    }
    else if(x==4)
    {
        unsigned char *ptr;
        ptr = tlo2->bits();
        unsigned char *ptr2;
        ptr2 = tlo->bits();
        for(int i=0; i<500; i++)
        {
            for(int j=0; j<500; j++)
            {
                ptr2[szer*4*i + 4*j] = ptr[szer*4*i + 4*j];
                ptr2[szer*4*i + 4*j + 1] = ptr[szer*4*i + 4*j + 1];
                ptr2[szer*4*i + 4*j + 2] = ptr[szer*4*i + 4*j + 2];
            }
        }
    }
}

//Funckja która rysuje linie//
void MainWindow::piszLinie(float x1, float y1, float x2, float y2, int warstwa, int z)
{
    if(x2!=x1)
    {
        double a = (y2-y1)/(x2-x1);
        double b = y1 - (x1*a);
        if(abs(a)<1)
        {
            if(x1>=x2)
            {
                swap(x1,x2);
            }

            for(int j=x1; j<=x2; j++)
            {
                int y = (a*j)+b;
                wstawPiksel(j,y,warstwa,z);
            }

        }
        else
        {
            if(y1>=y2)
            {
                swap(y1,y2);
            }

            for(int j=y1; j<=y2; j++)
            {
                int x = (j-b)/a;
                wstawPiksel(x,j,warstwa,z);
            }
        }
    }
    else
    {
        if(y1<=y2)
        {
            swap(y1,y2);
        }

        for(int j=y2; j<=y1; j++)
        {
            wstawPiksel(x1,j,warstwa,z);
        }
    }
}

//Funkcja kolorujaca dany piksel na planszy//
void MainWindow::wstawPiksel(int j,int i, int warstwa, int z)
{
    if(warstwa==0)
    {
        unsigned char *ptr;
        ptr = img->bits();
        if(j<szer && j>=0 && i<wys && i>=0)
        {
            ptr[szer*4*i + 4*j]=0; // Skladowa BLUE
            ptr[szer*4*i + 4*j + 1] = 0; // Skladowa GREEN
            ptr[szer*4*i + 4*j + 2] = 0; // Skladowa RED
        }
    }
    else if(warstwa==1)
    {
        unsigned char *ptr;
        ptr = tlo->bits();
        if(j<szer && j>=0 && i<wys && i>=0)
        {
            if(zBufor[500*i+j]>z)
            {
                ptr[szer*4*i + 4*j]=0; // Skladowa BLUE
                ptr[szer*4*i + 4*j + 1] = 0; // Skladowa GREEN
                ptr[szer*4*i + 4*j + 2] = 0; // Skladowa RED
                zBufor[500*i+j] = z;
            }
        }
    }
    else if(warstwa==2)
    {
        unsigned char *ptr;
        ptr = img->bits();
        if(j<szer && j>=0 && i<wys && i>=0)
        {
            if(zBufor[500*i+j]>z)
            {
                ptr[szer*4*i + 4*j]=0; // Skladowa BLUE
                ptr[szer*4*i + 4*j + 1] = 0; // Skladowa GREEN
                ptr[szer*4*i + 4*j + 2] = 0; // Skladowa RED
                zBufor[500*i+j] = z;
            }
        }
    }
}

//Funkcja startowa rysujaca tlo//
void MainWindow::ustawTlo()
{
    unsigned char *ptr;
    ptr = tlo->bits();
    for(int i=0; i<500; i++)
    {
        for(int j=0; j<500; j++)
        {
            ptr[szer*4*i + 4*j]=255; // Skladowa BLUE
            ptr[szer*4*i + 4*j + 1] = 255; // Skladowa GREEN
            ptr[szer*4*i + 4*j + 2] = 255; // Skladowa RED
            zBufor[500*i+j] = 9;
        }
    }

    piszLinie(0,0,170,170,1,8);
    piszLinie(100,0,202,170,1,8);
    piszLinie(200,0,234,170,1,8);
    piszLinie(300,0,266,170,1,8);
    piszLinie(400,0,298,170,1,8);
    piszLinie(500,0,330,170,1,8);

    piszLinie(0,500,170,330,1,8);
    piszLinie(100,500,202,330,1,8);
    piszLinie(200,500,234,330,1,8);
    piszLinie(300,500,266,330,1,8);
    piszLinie(400,500,298,330,1,8);
    piszLinie(500,500,330,330,1,8);

    piszLinie(170,202,0,100,1,8);
    piszLinie(170,234,0,200,1,8);
    piszLinie(170,266,0,300,1,8);
    piszLinie(170,298,0,400,1,8);

    piszLinie(330,202,500,100,1,8);
    piszLinie(330,234,500,200,1,8);
    piszLinie(330,266,500,300,1,8);
    piszLinie(330,298,500,400,1,8);

    unsigned char *ptr2;
    ptr2 = tlo->bits();
    for(int i=170; i<330; i++)
    {
        for(int j=170; j<330; j++)
        {
            ptr2[szer*4*i + 4*j] = 128;
            ptr2[szer*4*i + 4*j + 1] = 255;
            ptr2[szer*4*i + 4*j + 2] = 255;
        }
    }

    piszLinie(170,170,330,170,1,7);
    piszLinie(170,170,170,330,1,7);
    piszLinie(330,330,170,330,1,7);
    piszLinie(330,330,330,170,1,7);

    piszLinie(170,202,330,202,1,7);
    piszLinie(170,234,330,234,1,7);
    piszLinie(170,266,330,266,1,7);
    piszLinie(170,298,330,298,1,7);

    piszLinie(202,170,202,330,1,7);
    piszLinie(234,170,234,330,1,7);
    piszLinie(266,170,266,330,1,7);
    piszLinie(298,170,298,330,1,7);
}

//Funkcja odpowiedzialna za przesuniecie w lewo//
void MainWindow::moveLeft()
{
    bool isValid = true;
    for(int i=0; i<obecnyKlocek->iloscKostek; i++)
    {
        if(obecnyKlocek->kostki[i].x==0)
        {
            isValid = false;
            break;
        }
        else if(plansza[obecnyKlocek->kostki[i].z][obecnyKlocek->kostki[i].y][obecnyKlocek->kostki[i].x-1]==true)
        {
            isValid = false;
            for(int j=0; j<obecnyKlocek->iloscKostek; j++)
            {
                if((obecnyKlocek->kostki[i].x-1 == obecnyKlocek->kostki[j].x) && (obecnyKlocek->kostki[i].y == obecnyKlocek->kostki[j].y) && (obecnyKlocek->kostki[i].z == obecnyKlocek->kostki[j].z))
                {
                    isValid = true;
                }
            }
        }

        if(isValid==false)
        {
            break;
        }
    }

    if(isValid)
    {
        t.tablica[3] = t.tablica[3]-100;
        zmianaPolozeniaKlocka(0);
        for(int i=0; i<obecnyKlocek->iloscKostek; i++)
        {
            obecnyKlocek->kostki[i].setValues(obecnyKlocek->kostki[i].x-1,obecnyKlocek->kostki[i].y,obecnyKlocek->kostki[i].z);
        }
    }
}

//Funkcja odpowiedzialna za przesuniecie w prawo//
void MainWindow::moveRight()
{
    bool isValid = true;
    for(int i=0; i<obecnyKlocek->iloscKostek; i++)
    {
        if(obecnyKlocek->kostki[i].x==4)
        {
            isValid = false;
            break;
        }
        else if(plansza[obecnyKlocek->kostki[i].z][obecnyKlocek->kostki[i].y][obecnyKlocek->kostki[i].x+1]==true)
        {
            isValid = false;
            for(int j=0; j<obecnyKlocek->iloscKostek; j++)
            {
                if((obecnyKlocek->kostki[i].x+1 == obecnyKlocek->kostki[j].x) && (obecnyKlocek->kostki[i].y == obecnyKlocek->kostki[j].y) && (obecnyKlocek->kostki[i].z == obecnyKlocek->kostki[j].z))
                {
                    isValid = true;
                }
            }

            if(isValid==false)
            {
                break;
            }
        }
    }

    if(isValid)
    {
        t.tablica[3] = t.tablica[3]+100;
        zmianaPolozeniaKlocka(0);
        for(int i=0; i<obecnyKlocek->iloscKostek; i++)
        {
            obecnyKlocek->kostki[i].setValues(obecnyKlocek->kostki[i].x+1,obecnyKlocek->kostki[i].y,obecnyKlocek->kostki[i].z);
        }
    }
}

//Funkcja odpowiedzialna za przesuniecie w dol(nie mylic z opadaniem!)//
void MainWindow::moveDown()
{
    bool isValid = true;
    for(int i=0; i<obecnyKlocek->iloscKostek; i++)
    {
        if(obecnyKlocek->kostki[i].y==4)
        {
            isValid = false;
            break;
        }
        else if(plansza[obecnyKlocek->kostki[i].z][obecnyKlocek->kostki[i].y+1][obecnyKlocek->kostki[i].x]==true)
        {
            isValid = false;
            for(int j=0; j<obecnyKlocek->iloscKostek; j++)
            {
                if((obecnyKlocek->kostki[i].x == obecnyKlocek->kostki[j].x) && (obecnyKlocek->kostki[i].y+1 == obecnyKlocek->kostki[j].y) && (obecnyKlocek->kostki[i].z == obecnyKlocek->kostki[j].z))
                {
                    isValid = true;
                }
            }

            if(isValid==false)
            {
                break;
            }
        }
    }

    if(isValid)
    {
        t.tablica[7] = t.tablica[7]+100;
        zmianaPolozeniaKlocka(0);
        for(int i=0; i<obecnyKlocek->iloscKostek; i++)
        {
            obecnyKlocek->kostki[i].setValues(obecnyKlocek->kostki[i].x,obecnyKlocek->kostki[i].y+1,obecnyKlocek->kostki[i].z);
        }
    }
}

//Funkcja odpowiedzialna za przesuniecie w gore//
void MainWindow::moveUp()
{
    bool isValid = true;
    for(int i=0; i<obecnyKlocek->iloscKostek; i++)
    {
        if(obecnyKlocek->kostki[i].y==0)
        {
            isValid = false;
            break;
        }
        else if(plansza[obecnyKlocek->kostki[i].z][obecnyKlocek->kostki[i].y-1][obecnyKlocek->kostki[i].x]==true)
        {
            isValid = false;
            for(int j=0; j<obecnyKlocek->iloscKostek; j++)
            {
                if((obecnyKlocek->kostki[i].x == obecnyKlocek->kostki[j].x) && (obecnyKlocek->kostki[i].y-1 == obecnyKlocek->kostki[j].y) && (obecnyKlocek->kostki[i].z == obecnyKlocek->kostki[j].z))
                {
                    isValid = true;
                }
            }

            if(isValid==false)
            {
                break;
            }
        }
    }

    if(isValid)
    {
        t.tablica[7] = t.tablica[7]-100;
        zmianaPolozeniaKlocka(0);
        for(int i=0; i<obecnyKlocek->iloscKostek; i++)
        {
            obecnyKlocek->kostki[i].setValues(obecnyKlocek->kostki[i].x,obecnyKlocek->kostki[i].y-1,obecnyKlocek->kostki[i].z);
        }
    }
}

//Funkcja odpowiedzialna za rotacje na osi X//
void MainWindow::moveRotateX(int x)
{
    bool isValid = true;
    int przejscia[8][2] = {{2,0},{1,1},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1}};

    for(int i=1; i<obecnyKlocek->iloscKostek; i++)
    {
        int wynik = gdziePolozonyKlocekWzgledemGlownegoKlockaX(obecnyKlocek->kostki[i].y, obecnyKlocek->kostki[i].z);
        if(wynik>=0)
        {
            if((obecnyKlocek->kostki[i].y + przejscia[wynik][0] >= 0))
            {
                if((obecnyKlocek->kostki[i].y + przejscia[wynik][0] < 5))
                {
                    if((obecnyKlocek->kostki[i].z + przejscia[wynik][1] >= 0))
                    {
                        if((obecnyKlocek->kostki[i].z + przejscia[wynik][1] < 7))
                        {
                        }
                        else
                        {
                            isValid = false;
                            break;
                        }
                    }
                    else
                    {
                        isValid = false;
                        break;
                    }
                }
                else
                {
                    isValid = false;
                    break;
                }
            }
            else
            {
                isValid = false;
                break;
            }
        }
    }

    if(isValid == true)
    {
        for(int i=1; i<obecnyKlocek->iloscKostek; i++)
        {
            int wynik = gdziePolozonyKlocekWzgledemGlownegoKlockaX(obecnyKlocek->kostki[i].y, obecnyKlocek->kostki[i].z);
            if(wynik>=0)
            {
                obecnyKlocek->kostki[i].y = obecnyKlocek->kostki[i].y + przejscia[wynik][0];
                obecnyKlocek->kostki[i].z = obecnyKlocek->kostki[i].z + przejscia[wynik][1];
            }
        }

        xRotation = xRotation+90;
        rX.tablica[5] = cos(xRotation * M_PI / 180);
        rX.tablica[6] = -sin(xRotation * M_PI / 180);
        rX.tablica[9] = sin(xRotation * M_PI / 180);
        rX.tablica[10] = cos(xRotation * M_PI / 180);

        if(x==0)
        {
            if(ostatniRazX!=3)
            {
                ostatniRazX++;
            }
            else
            {
                ostatniRazX = 0;
            }
            zmianaPolozeniaKlocka(0);
        }
        else
        {
            tworzenieNowegoKlocka(obecnyKlocek->rodzaj);
        }
    }
}

//Funkcja zwracajaca jak ma sie zachowac dany szescian w klocku podczas nastepnej rotacji na osi X//
int MainWindow::gdziePolozonyKlocekWzgledemGlownegoKlockaX(int y, int z)
{
    int srodekY;
    int srodekZ;
    srodekY = obecnyKlocek->kostki[0].y;
    srodekZ = obecnyKlocek->kostki[0].z;

    if(y<srodekY && z<srodekZ)
    {
        return 0;
    }
    else if(y<srodekY && z==srodekZ)
    {
        return 7;
    }
    else if(y<srodekY && z>srodekZ)
    {
        return 2;
    }
    else if(y==srodekY && z>srodekZ)
    {
        return 5;
    }
    else if(y==srodekY && z<srodekZ)
    {
        return 1;
    }
    else if(y>srodekY && z==srodekZ)
    {
        return 3;
    }
    else if(y>srodekY && z>srodekZ)
    {
        return 6;
    }
    else if(y>srodekY && z<srodekZ)
    {
        return 4;
    }
    else
    {
        return -1;
    }
}

//Funkcja odpowiedzialna za rotacje na osi Y//
void MainWindow::moveRotateY(int x)
{
    bool isValid = true;
    int przejscia[8][2] = {{2,0},{1,1},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1}};

    for(int i=1; i<obecnyKlocek->iloscKostek; i++)
    {
        int wynik = gdziePolozonyKlocekWzgledemGlownegoKlockaY(obecnyKlocek->kostki[i].x, obecnyKlocek->kostki[i].z);
        if(wynik>=0)
        {
            if((obecnyKlocek->kostki[i].x + przejscia[wynik][0] >= 0))
            {
                if((obecnyKlocek->kostki[i].x + przejscia[wynik][0] < 5))
                {
                    if((obecnyKlocek->kostki[i].z + przejscia[wynik][1] >= 0))
                    {
                        if((obecnyKlocek->kostki[i].z + przejscia[wynik][1] < 7))
                        {
                        }
                        else
                        {
                            isValid = false;
                            break;
                        }
                    }
                    else
                    {
                        isValid = false;
                        break;
                    }
                }
                else
                {
                    isValid = false;
                    break;
                }
            }
            else
            {
                isValid = false;
                break;
            }
        }
    }

    if(isValid == true)
    {

        for(int i=1; i<obecnyKlocek->iloscKostek; i++)
        {
            int wynik = gdziePolozonyKlocekWzgledemGlownegoKlockaY(obecnyKlocek->kostki[i].x, obecnyKlocek->kostki[i].z);
            if(wynik>=0)
            {
                obecnyKlocek->kostki[i].x = obecnyKlocek->kostki[i].x + przejscia[wynik][0];
                obecnyKlocek->kostki[i].z = obecnyKlocek->kostki[i].z + przejscia[wynik][1];
            }
        }

        yRotation = yRotation+90;
        rY.tablica[0] = cos(yRotation * M_PI / 180);
        rY.tablica[8] = -sin(yRotation * M_PI / 180);
        rY.tablica[2] = sin(yRotation * M_PI / 180);
        rY.tablica[10] = cos(yRotation * M_PI / 180);

        if(x==0)
        {
            if(ostatniRazY!=3)
            {
                ostatniRazY++;
            }
            else
            {
                ostatniRazY = 0;
            }
            zmianaPolozeniaKlocka(0);
        }
        else
        {
            tworzenieNowegoKlocka(obecnyKlocek->rodzaj);
        }
    }
}

//Funkcja zwracajaca jak ma sie zachowac dany szescian w klocku podczas nastepnej rotacji na osi Y//
int MainWindow::gdziePolozonyKlocekWzgledemGlownegoKlockaY(int x, int z)
{
    int srodekX;
    int srodekZ;
    srodekX = obecnyKlocek->kostki[0].x;
    srodekZ = obecnyKlocek->kostki[0].z;

    if(x<srodekX && z<srodekZ)
    {
        return 2;
    }
    else if(x<srodekX && z==srodekZ)
    {
        return 1;
    }
    else if(x<srodekX && z>srodekZ)
    {
        return 0;
    }
    else if(x==srodekX && z>srodekZ)
    {
        return 7;
    }
    else if(x==srodekX && z<srodekZ)
    {
        return 3;
    }
    else if(x>srodekX && z==srodekZ)
    {
        return 5;
    }
    else if(x>srodekX && z>srodekZ)
    {
        return 6;
    }
    else if(x>srodekX && z<srodekZ)
    {
        return 4;
    }
    else
    {
        return -1;
    }
}

//Funkcja odpowiedzialna za rotacje na osi Z//
void MainWindow::moveRotateZ(int x)
{
    bool isValid = true;

    int przejscia[8][2] = {{2,0},{1,1},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1}};

    for(int i=1; i<obecnyKlocek->iloscKostek; i++)
    {
        int wynik = gdziePolozonyKlocekWzgledemGlownegoKlockaZ(obecnyKlocek->kostki[i].x, obecnyKlocek->kostki[i].y);
        if(wynik>=0)
        {
            if((obecnyKlocek->kostki[i].x + przejscia[wynik][0] >= 0))
            {
                if((obecnyKlocek->kostki[i].x + przejscia[wynik][0] < 5))
                {
                    if((obecnyKlocek->kostki[i].y + przejscia[wynik][1] >= 0))
                    {
                        if((obecnyKlocek->kostki[i].y + przejscia[wynik][1] < 5))
                        {
                        }
                        else
                        {
                            isValid = false;
                            break;
                        }
                    }
                    else
                    {
                        isValid = false;
                        break;
                    }
                }
                else
                {
                    isValid = false;
                    break;
                }
            }
            else
            {
                isValid = false;
                break;
            }
        }
    }

    if(isValid == true)
    {

        for(int i=1; i<obecnyKlocek->iloscKostek; i++)
        {
            int wynik = gdziePolozonyKlocekWzgledemGlownegoKlockaZ(obecnyKlocek->kostki[i].x, obecnyKlocek->kostki[i].y);
            if(wynik>=0)
            {
                obecnyKlocek->kostki[i].x = obecnyKlocek->kostki[i].x + przejscia[wynik][0];
                obecnyKlocek->kostki[i].y = obecnyKlocek->kostki[i].y + przejscia[wynik][1];
            }
        }
        zRotation = zRotation+90;
        rZ.tablica[0] = cos(zRotation * M_PI / 180);
        rZ.tablica[1] = -sin(zRotation * M_PI / 180);
        rZ.tablica[4] = sin(zRotation * M_PI / 180);
        rZ.tablica[5] = cos(zRotation * M_PI / 180);

        if(x==0)
        {
            if(ostatniRazZ!=3)
            {
                ostatniRazZ++;
            }
            else
            {
                ostatniRazZ = 0;
            }
            zmianaPolozeniaKlocka(0);
        }
        else
        {
            tworzenieNowegoKlocka(obecnyKlocek->rodzaj);
        }
    }
}

//Funkcja zwracajaca jak ma sie zachowac dany szescian w klocku podczas nastepnej rotacji na osi Z//
int MainWindow::gdziePolozonyKlocekWzgledemGlownegoKlockaZ(int x, int y)
{
    int srodekX;
    int srodekY;
    srodekX = obecnyKlocek->kostki[0].x;
    srodekY = obecnyKlocek->kostki[0].y;

    if(x<srodekX && y<srodekY)
    {
        return 0;
    }
    else if(x<srodekX && y==srodekY)
    {
        return 7;
    }
    else if(x<srodekX && y>srodekY)
    {
        return 6;
    }
    else if(x==srodekX && y>srodekY)
    {
        return 5;
    }
    else if(x==srodekX && y<srodekY)
    {
        return 1;
    }
    else if(x>srodekX && y==srodekY)
    {
        return 3;
    }
    else if(x>srodekX && y>srodekY)
    {
        return 4;
    }
    else if(x>srodekX && y<srodekY)
    {
        return 2;
    }
    else
    {
        return -1;
    }
}

//Funkcja odpowiedzialna za opuszczenie klocka na dol planszy//
void MainWindow::dropSquare()
{
    bool isValid = true;

    while(isValid)
    {
        for(int i=0; i<obecnyKlocek->iloscKostek; i++)
        {
            if(obecnyKlocek->kostki[i].z==6)
            {
                isValid = false;
            }
            else if(plansza[obecnyKlocek->kostki[i].z+1][obecnyKlocek->kostki[i].y][obecnyKlocek->kostki[i].x]==true)
            {
                isValid = false;
            }
        }

        if(isValid)
        {
            t.tablica[11] = t.tablica[11]+100;
            for(int i=0; i<obecnyKlocek->iloscKostek; i++)
            {
                obecnyKlocek->kostki[i].setValues(obecnyKlocek->kostki[i].x,obecnyKlocek->kostki[i].y,obecnyKlocek->kostki[i].z+1);
            }
        }
    }

    for(int i=0; i<obecnyKlocek->iloscKostek; i++)
    {
        if(obecnyKlocek->kostki[i].z==0)
        {
            endGame();
        }
    }

    for(int i=0; i<obecnyKlocek->iloscKostek; i++)
    {
        plansza[obecnyKlocek->kostki[i].z][obecnyKlocek->kostki[i].y][obecnyKlocek->kostki[i].x] = true;
    }

    kopiowaniePlansz(0);
    malujCalyKlocek();
    kopiowaniePlansz(1);
    checkBoard();
    tworzenieNowegoKlocka(-1);
}

//Funkcja wywolujaca sie jesli przegramy//
void MainWindow::endGame()
{
    QMessageBox msgBox;
    msgBox.setText("         PORAŻKA                   ");
    int ret = msgBox.exec();

    switch (ret) {
      default:
          QCoreApplication::quit();
          break;
    }
}

//Funkcja odpowiedzialna za czyszczenie planszy podczas zapelnienia, dodanie punktow, oraz sprawdzanie czy przegralismy//
void MainWindow::checkBoard()
{
    bool usun = true;
    int iloscUsunietychWierszy = 0;

    //Sprawdzam czy powinienem zakonczyc gre//
    for(int x=0; x<=4; x++)
    {
        for(int y=0; y<=4; y++)
        {
            if(plansza[0][y][x]==true)
            {
                endGame();
                break;
            }
        }
    }

    //sprawdzam czy moge usunac dany poziom//
    for(int i = 6; i>0; i--)
    {
        usun = true;
        for(int x=0; x<=4; x++)
        {
            for(int y=0; y<=4; y++)
            {
                if(plansza[i][y][x]==false)
                {
                    usun = false;
                }
            }
        }

        if(usun==true)
        {
            iloscUsunietychWierszy++;
            //zeruje wyczyszczony poziom//
            for(int x=0; x<=4; x++)
            {
                for(int y=0; y<=4; y++)
                {
                    plansza[i][y][x]=false;
                }
            }

            //opadanie reszty poziomow//
            for(int j = i; j>0; j--)
            {
                for(int x=0; x<=4; x++)
                {
                    for(int y=0; y<=4; y++)
                    {
                        plansza[j][y][x]=plansza[j-1][y][x];
                    }
                }

                for(int x=0; x<=4; x++)
                {
                    for(int y=0; y<=4; y++)
                    {
                        plansza[j-1][y][x]=false;
                    }
                }
            }
            i=7;
        }
    }

    //graficzne usuniecie poziomow, aktualizacja planszy, wyzerowanie zmiennych//
    if(iloscUsunietychWierszy!=0)
    {
        kopiowaniePlansz(3);
        kopiowaniePlansz(4);

        for(int i=0; i<16; i++)
        {
            if(i%5==0)
            {
                t.tablica[i] = 1;
                rX.tablica[i] = 1;
                rY.tablica[i] = 1;
                rZ.tablica[i] = 1;
            }
            else
            {
                t.tablica[i] = 0;
                rX.tablica[i] = 0;
                rY.tablica[i] = 0;
                rZ.tablica[i] = 0;
            }
        }

        for(int z=6; z>=0; z--)
        {
            for(int x=0; x<=4; x++)
            {
                for(int y=0; y<=4; y++)
                {
                    if(plansza[z][y][x] == true)
                    {
                        int a = -50 -(100*(2-x));
                        int b = -50 -(100*(2-y));
                        ustawKwadrat(0,a,b,z);
                        obecnyKlocek->iloscKostek = 1;
                        obecnyKlocek->kostki[0].setValues(x,y,z);
                        malujCalyKlocek();
                    }
                }
            }
        }

        kopiowaniePlansz(1);
        kopiowaniePlansz(0);
        changeScore(iloscUsunietychWierszy);
    }
}

//Funckja odpowiedzialna za kolorowanie klocka//
void MainWindow::malujCalyKlocek()
{
    zmianaPolozeniaKlocka(2);
    for(int i=0; i<obecnyKlocek->iloscKostek; i++)
    {
        teksturowanie(0,1,2,i);
        teksturowanie(1,2,3,i);
        teksturowanie(0,1,5,i);
        teksturowanie(0,1,4,i);
        teksturowanie(1,3,5,i);
        teksturowanie(3,5,7,i);
        teksturowanie(0,2,4,i);
        teksturowanie(2,4,6,i);
        teksturowanie(4,5,6,i);
        teksturowanie(5,6,7,i);
        teksturowanie(2,3,6,i);
        teksturowanie(3,6,7,i);
    }
}

//Funkcja ktora koloruje jedna polowe sciany//
void MainWindow::teksturowanie(int a, int b, int c, int numerKostki)
{
    float w,v,u;
    float W,Wv,Ww;
    W = ((obecnyKlocek->kostki[numerKostki].wierzcholki[b].x-obecnyKlocek->kostki[numerKostki].wierzcholki[a].x)*(obecnyKlocek->kostki[numerKostki].wierzcholki[c].y-obecnyKlocek->kostki[numerKostki].wierzcholki[a].y)) - ((obecnyKlocek->kostki[numerKostki].wierzcholki[b].y-obecnyKlocek->kostki[numerKostki].wierzcholki[a].y)*(obecnyKlocek->kostki[numerKostki].wierzcholki[c].x-obecnyKlocek->kostki[numerKostki].wierzcholki[a].x));
    for(int i=0; i<500; i++)
    {
        for(int j=0; j<500; j++)
        {
            Wv = ((j-obecnyKlocek->kostki[numerKostki].wierzcholki[a].x)*(obecnyKlocek->kostki[numerKostki].wierzcholki[c].y-obecnyKlocek->kostki[numerKostki].wierzcholki[a].y)) - ((i-obecnyKlocek->kostki[numerKostki].wierzcholki[a].y)*(obecnyKlocek->kostki[numerKostki].wierzcholki[c].x-obecnyKlocek->kostki[numerKostki].wierzcholki[a].x));
            Ww = ((obecnyKlocek->kostki[numerKostki].wierzcholki[b].x-obecnyKlocek->kostki[numerKostki].wierzcholki[a].x)*(i-obecnyKlocek->kostki[numerKostki].wierzcholki[a].y)) - ((obecnyKlocek->kostki[numerKostki].wierzcholki[b].y-obecnyKlocek->kostki[numerKostki].wierzcholki[a].y)*(j-obecnyKlocek->kostki[numerKostki].wierzcholki[a].x));
            v = Wv/W;
            w = Ww/W;
            u = 1-(v+w);
            if(v>=0 && v<=1 && w>=0 && w<=1 && u>=0 && u<=1)
            {
                kolorujPiksel(i,j,numerKostki);
            }
        }
    }
}

//Funkcja odpowiedzialna za wybor koloru klocka//
void MainWindow::kolorujPiksel(int i, int j, int numerKostki)
{
    int kolory[7][3] = {{255,0,0},{0,0,255},{0,255,0},{255,128,0},{255,0,255},{0,128,255},{128,128,128}};
    unsigned char *ptr;
    ptr = img->bits();
    if(obecnyKlocek->kostki[numerKostki].z < zBufor[i*500+j])
    {
        ptr[szer*4*i + 4*j] = kolory[obecnyKlocek->kostki[numerKostki].z][0];
        ptr[szer*4*i + 4*j + 1] = kolory[obecnyKlocek->kostki[numerKostki].z][1];
        ptr[szer*4*i + 4*j + 2] = kolory[obecnyKlocek->kostki[numerKostki].z][2];
        zBufor[500*i+j] = obecnyKlocek->kostki[numerKostki].z;
    }
}

void MainWindow::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    p.drawImage(0,0,*img);
}

MainWindow::~MainWindow()
{
    delete zBufor;
    delete img;
    delete tlo;
}
