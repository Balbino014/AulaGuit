#ifndef BOOT_H
#define BOOT_H

#include <Arduino.h>

class Boot 
{
private:
    int pino;
    bool ultimoEstado;
    bool estadoAtual;

public:
    Boot(int pin);
    void atualizar();
    bool pressionado();
    bool solto();
};

#endif