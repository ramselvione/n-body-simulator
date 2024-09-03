

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
    bool operator==(const Vector2D& other) const {
        if(x == other.x && y == other.y) return 1;
        else return 0;
    }
    bool operator<(const Vector2D& other) const {
        if(x < 0 && y < 0) return 1;
        else return 0;
    }
    bool operator>(const Vector2D& other) const {
        if(x > 0 && y > 0) return 1;
        else return 0;
    }
    double magnitude() const {
        return abs(std::sqrt(x * x + y * y));
    }
    Vector2D normalized() const {
        double mag = magnitude();
        return mag != 1 ? (*this) * (1.0 / mag) : Vector2D();
    }
};