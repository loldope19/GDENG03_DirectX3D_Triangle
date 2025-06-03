#include <DX3D/Game/Game.h>
#include <DX3D/Window/Window.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Core/Logger.h>
#include <DX3D/Game/Display.h>

dx3d::Game::Game(const GameDesc& desc) :
    Base({ *std::make_unique<Logger>(desc.logLevel).release() }),
    m_loggerPtr(&m_logger)
{
    m_graphicsEngine = std::make_unique<GraphicsEngine>(GraphicsEngineDesc{ m_logger });
    m_display = std::make_unique<Display>(DisplayDesc{ {m_logger,desc.windowSize},m_graphicsEngine->getGraphicsDevice() });

    // Initialize camera stuff
    m_camera = std::make_unique<Camera>();
    m_inputManager = std::make_unique<InputManager>();

    // Set up camera stuff
    m_camera->setPerspective(DirectX::XM_PIDIV4, static_cast<float>(desc.windowSize.width) / desc.windowSize.height, 0.1f, 1000.0f);
    m_camera->setPosition({ 0.0f, 0.0f, -5.0f });

    m_graphicsEngine->setCamera(m_camera.get());

    DX3DLogInfo("Game initialized.");
}

dx3d::Game::~Game()
{
    DX3DLogInfo("Game is shutting down...");
}

void dx3d::Game::onInternalUpdate()
{
    handleInput();
    m_graphicsEngine->render(m_display->getSwapChain());
}

void dx3d::Game::handleInput()
{
    // Update input state
    m_inputManager->update();

    // Handle camera movement
    const float moveSpeed = 0.1f;
    const float rotateSpeed = 0.01f;

    // WASD movement
    if (m_inputManager->isKeyDown('W'))
        m_camera->move({ 0.0f, 0.0f, moveSpeed });
    if (m_inputManager->isKeyDown('S'))
        m_camera->move({ 0.0f, 0.0f, -moveSpeed });
    if (m_inputManager->isKeyDown('A'))
        m_camera->move({ -moveSpeed, 0.0f, 0.0f });
    if (m_inputManager->isKeyDown('D'))
        m_camera->move({ moveSpeed, 0.0f, 0.0f });

    // Mouse look
    float deltaX = m_inputManager->getMouseDeltaX();
    float deltaY = m_inputManager->getMouseDeltaY();
    m_camera->rotate(deltaX * rotateSpeed, -deltaY * rotateSpeed);
}
