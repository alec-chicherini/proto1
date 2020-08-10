#define OLC_PGE_APPLICATION
#include <iostream>
#include "olcPixelGameEngine.h"
#include "experiment_engine.h"
#include <algorithm>

//gradus return direction angle

auto gradus = [](olc::vf2d& vec) {

	auto gradus = acos(vec.x / hypot(vec.y, vec.x)) * 180 / 3.14;

	if (vec.y > 0)gradus += 180;

	return gradus;

};

class experiment_proto : public olc::PixelGameEngine
{
private:
	
	int32_t mouseX;
	int32_t mouseY;

	molecules H2 = { "H2" };
	molecules CO2 = { "CO2" };

public:
	
	experiment_proto()
	{
		sAppName = "Chemical experiment prototype visualisation";
	}

public:
	bool OnUserCreate() override
	{

		// Called once at the start, so create things here
		poolInit(120,0,400,400);
		Clear(olc::WHITE);

		addMol(H2);
		addMol(CO2);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		if (GetKey(olc::Key::K1).bPressed)	addMol(H2);
		if (GetKey(olc::Key::K2).bPressed)	addMol(CO2);

		Clear(olc::WHITE);
		updateMolState(fElapsedTime);
		DrawPool();
		updateCollisionArray();
		updateMolStateAfterCollisions();

#ifdef SHOW_COLLISION_ARRAY
		printCollisionMatrix();
		printMolStates();
#endif
		return true;
	}

	
	//void inline  DrawMaterial(olc::vf2d* pos, material& mat, olc::Pixel clr)
	//{
	//	for (int i = 0; i < mat.size(); i++)DrawMolecule(pos[i], mat[i], clr);
	//}
	

////////////////////////////////////////////////////////////////////////////////////////////////
/////methods  for another class


	public:
		//update and draw possitions of moleculas
		void DrawPool() {
			DrawRect(pool_size_x, pool_size_y, pool_size_w, pool_size_h, olc::BLACK);
			for (auto& mol : molStates) {//draw molecule


#ifdef SHOW_COLLISION_ARRAY

				int gradus1 = round(gradus(mol.D));
				
#endif // SHOW_COLLISION_ARRAY
				DrawCircle(mol.P, mol.radius, mol.molColor);
				DrawString(mol.P.x - 10, mol.P.y,



#ifndef SHOW_COLLISION_ARRAY
					mol.molObj.get_name(),
#endif // !SHOW_COLLISION_ARRAY

#ifdef SHOW_COLLISION_ARRAY
					
					std::to_string(gradus1),
				
					
#endif // SHOW_COLLISION_ARRAY

					mol.molColor, 1);
			

#ifdef SHOW_COLLISION_ARRAY
			DrawLine(mol.P.x, mol.P.y, mol.P.x + mol.D.x, mol.P.y + mol.D.y, olc::RED);
#endif // !SHOW_COLLISION_ARRAY

			}
		}

		//update moleculas states for the next frame. Checking if next possition will be outside.
		void updateMolState(float fElapsedTime)
		{
			for (auto& mol : molStates) {

				//mol.D *= mol.A ;
			
				if (mol.D.x > 100.0f)mol.D.x = 100.0f;
				if (mol.D.x < -100.0f)mol.D.x = -100.0f;
				
				 if (mol.D.y > 100.0f)mol.D.y = 100.0f;
				 if (mol.D.y < -100.0f)mol.D.y = -100.0f;

				 mol.P += mol.D * fElapsedTime;


				 //moving to/from another part of borders
				 /*
				if (mol.P.x > pool_size_x + pool_size_w) { mol.P.x = float(pool_size_x);  }
				if (mol.P.y > pool_size_y + pool_size_h) { mol.P.y = float(pool_size_y);  }
				if (mol.P.x < pool_size_x) { mol.P.x = float(pool_size_h+ pool_size_x); }
				if (mol.P.y < pool_size_y) { mol.P.y = float(pool_size_w+ pool_size_y);  }
				*/
				
			}
		}

		void poolInit(int32_t x, int32_t y, int32_t w, int32_t h) {

			pool_size_x = x;
			pool_size_y = y;
			pool_size_h = h;
			pool_size_w = w;
		
		}

		//create mol in pool at random position
		void addMol(molecules mol) 
		{
			int32_t radius=15;
			olc::vf2d CurrentPos;
			int32_t randX = RAND_X%pool_size_w;
			int32_t randY = RAND_Y%pool_size_h;

			if (randX <= radius)CurrentPos.x = radius+ pool_size_x;
			else if (randX >= pool_size_w - radius)CurrentPos.x = pool_size_x+pool_size_w - radius;
			else	CurrentPos.x = randX+ pool_size_x;

			if (randY<= radius)CurrentPos.y = radius+ pool_size_y;
			else if (randY >= pool_size_h - radius)CurrentPos.y = pool_size_h - radius+ pool_size_y;
			else	CurrentPos.y = randY+ pool_size_y;

			molState currentMolState =
			{
			mol,
			olc::BLACK,
			{static_cast<float>(CurrentPos.x),//possition x
			static_cast<float>(CurrentPos.y)},//possition y

			{sin(static_cast<float>(RAND100)) * 10,//direction(speed) x
			 sin(static_cast<float>(RAND100)) * 10},//direction(speed)  y

			{50.0f,50.0f},//Acceleration x,y
			15,//radius
			getCollisionName(),//collision ID
			-1//last collision id

			};

			molStates.push_back(currentMolState);
		
		
		};

		//void addMaterial(material mat) {};


std::vector<std::pair<int32_t, int32_t>> updateCollisionArray() {
			//collision pair vector.
			

			//fill zeros
			for (auto& c : collisionArray)
				for (auto& cc : c)cc = 0;

			//fill 1 2 3 4 - walls
			//horisontal walls 1 3
			for (int x = 0; x < pool_size_w; x++) 
			{
				collisionArray[x][0] = 1;
				collisionArray[x][pool_size_h-1] = 3;
			}
			//vertical walls 2 4
			for (int y = 0; y < pool_size_h; y++)
			{
				collisionArray[0][y] = 2;
			    collisionArray[pool_size_w-1][y] = 4;
			}

			
			for (auto& mol : molStates)
			{
				//fill mols collision box - square in this implementation
				for (int32_t i_y = -mol.radius; i_y < mol.radius; i_y++)
					for (int32_t i_x = -mol.radius; i_x < mol.radius; i_x++)
				{	
//fill like circle
						if ((i_y * i_y + i_x * i_x) > mol.radius * mol.radius)continue;


#ifdef TESTT
	std::cout << "i_x = " << i_x << " = > " 
		<<" mol.P.x ="<< mol.P.x << " = > "
		<< i_x + static_cast<int32_t>(mol.P.x - pool_size_x) << std::endl;
	std::cout << "i_y = " << i_y << " = > " 
		<<" mol.P.y = "<< mol.P.y<< " = > "
		<< i_y + static_cast<int32_t>(mol.P.y - pool_size_y) << std::endl;
#endif

	int32_t x_collide_box_point = i_x + static_cast<int32_t>(mol.P.x - pool_size_x);
	int32_t y_collide_box_point = i_y + static_cast<int32_t>(mol.P.y - pool_size_y);

	//check if value for collision matrix inside pool
	if (x_collide_box_point >= 0 &&
		x_collide_box_point < pool_size_w &&
		y_collide_box_point >= 0 &&
		y_collide_box_point < pool_size_h)
	{
		//state of point where mol wanna to move
		int oldState = collisionArray[x_collide_box_point][y_collide_box_point];

		if (oldState == 0)collisionArray[x_collide_box_point][y_collide_box_point] = mol.CollisionId;//if was empty(==0)
		else if (oldState != mol.lastCollidedObject)//check last collided object
		{
		     if (oldState == 1)  collisionVector.push_back({ std::make_pair(mol.CollisionId,1) });//horisontal wall
		else if (oldState == 2)  collisionVector.push_back({ std::make_pair(mol.CollisionId,2) });//vertical wall 
		else if (oldState == 3)  collisionVector.push_back({ std::make_pair(mol.CollisionId,3) });//horisontal wall 2
		else if (oldState == 4)  collisionVector.push_back({ std::make_pair(mol.CollisionId,4) });//vertical wall 2
		else
			 if (oldState >= 100)  collisionVector.push_back({ std::make_pair(mol.CollisionId,oldState) });
		}//check las collided object
	};
				     }
			}

			//delete duplicates
			auto last = std::unique(collisionVector.begin(), collisionVector.end());
			collisionVector.erase(last, collisionVector.end());
	
			return collisionVector;
		};


		void updateMolStateAfterCollisions()
		{
			//for (auto& (*iterCollVec) : collisionVector)
			for(auto iterCollVec = collisionVector.begin(); iterCollVec !=collisionVector.end(); )
			{
#ifdef TEST
				std::cout << "collision vector: ";
				for (auto& v : collisionVector)std::cout << v.first << " - " << v.second<<"|";
				std::cout << std::endl;
#endif
				//colliding with horisontal walls
				if (((*iterCollVec).second == 1)||((*iterCollVec).second == 3))
				{
					for (auto& mol : molStates) {
						if (mol.CollisionId == (*iterCollVec).first) {
							//mol.D.x *= mol.D.x;
							mol.D.y *=-1.0f;
#ifdef TEST
							std::cout << "mol.CollisionId = " << mol.CollisionId << std::endl;
							std::cout << "mol.Direction = " << mol.D << std::endl;
							std::cout << "mol.Position = " << mol.P << std::endl;
#endif
							mol.lastCollidedObject = (*iterCollVec).second;
							break;
						}
					
					}
					//delete from collision vector because already done
					iterCollVec=collisionVector.erase(iterCollVec);
					
				}
				//colliding with vertical walls
				else if (((*iterCollVec).second == 2)||(*iterCollVec).second == 4)
				{

					for (auto& mol : molStates) {
						if (mol.CollisionId == (*iterCollVec).first) {
							mol.D.x *= -1.0f;
							//mol.D.y *= mol.D.x;
#ifdef TEST
							std::cout << "mol.CollisionId = " << mol.CollisionId << std::endl;
							std::cout << "mol.Direction = " << mol.D << std::endl;
							std::cout << "mol.Position = " << mol.P << std::endl;
#endif
							mol.lastCollidedObject = (*iterCollVec).second;
							break;
						}

					}
					//delete from collision vector because already done
					iterCollVec = collisionVector.erase(iterCollVec);
				}


				//colliding with mols
				else if ((*iterCollVec).second >= 100)
				{

					for (auto& mol : molStates) {
						if (mol.CollisionId == (*iterCollVec).first) {
							molState& mol2 = getMollStateByID((*iterCollVec).second);

#ifdef TEST					
							std::cout << "before update :" << std::endl;;
							std::cout << "mol.CollisionId = " << mol.CollisionId << std::endl;
							std::cout << "mol.Direction = " << mol.D << std::endl;
							std::cout << "mol.Position = " << mol.P << std::endl;
							std::cout << "mol2.CollisionId = " << mol2.CollisionId << std::endl;
							std::cout << "mol2.Direction = " << mol2.D << std::endl;
							std::cout << "mol2.Position = " << mol2.P << std::endl;
#endif
							 /*
							//   https://ru.wikipedia.org/wiki/%D0%A3%D0%B4%D0%B0%D1%80
						
							//int gradus = round(acos(mol.D.x / hypot(mol.D.y, mol.D.x)) * 180 / 3.14);
							//if (mol.D.y > 0)gradus += 180;

							float phi1 = gradus(mol.D) * 3.14 / 180;
							float phi2 = gradus(mol2.D) * 3.14 / 180;

							float phi = abs(phi1 - phi2);

							mol.D.x = mol2.D.x * cos(phi2 - phi) * cos(phi)+mol.D.x*sin(phi1-phi)*cos(phi+3.14/2);
							mol.D.y = mol2.D.y * cos(phi2 - phi) * sin(phi) + mol.D.y * sin(phi1 - phi) * cos(phi + 3.14 / 2);

							mol2.D.x = mol.D.x * cos(phi1 - phi) * cos(phi) + mol2.D.x * float(sin(phi2 - phi)) * cos(phi + 3.14 / 2);
							mol2.D.y = mol.D.y * cos(phi1 - phi) * sin(phi) + mol2.D.y * sin(phi2 - phi) * cos(phi + 3.14 / 2);


							if (mol.D.x > 100.0f)mol.D.x = 10.0f;
							if (mol.D.x < -100.0f)mol.D.x = -100.0f;

							if (mol.D.y > 100.0f)mol.D.y = 100.0f;
							if (mol.D.y < -100.0f)mol.D.y = -100.0f;


							if (mol2.D.x > 100.0f)mol2.D.x = 10.0f;
							if (mol2.D.x < -100.0f)mol2.D.x = -100.0f;

							if (mol2.D.y > 100.0f)mol2.D.y = 100.0f;
							if (mol2.D.y < -100.0f)mol2.D.y = -100.0f;

						////////////////////////////////////////////////

							*/
							//mol.D.x *= -1.f;
							//mol.D.y *= -1.f;
							//mol2.D.y *= -1.f;
							//mol2.D.x *= -1.f;


#ifdef TEST					
							std::cout << "after update: " << std::endl;;
							std::cout << "mol.CollisionId = " << mol.CollisionId << std::endl;
							std::cout << "mol.Direction = " << mol.D << std::endl;
							std::cout << "mol.Direction angle = "<< atan(mol.D.y / mol.D.x) << std::endl;
							std::cout << "mol.Position = " << mol.P << std::endl;
							std::cout << "mol2.CollisionId = " << mol2.CollisionId << std::endl;
							std::cout << "mol2.Direction = " << mol2.D << std::endl;
							std::cout << "mol2.Position = " << mol2.P << std::endl;

#endif
							mol.lastCollidedObject = mol2.CollisionId;
							mol2.lastCollidedObject = mol.CollisionId;
							//delete from collision vector because already done
							iterCollVec = collisionVector.erase(iterCollVec);
							//exit after one time done
							break;
						}

					}
				}
				else {iterCollVec++;}

			}


			collisionVector.clear();
		
		}

#ifdef SHOW_COLLISION_ARRAY

		void printCollisionMatrix() {

			for (int i = 0; i < pool_size_h; i++)
			{
				//std::cout << std::endl;
				for (int j = 0; j < pool_size_w; j++) {

					if (collisionArray[i][j] == 0)
					{
						Draw(i+120, j+480, olc::BLUE);
						//std::cout << "x";
					}

					if (collisionArray[i][j] >= 100)
					{ 
						Draw(i+120, j+480, olc::RED);
						//std::cout << "a";
					}

					if (collisionArray[i][j] == 1|| collisionArray[i][j] == 2 || collisionArray[i][j] == 3 || collisionArray[i][j] == 4)
					{
						Draw(i + 120, j + 480, olc::RED);
						//std::cout << "a";
					}
				
					
				}
			}
			
		}

		void printMolStates() {
			int step = 0;
			for(auto& m:molStates)
			{ 
				DrawString(640, (step++)*50,
					std::string("ID: "+ 
						std::to_string(m.CollisionId)+
						" P: "+
						std::to_string(m.P.x) +" "+ std::to_string(m.P.y) +
						" D: " +
						std::to_string(m.D.x) + " " + std::to_string(m.D.y)+
						" last collided:" +
						std::to_string(m.lastCollidedObject))

					,olc::BLACK,1);
				
			}
		}


#endif

		const int32_t getCollisionName() {
			static int32_t name = 100;

			return name++;
		}

	

		struct molState {
			molecules molObj;
			olc::Pixel molColor;
			olc::vf2d P; //Possition;
			olc::vf2d D; //Dirrection- Speed
			olc::vf2d A; //Accel;
			int32_t radius;
			int32_t CollisionId;
			//0 - empty cell
			//1 - horisontal walls
			//2 - vertical walls
			//100-10000 - particles
			int32_t lastCollidedObject;
		};

		molState& getMollStateByID(int32_t collisionId) {

			for (auto& s : molStates)if(s.CollisionId == collisionId)return s;

		}
	private:
		std::vector<std::pair<int32_t, int32_t>> collisionVector;

		std::vector<molState> molStates;

		int32_t pool_size_x,
			pool_size_y,
			pool_size_w,
			pool_size_h;

		std::array<std::array<int32_t, 400>, 400> collisionArray;


//////////////////////////////////////////////////////
	};



//static vars init
std::map<std::string, std::string> csv_to_RECIPIES::RECIPIES{ {"NULL","NULL" } };
int random_generator::iRand100 = 0;
int random_generator::iRand = 0;
int random_generator::iRandH = 0;
int random_generator::iRandW = 0;
int random_generator::iRandSign = 0;
std::vector<int> random_generator::vRand100 = { {} };
std::vector<int> random_generator::vRand = { {} };
std::vector<int> random_generator::vRandH = { {} };
std::vector<int> random_generator::vRandW = { {} };
std::vector<int> random_generator::vRandSign = { {} };
/////////////////////////


int main()
{
	//Initialisations of Experiment Engine
	csv_to_RECIPIES::csv_to_RECIPIES("X:\\Downloads\\recep.csv");

	random_generator R;
	R.rand_init(640,480);

	for (int i = 0; i < 100; i++) {
	
		//std::cout << "RAND= " << RAND << " RAND_W = "<< RAND_W <<" RAND_H = " << RAND_H << " RAND100 = "<< RAND100<<std::endl;
		std::cout << "RAND_SIGN " << RAND_SIGN << std::endl;
	}

	//end of Inits


	experiment_proto main_screen;
	if (main_screen.Construct(SCREEN_SIZE_X, SCREEN_SIZE_Y, 1, 1))
		main_screen.Start();



	return 0;
}

//update and draw possitions of moleculas

