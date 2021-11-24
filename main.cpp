#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;
ifstream fin("abce.in");
ofstream fout("abce.out");

const int t=700;

struct nod
{
    int chei[2*t];
    nod* ptr_copii[2*t+1]={nullptr}; //pointeri spre copii
    int nr_chei;

    bool frunza;

}*radacina=nullptr;

int val_posibila;


void Elimina(nod* nod_curent, int val); //se apeleaza circular functii

nod* Cauta(nod* nod_curent, int val)
{
    ///cautare binara in nodul curent
     if(nod_curent == nullptr) return nullptr;


    auto up=upper_bound(nod_curent->chei, nod_curent->chei+ nod_curent->nr_chei, val);
    int poz_up=up-nod_curent->chei; //pozitia in vector unde e upper bound

    ///valorile sunt distincte!!
    if(poz_up-1>=0 && val == nod_curent->chei[poz_up-1]) //am gasit chiar valoarea
        return nod_curent;

    if (nod_curent->frunza)
        return nullptr;

    return Cauta(nod_curent->ptr_copii[poz_up], val);
}

void DesparteCopii(nod * parinte, nod *de_divizat, int poz) //parinte, fiu de divizat care va da o cheie parintelui, pozitia unde vine cheia noua
{
    nod * fiu_dreapta= new nod;
    fiu_dreapta->frunza=de_divizat->frunza; //se mosteneste proprietatea de frunza sau nu
    fiu_dreapta->nr_chei=t-1;

    //mut ultimele t-1 chei in fiul din dreapta
    for(int i=t; i<2*t-1; ++i)
        fiu_dreapta->chei[i-t]=de_divizat->chei[i];

    //mut ultimii t poineri spre copii in fiul din dreapta
    if(de_divizat->frunza==0)
        for(int i=t; i<2*t; ++i)
            fiu_dreapta->ptr_copii[i-t]=de_divizat->ptr_copii[i];

    de_divizat->nr_chei=t-1; //devine nodul din stanga

    //mut pointerii catre fii de la mijloc spre dreapta ca sa fac loc
    for(int i=parinte->nr_chei; i>=poz+1; --i)
        parinte->ptr_copii[i+1]=parinte->ptr_copii[i];

    //fac legatura cu fiul drept
    parinte->ptr_copii[poz+1]=fiu_dreapta;

    //mut cheile spre dreapta ca sa fac loc
    for(int i=parinte->nr_chei-1; i>=poz; --i)
        parinte->chei[i+1]=parinte->chei[i];

    parinte->chei[poz]=de_divizat->chei[t-1]; //urc cheia din mijloc

    parinte->nr_chei++; //maresc nr de chei ale parintelui

}

void Insereaza2(nod* nod_curent, int val) //pentru nod ne-full
{
    if(nod_curent->frunza) //e frunza
    {
        int i=nod_curent->nr_chei-1; //plec din capatul din dreapta

        //muta spre dreapta cheile mai mari
        while (i>=0 && nod_curent->chei[i]>val)
        {
            nod_curent->chei[i+1]=nod_curent->chei[i];
            i--;
        }

        //pune noua valoare
        nod_curent->chei[i+1]=val;
        nod_curent->nr_chei++;
    }
    else //nu e frunza
    {
        //cauta fiul bun
        int i=nod_curent->nr_chei-1; //plec din capatul din dreapta

        while(i>=0 && nod_curent->chei[i]>val)
            i--;


        //verifica daca fiul gasit e plin sau nu
        if(nod_curent->ptr_copii[i+1]->nr_chei==2*t-1)
        {

            DesparteCopii(nod_curent, nod_curent->ptr_copii[i+1],i+1);
            if(nod_curent->chei[i+1] < val) //daca valoarea nou urcata e mai mica, avansez
                i++;
        }

        Insereaza2(nod_curent->ptr_copii[i+1], val); //apelez inserarea pe fiu
    }

}

void Insereaza(int val)
{

    if(radacina!=nullptr)
    {
        if(radacina->nr_chei==2*t-1) //radacina full
        {
            nod * temp= new nod; //va deveni noua radacina
            temp->frunza=0;
            //il leg pe stanga pe cel pe care urmeaza sa l divizez
            temp->ptr_copii[0]=radacina;
            temp->nr_chei=0;


            DesparteCopii(temp, radacina, 0);

            //radacina are 2 fii acum
            //pun cheia noua in unul dintre ei
            int nr_fiu;

            if(val <= temp->chei[0]) //valoarea mea e mai mica decat prima cheie
                nr_fiu=0;
            else
                nr_fiu=1;

            Insereaza2(temp->ptr_copii[nr_fiu],val);

            radacina=temp; //actualizez radacina
        }
        else
            Insereaza2(radacina, val);

    }
    else
    {
        //nu exista radacina
        nod* temp=new nod;
        temp->frunza=1;
        temp->nr_chei=1;
        temp->chei[0]=val;

        radacina=temp;
    }

}

void Uneste (nod* parinte, int poz_ptr_copil)
{
    //uneste copilul cu fratele din dreapta in nodul copil

    nod* copil=parinte->ptr_copii[poz_ptr_copil];
    nod* frate=parinte->ptr_copii[poz_ptr_copil+1];

    //parintele coboara la copil
    // mut cheile parintelui  si pointerii o pozitie spre stanga
    copil->chei[++copil->nr_chei]=parinte->chei[poz_ptr_copil];

    //muta cheile fratelui din dreapta in copil
    //si muta si pointerii spre copii
    for(int i=0; i<frate->nr_chei; ++i)
        copil->chei[i+t]= frate->chei[i];
    if(copil->frunza==0)
        for(int i=0; i<= frate->nr_chei; ++i)
            copil->ptr_copii[i+t]=frate->ptr_copii[i];


    for(int i=poz_ptr_copil+1; i < parinte->nr_chei; ++i)
        parinte->chei[i-1]=parinte->chei[i];

    for(int i=poz_ptr_copil+2; i<=parinte->nr_chei; ++i)
        parinte->ptr_copii[i-1] = parinte->ptr_copii[i];

    //actualizez numarul de chei
    parinte->nr_chei--;
    copil->nr_chei += frate->nr_chei +1;

    delete frate;
    return;

}

void ImprumutaDeLaFrateleStang(nod * parinte, int poz_ptr_copil)
{
    nod * copil=parinte->ptr_copii[poz_ptr_copil];
    nod * frate=parinte->ptr_copii[poz_ptr_copil-1];

    //ultima cheie din frate se duca in parinte si parintele se duce in copil pe prima pozitie

    //mut cheile si pointerii (daca nu e frunza) din copil la dreapta cu o pozitie
    for(int i=copil->nr_chei-1; i>=0; --i)
        copil->chei[i+1]=copil->chei[i];

    if(copil->frunza==0)
        for(int i=copil->nr_chei;   i>=0; --i)
            copil->ptr_copii[i+1]=copil->ptr_copii[i];


    //mut parintele in copil
    copil->chei[0]=parinte->chei[poz_ptr_copil-1];

    //mut ultimul copil al fratelui la copil, daca nu e frunza
    if(copil->frunza==0)
        copil->ptr_copii[0]=frate->ptr_copii[frate->nr_chei];

    //mut fratele drept in parinte (ultima cheie din frate)
    parinte->chei[poz_ptr_copil-1] = frate->chei[(frate->nr_chei)-1];

    copil->nr_chei++;
    frate->nr_chei--;
    return;

}

void ImprumutaDeLaFrateleDrept(nod * parinte, int poz_ptr_copil)
{
    nod * copil=parinte->ptr_copii[poz_ptr_copil];
    nod * frate=parinte->ptr_copii[poz_ptr_copil+1];

    //cheia parintelui vine pe ultima pozitie a cheilor copilului
    copil->chei[copil->nr_chei]=parinte->chei[poz_ptr_copil];
    //mut si pointerul, daca nu e frunza
    if(copil->frunza==0)
        copil->ptr_copii[(copil->nr_chei)+1]= frate->ptr_copii[0];

    //mut cheia fratelui in parinte
    parinte->chei[poz_ptr_copil]=frate->chei[0];

    //mut cheile fratelui un pas la dreapta
    for(int i=1; i<frate->nr_chei; ++i)
        frate->chei[i-1]=frate->chei[i];

    //mut si pointerii fratelui un pas la dreapta, daca nu e frunza
    if(frate->frunza==0)
        for(int i=1; i<= frate->nr_chei; ++i)
            frate->ptr_copii[i-1]=frate->ptr_copii[i];

    copil->nr_chei++;
    frate->nr_chei--;
    return;
}

void Completeaza(nod * parinte, int poz_ptr_copil)
{
    //nodul parinte si pozitia pointeruui pentru copilul ce trebuie completat
    //daca fratele din stanga exista si  are mai mult de t-1 noduri ma imprumut de la el

    if(poz_ptr_copil!=0 && parinte->ptr_copii[poz_ptr_copil-1]->nr_chei>t-1)
        ImprumutaDeLaFrateleStang(parinte, poz_ptr_copil);

    //incerc cu fratele drept
    else if (poz_ptr_copil!=parinte->nr_chei && parinte->ptr_copii[poz_ptr_copil+1]->nr_chei>t-1)
        ImprumutaDeLaFrateleDrept(parinte, poz_ptr_copil);

    else
        //trebuie sa cobor parintele si sa unesc fratii
        //daca nodul problematic e ultimul, atunci unesc cu fratele din stanga
        //daca nu e ultimul, unesc cu fratele din dreapta
        if(poz_ptr_copil == parinte->nr_chei)
            Uneste(parinte, poz_ptr_copil-1);
        else
            Uneste(parinte, poz_ptr_copil);
    return;

}
void StergeDinFrunza(nod* nod_curent, int poz)
{
    //sterge cheia de pe pozitia poz din nodul curent
    for(int i=poz+1; i<nod_curent->nr_chei; ++i)
        nod_curent->chei[i-1]=nod_curent->chei[i];

    nod_curent->nr_chei--;

    return;
}

int GasestePredecesor(nod * nod_curent, int poz_copil)
{
    nod* temp=nod_curent->ptr_copii[poz_copil];

    while(temp->frunza==0) //cat timp nu e frunza, merg pe dreapta
        temp=temp->ptr_copii[temp->nr_chei];

    return temp->chei[(temp->nr_chei)-1]; //cea mai din stanga cheie

}

int GasesteSuccesor(nod* nod_curent, int poz_copil)
{
    nod* temp=nod_curent->ptr_copii[poz_copil+1];

    while(temp->frunza==0) //cat timp nu e frunza, merg pe stanga
        temp=temp->ptr_copii[0];

    return temp->chei[0]; //cea mai din stanga cheie

}

void StergeDinNodIntern(nod * nod_curent, int poz)
{
    //sterge cheia de pe pozitia poz din nodul curent
    //caz 2
    int val=nod_curent->chei[poz];


    //verfic daca fiul din stanga are mai mult de t-1 chei
    if(nod_curent->ptr_copii[poz]->nr_chei>t-1)
    {
        //caz 2A
        //caut predecesorul valorii din nod si il pun peste valoarea eliminata

        int predecesor=GasestePredecesor(nod_curent,poz);
        nod_curent->chei[poz]=predecesor;
        Elimina(nod_curent->ptr_copii[poz], predecesor); //elimina predecesorul care se afla in subarborele cu radacina in fiul stang al nodului curent



    }
    //verific daca fiul din dreapta are mai mult de t-1 chei
    else if(nod_curent->ptr_copii[poz+1]->nr_chei>t-1)
    {
        //caz 2B
        //caut succesorul valorii din nod si il pun peste valoarea eliminata

        int succesor=GasesteSuccesor(nod_curent,poz);
        nod_curent->chei[poz]=succesor;
        Elimina(nod_curent->ptr_copii[poz+1], succesor); //elimina succesorul care se afla in subarborele cu radacina in fiul drept al nodului curent

    }
    else
    {
        //caz 2C
        //trebuie sa merge-uiesc pentru ca nu ma pot imprumuta
        Uneste(nod_curent, poz);
        Elimina(nod_curent->ptr_copii[poz], val); //Se aplica mai departe procesul de stergere recursiva a lui val din nodul curent

    }

    return;

}

int GasesteCheie(nod* nod_curent, int val)
{
    //returneaza pozitia pe care se afla valoarea sau unde se afla prima valoare cea mai mare
    ///cautare binara in nodul curent
    auto up=upper_bound(nod_curent->chei, nod_curent->chei+ nod_curent->nr_chei, val);
    int poz_up=up-nod_curent->chei; //pozitia in vector unde e upper bound

    ///valorile sunt distincte!!
    if(poz_up-1>=0 && val == nod_curent->chei[poz_up-1]) //am gasit chiar valoarea
        return poz_up-1;

    return poz_up;

}

void Elimina(nod* nod_curent, int val)
{
    //verific daca valoarea se afla in acest nod sau nu
    //daca se afla ma opresc pe poz ei
    //daca nu se afla, ma opresc la prima valoare mai mare
    //eficient-> upper_bound (timp logaritmic)

    int poz=GasesteCheie(nod_curent, val);

    if(poz < nod_curent->nr_chei && nod_curent->chei[poz]==val)
        //am gasit valoarea chiar in nod
    {
        if(nod_curent->frunza)
            //cazul 1
            StergeDinFrunza(nod_curent, poz);
        else
            //cazurile 2 si 3
            StergeDinNodIntern(nod_curent, poz);
    }
    else
    {
        //valoarea nu e in acest nod
        if(nod_curent->frunza)
        {
            cout<<"Nu exista acest nod!\n";
            return;
        }
        else
        {
            bool ultimul; //caz particular, retin daca trebuia sa merg pe extremitatea dreapta sau nu

            if(poz==nod_curent->nr_chei) //ar veni pe ultimul copil al nodului curent
                ultimul=1;

            else ultimul=0;

            //cheia pe care o caut se afla in subarborele cu radacina in acest nod
            if(nod_curent->ptr_copii[poz]->nr_chei< t )
                //daca nodul unde ar trebui sa se afle cheia are mai putin de t chei
                //trebuie sa-l mai umplu pentru ca o sa aiba < t-1 chei dupa ce o elimin pe a mea
                Completeaza(nod_curent, poz);

            if(ultimul && poz > nod_curent->nr_chei)
                //s-a facut un merge pe ultimul copil deci numarul de chei a scazut, deci
                //poz_up > nr de chei curent, si nu egal ca inainte
                //in plus, era ultimul nod
                Elimina(nod_curent->ptr_copii[poz-1], val); //ma mut cu unul spre stanga pentru ca am ramas in aer datorita merge-ului
            else
                //nu s-a produs niciun merge
                Elimina(nod_curent->ptr_copii[poz], val);

        }
    }

    return;

}

void Sterge(int val)
{
    if(radacina==nullptr)
    {
        cout<<"ARBORE GOL\n";
        return;
    }

    Elimina(radacina, val);

    if(radacina->nr_chei==0) //radacina a ramas faca chei, deci are
        //maxim un fiu
    {
        //il fac radacina pe fiu daca exista
        nod* temp=radacina; //pastrez undeva radacina ca sa o pot sterge fara memory leak

        if(radacina->frunza==1) //radacina era si frunza, deci nu are fii
            radacina=nullptr;
        else
            radacina=radacina->ptr_copii[0];

        delete temp;

    }

    return;

}

int CautaPredecesor(nod* nod_curent, int val)
{

   int poz=nod_curent->nr_chei-1;
   //cauta prima valoare mai mica
   while(poz>=0 && nod_curent->chei[poz]> val)
        poz--;

    if(poz>=0 && nod_curent->chei[poz]<val)   //concurent
        val_posibila=nod_curent->chei[poz];

    if(nod_curent->frunza)
        return val_posibila;

    return CautaPredecesor(nod_curent->ptr_copii[poz+1], val);

}

int Predecesor(int val)
{
    val_posibila=-1000000001;

    nod* rez=Cauta(radacina, val);
    if(rez==nullptr) //nu s-a gasit valoarea
    {
        return CautaPredecesor(radacina, val);
    }
    else //a gasit valoarea
        return val;
}

int CautaSuccesor(nod * nod_curent, int val)
{
    int poz = 0;
    //caut prima cheie mai mare
    while (poz < nod_curent->nr_chei && val > nod_curent->chei[poz])
        poz++;

    if(poz<nod_curent->nr_chei && nod_curent->chei[poz]>val)   //concurent
        val_posibila=nod_curent->chei[poz];

    if(nod_curent->frunza)
        return val_posibila;

    return CautaSuccesor(nod_curent->ptr_copii[poz], val);

}

int Succesor( int val)
{
    val_posibila=-1000000001;

    nod* rez=Cauta(radacina, val);
    if(rez==nullptr) //nu s-a gasit valoarea
    {
        return CautaSuccesor(radacina, val);
    }
    else //a gasit valoarea
        return val;

}

void Sortate(int x,int y)
{
    //merg din succesor in succesor

    int ant=x, urmator;
    int afisat;

    urmator=Succesor(ant);
    if(urmator<=y && urmator!=-1000000001) //exista si e mai mic decat y
        fout<<urmator<<" ";
    else
        return;

    ant=urmator;
    afisat=urmator;

    while(1)
    {
        urmator=Succesor(ant);
        if(urmator==afisat) //e valoare din arbore deci a mai fost gasita inainte
        {
            if(urmator+1<=y)
            {
                urmator++;
                ant=urmator;
                continue;
            }
            else
                return;
        }

        if(urmator<=y && urmator!=-1000000001) //exista, n-a mai fost afisat  si e mai mic decat y
            {fout<<urmator<<" "; afisat=urmator;}
        else //am trecut limita sau nu mai exista succesori
            return;

        ant=urmator;
    }

}

int main()
{
    int q,op,x,y;
    fin>>q;
    for(int i=0; i<q; ++i)
    {
        fin>>op>>x;
        if(op==1)
            Insereaza(x);

        else if(op==2)
            Sterge(x); //pleaca din radacina

        else if(op==3)
        {
            if(radacina ==nullptr)
            {
                fout<<"0\n";
                continue;
            }

            nod* rez=Cauta(radacina, x);
            if(rez==nullptr)
                fout<<"0\n";
            else
                fout<<"1\n";

        }
        else if(op==4)
            fout<<Predecesor(x)<<"\n";

        else if(op==5)
            fout<<Succesor(x)<<"\n";

        else //op=6
        {
            fin>>y;
            Sortate(x,y);
            fout<<"\n";
        }
    }

    return 0;
}
