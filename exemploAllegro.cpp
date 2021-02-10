/*==========================================================================================================*/
// Bibliotecas
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <sstream>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define HITBOX 5
#define MAXIMO_CENARIO 899
#define MAX_OBJETOS 80

/*==========================================================================================================*/
// Namespaces
using namespace std;

int personagemChao();

/*==========================================================================================================*/
// Constantes
const float FPS = 60;
const int SCREEN_W = 950;
const int SCREEN_H = 600;
const int TAMANHO_QUADRADO = 50;

/*==========================================================================================================*/
// Classes
class InterfaceEntidade
{
public:
    virtual void setPosicao(double x, double y) = 0;
    virtual void setTamanho(int largura, int altura) = 0;
    virtual void setImagem(ALLEGRO_BITMAP* imagem) = 0;
};

class Coordenadas
{
public:
    double x, y;
};

class Ator: public InterfaceEntidade, public Coordenadas
{
public:
    int largura, altura;
    double dx, dy;
    static int numeroAtores;

    Ator()
    {
        numeroAtores++;
    }

    void setPosicao(double x, double y)
    {
        this->x = x;
        this->y = y;
    }

    void setTamanho(int largura, int altura)
    {
        this->largura = largura;
        this->altura = altura;
    }

    void setDeslocamento(double dx, double dy)
    {
        this->dx = dx;
        this->dy = dy;
    }

    //Sobrecarga do metodo anterior quando o deslocamento em x e y forem iguais
    void setDeslocamento(double dx)
    {
        this->dx = dx;
        this->dy = dx;
    }

    double getX()
    {
        return(x);
    }

    double getY()
    {
        return(y);
    }

    double getDeslocamentoX()
    {
        return(dx);
    }

    double getDeslocamentoY()
    {
        return(dy);
    }

    int getAltura()
    {
        return(altura);
    }

    int getLargura()
    {
        return(largura);
    }
};

int Ator::numeroAtores = 0;

class Inimigo : public Ator
{
public:
    ALLEGRO_BITMAP *bmp;

    virtual void setImagem(ALLEGRO_BITMAP* imagem)
    {
        this->bmp = imagem;
    }

    void movimenta()
    {
        x += dx;
    }
};

class Jogador : public Inimigo
{
private:
    int fase;

public:
    ALLEGRO_BITMAP *bmp;
    int morreu;
    int vidas;
    int p;
    int i;

    //Sobreposicao da classe inimigo quando a imagem do personagem for diferente do inimigo
    void setImagem(ALLEGRO_BITMAP* imagem)
    {
        this->bmp = imagem;
    }

    void cair()
    {
        y += 5;
        i++;
        if(y > SCREEN_H)
            morreu = 1;
        if(personagemChao())
        {
            i = 0;
            p = 0;
        }
    }
    void pular()
    {
        if(p == 1)
            if(i < 20)
            {
                y -= 5;
                i++;
            }
            else
            {
                i = 0;
                p = 2;
            }
        if(p == 2 & !personagemChao())
            cair();
    }
    void pulou()
    {
        if(p == 0)
            p = 1;
    }

    void passaFase()
    {
        fase ++;
    }

    void setFase(int nova)
    {
        fase = nova;
    }

    int getFase()
    {
        return(fase);
    }
};

class Objeto : public InterfaceEntidade
{
public:
    ALLEGRO_BITMAP *bmp;
    double x, y;
    int largura, altura;
    int ultimoDaLista;

    Objeto()
    {
        ultimoDaLista = 0;
    }

    void setPosicao(double x, double y)
    {
        this->x = x;
        this->y = y;
    }

    void setTamanho(int largura, int altura)
    {
        this->largura = largura;
        this->altura = altura;
    }

    void setImagem(ALLEGRO_BITMAP* imagem)
    {
        this->bmp = imagem;
    }
};

/*==========================================================================================================*/
Jogador jogador;
Inimigo inimigo;
Objeto** caixas;
int indice = 0, intermediarioIndice;

ALLEGRO_BITMAP *fundo = NULL;

bool teclas[4] = {false, false, false, false};
enum TECLAS
{
    KEY_W, KEY_A, KEY_D, KEY_ESC
};

/*==========================================================================================================*/
// Funções
// Inicia as posições dos personagens e as valida
void iniciaPersonagens()
{
    jogador.setPosicao(0, 147);
    inimigo.setPosicao(0, SCREEN_H - inimigo.altura - 20);
}
//novo teste: Reseta posicao personagem
int passaCenario(Jogador* jogador)
{
    if(intermediarioIndice != -1)
    {
        jogador->x = 0;
        jogador->passaFase();
        indice = intermediarioIndice;
        return 1;
    }
    return 0;
}
/*------------------------------------------TESTE----------------------------------------------------------------*/
Objeto** retornaBlocos()
{
    Objeto** objetos = new Objeto*[MAX_OBJETOS];
    for(int i = 0; i < MAX_OBJETOS; i++)
        objetos[i] = new Objeto;

    int indice = 0;
    char c;
    string coordenada;
    ifstream inCoordenadas("example.txt");

    //ultimo da lista 1 = fim bloco; -1 = fim de tudo
    if(inCoordenadas.is_open())
    {
        while(c != '.')
        {
            if(c == ';')
            {
                //Da algum aviso pois tem outro bloco
                inCoordenadas.get(c);
                inCoordenadas.get(c);
                objetos[indice-1]->ultimoDaLista = 1;
            }

            for(int i = 0; i < 4; i++)
            {
                coordenada = "";
                for(int j = 0; j < 3; j++)
                {
                    inCoordenadas.get(c);
                    coordenada += c;
                }
                //coloca no seu respectivo lugar
                if(i == 0)
                    istringstream(coordenada) >> objetos[indice]->x;
                else if(i==1)
                    istringstream(coordenada) >> objetos[indice]->y;
                else if(i==2)
                    istringstream(coordenada) >> objetos[indice]->largura;
                else
                    istringstream(coordenada) >> objetos[indice]->altura;

                inCoordenadas.get(c);
            }
            indice++;
        }
        objetos[indice-1]->ultimoDaLista = -1;
    }
    else cout << "Não foi possivel abrir o arquivo.";

    inCoordenadas.close();

    return objetos;
}
/*----------------------------------------------------------------------------------------------------------*/
// Somente inicia as variáveis e coloca os personagens em posições aleatórias
void iniciaVariaveis()
{
    caixas = retornaBlocos();

    jogador.altura = 43;
    jogador.largura = 50;
    jogador.dx = 3;
    jogador.vidas = 3;
    jogador.setFase(0);
    jogador.p = 0;
    jogador.i = 0;

    inimigo.altura = 40;
    inimigo.largura = 50;
    inimigo.dx = 2;

    iniciaPersonagens();
}

/*----------------------------------------------------------------------------------------------------------*/
// Cria as imagens, de fundo e do jogador
int criarBmps(ALLEGRO_DISPLAY *display, ALLEGRO_TIMER *timer)
{
    // Carrega a imagem do jogador e verifica se carregou
    jogador.bmp = al_load_bitmap("isaachead2.png");
    if(!jogador.bmp)
    {
        cout << "Falha ao criar o jogador!\n";
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    // Carrega a imagem do jogador e verifica se carregou
    inimigo.bmp = al_load_bitmap("monstro.png");
    if(!inimigo.bmp)
    {
        cout << "Falha ao criar o inimigo!\n";
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }

    for(int i = indice; ; i++)
    {
        caixas[i]->bmp = al_create_bitmap(caixas[i]->largura, caixas[i]->altura);
        al_set_target_bitmap(caixas[i]->bmp);
        al_clear_to_color(al_map_rgb(0, 0, 0));

        if(caixas[i]->ultimoDaLista == -1 || caixas[i]->ultimoDaLista == 1)
            break;
    }

    return (0);
}
/*----------------------------------------------------------------------------------------------------------*/
int registrarEventos(ALLEGRO_DISPLAY *display, ALLEGRO_TIMER *timer, ALLEGRO_EVENT_QUEUE **event_queue)
{
    // Cria a fila de eventos e confirma se foi criada
    *event_queue = al_create_event_queue();
    if(*event_queue == 0)
    {
        cout << "Falha ao criar a fila de eventos!\n";
        al_destroy_display(display);
        al_destroy_timer(timer);
        return (-1);
    }
    //Registramos os eventos
    al_register_event_source(*event_queue, al_get_display_event_source(display));
    al_register_event_source(*event_queue, al_get_timer_event_source(timer));
    al_register_event_source(*event_queue, al_get_keyboard_event_source());

    return (0);
}

/*----------------------------------------------------------------------------------------------------------*/
// Verifica se o evento ocorrido foi de apertar ou soltar o teclado
void verificaTeclado(ALLEGRO_EVENT *ev)
{
    // Se apertou a tecla, ela vira true
    if(ev->type == ALLEGRO_EVENT_KEY_DOWN)
    {
        switch(ev->keyboard.keycode)
        {
        case ALLEGRO_KEY_UP:
            teclas[KEY_W] = true;
            break;
        case ALLEGRO_KEY_LEFT:
            teclas[KEY_A] = true;
            break;
        case ALLEGRO_KEY_RIGHT:
            teclas[KEY_D] = true;
            break;
        case ALLEGRO_KEY_ESCAPE:
            teclas[KEY_ESC] = true;
            break;
        }
    }
    // Se soltou a tecla, ela vira false
    else if(ev->type == ALLEGRO_EVENT_KEY_UP)
    {
        switch(ev->keyboard.keycode)
        {
        case ALLEGRO_KEY_UP:
            teclas[KEY_W] = false;
            break;
        case ALLEGRO_KEY_LEFT:
            teclas[KEY_A] = false;
            break;
        case ALLEGRO_KEY_RIGHT:
            teclas[KEY_D] = false;
            break;
        }
    }
}

/*----------------------------------------------------------------------------------------------------------*/
// Movimenta personagem de acordo com a tecla pressionada
void processaTeclado()
{
    if(teclas[KEY_W])
    {
        jogador.pulou();
    }
    if(teclas[KEY_A])
    {
        for(int i = indice; ; i++)
        {
            if(jogador.x <= caixas[i]->x + caixas[i]->largura && jogador.x >= caixas[i]->x &&
                    jogador.y + jogador.altura > caixas[i]->y && jogador.y < caixas[i]->y + caixas[i]->altura
                    || jogador.x - jogador.dx < 0)
            {
                break;
            }
            else if(caixas[i]->ultimoDaLista == -1 || caixas[i]->ultimoDaLista == 1)
            {
                jogador.x -= jogador.dx;
                break;
            }
        }
    }

    if(teclas[KEY_D])
    {
        if(jogador.x > 950)
            passaCenario(&jogador);

        for(int i = indice; ; i++)
        {
            if(jogador.x + jogador.largura <= caixas[i]->x + caixas[i]->largura && jogador.x + jogador.largura >= caixas[i]->x &&
                    jogador.y + jogador.altura > caixas[i]->y && jogador.y < caixas[i]->y + caixas[i]->altura
                    ||
                    jogador.x + jogador.largura + jogador.dx > SCREEN_W)
            {
                break;
            }
            else if(caixas[i]->ultimoDaLista == -1 || caixas[i]->ultimoDaLista == 1)
            {
                jogador.x += jogador.dx;
                break;
            }
        }
    }

}

/*----------------------------------------------------------------------------------------------------------*/
void criaMenu(ALLEGRO_FONT *font, ALLEGRO_TIMER *timer)
{
    // Deixa a tela em preto e printa, em branco, "o davi morreu" no centro da tela
    for(int i = 3; i > 0; i --)
    {
        al_clear_to_color(al_map_rgb(255,255,255));
        char texto[20];
        strcpy(texto, "AS AVENTURAS DE DAVI");
        al_draw_text(font, al_map_rgb(0,0,0), (SCREEN_W/2), (SCREEN_H/2),ALLEGRO_ALIGN_CENTRE, texto);
        sprintf(texto, "%d", i);
        al_draw_text(font, al_map_rgb(0,0,0), (SCREEN_W/2), (SCREEN_H/2 + SCREEN_H/6),ALLEGRO_ALIGN_CENTRE, texto);
        al_flip_display();
        al_stop_timer(timer);
        iniciaPersonagens();
        al_rest(1.0);
        al_start_timer(timer);
    }
}

/*----------------------------------------------------------------------------------------------------------*/
void gameOver(ALLEGRO_FONT *font, ALLEGRO_TIMER *timer)
{
    // Deixa a tela em preto e printa, em branco, "o davi morreu" no centro da tela
    jogador.vidas = 3;
    jogador.setFase(0);
    al_clear_to_color(al_map_rgb(0,0,0));
    char texto[9];
    strcpy(texto, "GAME OVER");
    al_draw_text(font, al_map_rgb(255,255,255), (SCREEN_W/2), (SCREEN_H/2),ALLEGRO_ALIGN_CENTRE, texto);
    al_flip_display();
    al_stop_timer(timer);
    iniciaPersonagens();
    al_rest(3.0);
    al_start_timer(timer);
    criaMenu(font, timer);
}

/*----------------------------------------------------------------------------------------------------------*/
void jogadorMorreu(ALLEGRO_FONT *font, ALLEGRO_TIMER *timer)
{
    // Deixa a tela em preto e printa, em branco, "o davi morreu" no centro da tela
    jogador.morreu = 0;
    jogador.p = 0;
    jogador.i = 0;
    jogador.vidas --;
    if(jogador.vidas < 0)
    {
        gameOver(font, timer);
        indice = 0;
        intermediarioIndice = 0;
    }
    else
    {
        al_clear_to_color(al_map_rgb(0,0,0));
        char texto[50];
        strcpy(texto, "O DAVI MORREU");
        al_draw_text(font, al_map_rgb(255,255,255), (SCREEN_W/2), (SCREEN_H/2),ALLEGRO_ALIGN_CENTRE, texto);
        al_flip_display();
        al_stop_timer(timer);
        iniciaPersonagens();
        al_rest(2.0);
        al_start_timer(timer);
    }
}

/*----------------------------------------------------------------------------------------------------------*/
// Verifica se tocou no inimigo
void tocouInimigo()
{
    if(jogador.x + jogador.largura - HITBOX > inimigo.getX() && jogador.x < inimigo.getX() + inimigo.largura - HITBOX &&
            jogador.y < inimigo.getY() + inimigo.altura - HITBOX && jogador.y + jogador.altura - HITBOX> inimigo.getY() &&
            jogador.getFase() == 2 ||
            jogador.x + jogador.largura - HITBOX > inimigo.getX() && jogador.x < inimigo.getX() + inimigo.largura - HITBOX &&
            jogador.y < inimigo.getY() + inimigo.altura - HITBOX && jogador.y + jogador.altura - HITBOX> inimigo.getY() &&
            jogador.getFase() == 5)
        jogador.morreu = 1;
}
/*----------------------------------------------------------------------------------------------------------*/
int personagemChao()
{
    for(int i = indice; ; i++)
    {
        if(jogador.x + jogador.largura >= caixas[i]->x && jogador.x <= caixas[i]->x + caixas[i]->largura &&
                jogador.y <= caixas[i]->y + caixas[i]->altura && jogador.y + jogador.altura >= caixas[i]->y)
            return(1);
        if(caixas[i]->ultimoDaLista == -1 || caixas[i]->ultimoDaLista == 1)
        {
            return(0);
        }
    }
}

/*----------------------------------------------------------------------------------------------------------*/
// Faz a movimentação do inimigo de acordo com a posição do jogador em relação a ele
void movimentaInimigo()
{
    if(jogador.getFase() == 2 || jogador.getFase() == 5)
    {
        if(inimigo.getX() + inimigo.dx < 0)
            inimigo.dx *= -1;
        else if(inimigo.getX() + inimigo.dx > 600 - inimigo.largura)
            inimigo.dx *= -1;
        else
            inimigo.movimenta();
    }
}

/*----------------------------------------------------------------------------------------------------------*/
// Deixa a tela preta, aí desenha o fundo, aí o jogador, aí o inimigo, aí as vidas e atualiza a tela
void imprimeTela(ALLEGRO_FONT *font)
{
    al_clear_to_color(al_map_rgb(255,255,255));
    if(jogador.getFase() == 2 || jogador.getFase() == 5)
        al_draw_bitmap(inimigo.bmp, inimigo.x, inimigo.y, 0);
    al_draw_bitmap(jogador.bmp, jogador.x, jogador.y, 0);

    for(int i = indice; ; i++)
    {
        al_draw_bitmap(caixas[i]->bmp, caixas[i]->x, caixas[i]->y, 0);
        if(caixas[i]->ultimoDaLista == -1 && indice != -1)
        {
            intermediarioIndice = -1;
        }
        else if(caixas[i]->ultimoDaLista == 1 && indice < i)
        {
            intermediarioIndice = i+1;
        }

        if(caixas[i]->ultimoDaLista == -1 || caixas[i]->ultimoDaLista == 1)
            break;
    }
    char texto[50], tmp[10];

    if(jogador.getFase() == 8)
    {
        strcpy(texto, "Você é bixão mesmo");
        al_draw_text(font, al_map_rgb(0,0,0), (SCREEN_W/2), (SCREEN_H/2),ALLEGRO_ALIGN_CENTRE, texto);
    }
    else
    {
        strcpy(texto, "Davis: ");
        sprintf(tmp,"%d",jogador.vidas);
        strcat(texto, tmp);
        al_draw_text(font, al_map_rgb(0,0,0), (SCREEN_W/8), 30,ALLEGRO_ALIGN_CENTRE, texto);

        strcpy(texto, "Fase: ");
        sprintf(tmp,"%d",jogador.getFase());
        strcat(texto, tmp);
        al_draw_text(font, al_map_rgb(0,0,0), (7*SCREEN_W/8), 30,ALLEGRO_ALIGN_CENTRE, texto);

        if(jogador.getFase() == 2 || jogador.getFase() == 5)
        {
            if(inimigo.getX() + inimigo.dx < 0)
                inimigo.dx *= -1;
            else if(inimigo.getX() + inimigo.dx > 600 - inimigo.largura)
                inimigo.dx *= -1;
            else
                inimigo.movimenta();
        }
    }

    al_flip_display();
}

/*==========================================================================================================*/
int main(void)
{
    srand((unsigned)time(NULL));

    // Reponsável pela impressão na tela
    ALLEGRO_DISPLAY *display = NULL;
    // Fila de eventos
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    // Eventos de tempo (ocorrem a cada X tempo)
    ALLEGRO_TIMER *timer = NULL;
    bool redraw = true;

    iniciaVariaveis();

    // Iniciamos o Allegro
    if(!al_init())
    {
        cout << "Falha ao iniciar o Allegro!\n";
        return (-1);
    }

    // Instala o teclado
    if(!al_install_keyboard())
    {
        cout << "Falha ao instalar o teclado!\n";
        return (-1);
    }

    // Inicializa a biblioteca de imagens
    if(!al_init_image_addon())
    {
        cout << "Falha ao iniciar o al_init_image_addon!\n";
        return (-1);
    }

    // Fonte
    al_init_font_addon(); // Inicializa a biblioteca de fontes
    al_init_ttf_addon();// (True Type Font)
    ALLEGRO_FONT *font = al_load_ttf_font("pirulen.ttf",12,0 );
    if (!font)
    {
        cout << "Nao conseguiu carregar 'pirulen.ttf'.\n";
        return (-1);
    }

    // Cria o timer
    timer = al_create_timer(1.0 / FPS);
    if(!timer)
    {
        cout << "Falha ao iniciar o timer!\n";
        return (-1);
    }

    // Cria o display
    display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display)
    {
        cout << "Falha ao criar o display!\n";
        return (-1);
    }

    // Configura o título da janela
    al_set_window_title(display, "As Aventuras de Davi");

    if(criarBmps(display, timer) == -1)
        return (-1);
    if(registrarEventos(display, timer, &event_queue) == -1)
        return (-1);

    // Altera o display, limpa a tela e inicia o timer
    al_set_target_bitmap(al_get_backbuffer(display));
    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();
    al_start_timer(timer);

    criaMenu(font,timer);
    while(1)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        int i = 0;

        // Chama as principais funções
        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            if(jogador.x > MAXIMO_CENARIO)
            {
                if(passaCenario(&jogador))
                {
                    criarBmps(display, timer);
                    registrarEventos(display, timer, &event_queue);
                    al_set_target_bitmap(al_get_backbuffer(display));
                    al_flip_display();
                }
                else
                {
                    indice = 0;
                    intermediarioIndice = 0;
                    gameOver(font, timer);
                }
            }
            i++;
            processaTeclado();
            tocouInimigo();
            movimentaInimigo();
            jogador.pular();
            if(!personagemChao() & jogador.p != 1)
                jogador.cair();
            if(jogador.morreu)
            {
                jogadorMorreu(font, timer);
            }
            redraw = true;
        }

        // Se fechar o jogo, ou apertar ESC
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || teclas[KEY_ESC])
        {
            break;
        }

        // Se pressionar ou soltar alguma tecla
        else if((ev.type == ALLEGRO_EVENT_KEY_DOWN) || (ev.type == ALLEGRO_EVENT_KEY_UP))
        {
            verificaTeclado(&ev);
        }

        if(redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;
            imprimeTela(font);
        }
    }

    // Destrói tudo
    al_destroy_font(font);
    al_destroy_bitmap(fundo);
    al_destroy_bitmap(jogador.bmp);
    al_destroy_bitmap(inimigo.bmp);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    //Desaloca a memoria dos elementos do cenario
    for(int i = 0; i < MAX_OBJETOS; i++)
    {
        delete caixas[i];
    }
    delete[] caixas;

    return (0);
}
