/// ============================================================================
///
/// SpiralGalaxy -- display a rotating spiral galaxy in a wireframe box.
///
/// The window is split vertically into a pair of images for cross-eyed
/// (glasses-free) stereo 3D viewing.  Press any keyboard key to exit.
///
/// This demo uses OpenGL for 3D rendering.  For cross-platform window
/// management, it also uses SFML 2.0 by Laurent Gomila.  Get SFML 2.0 here:
///
///   http://www.sfml-dev.org/
///

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

// Prototypes:
static void renderLoop(sf::Window & App);
static void drawWireframeCube();
static void drawParticles();

// Constants:
static const int     INITIAL_W             = 512;
static const int     INITIAL_H             = 512;
static const float   ROTATION_DEG_PER_SEC  = 30.0f;
static const int     NUM_PARTICLES         = 1024;
static const int     NUM_SPIRAL_ARMS       = 3;
static const float   SPIRAL_DENSITY        = 2.0f;
static const float   SPIRAL_WIDTH          = 0.2f;
static const float   GALAXY_TILT_DEG       = 30.0f;
static const float   WIREFRAME_LINE_WIDTH  = 4.0f;
static const float   POINT_PARTICLE_SIZE   = 4.0f;

static const GLubyte COLOR_FRONT[4] = { 165,   0,   0 };
static const GLubyte COLOR_BACK [4] = {   0, 125,   0 };
static const GLubyte COLOR_INNER[4] = {   0,   0, 155 };

static const float DEG_TO_RAD = float(M_PI / 180.0);
static const float RAD_TO_DEG = float(180.0 / M_PI);

// MAIN:
int main()
{
    srand((unsigned int)(time(NULL)));

    sf::Window app(sf::VideoMode(INITIAL_W, INITIAL_H, 32), "Stereo");

    // These introduce choppiness into the animation, so leave them out for now.
    //// App.SetFramerateLimit(64);
    //// App.SetVerticalSyncEnabled(true);

    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glShadeModel(GL_FLAT);  //SMOOTH);
    glLineWidth(WIREFRAME_LINE_WIDTH);
    glPointSize(POINT_PARTICLE_SIZE);

    renderLoop(app);

    return EXIT_SUCCESS;
}

static void renderLoop(sf::Window & app)
{
    static int w = INITIAL_W;
    static int h = INITIAL_H;
    static int printed = 0;

    sf::Event event;
    sf::Clock clock;

    while (app.isOpen())
    {
        while (app.pollEvent(event))
        {
            if((event.type == sf::Event::Closed) /*||
               (event.type == sf::Event::KeyPressed)*/)
                app.close();

            if (event.type == sf::Event::Resized)
            {
                w = event.size.width;
                h = event.size.height;
                printed = 0;
            }
        }

        // Perspective:
        const float aspect               = (0.5f * float(w)) / float(h);
        const float nearDist             = 1.0f;
        const float farDist              = nearDist * 64;
        const float focalDist            = 2.0f;
        const float ocularSeparation     = focalDist / 12.0f;
        const float fovDegX              = 25.0f;

        // Symmetric frustum properties:
        const float frustumHalfWidth     = nearDist * std::tan(DEG_TO_RAD * fovDegX/2);
        const float frustumHalfHeight    = frustumHalfWidth / aspect;
        const float frustumOffset        = (0.5f * ocularSeparation) * (nearDist / focalDist);

        // Asymmetric frustum properties:
        const float leftEyeFrustumLeft   = -frustumHalfWidth + frustumOffset;
        const float leftEyeFrustumRight  = +frustumHalfWidth + frustumOffset;
        const float rightEyeFrustumLeft  = -frustumHalfWidth - frustumOffset;
        const float rightEyeFrustumRight = +frustumHalfWidth - frustumOffset;

        // Animation:
        const float rotationAmtDeg   = clock.getElapsedTime().asSeconds() * ROTATION_DEG_PER_SEC;
        const float objectDist       = nearDist + 0.5f;
        const float objectElevation  = 0.0f;
        const float objectScale      = 0.45f;

        if( ! printed)
        {
            printed = 1;

            std::cout
                << "aspect               = " << aspect               << "\n"
                << "nearDist             = " << nearDist             << "\n"
                << "farDist              = " << farDist              << "\n"
                << "focalDist            = " << focalDist            << "\n"
                << "ocularSeparation     = " << ocularSeparation     << "\n"
                << "fovDegX              = " << fovDegX              << "\n"
                << "frustumHalfWidth     = " << frustumHalfWidth     << "\n"
                << "frustumHalfHeight    = " << frustumHalfHeight    << "\n"
                << "frustumOffset        = " << frustumOffset        << "\n"
                << "leftEyeFrustumLeft   = " << leftEyeFrustumLeft   << "\n"
                << "leftEyeFrustumRight  = " << leftEyeFrustumRight  << "\n"
                << "rightEyeFrustumLeft  = " << rightEyeFrustumLeft  << "\n"
                << "rightEyeFrustumRight = " << rightEyeFrustumRight << "\n"
                << "objectDist           = " << objectDist           << "\n"
                << "objectElevation      = " << objectElevation      << "\n"
                << "objectScale          = " << objectScale          << "\n"
                << "\n";
        }


        // Clear color and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        ///////////////////////////////////////////////////
        ///
        ///  DRAW LEFT SIDE:  (right eye perspective)
        ///

        glViewport(0, 0, w/2, h);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glFrustum(
            rightEyeFrustumLeft,
            rightEyeFrustumRight,
            -frustumHalfHeight,
            frustumHalfHeight,
            nearDist,
            farDist);

        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(-ocularSeparation/2, objectElevation, -objectDist);
        glScalef(objectScale, objectScale/aspect, objectScale);
        glRotatef(rotationAmtDeg, 0.0f, 1.0f, 0.0f);

        drawWireframeCube();

        glPushMatrix();
        {
            glScalef(0.5f, 0.5f, 0.5f);
            glRotatef(GALAXY_TILT_DEG, 1.0f, 0.0f, 0.0f);
            glRotatef(-rotationAmtDeg*2, 0.0f, 0.0f, 1.0f);
            drawParticles();
        }
        glPopMatrix();

        ///////////////////////////////////////////////////
        ///
        ///  DRAW RIGHT SIDE:  (left eye perspective)
        ///

        glViewport(w/2+1, 0, w/2, h);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glFrustum(
            leftEyeFrustumLeft,
            leftEyeFrustumRight,
            -frustumHalfHeight,
            frustumHalfHeight,
            nearDist,
            farDist);

        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(+ocularSeparation/2, objectElevation, -objectDist);
        glScalef(objectScale, objectScale/aspect, objectScale);
        glRotatef(rotationAmtDeg, 0.0f, 1.0f, 0.0f);

        drawWireframeCube();

        glPushMatrix();
        {
            glScalef(0.5f, 0.5f, 0.5f);
            glRotatef(GALAXY_TILT_DEG, 1.0f, 0.0f, 0.0f);
            glRotatef(-rotationAmtDeg*2, 0.0f, 0.0f, 1.0f);
            drawParticles();
        }
        glPopMatrix();

        // Finally, display rendered frame on screen
        app.display();
    }
}

static void drawWireframeCube()
{
    glBegin(GL_LINES);
    {
        // The +z face:
        glColor3ubv(COLOR_FRONT);    glVertex3f(-0.5f, -0.5f,  0.5f);
        glColor3ubv(COLOR_FRONT);    glVertex3f( 0.5f, -0.5f,  0.5f);
        glColor3ubv(COLOR_FRONT);    glVertex3f( 0.5f, -0.5f,  0.5f);
        glColor3ubv(COLOR_FRONT);    glVertex3f( 0.5f,  0.5f,  0.5f);
        glColor3ubv(COLOR_FRONT);    glVertex3f( 0.5f,  0.5f,  0.5f);
        glColor3ubv(COLOR_FRONT);    glVertex3f(-0.5f,  0.5f,  0.5f);
        glColor3ubv(COLOR_FRONT);    glVertex3f(-0.5f,  0.5f,  0.5f);
        glColor3ubv(COLOR_FRONT);    glVertex3f(-0.5f, -0.5f,  0.5f);

        // The -z face:
        glColor3ubv(COLOR_BACK);     glVertex3f(-0.5f, -0.5f, -0.5f);
        glColor3ubv(COLOR_BACK);     glVertex3f( 0.5f, -0.5f, -0.5f);
        glColor3ubv(COLOR_BACK);     glVertex3f( 0.5f, -0.5f, -0.5f);
        glColor3ubv(COLOR_BACK);     glVertex3f( 0.5f,  0.5f, -0.5f);
        glColor3ubv(COLOR_BACK);     glVertex3f( 0.5f,  0.5f, -0.5f);
        glColor3ubv(COLOR_BACK);     glVertex3f(-0.5f,  0.5f, -0.5f);
        glColor3ubv(COLOR_BACK);     glVertex3f(-0.5f,  0.5f, -0.5f);
        glColor3ubv(COLOR_BACK);     glVertex3f(-0.5f, -0.5f, -0.5f);

        // The z-aligned segments:
        glColor3ubv(COLOR_INNER);    glVertex3f(-0.5f, -0.5f, -0.5f);
        glColor3ubv(COLOR_INNER);    glVertex3f(-0.5f, -0.5f,  0.5f);
        glColor3ubv(COLOR_INNER);    glVertex3f(-0.5f,  0.5f, -0.5f);
        glColor3ubv(COLOR_INNER);    glVertex3f(-0.5f,  0.5f,  0.5f);
        glColor3ubv(COLOR_INNER);    glVertex3f( 0.5f,  0.5f, -0.5f);
        glColor3ubv(COLOR_INNER);    glVertex3f( 0.5f,  0.5f,  0.5f);
        glColor3ubv(COLOR_INNER);    glVertex3f( 0.5f, -0.5f, -0.5f);
        glColor3ubv(COLOR_INNER);    glVertex3f( 0.5f, -0.5f,  0.5f);
    }
    glEnd();
}

static void drawParticles()
{
    static GLubyte POINT_COLORS[NUM_PARTICLES * 3];
    static float   POINT_POSITIONS[NUM_PARTICLES * 3];
    static int     initialized = 0;

    if( ! initialized)
    {
        initialized = 1;

        for(int i=0; i<NUM_PARTICLES; ++i)
        {
            POINT_COLORS[i*3 + 0] = GLubyte(255 * (float(rand()) / RAND_MAX));
            POINT_COLORS[i*3 + 1] = GLubyte(255 * (float(rand()) / RAND_MAX));
            POINT_COLORS[i*3 + 2] = GLubyte(255 * (float(rand()) / RAND_MAX));

            // Pick a random "spiral arm" of the galaxy:
            const int arm = int(NUM_SPIRAL_ARMS * (float(rand())/(RAND_MAX + 1.0f)));

            // Pick a random radial distance along that arm:
            const float distToCenter = float(rand())/RAND_MAX;

            // Twist the spiral arm:
            const float angleDeg = (arm * 360)/NUM_SPIRAL_ARMS + (distToCenter * SPIRAL_DENSITY * 360);

            // Add a small perturbation:
            const float fuzzX = distToCenter * SPIRAL_WIDTH * (-0.5f + (float(rand()) / RAND_MAX));
            const float fuzzY = distToCenter * SPIRAL_WIDTH * (-0.5f + (float(rand()) / RAND_MAX));
            const float fuzzZ = 0.5f * (1.0f - distToCenter) * (1.0f - distToCenter) * SPIRAL_WIDTH * (-0.5f + (float(rand()) / RAND_MAX));

            POINT_POSITIONS[i*3 + 0] = fuzzX + distToCenter * std::cos(DEG_TO_RAD * angleDeg);
            POINT_POSITIONS[i*3 + 1] = fuzzY + distToCenter * std::sin(DEG_TO_RAD * angleDeg);
            POINT_POSITIONS[i*3 + 2] = fuzzZ;
        }
    }

    glBegin(GL_POINTS);
    {
        for(int i=0; i<NUM_PARTICLES; ++i)
        {
            glColor3ubv(&POINT_COLORS[i * 3]);
            glVertex3fv(&POINT_POSITIONS[i*3]);
        }
    }
    glEnd();
}

