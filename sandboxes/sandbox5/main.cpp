#include <engine/Application.h>






using namespace engine;

ECS ecs;



int main()
{

	GLFWwindow *window = engine::CreateGLWindow(800, 600, "Windos");

	engine::EngineLib engine;

	engine::Camera cam = engine::Camera(800, 600);
	cam.SetPosition(Vector3(0, 0, 1000));
	cam.SetRotation(Vector3(45, 0, 0));


	engine::SpriteRenderSystem::SetBackgroundColor(0, 0, 120); // tämä toimii decimali rgb codi 

	Model model("assets/LaMuerte.obj");

	/*Texture textura1("assets/laatikko.png", GL_LINEAR);
	Texture textura2("assets/latikko.png", GL_LINEAR);*/

	// Tee uusi Entiteetti. crea una nueva entida 

	Entity zuzanne = ecs.newEntity();
	/*Entity sprite1 = ecs.newEntity();*/


	// Add the sprite component with a texture and default shader
	// agregae el componente sprite con una textura y un shader predecterminado

	Transform& suzanneTransform = ecs.addComponent(zuzanne, Transform{ .position = Vector3(0,0,0), .rotation = Vector3(0,0,0), .scale = Vector3(60)  });

	//Add the transform component which is required for the Render System
	// Agregar el componente de trasformacion, que es nesesario para el Render System
	// dentro de clase Transform estan los componetes relacionados con la (rotacion , escala y posicion)

	ecs.addComponent(zuzanne, ModelRenderer{ .model = &model });
	/*ecs.addComponent(sprite1, Transform{ .x = 10, .y = -100, .xScale = 100, .yScale = 100 });
	ecs.addComponent(sprite2, Transform{ .x = -100, .y = 100, .xScale = 100, .yScale = 100 });*/
	engine.modelRenderSystem->SetLight(Vector3(0,200,200) , Vector3(255.0f));

	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			suzanneTransform.rotation.y += 1;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			suzanneTransform.rotation.y += -1;
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			suzanneTransform.rotation.x += -1;
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			suzanneTransform.rotation.x += 1;



		engine.Update(&cam);
		/*suzanneTransform.rotation = +1;
		suzanneTransform.rotation = +1;*/




		glfwSwapBuffers(window);
		glfwPollEvents();

	}



	glfwTerminate();


	return 0;
};

