#include "Particle.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <random>

/*
  This constructor will be responsible for generating a randomized shape with numPoints
  vertices, centered on mouseClickPosition mapped to the Cartesian plane, which is
  centered at (0,0) instead of (width / 2, height / 2).
*/
// constructor
Particle::Particle(RenderTarget& target, int numPoints, Vector2i mouseClickPosition)
    : m_A(2, numPoints) // initialize m_A with the chosen size
{
    //Initialize m_ttl with global constant TTL;
    m_ttl = TTL;

    //Initialize m_numPoints with numPoints
    m_numPoints = numPoints;

    //Initialize m_radiansPerSec to a random angular velocity
    m_radiansPerSec = ((float)rand() / (RAND_MAX)) * M_PI;

    //Call setCenter on m_cartesianPlane
    m_CartesianPlane.setCenter(0, 0);

    //Call setSize on m_cartesianPlane
    m_CartesianPlane.setSize(target.getSize().x, (-1.0) * target.getSize().y);

    //Store the location of the center of this particle on the Cartesian plane in m_centerCoordinate
    m_centerCoordinate = target.mapPixelToCoords(mouseClickPosition, m_CartesianPlane);

    //Assign m_vx and m_vy to random pixel velocities
    m_vx = rand() % 401 + 100;

    int random1 = rand() % 2;
    if (random1 != 0)
    {
        m_vx *= -1;
    }

    m_vy = rand() % 401 + 100;


    //Assign m_color1 and m_color2 with Colors, red and blue, we can change later
    m_color1.r = 255;
    m_color1.g = 255;
    m_color1.b = 255;

    m_color2.r = 46;
    m_color2.g = 149;
    m_color2.b = 245;


    //Generate numPoint vertices by sweeping a circular arc with randomized radii
    //Initialize theta to an angle between {0:PI/2}
    float theta = ((float)rand() / (RAND_MAX)) * (M_PI / 2);

    //Initialize dTheta
    float dTheta = 2 * M_PI / (numPoints - 1);

    //Loop from j up to numPoints
    for (int j = 0; j < numPoints; j++)
    {
        //declare local variables r,dx, dy;
        float r, dx, dy;

        //Assign a random number between 20:80 to r
        r = rand() % 61 + 20;

        //Assign dx and dy
        dx = r * cos(theta);
        dy = r * sin(theta);

        //Assign the Cartesian coordinate of the newly generated vertex to m_A
        m_A(0, j) = m_centerCoordinate.x + dx;
        m_A(1, j) = m_centerCoordinate.y + dy;

        //Increment theta by dTheta to move it to the next location for the next iteration
        theta += dTheta;
    }

}

void Particle::draw(RenderTarget& target, RenderStates states) const
{
    // array of numPoints + 1 vertices for a TriangleFan
    sf::VertexArray lines(TriangleFan, m_numPoints + 1); // fixed m_numPoints

    // Store the location on the monitor of the center of our particle
    sf:Vector2i centerPixel = target.mapCoordsToPixel(m_centerCoordinate, m_CartesianPlane);
    sf::Vector2f center(centerPixel);

    // assign lines first element
    lines[0].position = center;
    lines[0].color = m_color1; // instructions say m_color but theres no m_color

    // loop j from 1 up to and including m_numPoints
    // index in lines is 1-off, from index in m_A, lines must contain pixel coord for center
    for (int j = 1; j <= m_numPoints; j++)
    {
        // Retrieve Vector2f coordinates from Matrix m_A
        sf::Vector2f coords (m_A(0, j - 1), m_A(1, j - 1));

        // Map the coords from Cartesian to pixel coords
        sf::Vector2i pixelCoordsInt = target.mapCoordsToPixel(coords, m_CartesianPlane);
        sf::Vector2f pixelCoords(pixelCoordsInt);

        lines[j].position = pixelCoords;
        lines[j].color = m_color2;
    }
    // draw vertex array
    target.draw(lines, states);
}

void Particle::update(float dt)
{
    // subtract dt from m_ttl
    m_ttl -= dt;
    // call rotate and scale
    rotate(dt * m_radiansPerSec);
    scale(SCALE); // percentage to scale per frame (0.999 shrink speed)

    // how far to shift / translate our particle, using distance (dx,dy)
    float dx;
    float dy;

    dx = m_vx * dt;

    // for vertical velocity to allow particle to travel up, down, get pulled by gravity
    m_vy -= G * dt; // G is a global constant of value 0.999 for gravity
    dy = m_vy * dt;

    // call translate using dx and dy
    translate(dx, dy);
}


///shift the Particle by (xShift, yShift) coordinates
///construct a TranslationMatrix T, add it to m_A
void Particle::translate(double xShift, double yShift)
{
    // construct a TranslationMatrix T with the specified shift values xShift & yShift
    Matrices::TranslationMatrix T(xShift, yShift, m_A.getCols());
    // add T to m_A
    m_A = T + m_A;
    // update the particle's center coordinate
    m_centerCoordinate.x += xShift;
    m_centerCoordinate.y += yShift;
}


///rotate Particle by theta radians counter-clockwise
///construct a RotationMatrix R, left mulitply it to m_A
void Particle::rotate(double theta)
{
    // temporarily shift our particle to the origin before rotating it

    // declare temp Vector2f var to hold value of m_centerCoordinate
    Vector2f temp = m_centerCoordinate;

    // call translate to shift our particles center back to origin
    translate(-temp.x, -temp.y);

    // Construct RotationMatrix var with speicfied angle of rotation theta
    RotationMatrix R(theta);

    // make sure to left-multiply R (matrix multiplies lvalues rows into rvalues columns)
    m_A = R * m_A;

    // shift particle back to origin center
    translate(temp.x, temp.y);
}


///Scale the size of the Particle by factor c
///construct a ScalingMatrix S, left multiply it to m_A
void Particle::scale(double c)
{
    // we will have to temporarily shift back to the origin here before scaling

    // store m_centerCoordinate value in a Vector2f temp var
    Vector2f temp = m_centerCoordinate;

    // call translate to shift our particles center back to the origin
    translate(-m_centerCoordinate.x, -m_centerCoordinate.y);

    // Construct a ScalingMatrix S with specified scaling multiplier c
    ScalingMatrix S(c);

    // multiply S by m_A
    m_A = S * m_A;

    // shift particle back to its original center
    translate(temp.x, temp.y);
}


bool Particle::almostEqual(double a, double b, double eps)
{
    return fabs(a - b) < eps;
}


void Particle::unitTests()
{
    int score = 0;

    cout << "Testing RotationMatrix constructor...";
    double theta = M_PI / 4.0;
    RotationMatrix r(M_PI / 4);
    if (r.getRows() == 2 && r.getCols() == 2 && almostEqual(r(0, 0), cos(theta))
        && almostEqual(r(0, 1), -sin(theta))
        && almostEqual(r(1, 0), sin(theta))
        && almostEqual(r(1, 1), cos(theta)))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing ScalingMatrix constructor...";
    ScalingMatrix s(1.5);
    if (s.getRows() == 2 && s.getCols() == 2
        && almostEqual(s(0, 0), 1.5)
        && almostEqual(s(0, 1), 0)
        && almostEqual(s(1, 0), 0)
        && almostEqual(s(1, 1), 1.5))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing TranslationMatrix constructor...";
    TranslationMatrix t(5, -5, 3);
    if (t.getRows() == 2 && t.getCols() == 3
        && almostEqual(t(0, 0), 5)
        && almostEqual(t(1, 0), -5)
        && almostEqual(t(0, 1), 5)
        && almostEqual(t(1, 1), -5)
        && almostEqual(t(0, 2), 5)
        && almostEqual(t(1, 2), -5))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }


    cout << "Testing Particles..." << endl;
    cout << "Testing Particle mapping to Cartesian origin..." << endl;
    if (m_centerCoordinate.x != 0 || m_centerCoordinate.y != 0)
    {
        cout << "Failed.  Expected (0,0).  Received: (" << m_centerCoordinate.x << "," << m_centerCoordinate.y << ")" << endl;
    }
    else
    {
        cout << "Passed.  +1" << endl;
        score++;
    }

    cout << "Applying one rotation of 90 degrees about the origin..." << endl;
    Matrix initialCoords = m_A;
    rotate(M_PI / 2.0);
    bool rotationPassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), -initialCoords(1, j)) || !almostEqual(m_A(1, j), initialCoords(0, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            rotationPassed = false;
        }
    }
    if (rotationPassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Applying a scale of 0.5..." << endl;
    initialCoords = m_A;
    scale(0.5);
    bool scalePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 0.5 * initialCoords(0, j)) || !almostEqual(m_A(1, j), 0.5 * initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            scalePassed = false;
        }
    }
    if (scalePassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Applying a translation of (10, 5)..." << endl;
    initialCoords = m_A;
    translate(10, 5);
    bool translatePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 10 + initialCoords(0, j)) || !almostEqual(m_A(1, j), 5 + initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            translatePassed = false;
        }
    }
    if (translatePassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Score: " << score << " / 7" << endl;
}