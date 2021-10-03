#include <QMainWindow>
#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QGroupBox>
#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsScene>
#include <QTimer>
#include <QMessageBox>

class Macierz
{
public:

    float tablica[16];

    Macierz()
    {
        for(int i=0; i<16; i++)
        {
            if(i%5==0)
            {
                tablica[i] = 1;
            }
            else
            {
                tablica[i] = 0;
            }
        }
    }

    Macierz operator *( const Macierz &v )
    {
        Macierz wynik;
        for(int j=0; j<4; j++)
        {
            for(int i=0; i<4; i++)
            {
                wynik.tablica[i+(4*j)] = (tablica[0+(4*j)]*v.tablica[0+i]) + (tablica[1+(4*j)]*v.tablica[4+i]) + (tablica[2+(4*j)]*v.tablica[8+i]) + (tablica[3+(4*j)]*v.tablica[12+i]);
            }
        }
        return wynik;
    }
};

class Wierzcholek
{
public:
    float x;
    float y;
    float z;

    void setValues(int a, int b, int c)
    {
        x = a;
        y = b;
        z = c;
    }
};

class Wektor
{
public:
    float x;
    float y;
    float z;

    Wektor()
    {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    Wektor(Wierzcholek w1, Wierzcholek w2)
    {
        this->x = w2.x - w1.x;
        this->y = w2.y - w1.y;
        this->z = w2.z - w1.z;
    }
};

class Kostka
{
public:
    int x;
    int y;
    int z;

    Wierzcholek wierzcholki[8];

    void setValues(int a, int b, int c)
    {
        x = a;
        y = b;
        z = c;
    }
};

class Klocek
{
public:
    int iloscKostek;
    int rodzaj;
    Kostka *kostki;
    Klocek(int x, int y)
    {
        rodzaj = y;
        iloscKostek = x;
        kostki = new Kostka[x];
    }
};

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QImage *img;
    QImage *img2;
    QImage *tlo;
    QImage *tlo2;
    QGraphicsScene *scena;
    QGroupBox *grupa;
    QVBoxLayout *layout;
    QPushButton *drop;
    QPushButton *right;
    QPushButton *left;
    QPushButton *down;
    QPushButton *up;
    QPushButton *rotateX;
    QPushButton *rotateY;
    QPushButton *rotateZ;
    QLabel *score;
    QTimer *timer;
    int szer;
    int wys;
    Macierz t,rX,rY,rZ;
    Wierzcholek wierzcholkiPom[32];
    bool plansza[7][5][5];
    Klocek *obecnyKlocek;
    int xRotation = 0;
    int yRotation = 0;
    int zRotation = 0;
    int *zBufor;
    int wynikk = 0;
    int ostatniRazX = 0;
    int ostatniRazY = 0;
    int ostatniRazZ = 0;

    // Deklaracje funkcji
    void wstawPiksel(int i, int j, int warstwa, int z);
    void piszLinie(float x1, float y1, float x2, float y2, int warstwa, int z);
    void kopiowaniePlansz(int x);
    void kolorujPlanszeNaBialo();
    void rysujKlocek(int mode);
    void zmianaPolozeniaKlocka(int mode);
    void tworzenieNowegoKlocka(int pom);
    bool powierzchniaNiewidoczna(int a, int b, int c, int numerKostki);
    void planszaStart();
    void endGame();
    void losujKlocek(int pom);
    void ustawKwadrat(int i, int x, int y, int z);
    void ustawTlo();
    void teksturowanie(int x, int y, int z, int numerKostki);
    void cieniowanie(int w1, int w2, int w3, int numerKostki);
    void malujCalyKlocek();
    void kolorujPiksel(int i, int j, int numerKostki);
    void checkBoard();
    void changeScore(int x);
    int gdziePolozonyKlocekWzgledemGlownegoKlockaZ(int x, int y);
    int gdziePolozonyKlocekWzgledemGlownegoKlockaY(int x, int z);
    int gdziePolozonyKlocekWzgledemGlownegoKlockaX(int y, int z);
    void moveRotateX(int x);
    void moveRotateY(int x);
    void moveRotateZ(int x);

private slots:
    void paintEvent(QPaintEvent*);
    void moveRight();
    void moveLeft();
    void moveDown();
    void moveUp();
    void dropSquare();
    void goDown();
    void pomX();
    void pomY();
    void pomZ();
};
