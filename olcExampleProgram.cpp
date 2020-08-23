

#define OLC_PGE_APPLICATION
#include <iostream>
#include "olcPixelGameEngine.h"
#include "experiment_engine.h"
#include <algorithm>
#include <memory>
#include "defined_globals.h"
#include <mutex>
#include <algorithm>


auto gradus = [](olc::vf2d& vec) {

	auto gradus = acos(vec.x / hypot(vec.y, vec.x)) * 180 / 3.14;

	if (vec.y > 0)gradus += 180;

	return gradus;

};

class experiment_proto : public olc::PixelGameEngine
{
private:
	
	
	molecules A = { "Al" };
	molecules B = { "H20" };
	molecules C = { "Cl2" };
	class molState;

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

		//addMol(A);
		addMol(B);
		//addMol(C);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime)
	{

		if (GetKey(olc::Key::K1).bPressed)	addMol(A);
		if (GetKey(olc::Key::K2).bPressed)	addMol(B);
		if (GetKey(olc::Key::K3).bPressed)	addMol(C);


		mouseControlling();
		molStateUpdate(fElapsedTime);
		molCollisionUpdate();
		molIteractions();
		molDecay(fElapsedTime);
		molDecayIteractions();
		molStaticCollisions();
		molDynamicCollisions();
		Clear(olc::WHITE);
		DrawPool();
		DrawMols();
		DrawStatistics();


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
#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG
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
#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG
			for (auto& mol : molStates) {//draw molecule

#ifdef SHOW_TEST_INFO

				int gradus1 = round(gradus(mol.D));
				
#endif // SHOW_TEST_INFO
				DrawCircle(mol.P, mol.radius, mol.molColor);
				FillCircle(mol.P, mol.radius, mol.molColor);
				DrawString(mol.P.x - 10, mol.P.y,

#ifndef SHOW_TEST_INFO
					mol.molObj.get_name(),
#endif // !SHOW_TEST_INFO

#ifdef SHOW_TEST_INFO
					mol.molObj.get_name(),
					//std::to_string(mol.Id),
#endif // SHOW_TEST_INFO
					olc::WHITE, 1);

#ifdef SHOW_TEST_INFO


				DrawString(mol.P.x + mol.radius,mol.P.y - mol.radius, std::to_string(mol.lifetime), mol.molColor,1);
#endif


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



		void DrawStatistics() 
		{
#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG
			int step = 0;

			auto drawPosX = pool_size_w + SCREEN_OFFSET + SCREEN_OFFSET + left_pool_width;
			

				for(auto& m:molStatistics)
				{
					DrawString(drawPosX, SCREEN_OFFSET+SCREEN_OFFSET +(step++)*10,
						std::string(m.first +
							" - "+
							std::to_string(m.second)
									)

						,olc::BLACK,1);

				}

		
		}

		void molStateUpdate(float fElapsedTime) {

#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG

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
#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG
			//return true if mols will overlap this frame
			auto isMolOverlap = [](molState& mol1, molState& mol2) {
				return ((mol1.radius + mol2.radius) * (mol1.radius + mol2.radius)) >=
					((mol1.P.x - mol2.P.x) * (mol1.P.x - mol2.P.x) + (mol1.P.y - mol2.P.y) * (mol1.P.y - mol2.P.y)); };

			for (auto& mol1 : molStates)
				for (auto& mol2 : molStates)
				{
					//not check mol for itself
					if (mol1.Id != mol2.Id)
					{
						//if mol pair already in current collision array dont add new pair
						auto search = std::find_if(
							collisionVector.begin(),
							collisionVector.end(),
							[&mol1, &mol2]( std::pair<molState*,molState*> &currentCollision)
							{
							return (currentCollision.first->Id == mol1.Id) && (currentCollision.second->Id == mol2.Id);
							}
						);

						//if two molls overlapping add it on collision vector to process
						if (isMolOverlap(mol1, mol2))
						{

							if ((mol1.molObj.is_recepie_with(mol2.molObj)))
							{

								auto searchItVec = std::find_if(
									iteractionVector.begin(),
									iteractionVector.end(),
									[&mol1, &mol2](std::pair<int32_t, int32_t>& currentIteration)
									{//if one of new collisions already at iteraction vector do not add this pair. olny one pair per tick.
									 return (currentIteration.first  == mol1.Id) || 
											(currentIteration.second == mol1.Id) ||
											(currentIteration.first  == mol2.Id) || 
											(currentIteration.second == mol2.Id);
									});

								if(searchItVec == iteractionVector.end()){
									iteractionVector.push_back(std::move(std::make_pair(mol1.Id, mol2.Id)));
#ifdef TEST
								//std::cout << "iteration vector size= " << iteractionVector.size() << std::endl;
#endif
								}
							}


							else if(search == collisionVector.end())
							{
									collisionVector.push_back(std::make_pair(&mol1,&mol2));
								}
						}
						else
						{
							//if already not and exist pair still exist in collisionvector - erease this pair
							if(search!=collisionVector.end()) collisionVector.end()=collisionVector.erase(search);
						}
					}
				}
		};


//chemical iterations. creating new  moleculas and destroy old  

		void molIteractions()
		{
#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG

#ifdef  TESTT
			if (iteractionVector.size()) {
				std::cout << "Iteration_vector::size() = " << iteractionVector.size() << std::endl;
				for (auto& i : iteractionVector)
				{
					std::cout << i.first << "|"<<i.second << "||";
				}
				std::cout << std::endl;
			}
#endif //  TEST
			bool b = false;
			std::vector<int32_t> removeVector;
			for (auto& i : iteractionVector)
			{

				const auto const mol1 = molIterById(i.first);
				const auto const mol2 = molIterById(i.second);
#ifdef  TEST
				std::cout << mol1->molObj.get_name()<<" + "<<mol2->molObj.get_name();
#endif
				std::vector<molecules> sum(mol1->molObj + mol2->molObj);
#ifdef  TEST
				//std::cout << "molIteractions::sum.size() = " << sum.size() << std::endl;
		         std::cout << " |=> ";
				for (auto& r : sum)
				{
					std::cout << r.get_name() << "|";

				}
				b = true;
				

#endif //  TEST

				auto pos_middle = (mol1->P + mol2->P) / 2.0f;
				auto dir_middle = (mol1->D + mol2->D) / 2.0f;
				int molNum = 0;

				for (molecules& s : sum)
				{

					//radius of new group of molecules circle
					float R=0;
					for (molecules& r : sum)R += 35;
					R /= 3.14f;

					float segSize = 2*3.14/sum.size();
					olc::vf2d rotVec={ R * cos(segSize * molNum),
									   R *sin(segSize * molNum)};

					auto pos = pos_middle + rotVec;

					addMol(s, pos, dir_middle);
					molNum++;
				}

				removeVector.push_back(i.first);
				removeVector.push_back(i.second);


			}

			std::sort(removeVector.begin(), removeVector.end());
			removeVector.erase(std::unique(removeVector.begin(), removeVector.end()), removeVector.end());

			for(auto &r:removeVector)
				removeMol(r);
			

			iteractionVector.clear();
#ifdef TEST
			if (b) {
				std::cout << " molStates:| ";
				for (auto& s : molStates)
					std::cout << s.molObj.get_name() << "|";
				std::cout << std::endl;
				b = false;
			}
#endif
		}


		void molDecay(float fElapsedTime){

#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG
		
		fLastSecond += fElapsedTime;
		if (fLastSecond >= 1.0f) 
		{
			fLastSecond = 0.f;
			iSecondFromStart++;
#ifdef TESTT
			std::cout << "iSecondFromStart = " << iSecondFromStart << std::endl;
#endif
			for (auto& m : molStates)
				if ((m.lifetime != -1)&&(m.lifetime!=0))m.lifetime--;
		}

		}

		void molDecayIteractions() {

#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG

			std::vector<int32_t> removeVector;


			for (auto& const m : molStates)
				if(m.lifetime==0)
				{
					const auto iID = m.Id;
#ifdef TESTT
					std::cout << m.Id << "|" << std::endl;
#endif
				
					const std::vector<molecules> sum(m.molObj.decay());
					auto pos_middle = m.P;
					auto dir_middle = m.D;
					int molNum = 0;
					
					//radius of new group of molecules circle


					auto max_r= std::max_element(sum.begin(), sum.end(), []( const molecules& a, const  molecules& b) {return a.get_radius() < b.get_radius(); });
					float R =  max_r->get_radius()* sum.size() / 3.14f;
					if (R < max_r->get_radius())R = max_r->get_radius() * 1.3f;

					
					olc::vf2d prev_rotVec;
					int32_t prev_radius;
					

					for (molecules s : sum)
					{
#ifdef TESTT
						std::cout << m.Id << "||" << std::endl;
#endif
						float segSize = 2 * 3.14 / sum.size();
						olc::vf2d rotVec = { R * cos(segSize * molNum),
										   R * -1.f*sin(segSize * molNum) };

						if (molNum != 0)
						{
							auto distance = std::hypot(rotVec.x - prev_rotVec.x, rotVec.y - prev_rotVec.y);
							if (distance < (s.get_radius() + prev_radius))
							{
								rotVec.x *= (s.get_radius() + prev_radius) / distance;
								rotVec.y *= (s.get_radius() + prev_radius) / distance;

								prev_rotVec = rotVec;
								prev_radius = s.get_radius();
							}

						}
						else {
							prev_rotVec = rotVec;
							prev_radius = s.get_radius();
						}
						
						auto pos = pos_middle + rotVec*1.1f;

						addMol(s, pos, dir_middle);
						molNum++;
#ifdef TEST
						std::cout << "| " << pos << "  = " << pos_middle << " + " << rotVec<<"radius= " << s.get_radius()<<std::endl;
#endif
					}
#ifdef TESTT
					std::cout << m.Id << "||||" << std::endl;
#endif

					removeVector.push_back(iID);
#ifdef TESTT
					std::cout << m.Id<<"|||||" << std::endl;
#endif

				}

#ifdef TESTT
			
			std::cout << __FUNCTION__ << " removeVector before unique: ";
			for (auto& r : removeVector)
				std::cout << r << "|";
			std::cout << std::endl; 

#endif

			std::sort(removeVector.begin(), removeVector.end());
			removeVector.erase(std::unique(removeVector.begin(), removeVector.end()), removeVector.end());
#ifdef TESTT

			std::cout << __FUNCTION__ << " removeVector: ";
			for (auto& r : removeVector)
				std::cout << r << "|";
			std::cout << std::endl ;

			std::cout << __FUNCTION__ << " molStates before remove: ";
			for (auto& s : molStates)
				std::cout << s.Id << "-"<<s.molObj.get_name() << "|";
			std::cout << std::endl;
#endif

			for (auto& r : removeVector)
				removeMol(r);
#ifdef TESTT
			std::cout << __FUNCTION__ << " molStates: ";
			for (auto& s : molStates)
				std::cout << s.Id << "-" << s.molObj.get_name() << "|";
			std::cout << std::endl;
#endif
		}




		void molStaticCollisions() {

#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG

			for (auto& c : collisionVector) {

				float fDistance = hypotf(c.first->P.x - c.second->P.x, c.first->P.y - c.second->P.y);
				float fOverlap = 0.5f * (fDistance - c.first->radius - c.second->radius);
#ifdef TESTT
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

#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG

			for (auto& c : collisionVector) {
#ifdef TESTT


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


#ifdef TESTT


				std::cout << "!Dynamic:c.first->D = " << c.first->D << "c.second->D = " << c.second->D << std::endl;
#endif
			}
			
		}


		void mouseControlling() {

#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG

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

#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG

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

		//create mol - super add
		void addMol(molecules& mol, olc::vf2d position, olc::vf2d direction,
			        olc::vf2d acceleration, olc::Pixel color, float mass)
		{
			std::lock_guard<std::mutex> lg(addMol_mtx);
			olc::vf2d CurrentPos;
			int32_t randX = RAND_X%pool_size_w;
			int32_t randY = RAND_Y%pool_size_h;

			if (randX <= mol.get_radius())CurrentPos.x = mol.get_radius() + pool_size_x;
			else if (randX >= pool_size_w - mol.get_radius())CurrentPos.x = pool_size_x+pool_size_w - mol.get_radius();
			else	CurrentPos.x = randX+ pool_size_x;

			if (randY<= mol.get_radius())CurrentPos.y = mol.get_radius() + pool_size_y;
			else if (randY >= pool_size_h - mol.get_radius())CurrentPos.y = pool_size_h - mol.get_radius() + pool_size_y;
			else	CurrentPos.y = randY+ pool_size_y;

			molState currentMolState =
			{
			mol,
			olc::Pixel(mol.get_color()[0],mol.get_color()[1],mol.get_color()[2]),
			position,//possition x,y
			direction,
			acceleration,//Acceleration x,y
			mol.get_radius(),//radius
			getId(),// ID
			mass,//mass
			mol.get_lifetime()//lifetime
			};
			
			/*
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

			*/
			molStates.push_back(currentMolState);


			auto val = [&]() {
				auto search = molStatistics.find(mol.get_name());
				if (search == molStatistics.end()) return int32_t(1);
				else return int32_t(search->second + 1);
				};

			molStatistics.insert_or_assign(mol.get_name(), val());
				
				
		};

		//add at  possition with direction
		void addMol(molecules& mol, olc::vf2d position, olc::vf2d direction)
		{

			addMol(mol,
				position,//possition x y
				direction,//direction aka velocity vector
				{ 0,0 },//acceleration 
				olc::Pixel(mol.get_color()[0], mol.get_color()[1], mol.get_color()[2]),
				1.f);

		};

		//add at random possition with some attributes
		void addMol(molecules& mol)
		{
			int32_t radius = 30;
			olc::vf2d CurrentPos;
			int32_t randX = RAND_X % pool_size_w;
			int32_t randY = RAND_Y % pool_size_h;

			if (randX <= radius)CurrentPos.x = radius + pool_size_x;
			else if (randX >= pool_size_w - radius)CurrentPos.x = pool_size_x + pool_size_w - radius;
			else	CurrentPos.x = randX + pool_size_x;

			if (randY <= radius)CurrentPos.y = radius + pool_size_y;
			else if (randY >= pool_size_h - radius)CurrentPos.y = pool_size_h - radius + pool_size_y;
			else	CurrentPos.y = randY + pool_size_y;

			addMol(mol,
				{ static_cast<float>(CurrentPos.x), static_cast<float>(CurrentPos.y) },//possition x y
				{ 0,0 },//direction aka velocity vector
				{ 0,0 },//acceleration 

				olc::Pixel(mol.get_color()[0], mol.get_color()[1], mol.get_color()[2]),
				1.f);
				  
		};


		 const std::vector<molState>::iterator molIterById(int32_t &id)
		{
			auto search = std::find_if(
				molStates.begin(),
				molStates.end(),
				[&id](molState& mol) {return mol.Id == id; });

			if (search != molStates.end())
				return search;
			else throw("ID NOT FOUND");

		}

/*
		 molecules molById(int32_t& id)
		{
				   auto search = std::find_if(
								 molStates.begin(),
								 molStates.end(),
								 [&id](molState& mol) {return mol.Id == id;});

				   auto mol = (*search).molObj;

				   return mol;
		}
	*/	


		void removeMol(int32_t &id)
		{
			auto search = molIterById(id);
			if (search != molStates.end()) 
			{
				auto val =[&]() {
					auto search2 = molStatistics.find(search->molObj.get_name());
					return search2->second - 1;
					};
				if (auto num = val(); num != 0)
					molStatistics.insert_or_assign(search->molObj.get_name(), num);
				else molStatistics.erase(search->molObj.get_name());
					

				//molStates.end() = 
					molStates.erase(search);
			};

		}


		//void addMaterial(material mat) {};


#ifdef SHOW_TEST_INFO

		void printMolStates() {
			int step = 0;

			auto drawPos = pool_size_h + SCREEN_OFFSET + SCREEN_OFFSET+button_panel_side;
		/*
		
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
			

			for (auto& m : molStates)
			{
				DrawString(SCREEN_OFFSET, drawPos + (step++) * 20,
					std::string(
						" ID1: " + std::to_string(m.Id) +
						" ID2: " + m.molObj.get_name())
						, olc::BLACK, 1);

			}



			



			for (auto& i : iteractionVector)
			{
				DrawString(SCREEN_OFFSET, drawPos + (step++) * 20,
					std::string(
						" ID1: " + std::to_string(i.first) +
						" ID2: " + std::to_string(i.second))
						, olc::BLACK, 1);
			}

			*/

			DrawString(SCREEN_OFFSET, drawPos + (step++) * 20,
				std::string("molStates.size: " + std::to_string(molStates.size()))
				, olc::BLACK, 1);

			DrawString(SCREEN_OFFSET, drawPos + (step++) * 20,
				std::string("last added mol ID: " + std::to_string(molStates.back().Id))
				, olc::BLACK, 1);

			DrawString(SCREEN_OFFSET, drawPos + (step++) * 20,
				std::string("iteractionVector.size: " + std::to_string(iteractionVector.size()))


				, olc::BLACK, 1);

			DrawString(SCREEN_OFFSET, drawPos + (step++) * 20,
				std::string("SECONDS FROM START: " + std::to_string(iSecondFromStart))


				, olc::BLACK, 1);

			//iteractionVector

		}


#endif

		const int32_t getId() {
			

			std::lock_guard<std::mutex> lg(get_Id_mtx);
			static int32_t name = 100;

			return name++;
		}

	
private:
		std::mutex get_Id_mtx,
				   addMol_mtx;

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
			int32_t lifetime;
			//0 - empty cell
			//1 - horisontal walls
			//2 - vertical walls
			//100-10000 - particles
			//friend bool operator==(const molState& mol1, const molState& mol2);
		};

		molState& getMollStateByID(int32_t collisionId) {

			for (auto& s : molStates)if(s.Id == collisionId)return s;

		}

		 std::vector<std::pair<molState*,molState*>> collisionVector;
		 std::vector<std::pair<int32_t, int32_t>> iteractionVector;
		 std::vector<molState> molStates;
		 std::map<std::string, int32_t> molStatistics;

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

		int32_t iSecondFromStart=0;
		float fLastSecond=0;


//////////////////////////////////////////////////////
	};



//static vars init
std::map<std::string, std::string> csv_to_RECIPIES::RECIPIES{ {"NULL","NULL" } };
std::map<std::string, std::vector<int32_t>> molecules::MOLS{ {"NULL",{} } };
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
	csv_to_RECIPIES::csv_to_RECIPIES("recep.csv");
#ifdef TEST
	csv_to_RECIPIES rcp;
	rcp.print_RECIPIES();
#endif
	molecules init;
	init.csv_to_MOLS("mols.csv");

#ifdef TEST
	init.print_MOLS();
#endif
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

 
