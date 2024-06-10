#include "snake.h"

// Resolution;
#define WINDOW_X \
  SDL_WINDOWPOS_CENTERED  // Define o eixo x e y no centro da tela;
#define WINDOW_Y SDL_WINDOWPOS_CENTERED
#define WINDOW_HEIGHT 500
#define WINDOW_WIDTH 500
// Grid;
#define GRID_SIZE 20  // 20 cedulas ;
#define GRID_DIM 400  // 400x400;

// Enunciado de direcoes, usado para mover snake;
typedef enum { SNAKE_UP, SNAKE_DOWN, SNAKE_LEFT, SNAKE_RIGHT } Direction;

// Enumeração para as opções do menu
typedef enum { MENU_START, MENU_SETTINGS, MENU_EXIT } MenuOption;
MenuOption currentOption = MENU_START;  // Inicia o menu no start;

typedef struct {
  int score;
  int level;
  int record;
} GameState;

typedef struct {
  int x;
  int y;
} Apple;

typedef struct Snake {
  int x;
  int y;
  Direction dir;
  // Ponteiro para o proximo segmento;
  struct Snake *next;
} SnakeSegment;

// Variáveis globais;
GameState game;
Apple apple;
// Inicia os sementos da snake como ender vaziu;
SnakeSegment *snakeHead = NULL;
SnakeSegment *snakeTail = NULL;
bool isPaused = false;
bool inMenu = true;
bool quitGame = false;
// Deixe true para visualizar o grid;
bool gridOn = false;
// Define o calculo da posicao do grid com base na resolucao;
int gridResolutionX = (WINDOW_WIDTH / 2) - (GRID_DIM / 2);
int gridResolutionY = (WINDOW_HEIGHT / 2) - (GRID_DIM / 2);

// SDL globals;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;

// Funções do jogo;
void initSDL();
void initGame();
void initSnake();
void increaseSnake();
void moveSnake();
void generateApple();
void renderGrid(SDL_Renderer *r, int x, int y);
void renderSnake();
void renderApple();
void renderText(const char *text, int x, int y, SDL_Color color);
void handleInput();
void updateGame();
void detectAppleCollision();
void detectSelfCollision();
void resetGame();
void cleanUp();
// Funções do menu
void renderMenu();
void handleMenuInput();

// App;
int main() {
  // Inicializacao de numeros random baseado no tempo;
  srand(time(0));
  // Inicializacao de variaveis do SDL;
  initSDL();
  // Inicializacao de variaveis do Jogo;
  initGame();

  // Loop inicial do jogo;
  while (!quitGame) {
    if (inMenu) {
      handleMenuInput();
      renderMenu();
    } else {
      handleInput();
      if (!isPaused) {
        updateGame();
      }
      // Limpa a tela;
      SDL_RenderClear(renderer);
      // Rendereizacao dos itens do jogo;
      renderGrid(renderer, gridResolutionX, gridResolutionY);
      renderSnake();
      renderApple();

      // Inicializacao das variaveis para o score;
      SDL_Color textColor = {255, 255, 255, 255};
      char scoreText[50];
      snprintf(scoreText, sizeof(scoreText), "Score: %d Record: %d, Level: %d ",
               game.score, game.record, game.level);
      renderText(scoreText, WINDOW_WIDTH - 490, WINDOW_HEIGHT - 30, textColor);

      // Cor de fundo;
      SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
    }

    SDL_RenderPresent(renderer);
    // Altera a velocidade com base no nivel;
    SDL_Delay(130 / game.level);
  }

  cleanUp();
  return 0;
}

// Inicializa SDL e TTF
void initSDL() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "ERROR: SDL_Init: %s\n", SDL_GetError());
    exit(1);
  }

  if (TTF_Init() == -1) {
    fprintf(stderr, "ERROR: TTF_Init: %s\n", TTF_GetError());
    SDL_Quit();
    exit(1);
  }

  window = SDL_CreateWindow("Snake", WINDOW_X, WINDOW_Y, WINDOW_WIDTH,
                            WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    fprintf(stderr, "ERROR: SDL_CreateWindow: %s\n", SDL_GetError());
    TTF_Quit();
    SDL_Quit();
    exit(1);
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    fprintf(stderr, "ERROR: SDL_CreateRenderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    exit(1);
  }

  font = TTF_OpenFont("font/Inconsolata-Regular.ttf", 14);
  if (!font) {
    fprintf(stderr, "ERROR: TTF_OpenFont: %s\n", TTF_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    exit(1);
  }
}

// Inicializa o estado do jogo
void initGame() {
  game.score = 0;
  game.record = 0;
  game.level = 100;
  initSnake();
  increaseSnake();
  increaseSnake();
  generateApple();
}

/// Inicializa a cobra
void initSnake() {
  // Alocamento de memora para a str snake;
  SnakeSegment *newSegment = (SnakeSegment *)malloc(sizeof(SnakeSegment));
  if (!newSegment) {
    fprintf(stderr, "ERROR: Memory allocation failed\n");
    exit(1);
  }
  newSegment->x = GRID_SIZE / 2;
  newSegment->y = GRID_SIZE / 2;
  newSegment->dir = SNAKE_UP;
  newSegment->next = NULL;

  snakeHead = newSegment;
  snakeTail = newSegment;
}

// Aumenta a cobra
void increaseSnake() {
  // Aloca memoria para novo segmento;
  SnakeSegment *newSegment = (SnakeSegment *)malloc(sizeof(SnakeSegment));
  if (!newSegment) {
    fprintf(stderr, "ERROR: Memory allocation failed\n");
    exit(1);
  }

  // Dependendo da direcao da snake a calda e adicionada em um eixo diferente;
  switch (snakeTail->dir) {
    case SNAKE_UP:
      newSegment->x = snakeTail->x;
      newSegment->y = snakeTail->y + 1;
      break;
    case SNAKE_DOWN:
      newSegment->x = snakeTail->x;
      newSegment->y = snakeTail->y - 1;
      break;
    case SNAKE_LEFT:
      newSegment->x = snakeTail->x + 1;
      newSegment->y = snakeTail->y;
      break;
    case SNAKE_RIGHT:
      newSegment->x = snakeTail->x - 1;
      newSegment->y = snakeTail->y;
      break;
  }

  // Pega a direcao do semento anterios de calda(calda antiga);
  newSegment->dir = snakeTail->dir;
  // Difine o novo ultimo segmento como sem proximo segmento;
  newSegment->next = NULL;
  snakeTail->next = newSegment;
  snakeTail = newSegment;
}

// Move a cobra
void moveSnake() {
  // Armazena o eixo X, Y e a direcao anterior ao movimento;
  int prevX = snakeHead->x;
  int prevY = snakeHead->y;
  int prevDir = snakeHead->dir;

  switch (snakeHead->dir) {
    case SNAKE_UP:
      snakeHead->y--;
      break;
    case SNAKE_DOWN:
      snakeHead->y++;
      break;
    case SNAKE_LEFT:
      snakeHead->x--;
      break;
    case SNAKE_RIGHT:
      snakeHead->x++;
      break;
  }

  // Define um ponteiro de segmento de snake que inicia no segundo segmento da
  // cobra (a cabeça da cobra é snakeHead, então começamos pelo próximo
  // segmento).
  SnakeSegment *track = snakeHead->next;
  // Enquanto nao for = NULL vai executar;
  while (track) {
    // Armazena a posição atual (x, y) e a direção do segmento atual da cobra em
    // variáveis temporárias. Isso é necessário para preservar os valores atuais
    // antes de atualizar o segmento com novos valores.
    int tempX = track->x;
    int tempY = track->y;
    int tempDir = track->dir;

    // Atualiza a posição (x, y) e a direção do segmento atual da cobra
    // para igualar a posição e a direção do segmento anterior.
    // Isso faz com que o segmento siga o movimento do segmento da frente.
    track->x = prevX;
    track->y = prevY;
    track->dir = prevDir;

    // Atualiza as variáveis prevX, prevY e prevDir com os valores temporários
    // armazenados anteriormente.
    // Isso prepara os valores para o próximo segmento da cobra no loop.
    prevX = tempX;
    prevY = tempY;
    prevDir = tempDir;

    // Move o ponteiro track para o próximo segmento da cobra na lista ligada.
    // Isso continua o loop até que todos os segmentos tenham sido processados.
    track = track->next;
  }
}

// Gera uma nova maçã
void generateApple() {
  bool insideSnake = false;

  do {
    insideSnake = false;
    apple.x = rand() % GRID_SIZE;
    apple.y = rand() % GRID_SIZE;

    SnakeSegment *track = snakeHead;
    while (track) {
      if (track->x == apple.x && track->y == apple.y) {
        insideSnake = true;
        break;
      }
      track = track->next;
    }
  } while (insideSnake);
}

// Renderiza o grid
void renderGrid(SDL_Renderer *r, int x, int y) {
  SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 255);

  // Grid ON/OFF;
  if (gridOn == true) {
    int cell_size = GRID_DIM / GRID_SIZE;

    SDL_Rect cell;
    cell.w = cell_size;
    cell.h = cell_size;

    for (int i = 0; i < GRID_SIZE; i++) {
      for (int j = 0; j < GRID_SIZE; j++) {
        cell.x = x + (i * cell_size);
        cell.y = y + (j * cell_size);

        SDL_RenderDrawRect(r, &cell);
      }
    }
  }
  if (gridOn == false) {
    SDL_Rect outline;
    outline.x = x;
    outline.y = y;
    outline.w = GRID_DIM;
    outline.h = GRID_DIM;

    SDL_RenderDrawRect(r, &outline);
  }
}

// Renderiza a cobra
void renderSnake() {
  SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 255);

  // Difine o tamanho do segmento da snake;
  int segSize = GRID_DIM / GRID_SIZE;
  SDL_Rect seg = {0, 0, segSize, segSize};

  // Inicia um tracker da cabeca;
  SnakeSegment *track = snakeHead;
  // Enquanto tiver segemntos (!=NULL) o loop ira passar por cada um e os
  // renderizar;
  while (track) {
    seg.x = gridResolutionX + track->x * segSize;
    seg.y = gridResolutionY + track->y * segSize;
    SDL_RenderFillRect(renderer, &seg);
    track = track->next;
  }
}

// Renderiza a maçã
void renderApple() {
  SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 255);

  // Difine q a maca pode aparecer em qualquer lugar do mapa;
  int appleSize = GRID_DIM / GRID_SIZE;
  SDL_Rect appleRect = {gridResolutionX + apple.x * appleSize,
                        gridResolutionY + apple.y * appleSize, appleSize,
                        appleSize};
  SDL_RenderFillRect(renderer, &appleRect);
}

// Função para renderizar texto na tela
void renderText(const char *text, int x, int y, SDL_Color color) {
  // Cria uma superfície com o texto renderizado em modo sólido, usando a fonte
  // global 'font'.
  SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
  // Verifica se a criação da superfície falhou.
  if (!surface) {
    fprintf(stderr, "ERROR: TTF_RenderText_Solid: %s\n", TTF_GetError());
    return;
  }
  // Cria uma textura a partir da superfície renderizada.
  // A textura é usada para desenhar a superfície na tela com mais eficiência.
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  // Verifica se a criação da textura falhou.
  if (!texture) {
    fprintf(stderr, "ERROR: SDL_CreateTextureFromSurface: %s\n",
            SDL_GetError());
    SDL_FreeSurface(surface);
    return;
  }

  // Define um retângulo de destino (dstRect) onde a textura será desenhada na
  // tela.
  // As coordenadas x e y são as passadas para a função.
  // As larguras e alturas do retângulo são as mesmas da superfície.
  SDL_Rect dstRect = {x, y, surface->w, surface->h};

  // Copia a textura para o renderizador, desenhando-a na posição especificada
  // por dstRect.
  // O NULL indica que queremos desenhar toda a textura sem recortes.
  SDL_RenderCopy(renderer, texture, NULL, &dstRect);

  // Libera memoria alocada;
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

// Manipula a entrada do usuário
void handleInput() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
    } else if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          quitGame = true;
          break;
        case SDLK_UP:
          if (snakeHead->dir != SNAKE_DOWN) snakeHead->dir = SNAKE_UP;
          break;
        case SDLK_DOWN:
          if (snakeHead->dir != SNAKE_UP) snakeHead->dir = SNAKE_DOWN;
          break;
        case SDLK_LEFT:
          if (snakeHead->dir != SNAKE_RIGHT) snakeHead->dir = SNAKE_LEFT;
          break;
        case SDLK_RIGHT:
          if (snakeHead->dir != SNAKE_LEFT) snakeHead->dir = SNAKE_RIGHT;
          break;
        case SDLK_p:
          isPaused = !isPaused;
          break;
      }
    }
  }
}

// Atualiza o estado do jogo
void updateGame() {
  moveSnake();
  detectAppleCollision();
  detectSelfCollision();
}

// Detecta colisão com a maçã
void detectAppleCollision() {
  if (snakeHead->x == apple.x && snakeHead->y == apple.y) {
    generateApple();
    increaseSnake();
    game.score += 10;

    if (game.score != 0 && game.score % 150 == 0) {
      game.level++;
    }
  }
}

// Detecta colisão com o próprio corpo
void detectSelfCollision() {
  SnakeSegment *track = snakeHead->next;
  while (track) {
    if (track->x == snakeHead->x && track->y == snakeHead->y) {
      resetGame();
      return;
    }
    track = track->next;
  }

  if (snakeHead->x < 0 || snakeHead->x >= GRID_SIZE || snakeHead->y < 0 ||
      snakeHead->y >= GRID_SIZE) {
    resetGame();
  }
}

// Reseta o estado do jogo
void resetGame() {
  SnakeSegment *track = snakeHead;
  while (track) {
    SnakeSegment *next = track->next;
    free(track);
    track = next;
  }
  initSnake();
  increaseSnake();
  increaseSnake();

  if (game.score > game.record) {
    game.record = game.score;
  }
  game.score = 0;
  game.level = 1;
}

// Libera os recursos alocados
void cleanUp() {
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

// Funcao de menu;
void handleMenuInput() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      quitGame = true;
    } else if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case SDLK_UP:
          currentOption = (currentOption == MENU_START)
                              ? MENU_EXIT
                              : (MenuOption)(currentOption - 1);
          break;
        case SDLK_DOWN:
          currentOption = (currentOption == MENU_EXIT)
                              ? MENU_START
                              : (MenuOption)(currentOption + 1);
          break;
        case SDLK_RETURN:
          switch (currentOption) {
            case MENU_START:
              inMenu = false;
              resetGame();
              break;
            case MENU_SETTINGS:
              // Implementar configurações aqui
              break;
            case MENU_EXIT:
              quitGame = true;
              break;
          }
          break;
      }
    }
  }
}

void renderMenu() {
  SDL_RenderClear(renderer);

  // Cor cinza para opções não selecionadas: R=128, G=128, B=128
  SDL_Color textColor = {128, 128, 128, 255};
  // Cor lavanda para opção selecionada: R=230, G=230, B=250
  SDL_Color highlightColor = {230, 230, 250, 255};

  // Renderizando o título "Snake"
  renderText("Snake", WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 - 50, textColor);

  const char *menuOptions[] = {"Start", "Settings", "Exit"};

  for (int i = 0; i < 3; i++) {
    if (i == (int)currentOption) {
      renderText(menuOptions[i], WINDOW_WIDTH / 2 - 50,
                 WINDOW_HEIGHT / 2 + 20 * i, highlightColor);
    } else {
      renderText(menuOptions[i], WINDOW_WIDTH / 2 - 50,
                 WINDOW_HEIGHT / 2 + 20 * i, textColor);
    }
  }

  SDL_RenderPresent(renderer);
}
