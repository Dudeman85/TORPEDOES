#include <engine/Application.h>

#define WIN32_LEAN_AND_MEAN

#include <engine/Networking.h>
#include <engine/Primitive.h>
#include <map>
#include <stdio.h>


using namespace engine;

//Creates a global instance of the ECS manager
ECS ecs;

void OnCollision(Collision c)
{
	cout << "a";
}

int main()
{
	NO_OPENAL = true;

	//Create the window and OpenGL context before creating EngineLib
	//Paraeters define window size(x,y) and name
	GLFWwindow* window = CreateGLWindow(1000, 1000, "Window");
	//Initialize the default engine library
	EngineLib engine;
	//Create the camera
	Camera cam = Camera(400, 400);
	cam.perspective = false;
	cam.SetPosition(Vector3(0, -0, 2000));
	engine.modelRenderSystem->SetLight(Vector3(0, 0, 200), Vector3(255));

	//changes window backround color
	SpriteRenderSystem::SetBackgroundColor(0, 125, 200);
	//engine.physicsSystem->gravity = Vector3(0, -9.81, 0);

	//Model loading
	Model model("assets/achelous.obj");
	Model model2("assets/LaMuerte.obj");

	Entity shipA = ecs.newEntity();
	Transform& aTransform = ecs.addComponent(shipA, Transform{ .position = Vector3(80, -0, 20), .rotation = Vector3(90, 0, 0), .scale = Vector3(10) });
	ecs.addComponent(shipA, ModelRenderer{ .model = &model });
	ecs.addComponent(shipA, Rigidbody{ .gravityScale = 0.2, .drag = 0.01, .restitution = 0.5 });
	vector<Vector2> aColliderVerts{ Vector2(5, 5), Vector2(5, -5), Vector2(-5, -5), Vector2(-5, 5) };
	PolygonCollider& shipCollider = ecs.addComponent(shipA, PolygonCollider{ .vertices = aColliderVerts, .callback = OnCollision, .trigger = false, .visualise = true });

	Entity shipB = ecs.newEntity();
	Transform& bTransform = ecs.addComponent(shipB, Transform{ .position = Vector3(0, 0, 20), .rotation = Vector3(90, 200, 0), .scale = Vector3(15) });
	ecs.addComponent(shipB, ModelRenderer{ .model = &model });
	ecs.addComponent(shipB, Rigidbody{ .gravityScale = 0.0, .restitution = 0.5 });
	vector<Vector2> bColliderVerts{ Vector2(5, 5), Vector2(5, -5), Vector2(-5, -5), Vector2(-5, 5) };
	ecs.addComponent(shipB, PolygonCollider{ .vertices = bColliderVerts, .visualise = true, .rotationOverride = 200 });

	Entity shipC = ecs.newEntity();
	//Transform& cTransform = ecs.addComponent(shipC, Transform{ .position = Vector3(10, 0, 0), .rotation = Vector3(0, 0, 0), .scale = Vector3(1), .parent = shipA });
	ecs.addComponent(shipC, ModelRenderer{ .model = &model2 });

	//PhysicsSystem::Impulse(shipB, Vector3(0, 200, 0));

	engine.collisionSystem->cam = &cam;

	//Load the tilemap
	Tilemap map(&cam);
	map.loadMap("assets/torptest.tmx");
	//engine.spriteRenderSystem->SetTilemap(&map);

	//engine.collisionSystem->SetTilemap(&map);

	float rotationSpeed = 5;
	float angle = 0;

	//Game Loop
	while (!glfwWindowShouldClose(window))
	{
		//Close window when Esc is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		//Rotate CW
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			angle -= rotationSpeed;
			if (angle < 0)
				angle += 360;

			TransformSystem::Rotate(shipA, Vector3(0, -rotationSpeed, rotationSpeed));
		}
		//Rotate CCW
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			angle += rotationSpeed;
			if (angle >= 360)
				angle -= 360;

			TransformSystem::Rotate(shipA, Vector3(0, rotationSpeed, -rotationSpeed));
		}
		//Thrust
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			//TransformSystem::Translate(shipA, TransformSystem::ForwardVector(shipA) * 8);
			PhysicsSystem::Impulse(shipA, TransformSystem::ForwardVector(shipA) * 8);
		}

		//cam.Rotate(Vector3(0,3, 0));

		shipCollider.rotationOverride = angle;

		//TransformSystem::Rotate(shipC, Vector3(0, 1, 0));

		//Update all engine systems, this usually should go last in the game loop
		//For greater control of system execution, you can update each one manually
		engine.Update(&cam);

		//OpenGL stuff, goes very last
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}