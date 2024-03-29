#define SDL_MAIN_HANDLED
#include <iostream>
#include <cstdio>
#include <SDL.h>
#include <vector>
#include <csignal>
#include <cmath>
#include <cstring>

//pixel = meter

const double PI = 3.14;
const double G = 6.67e-11;

int quit = 0;
void handleSignal(int signal) {
    quit = 1;
}

// Initialize SDL and create a window
bool init(SDL_Window*& window, SDL_Renderer*& renderer, int screenWidth, int screenHeight) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }
    else std::cout << "Initialized." << std::endl;

    window = SDL_CreateWindow("n-Body Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    else std::cout << "Window created." << std::endl;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    else std::cout << "Renderer created." << std::endl;

    return true;
}

// Define a structure for a 2D vector
struct Vector2D {
    double x, y;
    Vector2D(double x = 0, double y = 0) : x(x), y(y) {}
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }
    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }
    Vector2D operator*(double scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }
    double magnitude() const {
        return abs(std::sqrt(x * x + y * y));
    }
    Vector2D normalized() const {
        double mag = magnitude();
        return mag != 1 ? (*this) * (1.0 / mag) : Vector2D();
    }
};

// Define a data structure for a circle
struct Circle {
    Vector2D position, velocity, acc;
    double radius;
    unsigned long long mass;
    Circle(Vector2D pos = Vector2D(), Vector2D vel = Vector2D(), Vector2D a = Vector2D(), 
    double rad = 0, unsigned long long m = 0) : position(pos), velocity(vel), radius(rad), mass(m), acc(a) {}
};

// Render circles on the screen
void render(SDL_Renderer* renderer, const std::vector<Circle>& circles) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //black
    SDL_RenderClear(renderer); //paint all black
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //white
    for (const auto& circle : circles) {
        int x = static_cast<int>(circle.position.x);
        int y = static_cast<int>(circle.position.y);
        SDL_RenderDrawPoint(renderer, x, y);
        for(double i = 0.0; i <= 2 * PI; i += 0.001){
            x = static_cast<int>(circle.position.x + cos(i) * circle.radius);
            y = static_cast<int>(circle.position.y + sin(i) * circle.radius);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); //red
    for (const auto& circle : circles) {
        SDL_RenderDrawLine(renderer, static_cast<int>(circle.position.x), // velocity vector
        static_cast<int>(circle.position.y), static_cast<int>(circle.position.x + 1e2 * circle.velocity.x), 
        static_cast<int>(circle.position.y + 1e2 * circle.velocity.y));
    }
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); //green
    for (const auto& circle : circles) {
        SDL_RenderDrawLine(renderer, static_cast<int>(circle.position.x), // acceleration vector
        static_cast<int>(circle.position.y), static_cast<int>(circle.position.x + 1e4 * circle.acc.x), 
        static_cast<int>(circle.position.y + 1e4 * circle.acc.y));
    }
    SDL_RenderPresent(renderer);
}

Vector2D gravacc(Circle frst, Circle scnd){ //acceleration of first circle because of second circle
    Vector2D direction = scnd.position - frst.position;
    double distance = direction.magnitude();
    double a;
    if(distance < frst.radius || distance < scnd.radius){
        a = 0;
    }
    else a = G * (scnd.mass / pow(distance, 2));
    return direction * a;
}

int main() {

    int SCREEN_WIDTH = 1000, SCREEN_HEIGHT = 800, radius = 10;
    unsigned long long mass = 1e9;
    std::vector<Circle> circles;

    printf("\nSimulating...\n");
    signal(SIGINT, handleSignal);

    SDL_Window* window = nullptr;   // (0,0) at top-left corner
    SDL_Renderer* renderer = nullptr;

    if (!init(window, renderer, SCREEN_WIDTH, SCREEN_HEIGHT)) {
        std::cout << "Initialization failed!" << std::endl;
        return -1;
    }
    
    Circle circle;
    Vector2D position;
    Vector2D velocity(0,0);
    int count = 0;
    bool run = 0;
    int delay = 16;

    while (!quit) {
        // Poll for events
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                printf("Window closed.\n");
                quit = 1;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    position.x = mouseX;
                    position.y = mouseY;
                    circle.position = position;
                    circle.mass = mass;
                    circle.radius = radius;
                    circle.velocity = velocity;
                    circles.push_back(circle);
                    count++;
                    printf("Body %d with mass %llu kg and radius %d m added.\n", count, mass, radius);
                }
                else if (event.button.button == SDL_BUTTON_RIGHT) {
                   
                }
                
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_PLUS:
                        mass += 1e9;
                        radius *= 2;
                        printf("Mass: %llu, radius: %d\n", mass, radius);
                        break;
                    case SDLK_MINUS:
                        if(mass > 0) mass -= 1e9;
                        radius /= 2;
                        printf("Mass: %llu, radius: %d\n", mass, radius);
                        break;
                    case SDLK_UP:
                        velocity.y += -0.1;
                        printf("vy0: %f\n", velocity.y);
                        break;
                    case SDLK_DOWN:
                        velocity.y += 0.1;
                        printf("vy0: %f\n", velocity.y);
                        break;
                    case SDLK_LEFT:
                        velocity.x += -0.1;
                        printf("vx0: %f\n", velocity.x);
                        break;
                    case SDLK_RIGHT:
                        velocity.x += 0.1;
                        printf("vx0: %f\n", velocity.x);
                        break;
                    case SDLK_BACKSPACE:
                        circles.clear();
                        printf("All circles removed.\n");
                        count = 0;
                        break;
                    case SDLK_SPACE:
                        if (run == false) {
                            run = true;
                            printf("Start.\n");
                        }
                        else if (run == true) {
                            run = false;
                            printf("Pause.\n");
                        }
                        break;
                    case SDLK_m:
                        delay += 1;
                        printf("Delay: %d\n", delay);
                        break;
                    case SDLK_n:
                        if (delay > 1) delay -= 1;
                        printf("Delay: %d\n", delay);
                        break;
                }
            }
        }

        // Update game state
        if(run) {
        for (auto& circle : circles) {
            Vector2D acceleration(0, 0);
            for (auto& IIcircle : circles) {
                if (&circle != &IIcircle) {
                    acceleration = gravacc(circle, IIcircle) + acceleration;
                }
            } 
            circle.acc = acceleration;
            circle.velocity = acceleration + circle.velocity;
            circle.position = circle.velocity + circle.position;
            }
        }

        // Render
        render(renderer, circles);

        // Limit frame rate
        SDL_Delay(delay);  
    }

    printf("Terminating SDL.\n");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("Terminated.\n");
    return 0;
}
