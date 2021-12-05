#include <iostream>
#include <cmath>

#include <zengine/zengine.hpp>
#include <zgraphics2D/zgraphics2D.hpp>
#include "BarrageApplication.hpp"
#include "Mesh.hpp"

// Note: the word "Barrage" throughout this project actually means "Dam"

void showResults(Mesh const& reference, Mesh const& distorted);

int main()
{
   // Mesh properties
   size_t n = 20;
   float opposite_length = 6.f;
   float adjacent_length = 6.f;

   // Physical properties
   float g = 9.81f;

   Material concrete = { .E = 1000000.f, .nu = .3f, .rho = 2000.f };
   float rho_w = 1000.f;

   // Barrage calculations
   Mesh referenceBarrage = create_right_triangle_shaped_mesh(n, opposite_length, adjacent_length);
   Mesh distortedBarrage = compute_barrage_distortion(referenceBarrage, g, rho_w, concrete, opposite_length);

   showResults(referenceBarrage, distortedBarrage);

   return 0;
}

void showResults(Mesh const& reference, Mesh const& distorted)
{
   ze::Core::Initialise();
   ze::Core::SetTickRate(60);

   zg::GraphicsSettings settings;
   //settings.window.resisable = false;
   settings.context.versionMajor = 3;
   settings.context.versionMinor = 3;
   settings.context.openglProfile = zg::ContextSettings::Profile::Core;
   //settings.context.forwardCompatibility = true;
   settings.color = zg::Colors::Black;
   settings.size = { 1600, 1000 };

   zg::GraphicsEngine gfx(settings);

   ze::Core::ConnectEngine(gfx);

   BarrageApplication app(&gfx);

   ze::Core::PlaceApplication(app);

   app.setReferenceMesh(&reference);
   app.setDistortedMesh(&distorted);
   ze::Core::Run();

   ze::Core::Terminate();
}