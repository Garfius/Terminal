#include <menu.h>
//---------------------opcions concretes
class fesAlgo : public menuOption{
    public:
        fesAlgo();
        void run();
};
class fesAlgoAltre : public menuOption{
    public:
        fesAlgoAltre();
        void run();
};
class canviaMenuPrincipal : public menuOption{

    public:
        menu * menuPantalla;
        canviaMenuPrincipal(menu * menuPantalla);
        void run();
};


