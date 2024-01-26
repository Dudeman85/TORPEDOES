#include <engine/Application.h>
#include <engine/Tilemap.h>
#include <vector>
#include <set>
#include <chrono>
#include <thread>
#include "PlayerController.h"
#include "PickupController.h"
#include "Physics.h"

using namespace std;
using namespace engine;

ECS ecs;


int main()
{
	NO_OPENAL = true;

	Model model2("assets/Finish_line.obj");

	return 0;
}
