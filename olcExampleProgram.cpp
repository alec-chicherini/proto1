

#define OLC_PGE_APPLICATION
#include <iostream>
#include "olcPixelGameEngine.h"
#include "experiment_engine.h"
#include <algorithm>
#include <memory>

#define SCREEN_OFFSET 50 //aka. MAX_MOL_RADIUS

//gradus return direction angle

auto gradus = [](olc::vf2d& vec) {

	auto gradus = acos(vec.x / hypot(vec.y, vec.x)) * 180 / 3.14;

	if (vec.y > 0)gradus += 180;

	return gradus;

};

class experiment_proto : public olc::PixelGameEngine
{
private:
	
	
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
		poolInit();
		Clear(olc::WHITE);

		addMol(H2);
		addMol(CO2);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		if (GetKey(olc::Key::K1).bPressed)	addMol(H2);
		if (GetKey(olc::Key::K2).bPressed)	addMol(CO2);

		mouseControlling();
		molStateUpdate(fElapsedTime);
		molCollisionUpdate();
		molStaticCollisions();
		molDynamicCollisions();
		Clear(olc::WHITE);
		DrawPool();
		DrawMols();


#ifdef SHOW_TEST_INFO

		printMolStates();
#endif

		//collisionVector.clear();
		return true;
	}

	
	//void inline  DrawMaterial(olc::vf2d* pos, material& mat, olc::Pixel clr)
	//{
	//	for (int i = 0; i < mat.size(); i++)DrawMolecule(pos[i], mat[i], clr);
	//}
	

////////////////////////////////////////////////////////////////////////////////////////////////
/////methods  for another class


	public:

		//update UI
		void DrawPool() {
		
			
			//main pool
			DrawRect(pool_size_x, pool_size_y, pool_size_w, pool_size_h, olc::BLACK);

			//left pool
			DrawRect(SCREEN_OFFSET, SCREEN_OFFSET, left_pool_width, left_pool_height, olc::BLACK);

			//right pool
			DrawRect(TARGET_SCREEN_SIZE_X- SCREEN_OFFSET- right_pool_width, SCREEN_OFFSET, right_pool_width, right_pool_height, olc::BLACK);

			//Draw button1
			DrawRect(SCREEN_OFFSET, pool_size_h + SCREEN_OFFSET, left_pool_width, button_panel_side, olc::BLACK);
			DrawString(SCREEN_OFFSET + left_pool_width/2, pool_size_h + SCREEN_OFFSET + button_panel_side/2, "BUTTON1", olc::BLACK);




		}

		void DrawMols() {
			for (auto& mol : molStates) {//draw molecule

#ifdef SHOW_TEST_INFO

				int gradus1 = round(gradus(mol.D));
				
#endif // SHOW_TEST_INFO
				DrawCircle(mol.P, mol.radius, mol.molColor);
				DrawString(mol.P.x - 10, mol.P.y,

#ifndef SHOW_TEST_INFO
					mol.molObj.get_name(),
#endif // !SHOW_TEST_INFO

#ifdef SHOW_TEST_INFO
					std::to_string(mol.Id),
#endif // SHOW_TEST_INFO
					mol.molColor, 1);
			}

#ifdef SHOW_TEST_INFO
			for(auto& c:collisionVector)
			DrawLine(c.first->P.x,c.first->P.y, c.second->P.x, c.second->P.y, olc::RED);
#endif // !SHOW_TEST_INFO

			if (pSelectedMol != nullptr)
			{
				DrawLine(pSelectedMol->P.x, pSelectedMol->P.y,GetMouseX(), GetMouseY(),olc::BLACK);
			}
		}
		void molStateUpdate(float fElapsedTime) {

			for (auto& mol : molStates) {

				mol.A.x = -mol.D.x * 0.8f;
				mol.A.y = -mol.D.y * 0.8f;

				mol.D.x += mol.A.x * fElapsedTime;
				mol.D.y += mol.A.y * fElapsedTime;

				mol.P.x += mol.D.x * fElapsedTime;
				mol.P.y += mol.D.y * fElapsedTime;

				if (mol.P.x > pool_size_x + pool_size_w) { mol.P.x = float(pool_size_x); }
				if (mol.P.y > pool_size_y + pool_size_h) { mol.P.y = float(pool_size_y); }
				if (mol.P.x < pool_size_x) { mol.P.x = float(pool_size_w + pool_size_x); }
				if (mol.P.y < pool_size_y) { mol.P.y = float(pool_size_h + pool_size_y); }

				if ((mol.D.x * mol.D.x + mol.D.y * mol.D.y) < 0.001f)
				{
					mol.D.x = 0;
					mol.D.y = 0;
				}
			}
		}

		void molCollisionUpdate()
		{

			auto isMolOverlap = [](molState& mol1, molState& mol2) {
				return ((mol1.radius + mol2.radius) * (mol1.radius + mol2.radius)) >=
					((mol1.P.x - mol2.P.x) * (mol1.P.x - mol2.P.x) + (mol1.P.y - mol2.P.y) * (mol1.P.y - mol2.P.y)); };

			for (auto& mol1 : molStates)
				for (auto& mol2 : molStates)
				{
					if (mol1.Id != mol2.Id)
					{
						
						auto search = std::find_if(
							collisionVector.begin(),
							collisionVector.end(),
							[&mol1, &mol2]( std::pair<molState*,molState*> &currentCollision)
							{
							return (currentCollision.first->Id == mol1.Id) && (currentCollision.second->Id == mol2.Id);
							}
						);

						if (isMolOverlap(mol1, mol2))
						{
							if (search == collisionVector.end())
							{
									collisionVector.push_back(std::make_pair(&mol1,&mol2));
								}
						}
						else
						{
							if(search!=collisionVector.end()) collisionVector.end()=collisionVector.erase(search);
						}
					}
				}


		};

		void molStaticCollisions() {

			for (auto& c : collisionVector) {

				float fDistance = hypotf(c.first->P.x - c.second->P.x, c.first->P.y - c.second->P.y);
				float fOverlap = 0.5f * (fDistance - c.first->radius - c.second->radius);
#ifdef TEST
				std::cout << "collisionVector.size = " << collisionVector.size()
					<< " c.first->Id =  " << c.first->Id << " c.second->Id = " << c.second->Id;
				std::cout << " fDistance = " << fDistance << " fOverlap = " << fOverlap << std::endl;
				std::cout << "c.first->P = " << c.first->P << "c.second->P = " << c.second->P << std::endl;
#endif

				//displace 1 mol
				c.first->P.x -= fOverlap * (c.first->P.x - c.second->P.x) / fDistance;
				c.first->P.y -= fOverlap * (c.first->P.y - c.second->P.y) / fDistance;


				//displace 2 mol
				c.second->P.x += fOverlap * (c.first->P.x - c.second->P.x) / fDistance;
				c.second->P.y += fOverlap * (c.first->P.y - c.second->P.y) / fDistance;

				//std::cout << "!c.first->P = " << c.first->P << "c.second->P = " << c.second->P << std::endl;
			}

		};

		//dynamics collision
		void molDynamicCollisions( )
		{
			for (auto& c : collisionVector) {
#ifdef TEST


				std::cout << "Dynamic:c.first->D = " << c.first->D << "c.second->D = " << c.second->D << std::endl;
#endif
				
				float fDistance = hypotf((c.first->P.x - c.second->P.x), (c.first->P.y - c.second->P.y));

				float kx = (c.second->P.x - c.first->P.x) / fDistance;
				float ky = (c.second->P.y - c.first->P.y) / fDistance;

				float P = c.first->mass * hypotf(c.first->D.x, c.first->D.y);
				
				c.first->D.x = c.first->mass * c.first->D.x - P * kx;
				c.first->D.y = c.first->mass * c.first->D.y - P * ky;
				c.second->D.x = c.second->mass * c.second->D.x + P * kx;
				c.second->D.y = c.second->mass * c.second->D.y + P * ky;


				//limiting velocities
				float speedLimit = 500.f;
				
				if(abs(c.first->D.x) > speedLimit || abs(c.first->D.y) > speedLimit)
				{
					float signX = c.first->D.x >= 0 ? 1.f : -1.f,
						  signY = c.first->D.y >= 0 ? 1.f : -1.f;
					
					c.first->D.x *= speedLimit / max(abs(c.first->D.x), abs(c.first->D.y)) * signX;
					c.first->D.y *= speedLimit / max(abs(c.first->D.x), abs(c.first->D.y)) * signY;
				}

				if (abs(c.second->D.x) > speedLimit || abs(c.second->D.y) > speedLimit)
				{
					float signX = c.second->D.x >= 0 ? 1.f : -1.f,
						  signY = c.second->D.y >= 0 ? 1.f : -1.f;

					c.second->D.x *= speedLimit / max(abs(c.second->D.x), abs(c.second->D.y)) * signX;
					c.second->D.y *= speedLimit / max(abs(c.second->D.x), abs(c.second->D.y)) * signY;
				}


#ifdef TEST


				std::cout << "!Dynamic:c.first->D = " << c.first->D << "c.second->D = " << c.second->D << std::endl;
#endif
			}
			
		}


		void mouseControlling() {


			auto isPointOverlap = [](const int32_t& x, const int32_t& y, molState& mol) {
				return x < (mol.P.x + mol.radius)&&
					   x > (mol.P.x - mol.radius)&&
					   y > (mol.P.y - mol.radius)&&
					   y < (mol.P.y + mol.radius);
			};

			if (GetMouse(0).bPressed||GetMouse(1).bPressed)
			{

				pSelectedMol = nullptr;

				for (auto& mol : molStates) {
					if (isPointOverlap(GetMouseX(), GetMouseY(), mol))
					{
						pSelectedMol = &mol;
						break;
					}

				}

			}

			if (GetMouse(0).bReleased)
			{
				pSelectedMol = nullptr;
			}

			if (GetMouse(1).bReleased)
			{
				if (pSelectedMol != nullptr) 
				{
					pSelectedMol->D.x = 5.0f * ((pSelectedMol->P.x) - static_cast<float>(GetMouseX()));
					pSelectedMol->D.y = 5.0f * ((pSelectedMol->P.y) - static_cast<float>(GetMouseY()));
				};
				pSelectedMol = nullptr;
			}

			if (GetMouse(0).bHeld)
			{

				if (pSelectedMol != nullptr)
				{
					pSelectedMol->P.x = GetMouseX();
					pSelectedMol->P.y = GetMouseY();
				}
			}

		};

		void poolInit() {
			button_panel_side = 100;

			left_pool_width = SCREEN_OFFSET * 4;
			

			right_pool_width= SCREEN_OFFSET * 4;
			

			pool_size_x = SCREEN_OFFSET + left_pool_width;
			pool_size_y = SCREEN_OFFSET;
			pool_size_h = TARGET_SCREEN_SIZE_Y - SCREEN_OFFSET * 2 - button_panel_side;
			pool_size_w = TARGET_SCREEN_SIZE_X - SCREEN_OFFSET * 2 - left_pool_width - right_pool_width;

			right_pool_height = pool_size_h;
			left_pool_height = pool_size_h;
		
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

			{0,0},
			//{sin(static_cast<float>(RAND100)) * 10,//direction(speed) x
			 //sin(static_cast<float>(RAND100)) * 10},//direction(speed)  y

			{0,0},//Acceleration x,y
			30,//radius
			getId(),// ID
			1.0f//mass
			

			};

			molStates.push_back(currentMolState);
		
		
		};

		//void addMaterial(material mat) {};


#ifdef SHOW_TEST_INFO

		void printMolStates() {
			int step = 0;
			auto drawPos = pool_size_h + SCREEN_OFFSET + SCREEN_OFFSET+button_panel_side;
			for(auto& m:molStates)
			{ 
				DrawString(SCREEN_OFFSET, drawPos +(step++)*20,
					std::string("ID: "+ 
						std::to_string(m.Id)+
						" P: "+
						std::to_string(m.P.x) +" "+ std::to_string(m.P.y) +
						" D: " +
						std::to_string(m.D.x) + " " + std::to_string(m.D.y))

					,olc::BLACK,1);
				
			}


			for (auto& c : collisionVector)
			{
				DrawString(SCREEN_OFFSET, drawPos +(step++) * 20,
					std::string(
								 " ID1: " + std::to_string(c.first->Id) +
								" ID2: " + std::to_string(c.second->Id))
						

					, olc::BLACK, 1);

			}


			DrawString(SCREEN_OFFSET, drawPos + (step++) * 20,
				std::string("colvec.size: " + std::to_string(collisionVector.size()))


				, olc::BLACK, 1);
		}


#endif

		const int32_t getId() {
			static int32_t name = 100;

			return name++;
		}

	

		class molState {
		public:
			molecules molObj;
			olc::Pixel molColor;
			olc::vf2d P; //Possition;
			olc::vf2d D; //Dirrection- Speed
			olc::vf2d A; //Accel;
			int32_t radius;
			int32_t Id;
			float mass;
			//0 - empty cell
			//1 - horisontal walls
			//2 - vertical walls
			//100-10000 - particles

			//friend bool operator==(const molState& mol1, const molState& mol2);
		};

		

		molState& getMollStateByID(int32_t collisionId) {

			for (auto& s : molStates)if(s.Id == collisionId)return s;

		}

	private:
	
		 std::vector<std::pair<molState*,molState*>> collisionVector;
		 std::vector<molState> molStates;

		int32_t pool_size_x,
				pool_size_y,
				pool_size_w,
				pool_size_h,
				left_pool_width,
				left_pool_height,
				right_pool_width,
				right_pool_height,
				button_panel_side;


		molState* pSelectedMol=nullptr;


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

	//for (int i = 0; i < 100; i++) {
	
		//std::cout << "RAND= " << RAND << " RAND_W = "<< RAND_W <<" RAND_H = " << RAND_H << " RAND100 = "<< RAND100<<std::endl;
		//std::cout << "RAND_SIGN " << RAND_SIGN << std::endl;}

	//end of Inits


	experiment_proto main_screen;
	if (main_screen.Construct(SCREEN_SIZE_X, SCREEN_SIZE_Y, 1, 1))
		main_screen.Start();



	return 0;
}

