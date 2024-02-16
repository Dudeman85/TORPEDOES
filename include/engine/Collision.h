#pragma once
#include <engine/Vector.h>
#include <engine/ECS.h>
#include <engine/Primitive.h>
#include <engine/Tilemap.h>
#include <vector>
#include <array>
#include <functional>
#include <unordered_set>

namespace engine
{
	///Collision Event struct, not a component
	struct Collision
	{
		enum class Type { miss, collision, trigger, tilemapCollision, tilemapTrigger };

		Type type;

		///The entity which instigated the collision
		ecs::Entity a;
		///The entity which was subject to the collision, In a tilemap collision this will be the tile ID
		ecs::Entity b;

		///The point of collision, will always be a vertice of one collider
		Vector3 point;
		///The normal of the collision surface, will always be the normal of a side of the other collider than point
		Vector3 normal;
		///Minimum Translation Vector is the smallest translation needed to end overlap
		Vector3 mtv;
	};

	///Polygon Collider component
	ECS_REGISTER_COMPONENT(PolygonCollider)
		struct PolygonCollider
	{
		///The vertices of the polygon making up the collider, going clockwise. The vertices must form a convex polygon
		std::vector<Vector2> vertices;
		///Callback function on collision
		std::function<void(Collision)> callback;
		///Should the collider only act as a trigger
		bool trigger = false;
		///Draw this collider
		bool visualise = false;
		///Override the rotation of the collider, (0-360)degrees. This is useful if attaching a 2D collider to a 3D model
		float rotationOverride = -1;
		//The axis-aligned bounding box of the collider. This is updated automatically and is set in world coordinates
		std::array<float, 4> bounds;
	};

	///Collision System, Requires Transform and PolygonCollider
	ECS_REGISTER_SYSTEM(CollisionSystem, Transform, PolygonCollider)
		class CollisionSystem : public ecs::System
	{
	public:

		///Called every frame
		void Update()
		{
			//For each entity
			for (auto itr = entities.begin(); itr != entities.end();)
			{
				ecs::Entity entity = *itr++;

				PolygonCollider& collider = ecs::GetComponent<PolygonCollider>(entity);
				Transform& transform = ecs::GetComponent<Transform>(entity);

				//Update bounding box if the entity has moved and check collision if it is a trigger 
				if (transform.staleCache)
				{
					UpdateAABB(entity);
					if (collider.trigger)
					{
						CheckCollision(entity);
					}
				}

				//Draw the bounding box and polygon collider
				if (collider.visualise)
				{
					//Calculate the vertices of the collision and bounding boxes in world coordinates 
					float rotation = collider.rotationOverride >= 0 ? collider.rotationOverride : transform.rotation.z;
					std::vector<Vector2> colliderVerts = TransformSystem::ApplyTransforms2D(collider.vertices, rotation, transform.scale, transform.position);
					Primitive colliderPrimitive = Primitive::Polygon(colliderVerts);
					std::vector<Vector2> boundingBoxVerts{ Vector2(collider.bounds[3], collider.bounds[0]), Vector2(collider.bounds[1], collider.bounds[0]), Vector2(collider.bounds[1], collider.bounds[2]), Vector2(collider.bounds[3], collider.bounds[2]) };
					Primitive boundingBoxPrimitive = Primitive::Polygon(boundingBoxVerts);

					//Draw those vertices
					colliderPrimitive.Draw(cam, Vector3(255, 0, 0), Transform{ .position = Vector3(0, 0, 0) });
					boundingBoxPrimitive.Draw(cam, Vector3(0, 255, 0), Transform{ .position = Vector3(0, 0, 0) });
				}
			}
		}

		///Checks collision between entity a and every other entity, Returns the collisions from the perspective of a, and calls every applicable callback function
		std::vector<Collision> CheckCollision(ecs::Entity a)
		{
			Transform& aTransform = ecs::GetComponent<Transform>(a);
			PolygonCollider& aCollider = ecs::GetComponent<PolygonCollider>(a);

			//Check tilemap collision
			std::vector<Collision> tilemapCollisions = CheckTilemapCollision(a);
			std::vector<Collision> entityCollisions;

			//For each entity
			for (ecs::Entity b : entities)
			{
				//Don't collide with self
				if (a == b)
					continue;

				//Check AABB collision first because it's cheaper
				if (!AABBIntersect(a, b))
					continue;

				Collision collision = CheckEntityCollision(a, b);
				if (collision.type != Collision::Type::miss)
					entityCollisions.push_back(collision);
			}

			//For each collision call a and b callbacks
			for (const Collision& entityCollision : entityCollisions)
			{
				PolygonCollider& bCollider = ecs::GetComponent<PolygonCollider>(entityCollision.b);

				//Setup collision event from b's perspective
				Collision bToA;
				bToA.a = entityCollision.b;
				bToA.b = a;
				bToA.type = entityCollision.type;
				bToA.normal = Vector2() - entityCollision.normal;
				bToA.mtv = Vector2() - entityCollision.mtv;

				if (ecs::EntityExists(entityCollision.b))
				{
					//Call b's callback
					if (bCollider.callback)
						bCollider.callback(bToA);
				}
				if (ecs::EntityExists(entityCollision.a))
				{
					//Call a's callback
					if (aCollider.callback)
						aCollider.callback(entityCollision);
				}
			}
			//For each tilemap collision call a's destructor
			for (const Collision& tilemapCollision : tilemapCollisions)
			{
				if (ecs::EntityExists(tilemapCollision.a))
				{
					//Call a's callback
					if (aCollider.callback)
						aCollider.callback(tilemapCollision);
				}
			}

			//Combine the tilemap and entity collisions
			std::vector<Collision> collisions = tilemapCollisions;
			collisions.insert(collisions.end(), entityCollisions.begin(), entityCollisions.end());

			return collisions;
		}

		///Checks for collision between a tilemap collision layer and an entity. Does not call callbacks
		std::vector<Collision> CheckTilemapCollision(ecs::Entity entity)
		{
			//If no tilemap collision layer is set return no collisions
			if (!tilemap)
				return std::vector<Collision>();

			Transform& transform = ecs::GetComponent<Transform>(entity);
			PolygonCollider& collider = ecs::GetComponent<PolygonCollider>(entity);

			//Log the collisions
			std::vector<Collision> collisions;

			//Find collider tiles inside bounding box
			std::vector<Vector2> possibleCollisions = tilemap->CheckCollisionBox(Vector2(collider.bounds[3], collider.bounds[0]), Vector2(collider.bounds[1], collider.bounds[2]));

			//Apply transforms to entity verts
			float rotation = collider.rotationOverride >= 0 ? collider.rotationOverride : transform.rotation.z;
			std::vector<Vector2> entityVerts = TransformSystem::ApplyTransforms2D(collider.vertices, rotation, transform.scale, transform.position);

			//Vertices of a tile
			std::vector<Vector2> tileVerts{
				Vector2(-((float)tilemap->tileSize.x / 2), (float)tilemap->tileSize.y / 2), //Top-Left
					Vector2((float)tilemap->tileSize.x / 2, (float)tilemap->tileSize.y / 2),  //Top-Right
					Vector2((float)tilemap->tileSize.x / 2, -((float)tilemap->tileSize.y / 2)), //Bottom-Right
					Vector2(-((float)tilemap->tileSize.x / 2), -((float)tilemap->tileSize.y / 2)) //Bottom-Left
			};
			//Visualise tile collider
			Primitive tileCollider = Primitive::Polygon(tileVerts);

			//For each tile inside the bounding box
			for (int i = 0; i < possibleCollisions.size(); i++)
			{
				//Move the tile verts to position
				Vector2 tilePosition = tilemap->GetTilePosition(possibleCollisions[i].x, possibleCollisions[i].y);
				std::vector<Vector2> transformedTileVerts = TransformSystem::ApplyTransforms2D(tileVerts, 0, 1, tilePosition);
				//Check entity-tile collision
				Collision collision = SATIntersect(entityVerts, transformedTileVerts);

				//If collided with tile
				if (collision.type != Collision::Type::miss)
				{
					//Draw tiles which are colliding
					if (collider.visualise)
						tileCollider.Draw(cam, Vector3(255, 0, 0), Transform{ .position = Vector3(tilePosition, 10) });

					//If the mtv is facing in to the other collider, flip it
					Vector3 directionAtoB = transform.position - tilePosition;
					if (directionAtoB.Dot(collision.mtv) < 0)
						collision.mtv = Vector2() - collision.mtv;

					//Setup the collision and call the callback
					collision.a = entity;
					collision.b = tilemap->collisionLayer[possibleCollisions[i].x][possibleCollisions[i].y];
					collision.type = collider.trigger ? Collision::Type::tilemapTrigger : Collision::Type::tilemapCollision;

					collisions.push_back(collision);
				}
				else
				{
					//Draw tiles inside bounding box
					if (collider.visualise)
						tileCollider.Draw(cam, Vector3(0, 255, 0), Transform{ .position = Vector3(tilePosition, 10) });
				}
			}

			return collisions;
		}

		///Check Entity-Entity collision. Does not call callbacks
		static Collision CheckEntityCollision(ecs::Entity a, ecs::Entity b)
		{
			//Get relevant components from a and b
			Transform& aTransform = ecs::GetComponent<Transform>(a);
			PolygonCollider& aCollider = ecs::GetComponent<PolygonCollider>(a);
			Transform& bTransform = ecs::GetComponent<Transform>(b);
			PolygonCollider& bCollider = ecs::GetComponent<PolygonCollider>(b);

			//Rotate and scale every vertex of a, movement is handled later
			float aRotation = aCollider.rotationOverride >= 0 ? aCollider.rotationOverride : aTransform.rotation.z;
			std::vector<Vector2> aVerts = TransformSystem::ApplyTransforms2D(aCollider.vertices, aRotation, aTransform.scale, aTransform.position);
			//Rotate and scale every vertex of b, movement is handled later
			float bRotation = bCollider.rotationOverride >= 0 ? bCollider.rotationOverride : bTransform.rotation.z;
			std::vector<Vector2> bVerts = TransformSystem::ApplyTransforms2D(bCollider.vertices, bRotation, bTransform.scale, bTransform.position);

			//Check SAT collision
			Collision collision = SATIntersect(aVerts, bVerts);

			//If there was a collision
			if (collision.type != Collision::Type::miss)
			{
				Collision::Type type = aCollider.trigger || bCollider.trigger ? Collision::Type::trigger : Collision::Type::collision;

				//If the mtv is facing in to the other collider from a's pov, flip it
				Vector3 directionAtoB = aTransform.position - bTransform.position;
				if (directionAtoB.Dot(collision.mtv) < 0)
					collision.mtv = Vector2() - collision.mtv;

				//Set the proper data for a's collision event
				Collision aToB;
				aToB.a = a;
				aToB.b = b;
				aToB.type = type;
				aToB.mtv = collision.mtv;
				aToB.normal = collision.normal;
				//Set the proper data for b's collision event
				Collision bToA;
				bToA.a = b;
				bToA.b = a;
				bToA.type = type;
				bToA.normal = Vector2() - collision.normal;
				bToA.mtv = Vector2() - collision.mtv;

				return aToB;

			}
			return Collision{ .type = Collision::Type::miss };
		}

		///Check SAT intersection between two convex polygons, Expects Vertices to have Transforms applied
		static Collision SATIntersect(std::vector<Vector2> aVerts, std::vector<Vector2> bVerts)
		{
			//Calculate all axes to check
			std::vector<Vector2> axes;
			//For each vertice in a
			for (int i = 0; i < aVerts.size(); i++)
			{
				//Overflow nextVertice to beginning
				int nextVertice = i < aVerts.size() - 1 ? i + 1 : 0;
				//Calculate the normal vector to project the other polygon to (left normal because clockwise)
				Vector2 axis = (aVerts[nextVertice] - aVerts[i]).LeftNormal();

				//If axis is not already in axes
				if (axes.end() == find_if(axes.begin(), axes.end(), [axis](Vector2& vec) { return axis == vec || axis == Vector2() - vec; }))
					axes.push_back(axis.Normalize());

			}
			//For each vertice in b
			for (int i = 0; i < bVerts.size(); i++)
			{
				//Overflow nextVertice to beginning
				int nextVertice = i < bVerts.size() - 1 ? i + 1 : 0;
				//Calculate the normal vector to project the other polygon to (left normal because clockwise)
				Vector2 axis = (bVerts[nextVertice] - bVerts[i]).LeftNormal();

				//If axis is not already in axes
				if (axes.end() == find_if(axes.begin(), axes.end(), [axis](Vector2& vec) { return axis == vec || axis == Vector2() - vec; }))
					axes.push_back(axis.Normalize());
			}

			//Keep track of collision data and mtv axis and magnitude
			Collision collision;
			float minOverlap = INFINITY;
			Vector2 minAxis;

			//For each necesary axis
			for (Vector2& axis : axes)
			{
				//Project each vertice of a to axis and calculate it's bounds
				float aMin = INFINITY;
				float aMax = -INFINITY;
				std::vector<float> aProjections;
				for (Vector2& vertice : aVerts)
				{
					//Project to axis
					float projection = axis.Dot(vertice);
					aProjections.push_back(projection);

					//Get min and max bounds
					if (projection < aMin)
						aMin = projection;
					if (projection > aMax)
						aMax = projection;
				}
				//Project each vertice of b to axis and calculate it's bounds
				float bMin = INFINITY;
				float bMax = -INFINITY;
				std::vector<float> bProjections;
				for (Vector2& vertice : bVerts)
				{
					//Project to axis
					float projection = axis.Dot(vertice);
					bProjections.push_back(projection);

					//Get min and max bounds
					if (projection < bMin)
						bMin = projection;
					if (projection > bMax)
						bMax = projection;
				}

				//The maximum overlap of this axis
				float maxAxisOverlap = -INFINITY;

				//Did any a or b vertice intersect
				bool hit = false;

				//Check a to b overlap
				for (float aProjection : aProjections)
				{
					//Vertice is intersecting b bounds
					if (aProjection < bMax && aProjection > bMin)
					{
						float overlap = std::min(abs(aProjection - bMin), abs(aProjection - bMax));

						//Hit confirmed
						hit = true;

						//Get the maximum overlap on this axis
						if (overlap > maxAxisOverlap)
							maxAxisOverlap = overlap;
					}
				}
				//Check b to a overlap
				for (float bProjection : bProjections)
				{
					//Vertice is intersecting b bounds
					if (bProjection < aMax && bProjection > aMin)
					{
						float overlap = std::min(abs(bProjection - aMin), abs(bProjection - aMax));

						//Hit confirmed
						hit = true;

						//Get the maximum overlap on this axis
						if (overlap > maxAxisOverlap)
							maxAxisOverlap = overlap;
					}
				}

				//If any axis misses there is no collision
				if (!hit)
				{
					collision.type = Collision::Type::miss;
					return collision;
				}

				//If this axis had the smallest overlap
				if (maxAxisOverlap < minOverlap)
				{
					minOverlap = maxAxisOverlap;
					minAxis = axis;
				}
			}

			//Collided
			collision.mtv = minAxis.Normalize() * minOverlap;
			collision.normal = minAxis.Normalize();
			collision.type = Collision::Type::collision;
			return collision;
		}

		///Checks if a and b bounds are intersecting
		static bool AABBIntersect(ecs::Entity a, ecs::Entity b)
		{
			//Get the bounds
			std::array<float, 4>& aBounds = ecs::GetComponent<PolygonCollider>(a).bounds;
			std::array<float, 4>& bBounds = ecs::GetComponent<PolygonCollider>(b).bounds;

			//Perform AABB intersect
			return (aBounds[3] < bBounds[1] && aBounds[1] > bBounds[3] && aBounds[2] < bBounds[0] && aBounds[0] > bBounds[2]);
		}

		///Update the AABB of the polygon collider
		static void UpdateAABB(ecs::Entity entity)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);
			PolygonCollider& collider = ecs::GetComponent<PolygonCollider>(entity);

			//Bounds go top, right, bottom, left
			std::array<float, 4> bounds{ -INFINITY, -INFINITY, INFINITY, INFINITY };

			//Apply transforms to collider vertices
			float rotation = collider.rotationOverride >= 0 ? collider.rotationOverride : transform.rotation.z;
			std::vector<Vector2> transformedVerts = TransformSystem::ApplyTransforms2D(collider.vertices, rotation, transform.scale, transform.position);

			//For each vertice apply the transform and calculate min and max bounds
			for (const Vector2& transformedVert : transformedVerts)
			{
				//Calculate bounds
				//Top bound
				if (transformedVert.y > bounds[0])
					bounds[0] = transformedVert.y;
				//Right bound
				if (transformedVert.x > bounds[1])
					bounds[1] = transformedVert.x;
				//Bottom bound
				if (transformedVert.y < bounds[2])
					bounds[2] = transformedVert.y;
				//Left bound
				if (transformedVert.x < bounds[3])
					bounds[3] = transformedVert.x;
			}

			collider.bounds = bounds;
		}

		///Set the tilemap collision layer
		void SetTilemap(Tilemap* collisionTilemap)
		{
			if (collisionTilemap->collisionLayer.empty())
			{
				std::cout << "WARNING: No collision layer in this tilemap!\n";
			}
			else
			{
				tilemap = collisionTilemap;
			}
		}

		///Removes the tilemap from collision
		void RemoveTilemap()
		{
			tilemap = nullptr;
		}

		///Camera is needed for visualisation
		Camera* cam = nullptr;

	private:
		Tilemap* tilemap = nullptr;
	};
}