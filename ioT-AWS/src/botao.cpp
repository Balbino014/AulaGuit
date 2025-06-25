#include "botao.h"

Boot::Boot(int pin) 
{
    pino = pin;
    pinMode(pino, INPUT_PULLUP);
    ultimoEstado = digitalRead(pino);
    estadoAtual = ultimoEstado;

}



void Boot::atualizar() 
{
    ultimoEstado = estadoAtual;
    estadoAtual = digitalRead(pino);
}

bool Boot::pressionado()
{
    bool estadoAtual = digitalRead(pino);
    bool foiPressionado = (estadoAtual == LOW && ultimoEstado == HIGH);
    ultimoEstado = estadoAtual;
    return foiPressionado;
}

bool Boot::solto() 
{
    bool estadoAtual = digitalRead(pino);
    bool foiSolto = (estadoAtual == HIGH && ultimoEstado == LOW);
    ultimoEstado = estadoAtual;
    return foiSolto;
}