#include <menu.h>
#include <Arduino.h>
//---------------------opcions concretes
class fesAlgo : public menuOption{
    public:
        int pin;
        fesAlgo(int pin);
        void run();
};
class fesAlgoAltre : public menuOption{
    public:
        int pin;
        fesAlgoAltre(int pin);
        void run();
};
class canviaMenuPrincipal : public menuOption{

    public:
        menu * menuPantalla;
        canviaMenuPrincipal(menu * menuPantalla);
        void run();
};
class escriuPerPantalla : public menuOption{
    public:
        Stream *userTty;
        escriuPerPantalla();
        void run();
};