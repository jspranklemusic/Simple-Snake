#include <iostream>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <random>
#include <sstream>
#include <cstdlib>
#include <map>
#include <thread> 

using std::string;
using std::vector;
using std::cout;
using std::unique_ptr;
using std::map;

const int WIDTH = 800;
const int HEIGHT = 800;
const int GRID_WIDTH = 25;

class Element;

vector <Element*> elements = {};

float vw(int val) {
    return (WIDTH*100)/val;
};

float vh(int val) {
    return (HEIGHT*100)/val;
};
// grid units
int gu(int val) {
    return val * WIDTH/GRID_WIDTH;
};

void sleep(int num) {
    std::this_thread::sleep_for(std::chrono::milliseconds(num));
};

namespace uuid {
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::string generate_uuid_v4() {
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }
};


enum class ElementTypes {
    Text,
    Button,
    Circle,
    Grid,
    Rectangle
};

class Element {
    public:
        static vector<string> nums;
        static int totalCreatedCount;
        static size_t currentCount() {
            return elements.size();
        }
        static Element* findById(string id) {
            for(auto& e : elements) {
                if (e->id == id) {
                    return e;
                }
            }
            return nullptr;
        }
        int x;
        int y;
        string id;
        ElementTypes type;
        sf::RenderWindow& window;

        Element(sf::RenderWindow& window, ElementTypes type, int x = 0, int y = 0) : window(window), x(x), y(y), type(type) {
            totalCreatedCount++;
            cout << "the total count is: " << totalCreatedCount << "\n";
            id = uuid::generate_uuid_v4();
            nums.push_back(id);
        }

        void renderAll() {
            window.clear();
            cout << "Looping through elements...\n";
            cout << "It is " << elements.size() << "elements long...\n";
            for (int i = 0; i < elements.size(); i++) {
                // cout << "num: " << nums[i] << "\n";
                // cout << elements[i]->id << "\n";
                elements[i]->draw();
            }
            window.display();
        }

        virtual void draw(){
            cout << "doing nothing, drawing from base class\n";
        }

        virtual void destroy() {
            int foundIndex = -1;
            for (int i = 0; i < elements.size(); i++) {
                if (elements[i]->id == id) {
                    foundIndex = i;
                    break;
                }
            }
            elements.erase(elements.begin() + foundIndex);
            delete this;
        }

        static void destroyAll() {
            for (auto &e: elements) {
                delete e;
            }
            elements.clear();
        }
};

int Element::totalCreatedCount = 0;
vector<string> Element::nums = {};

struct TextConfig {
    int font_size;
    int line_spacing;
    string content;
    string font_location;
    sf::Uint32 style;
    sf::Color color;
};

class TextElement : public Element {
    public:
        static vector<TextElement*> text_elements;
        static TextConfig defaultConfig() {
            TextConfig textConfig;
            textConfig.color = sf::Color::White;
            textConfig.font_location = "./ShortBaby-Mg2w.ttf";
            textConfig.content = "";
            textConfig.style = sf::Text::Bold;
            textConfig.font_size = 40;
            textConfig.line_spacing = 1;
            return textConfig;
        }
        string text_content;
        sf::Text text;
        sf::Font font;
        sf::Color color;
        TextElement(sf::RenderWindow& window, TextConfig config, int x = 0, int y = 0) : Element(window, ElementTypes::Text, x, y) {
            // this->y = (totalCreatedCount * 45) - 45;
            setTextFromConfig(config);
            afterInitialize();
        }
        TextElement(sf::RenderWindow& window, string content = "", int x = 0, int y = 0) : Element(window, ElementTypes::Text, x, y) {
            // this->y = (totalCreatedCount * 45) - 45;
            TextConfig config = defaultConfig();
            config.content = content;
            setTextFromConfig(config);
            afterInitialize();
        }
        void changeText(string newText) {
            text_content = newText;
            text.setString(text_content);
        }

        void afterInitialize() {
            elements.push_back(this);
            text_elements.push_back(this);
            renderAll();
        }

        void setTextFromConfig(TextConfig config) {
            text_content = config.content;
            font.loadFromFile(config.font_location);
            text.setFont(font);
            text.setString(text_content);
            text.setCharacterSize(config.font_size);
            text.setFillColor(config.color);
            text.setStyle(config.style);
            cout << "y: " << y<< "\n";
            text.setPosition(x, y);
            text.setLineSpacing(config.line_spacing);
            cout << text_content << "\n";
        }

        virtual void draw() {
            cout << "drawing text: " << text_content << "\n";
            window.draw(text);
        }
};

vector<TextElement*> TextElement::text_elements = {};

class CircleElement : public Element {
    public:
        static vector<CircleElement*> circle_elements;
        sf::CircleShape shape;
        
        CircleElement(sf::RenderWindow& window, int x = 0, int y = 0) : Element(window, ElementTypes::Circle, x, y) {
            sf::CircleShape shape(50.f);
            shape.setFillColor(sf::Color(150, 50, 250));

            // set a 10-pixel wide orange outline
            shape.setOutlineThickness(10.f);
            shape.setOutlineColor(sf::Color(250, 150, 100));
            this->shape = shape;
            afterInitialize();
        }

        void afterInitialize() {
            elements.push_back(this);
            circle_elements.push_back(this);
            renderAll();
        }

        void draw() {
            window.draw(shape);
        }

        static CircleElement* create(sf::RenderWindow& window, int x = 0, int y = 0) {
            return new CircleElement(window, x, y);
        }
};
vector<CircleElement*> CircleElement::circle_elements = {};


class GridElement : public Element  {
    public:
        static vector<GridElement*> grid_elements;
        int grid_width;
        GridElement(sf::RenderWindow& window, int grid_width = GRID_WIDTH) : grid_width(grid_width), Element(window, ElementTypes::Grid) {
            afterInitialize();
        }
        void draw(){
            for (int i = 0; i < grid_width; i++) {
                sf::Vertex line[] =
                {
                    sf::Vertex(sf::Vector2f((WIDTH/grid_width)*i, 0.f)),
                    sf::Vertex(sf::Vector2f((WIDTH/grid_width)*i,  WIDTH))
                };
                window.draw(line, 2, sf::Lines);
                sf::Vertex line2[] =
                {
                    sf::Vertex(sf::Vector2f(0.f, (WIDTH/grid_width)*i)),
                    sf::Vertex(sf::Vector2f(WIDTH, (WIDTH/grid_width)*i))
                };
                window.draw(line2, 2, sf::Lines);
            }
        }
         void afterInitialize() {
            elements.push_back(this);
            grid_elements.push_back(this);
            renderAll();
        }
};
vector<GridElement*> GridElement::grid_elements;

class RectangleElement : public Element {
    public:
        sf::RectangleShape rectangle;
        static vector<RectangleElement*> rectangle_elements;
        RectangleElement(sf::RenderWindow& window, int width, int height, sf::Color color = sf::Color::Green, int x = 0, int y = 0) : Element(window, ElementTypes::Rectangle, x, y) {
            sf::RectangleShape rectangle(sf::Vector2f(width, height));
            rectangle.setFillColor(color);
            rectangle.setPosition(this->x, this->y);
            this->rectangle = rectangle;
            window.draw(rectangle);
            afterInitialize();
        }

        void afterInitialize() {
            elements.push_back(this);
            rectangle_elements.push_back(this);
            renderAll();
        }

        void draw() {
            window.draw(rectangle);
        }

        void move(int x, int y) {
            this->x = x;
            this->y = y;
            sync();
        }   
        void sync() {
            rectangle.setPosition(this->x, this->y);
        }
};

vector<RectangleElement*> RectangleElement::rectangle_elements;


class Snake {
    public:
        vector<RectangleElement*> rectangles;
        string currentDirection;
        string prevDirection;
        TextElement* displayMessage;
        RectangleElement* food;
        sf::RenderWindow& window;
        bool paused;
        bool gameover;
        Snake(sf::RenderWindow& window) : window(window) {
            setSnake();
        }
        void setDirection(string direction) {
            prevDirection = currentDirection;
            currentDirection = direction;
        }
        void unsetSnake() {
            for (auto &e : rectangles) {
                e->destroy();
            }
            food->destroy();
            rectangles.clear();
        }
        void setSnake() {
            for (int i = 4; i < 7; i++) {
                auto rect = new RectangleElement(window, gu(1), gu(1), sf::Color::Green, gu(i), gu(4));
                rectangles.push_back(rect);
            }
            currentDirection = "right";
            paused = true;
            setFood();
        }
        void move() {
            if (!paused) {
                auto lastRect = rectangles.back();
                float newX = lastRect->x;
                float newY = lastRect->y;

                if (currentDirection == "right"){
                    newX += gu(1);
                } else if (currentDirection == "left") {
                    newX -= gu(1);
                } else if (currentDirection == "up") {
                    newY -= gu(1);
                } else if (currentDirection == "down") {
                    newY += gu(1);  
                }
                auto prevRect  = rectangles[rectangles.size() - 2];
                if (prevRect->y == newY && prevRect->x == newX) {
                    setDirection(prevDirection);
                    return move();
                } 

                cout << "current direction: " << currentDirection << "\n";
                size_t snakeSize = rectangles.size();
                for (int i = 0; i < snakeSize - 1; i++) {
                    auto curRect = rectangles[i];
                    auto nextRect = rectangles[i + 1];
                    curRect->move(nextRect->x, nextRect->y);
                }
                
                lastRect->y = newY;
                lastRect->x = newX;
                lastRect->sync();
                // action conditions
                if (food->x == lastRect->x && food->y == lastRect->y) {
                    food->destroy();
                    setFood();
                    preppendRectangle();
                }
                // game over
                for (int i = 0; i < rectangles.size() - 1; i++) {
                    auto rect = rectangles[i];
                    if (rect->y == lastRect->y && rect->x == lastRect->x) {
                        die(lastRect);
                    }
                }
                if (lastRect->y < 0 || lastRect->y >= HEIGHT || lastRect->x < 0 || lastRect->x >= WIDTH) {
                    die(lastRect);
                }
                lastRect->renderAll();
            }
         
            sleep(75);
            move();
        }

        void die(RectangleElement* lastRect) {
            displayMessage->changeText("You scored " + std::to_string(rectangles.size() - 3) + " points. click to restart.");
            paused = true;
            gameover = true;
            lastRect->renderAll();
        }

        void preppendRectangle() {
            auto firstRect = rectangles.front();
            auto rect = new RectangleElement(window, gu(1), gu(1), sf::Color::Green, firstRect->x, firstRect->y);
            rectangles.insert(rectangles.begin(), rect);
        }

        void setFood() {
            food = new RectangleElement(rectangles.back()->window, 1*HEIGHT/GRID_WIDTH, 1*WIDTH/GRID_WIDTH, sf::Color::Red, gu(rand()%GRID_WIDTH), gu(rand()%GRID_WIDTH));
        }
};


int main() {
    srand((unsigned) time(NULL));
  

    bool initialized = false;
    std::string str = "";
    sf::RenderWindow window(sf::VideoMode(WIDTH,HEIGHT), "Snake");

    window.setFramerateLimit(60);

    vector<Element*> els = {};

    auto grid = new GridElement(window, GRID_WIDTH);

    Snake snake(window);
    std::thread move_thread (&Snake::move, &snake);
    snake.displayMessage = new TextElement(window, "Click to start/pause snake game.");
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        if (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                // textDrawer->renderAll();
                if (!snake.gameover) {
                    if (snake.paused) {
                        initialized = true;
                        snake.displayMessage->changeText("");
                        snake.rectangles.back()->renderAll();
                        snake.paused = false;
                    } else {
                        snake.paused = true;
                        snake.displayMessage->changeText("Game paused");
                        snake.displayMessage->renderAll();
                    }
                } else {
                    snake.unsetSnake();
                    snake.setSnake();
                    snake.displayMessage->changeText("");
                    snake.paused = false;
                    snake.gameover = false;
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace) | sf::Keyboard::isKeyPressed(sf::Keyboard::Delete)) {
                size_t strSize = str.size();
                if (strSize > 0) {
                    str.resize(str.size() - 1);
                }
            }   
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                snake.setDirection("right");
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                snake.setDirection("left");
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                snake.setDirection("up");
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                snake.setDirection("down");
            }
                
        }
        sleep(1);
    }

    return 0;
}
