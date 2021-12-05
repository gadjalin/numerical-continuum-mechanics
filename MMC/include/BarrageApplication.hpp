#ifndef BARRAGEAPP_HPP
#define BARRAGEAPP_HPP

#include <zengine/zengine.hpp>
#include <zgraphics2D/zgraphics2D.hpp>

#include "Plot.hpp"

class BarrageApplication : public ze::Application
{
public:
   void onConnection() override;
   void onDisconnection() override;

   void tick(ze::Time deltaTime) override;
   void onRender();
   void onEvent(ze::Event& event);

   void setReferenceMesh(Mesh const* mesh);
   void setDistortedMesh(Mesh const* mesh);

   BarrageApplication(zg::GraphicsEngine* gfx);

private:
   zg::GraphicsEngine* m_gfx;

   ze::EventSubscriber<ze::Event> m_eventSub;
   ze::Listener<void ()> m_renderListener;

   Plot m_refPlot;
   zg::Transform m_transform;

   Plot m_distPlot;
   zg::Transform m_distTransform;

   zg::OrthographicCamera m_camera;
   zg::Shader m_shader;

   float m_zoom;
};

#endif // BARRAGEAPP_HPP
