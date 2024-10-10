#include <BarrageApplication.hpp>

BarrageApplication::BarrageApplication(zg::GraphicsEngine* gfx)
   : ze::Application("BarrageApplication"), m_gfx(gfx),
     m_eventSub(&BarrageApplication::onEvent, this), m_renderListener(&BarrageApplication::onRender, this),
     m_refPlot({ -1.f, 7.f }, { -1.f, 7.f }), m_distPlot({ -1.f, 7.f }, { -1.f, 7.f }), m_camera(gfx->getWindow().getSize()),
     m_zoom(1.f)
{
   APP_LOG_INFO("Creating Barrage Application...");

   m_gfx->renderingSignal.addListener(m_renderListener);
   ze::Core::UseEventBus().addSubscriber(m_eventSub);
}

void BarrageApplication::onConnection()
{
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   m_shader.loadFile("shaders/default.vs", "shaders/default.fs");
   m_transform.setScale({ 70.f, -70.f, 1.f });
   m_distTransform.setScale({ 70.f, -70.f, 1.f });
   m_transform.setPosition({ -10.f, -4.f, 0.f });
   m_distTransform.setPosition({ 2.f, -4.f, 0.f });
}

void BarrageApplication::onDisconnection() {}

void BarrageApplication::tick(ze::Time deltaTime)
{
   //float speed = .5f;
   //if (zg::Keyboard::IsKeyPressed(zg::Keyboard::Key::D))
   //   m_camera.move({ speed * deltaTime.asMilliseconds(), 0.f});
   //if (zg::Keyboard::IsKeyPressed(zg::Keyboard::Key::A))
   //   m_camera.move({ -speed * deltaTime.asMilliseconds(), 0.f});
   //if (zg::Keyboard::IsKeyPressed(zg::Keyboard::Key::W))
   //   m_camera.move({ 0.f, -speed * deltaTime.asMilliseconds() });
   //if (zg::Keyboard::IsKeyPressed(zg::Keyboard::Key::S))
   //   m_camera.move({ 0.f, speed * deltaTime.asMilliseconds() });

   //if (zg::Keyboard::IsKeyPressed(zg::Keyboard::Key::Up))
   //   m_zoom += 0.002f * deltaTime.asMilliseconds();
   //if (zg::Keyboard::IsKeyPressed(zg::Keyboard::Key::Down))
   //   m_zoom -= 0.002f * deltaTime.asMilliseconds();
   //m_camera.setSize(glm::vec2(m_gfx->getWindow().getSize()) * m_zoom);
}

void BarrageApplication::setReferenceMesh(Mesh const* mesh)
{
   m_refPlot.setMesh(mesh);
}

void BarrageApplication::setDistortedMesh(Mesh const* mesh)
{
   m_distPlot.setMesh(mesh);
}

void BarrageApplication::onRender()
{
   m_shader.use();
   m_shader.setMatrix4("ViewProjectionMatrix", m_camera.getProjectionMatrix() * m_camera.getViewMatrix());
   m_shader.setMatrix4("ModelMatrix", m_transform.getTransformationMatrix());
   m_refPlot.render();
   m_shader.setMatrix4("ModelMatrix", m_distTransform.getTransformationMatrix());
   m_distPlot.render();
}

void BarrageApplication::onEvent(ze::Event& event)
{
   ze::EventDispatcher dispatcher(event);

   dispatcher.dispatch<zg::KeyPressedEvent>(
      [&](zg::KeyPressedEvent& event)
      {
         if (event.getKey() == zg::Keyboard::Key::Escape)
            ze::Core::Stop();
      });
}
