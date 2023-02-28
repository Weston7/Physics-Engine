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


class Particle
{
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::CircleShape s;


public:
    Particle(float pos_x, float pos_y, float vel_x, float vel_y)
    {
        pos.x = pos_x;
        pos.y = pos_y;

        vel.x = vel_x;
        vel.y = vel_y;

        s.setPosition(pos);
        s.setFillColor(sf::Color::White);
        s.setRadius(20);
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

//Button
Button::Button(float x, float y, float width, float height,
    std::string text, sf::Font* font,
    sf::Color idleColor, sf::Color hoverColor, sf::Color activeColor)
{
    this->buttonState = BTN_IDLE;

    this->shape.setPosition(sf::Vector2f(x, y));
    this->shape.setSize(sf::Vector2f(width, height));


    this->font = font;
    this->text.setFont(*this->font);
    this->text.setString(text);
    this->text.setFillColor(sf::Color::White);
    this->text.setCharacterSize(12);
    this->text.setPosition(
        this->shape.getPosition().x / 2.f + (this->shape.getGlobalBounds().width / 2.f) - this->text.getGlobalBounds().width / 2.f,
        this->shape.getPosition().y + (this->shape.getGlobalBounds().height / 2.f) - this->text.getGlobalBounds().height / 2.f
    );

    this->idleColor = idleColor;
    this->hoverColor = hoverColor;
    this->activeColor = activeColor;

    this->shape.setFillColor(this->idleColor);
}

Button::~Button()
{

}

const bool Button::isPressed() const
{
    if (this->buttonState == BTN_ACTIVE)
        return true;


    return false;
}

void Button::update(const sf::Vector2f mousePos)
{
    //Update the booleans for hover and pressed

    //Idle
    this->buttonState = BTN_IDLE;

    //Hover
    if (this->shape.getGlobalBounds().contains(mousePos))
    {
        this->buttonState = BTN_HOVER;


        //Pressed
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            this->buttonState = BTN_ACTIVE;
        }
    }

    switch (this->buttonState)
    {
    case BTN_IDLE:
        this->shape.setFillColor(this->idleColor);
        break;

    case BTN_HOVER:
        this->shape.setFillColor(this->hoverColor);
        break;

    case BTN_ACTIVE:
        this->shape.setFillColor(this->activeColor);
        break;

    default:
        this->shape.setFillColor(sf::Color::Red);
        break;
    }
}




void Button::render(sf::RenderTarget* target)
{
    target->draw(this->shape);
}


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

    Particle particle(1520, 800, 3, 0);

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

    //Walls
    std::vector<RectangleShape> walls;

    RectangleShape wall;
    wall.setFillColor(Color::Yellow);
    wall.setSize(Vector2f(gridSize, gridSize));
    wall.setPosition(gridSize * 5, gridSize * 2);

    walls.push_back(wall);

    //Collision
    FloatRect nextPos;



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

     //Add Walls
        if (Mouse::isButtonPressed(Mouse::Left))
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
        } 

        //Remove Wall
        if (Mouse::isButtonPressed(Mouse::Right))
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

        for (auto& i : walls)
        {
            window.draw(i);
        }
        source.render(window);
        particle.render(window);

        window.display();
    }

    return EXIT_SUCCESS;
}

