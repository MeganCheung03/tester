#include "Engine.h"
#include <iostream> 
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <cstdlib>
#include <ctime>

Engine::Engine()
{
    VideoMode customMode(1440, 1080); // custom resolution
    m_Window.create(customMode, "Particles");
}

void Engine::run()
{
    // local Clock object to track time per fram
    Clock clock;

    // unit test
    cout << "Starting Particle unit tests..." << endl;
    Particle p(m_Window, 4, { (int)m_Window.getSize().x / 2, (int)m_Window.getSize().y / 2 });
    p.unitTests();
    cout << "Unit tests complete.  Starting engine..." << endl;

    // loop for time, input, update, draw
    while (m_Window.isOpen())
    {
        // restart clock
        sf::Time dt = clock.restart();


        // call input
        input();

        // call update and pass dt as Seconds
        update(dt.asSeconds());

        // call draw
        draw();
    }
}


void Engine::input()
{
    Event event;
    while (m_Window.pollEvent(event))
    {
        // if keybaord key is pressed
        if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            m_Window.close();
        }
        // vector to store newly created particles
        std::vector<Particle> newParticles;
        // if left mouse button is pressed
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
        {
            // get position of mouse click
            sf::Vector2i mousePos = sf::Mouse::getPosition(m_Window);
            // loop to construct 5 particles
            int num_particles = 5;
            for (int i = 0; i < num_particles; i++)
            {
                // random number for numPoints wihtin [25:50]
                int numPoints = rand() % 26 + 25;

                // call Particle constructor with random numPoints and mouse pos
                // Particle p(m_Window, numPoints, mousePos);

                // create Particle object and store it in newParticles vector
                newParticles.push_back(Particle(m_Window, numPoints, mousePos));
            }
        }
        // add newParticles to m_Particles vector
        m_Particles.insert(m_Particles.begin(), newParticles.begin(), newParticles.end());
    }
}


void Engine::update(float dtAsSeconds)
{
    // loop through m_particles vector and call update on each Particle in the vectror whose ttl has not expired
    std::vector<Particle>::iterator it; // iterator for a vector

    for (it = m_Particles.begin(); it != m_Particles.end();)
    { // if time to live of the Particle has not expired
        if (it->getTTL() > 0.0)
        { // update the Particle
            it->update(dtAsSeconds);
            // increment the iterator
            it++;
        }
        else
        {
            // erase the particle whose ttl has expired
            it = m_Particles.erase(it);
        }
    }
}


void Engine::draw()
{
    // clear the window
    m_Window.clear();

    // loop through each Particle in m_Particles
    std::vector<Particle>::iterator it;
    for (it = m_Particles.begin(); it != m_Particles.end(); it++)
    { // pass each element into window draw function
        m_Window.draw(*it);
    }

    // display the window
    m_Window.display();
}