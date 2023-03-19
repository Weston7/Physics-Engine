#include <iostream>
#include "SFML\Graphics.hpp"
#include "SFML\Window.hpp"
#include "SFML\System.hpp"
#include <math.h>
#include "Button.h"

class GravitySource
{
    sf::Vector2f pos;
    float strength;
    sf::CircleShape s;


public:
    GravitySource(float pos_x, float pos_y, float strength)
    {
        pos.x = pos_x;
        pos.y = pos_y;
        this->strength = strength;

        s.setPosition(pos);
        s.setFillColor(sf::Color::White);
        s.setRadius(20);
    }

    void render(sf::RenderWindow& wind)
    {
        wind.draw(s);
    }

    sf::Vector2f get_pos()
    {
        return pos;
    }

    float get_strength()
    {
        return strength;
    }
};



class Particle {
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::CircleShape s;

public:
    Particle()
    {
        s.setFillColor(sf::Color::White);
        s.setRadius(20);
    }

    void start_placing(sf::Vector2f pos) {
        this->pos = pos;
    }

    void update_placing(sf::Vector2f pos) {
        this->pos = pos;
    }

    void end_placing() {
        vel.x = 0;
        vel.y = 0;
    }

    void render(sf::RenderWindow& wind)
    {
        s.setPosition(pos);
        wind.draw(s);
    }

    void update_physics(GravitySource& s)
    {
        float distance_x = s.get_pos().x - pos.x;
        float distance_y = s.get_pos().y - pos.y;

        float distance = sqrt(distance_x * distance_x + distance_y * distance_y);

        float inverse_distance = 1.f / distance;

        float normalized_x = inverse_distance * distance_x;
        float normalized_y = inverse_distance * distance_y;

        float inverse_square_dropoff = inverse_distance * inverse_distance;

        float acceleration_x = normalized_x * s.get_strength() * inverse_square_dropoff;
        float acceleration_y = normalized_y * s.get_strength() * inverse_square_dropoff;

        vel.x += acceleration_x;
        vel.y += acceleration_y;

        pos.x += vel.x;
        pos.y += vel.y;
    }

};



using namespace sf;

int main()
{
    const unsigned WINDOW_WIDTH = 3840;
    const unsigned WINDOW_HEIGHT = 2160;
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML Works!");
    window.setFramerateLimit(120);

    float dt = 0;
    Clock dt_clock;

    GravitySource source(1920, 1080, 7000);
    GravitySource current_source(0, 0, 0); // test

    Particle particle;
    bool placing_particle = false;
    bool placing_source = false;

    const float gridSize = 50.f;

    Vector2i mousePosGrid;

    sf::Vector2f rectanglePosition(600, 350);

    float xVelocity = 3;
    float yVelocity = 3;


    //Player
    const float movementSpeed = 400.f;
    Vector2f velocity;
    RectangleShape player;
    player.setFillColor(Color::Green);
    player.setSize(Vector2f(gridSize, gridSize));

    //Font
    sf::Font font;

    if (!font.loadFromFile("C:\\Fonts\\Tahoma.ttf"))
    {
    }

    //Button Square Placement
    Button wallPlacementButton(50.f, 100.f, 150.f, 150.f, "Squares", &font, sf::Color::Blue, sf::Color::Green, sf::Color::Red);

    //Button Circle Placement
    Button myButton(50.f, 300.f, 150.f, 150.f, "Circles", &font, sf::Color::Blue, sf::Color::Green, sf::Color::Red);

    //Gravity Source
    Button gravitySourceButton(50.f, 500.f, 150.f, 150.f, "Gravity Source", &font, sf::Color::Blue, sf::Color::Green, sf::Color::Red);

    //Particle Placement
    Button particlePlacementButton(50.f, 700.f, 150.f, 150.f, "Particles", &font, sf::Color::Blue, sf::Color::Green, sf::Color::Red);


    //Circle
    std::vector<sf::CircleShape> circles;

    //Particle
    std::vector<Particle> particles;


    std::vector<GravitySource> sources;
    sources.push_back(current_source);

    std::vector<RectangleShape> walls;

    RectangleShape wall;
    wall.setFillColor(Color::Yellow);
    wall.setSize(Vector2f(gridSize, gridSize));


    //Collision
    FloatRect nextPos;

    // 0 = no placement || 1 = Square Placement || 2 = Circle Placement
    int currentPlacementMode = 0;
    bool placing = false;



    while (window.isOpen()) {
        dt = dt_clock.restart().asSeconds();

        mousePosGrid.x = Mouse::getPosition(window).x / (int)gridSize;
        mousePosGrid.y = Mouse::getPosition(window).y / (int)gridSize;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }


        // Placement 0 = no placement || 1 = Square Placement || 2 = Circle Placement || 3 = Particle Placement || 4 = Gravity Source Placement
        if (Mouse::isButtonPressed(Mouse::Left))
        {
            switch (currentPlacementMode)
            {
            case 0:
                break;
            case 1:
            {
                bool exists = false;
                for (size_t i = 0; i < walls.size() && !exists; i++)
                {
                    if (walls[i].getPosition().x / (int)gridSize == mousePosGrid.x
                        && walls[i].getPosition().y / (int)gridSize == mousePosGrid.y)
                    {
                        exists = true;
                    }
                }

                if (!exists)
                {
                    wall.setPosition(mousePosGrid.x * gridSize, mousePosGrid.y * gridSize);
                    walls.push_back(wall);
                }
                break;
            }

            case 2:
            {
                static sf::Clock clock;
                bool mouseClicked = false;
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mouseClicked && clock.getElapsedTime().asMilliseconds() > 500) {
                    sf::Vector2f position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    sf::CircleShape circle(50.f);
                    circle.setFillColor(sf::Color::Green);
                    circle.setPosition(position);
                    circles.push_back(circle);
                    mouseClicked = true;
                    clock.restart();
                }
                if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    mouseClicked = false;
                }
                break;
            }

            case 3:
            {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    // create a new particle and start placing it
                    Particle p;
                    p.start_placing(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                    particles.push_back(p);
                    placing = true;
                }
                // check if left mouse button is released
                else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    // end placing the particle
                    particles.back().end_placing();
                    placing = false;
                }

                // update the particles
                if (placing) {
                    particles.back().update_placing(sf::Vector2f(sf::Mouse::getPosition(window)));
                }
                else {
                    for (auto& p : particles) {
                        p.update_physics(source);
                    }
                }
                break;
            }



            case 4: // Place GravitySource
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    if (!placing_source) {
                        // Start placing a new gravity source
                        placing_source = true;
                        sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                        current_source = GravitySource(pos.x, pos.y, 50.f); // Set default strength to 50
                    }
                    else {
                        // Update position of the current gravity source
                        sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                        current_source = GravitySource(pos.x, pos.y, current_source.get_strength());
                    }
                }
                else if (placing_source) {
                    // End placing a new gravity source
                    placing_source = false;
                    sources.push_back(GravitySource(current_source.get_pos().x, current_source.get_pos().y, current_source.get_strength()));
                }
                break;


            default:
                break;
            }
        }

        //0 = no placement || 1 = Square Removal || 2 = Circle Removal || 3 = Particle Removal || 4 = Gravity Source Removal
        if (Mouse::isButtonPressed(Mouse::Right))
        {
            switch (currentPlacementMode)
            {
            case 0:
                break;
            case 1:
            {
                bool exists = false;
                int index = -1;
                for (size_t i = 0; i < walls.size() && !exists; i++)
                {
                    if (walls[i].getPosition().x / (int)gridSize == mousePosGrid.x
                        && walls[i].getPosition().y / (int)gridSize == mousePosGrid.y)
                    {
                        exists = true;
                        index = i;
                    }
                }
                if (exists)
                {
                    walls.erase(walls.begin() + index);
                }
                break;
            }

            case 2:
            {
                sf::Vector2f position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                for (auto it = circles.begin(); it != circles.end(); ++it)
                {
                    if (it->getGlobalBounds().contains(position))
                    {
                        circles.erase(it);
                        break;
                    }
                }
            }

            default:
                break;
            }


        }

        // Update the circles
        float deltaTime = dt_clock.restart().asSeconds();
        for (auto& circle : circles)
        {
            // Move the circle
            circle.move(50.f * deltaTime, 50.f * deltaTime);

            // Check for collisions with other circles
            for (auto& otherCircle : circles)
            {
                if (&circle != &otherCircle) // Don't check for collision with itself
                {
                    sf::FloatRect intersection;
                    if (circle.getGlobalBounds().intersects(otherCircle.getGlobalBounds(), intersection))
                    {
                        // Collision detected, reverse the direction of the circle
                        circle.move(-2 * intersection.width, -2 * intersection.height);
                    }
                }
            }
        }

        //Button Update
        sf::Vector2f mousePos(sf::Mouse::getPosition(window));
        myButton.update(mousePos);

        if (myButton.isPressed())
        {
            currentPlacementMode = 2;

        }

        //Wall Placement Button Update
        wallPlacementButton.update(mousePos);

        if (wallPlacementButton.isPressed())
        {
            currentPlacementMode = 1;

        }


        //Particle Placement Button Update
        particlePlacementButton.update(mousePos);

        if (particlePlacementButton.isPressed())
        {
            currentPlacementMode = 3;

        }

        //Gravity Source Button Update
        gravitySourceButton.update(mousePos);

        if (gravitySourceButton.isPressed())
        {
            currentPlacementMode = 4;

        }


        //Player Movement
        velocity.y = 0.f;
        velocity.x = 0.f;


        if (Keyboard::isKeyPressed(Keyboard::W))
        {
            velocity.y += -movementSpeed * dt;
        }
        if (Keyboard::isKeyPressed(Keyboard::S))
        {
            velocity.y += movementSpeed * dt;
        }
        if (Keyboard::isKeyPressed(Keyboard::A))
        {
            velocity.x += -movementSpeed * dt;
        }
        if (Keyboard::isKeyPressed(Keyboard::D))
        {
            velocity.x += movementSpeed * dt;
        }

        //Collision
        for (auto& wall : walls)
        {
            FloatRect playerBounds = player.getGlobalBounds();
            FloatRect wallBounds = wall.getGlobalBounds();

            nextPos = playerBounds;
            nextPos.left += velocity.x * 10;
            nextPos.top += velocity.y * 10;

            if (wallBounds.intersects(nextPos))
            {

                //Bottom Collision
                if (playerBounds.top < wallBounds.top
                    && playerBounds.top + playerBounds.height < wallBounds.top + wallBounds.height
                    && playerBounds.left < wallBounds.left + wallBounds.width
                    && playerBounds.left + playerBounds.width > wallBounds.left
                    )
                {
                    velocity.y = 0.f;
                    player.setPosition(playerBounds.left, wallBounds.top - playerBounds.height);
                }

                //Top Collision
                else if (playerBounds.top > wallBounds.top
                    && playerBounds.top + playerBounds.height > wallBounds.top + wallBounds.height
                    && playerBounds.left < wallBounds.left + wallBounds.width
                    && playerBounds.left + playerBounds.width > wallBounds.left
                    )

                {
                    velocity.y = 0.f;
                    player.setPosition(playerBounds.left, wallBounds.top + wallBounds.height);
                }




                // Right Collision
                else if (playerBounds.left < wallBounds.left
                    && playerBounds.left + playerBounds.width < wallBounds.left + wallBounds.width
                    && playerBounds.top < wallBounds.top + wallBounds.height
                    && playerBounds.top + playerBounds.height > wallBounds.top
                    )
                {
                    velocity.x = 0.f;
                    player.setPosition(wallBounds.left - playerBounds.width, playerBounds.top);
                }

                // Left Collision
                else if (playerBounds.left > wallBounds.left
                    && playerBounds.left + playerBounds.width > wallBounds.left + wallBounds.width
                    && playerBounds.top < wallBounds.top + wallBounds.height
                    && playerBounds.top + playerBounds.height > wallBounds.top
                    )
                {
                    velocity.x = 0.f;
                    player.setPosition(wallBounds.left + wallBounds.width, playerBounds.top);
                }
            }
        }


        player.move(velocity);

        //Collision Screen
        //Left Collision
        if (player.getPosition().x < 0.f)
            player.setPosition(0.f, player.getPosition().y);

        //Top Collision
        if (player.getPosition().y < 0.f)
            player.setPosition(player.getPosition().x, 0.f);

        //Right Collision
        if (player.getPosition().x + player.getGlobalBounds().width > WINDOW_WIDTH)
            player.setPosition(WINDOW_WIDTH - player.getGlobalBounds().width, player.getPosition().y);

        //Bottom Collision
        if (player.getPosition().y + player.getGlobalBounds().height > WINDOW_HEIGHT)
            player.setPosition(player.getPosition().x, WINDOW_HEIGHT - player.getGlobalBounds().height);


        //Render
        window.clear();
        particle.update_physics(source);

        window.draw(player);
        myButton.render(&window);
        wallPlacementButton.render(&window);
        gravitySourceButton.render(&window);
        particlePlacementButton.render(&window);


        // Draw Walls
        for (auto& i : walls)
        {
            window.draw(i);
        }

        // Draw Circles
        for (auto& circle : circles)
        {
            window.draw(circle);
        }
        //Draw Particles
        for (auto& p : particles) {
            p.render(window);
        }

        source.render(window);
        particle.render(window);

        window.display();
    }

    return EXIT_SUCCESS;
}

