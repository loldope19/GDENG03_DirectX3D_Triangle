#include <DX3D/Game/Game.h>
#include <DX3D/Window/Window.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Core/Logger.h>
#include <DX3D/Game/Display.h>
#include <iostream>
#include <DirectXMath.h>

dx3d::Game::Game(const GameDesc& desc) :
    Base({ *std::make_unique<Logger>(desc.logLevel).release() }),
    m_loggerPtr(&m_logger)
{
    m_graphicsEngine = std::make_unique<GraphicsEngine>(GraphicsEngineDesc{ m_logger });
    m_display = std::make_unique<Display>(DisplayDesc{ {m_logger,desc.windowSize},m_graphicsEngine->getGraphicsDevice() });
    
    // Initialize camera and input manager
    m_camera = std::make_unique<Camera>();
    m_inputManager = std::make_unique<InputManager>();
    
    // Set up initial camera properties
    m_camera->setPerspective(DirectX::XM_PIDIV4, static_cast<float>(desc.windowSize.width) / desc.windowSize.height, 0.1f, 1000.0f);
    m_camera->setPosition({ 0.0f, 0.0f, -5.0f });

    // Debug output
    auto pos = m_camera->getPosition();
    std::cout << "Initial Camera Position: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
    
    auto viewMatrix = m_camera->getViewMatrix();
    DirectX::XMFLOAT4X4 viewMatrixFloat;
    DirectX::XMStoreFloat4x4(&viewMatrixFloat, viewMatrix);
    std::cout << "View Matrix:" << std::endl;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << viewMatrixFloat.m[i][j] << " ";
        }
        std::cout << std::endl;
    }

    // Connect camera to graphics engine
    m_graphicsEngine->setCamera(m_camera.get());

    DX3DLogInfo("Game initialized.");
}

dx3d::Game::~Game()
{
    DX3DLogInfo("Game is shutting down...");
}

void dx3d::Game::handleInput()
{
    // Update input state
    m_inputManager->update();

    // Handle camera movement
    const float moveSpeed = 1.0f;
    const float rotateSpeed = 5.0f;

    // WASD movement
    if (m_inputManager->isKeyDown('W')) {
        m_camera->move({ 0.0f, 0.0f, moveSpeed });
        std::cout << "Camera Pos: " << m_camera->getPosition().x << ", " << m_camera->getPosition().y
            << ", " << m_camera->getPosition().z << std::endl;
    }
    if (m_inputManager->isKeyDown('S')) {
        m_camera->move({ 0.0f, 0.0f, -moveSpeed });
        std::cout << "Camera Pos: " << m_camera->getPosition().x << ", " << m_camera->getPosition().y
            << ", " << m_camera->getPosition().z << std::endl;
    }
    if (m_inputManager->isKeyDown('A')) {
        m_camera->move({ -moveSpeed, 0.0f, 0.0f });
        std::cout << "Camera Pos: " << m_camera->getPosition().x << ", " << m_camera->getPosition().y
            << ", " << m_camera->getPosition().z << std::endl;
    }
    if (m_inputManager->isKeyDown('D')) {
        m_camera->move({ moveSpeed, 0.0f, 0.0f });
        std::cout << "Camera Pos: " << m_camera->getPosition().x << ", " << m_camera->getPosition().y
            << ", " << m_camera->getPosition().z << std::endl;
    }
    // Mouse look
    float deltaX = m_inputManager->getMouseDeltaX();
    float deltaY = m_inputManager->getMouseDeltaY();
    m_camera->rotate(deltaX * rotateSpeed, -deltaY * rotateSpeed);
}

void dx3d::Game::onInternalUpdate()
{
    handleInput();
    m_graphicsEngine->render(m_display->getSwapChain());
}
