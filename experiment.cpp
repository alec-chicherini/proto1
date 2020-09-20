/*
Chemical experiment engine  prototype v 0.6

Game graphics made at Pixel GAME ENGINE v2.08 License (OLC-3) Copyright 2018 - 2020 OneLoneCoder.com

Some of game mechanics from: https://www.youtube.com/javidx9



Compiling at:Microsoft Visual Studio Community 2019Version 16.7.2 - std:c++17

Tested at Microsoft Windows HOME 10 Version	10.0.19041 Build 19041

Screen resolution not lower than: 1280x720

GitHub:		https://github.com/ikvasir/proto1
mail:       chicherin.alexey@gmail.com

*/

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

	auto gradus = acos(vec.x / hypot(vec.y, vec.x)) * float(180) / 3.14f;

	if (vec.y > 0)gradus += 180;

	return gradus;

};

class experiment_proto : public olc::PixelGameEngine
{
private:
	
#ifdef TEST
	molecules A = { "HCl" };
	molecules B = { "Al(OH)3" };
	molecules C = { "Cl2" };
#endif

	class molState;

public:
	
	experiment_proto()
	{
		sAppName = "Chemical experiment prototype engine v 0.6";
	}

public:
	bool OnUserCreate() override
	{

		// Called once at the start, so create things here
		poolInit();
		Clear(olc::WHITE);

#ifdef TEST
		//addMol(A);
		//addMol(B,molStates);
		//addMol(C);
#endif
		current_game_lvl = "lvls\\demo.csv";
		load_lvl(current_game_lvl);

#ifdef TEST

		std::cout << "availible mols:";
		for (auto& m : molLeftPool) std::cout << m.molObj.get_name() << "|";
		std::cout << std::endl;

		std::cout << "added recepies: ";
		std::cout << std::endl;
		csv_to_RECIPIES chek_rep; 
		chek_rep.print_RECIPIES();
		std::cout << std::endl;
		for(auto& r:molResult)
			std::cout << "awaiting result: "<<r.molObj.get_name()<<" ";

#endif

		return true;
	}

	bool OnUserUpdate(float fElapsedTime)
	{

#ifdef TEST

		if (GetKey(olc::Key::K1).bPressed)	addMol(A, molStates);
		if (GetKey(olc::Key::K2).bPressed)	addMol(B, molStates);
		if (GetKey(olc::Key::K3).bPressed)	addMol(C, molStates);
#endif


		if(!GAME_PAUSE){
		
		molStateUpdate(fElapsedTime);
		molCollisionUpdate();
		molIteractions();
		molDecay(fElapsedTime);
		molDecayIteractions();
		molStaticCollisions();
		molDynamicCollisions();
		}
		Clear(olc::WHITE);
		mouseControlling();
		DrawPool();
		DrawMols();
		DrawLeftPool();
		DrawStatistics();
		DrawRecepies();
		DrawLog();
		if (HELP_FLAG)DrawHelp();
		gameLogic();

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

		void gameLogic() {
		
			for (auto& r : molResult)
			{
				std::string name=r.molObj.get_name();
				int32_t num;

				parseMol(name, num);

				auto search = molStatistics.find(name);

				if (search != molStatistics.end())
					if (molStatistics.at(name) >= num)
						drawMsg(SCREEN_OFFSET + pool_size_w / 4,
							SCREEN_OFFSET + pool_size_h / 2,
							" YOU DID IT :D\n"
							"\n"
							"    IT TAKES "  + std::to_string(iSecondFromStart) + " SEC.",
							4);

			}

		};

		void drawMsg(int32_t x, int32_t y, std::string msg, int32_t scale)
		{
			GAME_PAUSE = true;
			DrawString( x,y,msg, olc::BLACK, scale);

		}

		void restart_lvl()
		{
			
			collisionVector.clear();
			iteractionVector.clear();
			molStates.clear();
			molStatistics.clear();
		    molLeftPool.clear();
			molResult.clear();
			csv_to_RECIPIES::RECIPIES.clear();
			IterLog::iteractionsLog.clear();

            pSelectedMol = nullptr;
			iSecondFromStart = 0;
			fLastSecond = 0;
			HELP_FLAG = true;
			

		}

		void load_lvl(std::string lvlName) {

			
			////////////////////////////////////
			std::ifstream file;
			file.open(lvlName);
			if (!file)
#ifdef TEST
				std::cout << "File with this LVL name not oppened :" << lvlName << std::endl;
#endif

#ifndef TEST
			throw(std::exception("FILE WITH THIS LVL NAME NOT OPPENED"));
#endif // !TEST
			else {
				std::string s;
				for (file >> s; !file.eof(); file >> s) {
#ifdef TESTT
					std::cout << s << std::endl;
#endif
					s.erase(remove_if(s.begin(), s.end(), isspace), s.end());

					if (s[0] == '/' && s[1] == '/')continue;

					else if (s.find("mols") != s.npos)
					{
						int32_t pos_y = SCREEN_OFFSET+15;
						std::string currentMol = "";
						for (auto ch = 5; ch < s.size(); ch++)
						{
							if (s[ch] != ',') currentMol += s[ch];

							if(s[ch] == ','||ch+1==s.size())
							{
								molecules mol(currentMol);
								pos_y += mol.get_radius();
								addMol(mol, { float(SCREEN_OFFSET + left_pool_width / 2),float(pos_y) }, molLeftPool,false);
								pos_y += mol.get_radius();
								currentMol = "";
							};
						}
					}
					else if (s.find("result=") != s.npos) {
						molecules res_mol(s.substr(7));
						addMol(res_mol, { float(SCREEN_OFFSET + left_pool_width / 2),float(SCREEN_OFFSET+left_pool_height- res_mol.get_radius()-5) }, molResult,false);
					}

					else{

						std::string s1, s2, s3;

						s1 = s.substr(0, s.find_first_of(","));
						s2 = s.substr(s.find_first_of(",") + 1, s.find_last_of(",") - s.find_first_of(",") - 1);
						s3 = s.substr(s.find_last_of(",") + 1, s.size() - s.find_last_of(","));

#ifdef TEST
						// std::cout << s1 << ' ' << s2 << ' ' << s3 << ' ' << std::endl;
#endif


						int32_t num1, num2;

						parseMol(s1, num1);
						parseMol(s2, num2);


						//insert recepie
						auto tup = std::make_tuple(num1, s1, num2, s2, s3);
						csv_to_RECIPIES::RECIPIES.push_back(tup);
					}
				}
			};
			////////////////////////////////

		};


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

			//Draw Recepie window
			DrawRect(SCREEN_OFFSET , pool_size_h + SCREEN_OFFSET, TARGET_SCREEN_SIZE_X- log_pool_width- SCREEN_OFFSET- SCREEN_OFFSET, log_pool_height, olc::BLACK);

			//Draw Log window
			DrawRect(TARGET_SCREEN_SIZE_X - log_pool_width - SCREEN_OFFSET, pool_size_h + SCREEN_OFFSET, log_pool_width, log_pool_height, olc::BLACK);

			//TEXT LIKE BUTTONS
			DrawString(TARGET_SCREEN_SIZE_X - SCREEN_OFFSET - 40, SCREEN_OFFSET-10, "HELP", olc::BLACK);

			DrawString(TARGET_SCREEN_SIZE_X - SCREEN_OFFSET - 140, SCREEN_OFFSET - 10, "RESTART", olc::BLACK);

			DrawString(SCREEN_OFFSET + 5, SCREEN_OFFSET - 10, std::string("SECONDS FROM START: " + std::to_string(iSecondFromStart)), olc::BLACK, 1);

		}

		void DrawLeftPool()
		{

			DrawString({ SCREEN_OFFSET + 5, SCREEN_OFFSET + 5 }, "MOLECULES:", olc::BLACK, 1);
			for (auto& mol : molLeftPool)
			{

				DrawCircle(mol.P, mol.radius, mol.molColor);
				FillCircle(mol.P, mol.radius, mol.molColor);
				DrawString(mol.P.x - mol.radius + 5, mol.P.y, mol.molObj.get_name(),olc::WHITE, 1);

			}


			for (auto& mol : molResult) 
			{
				DrawCircle(mol.P, mol.radius, mol.molColor);
				FillCircle(mol.P, mol.radius, mol.molColor);
				DrawString(mol.P.x - mol.radius + 5, mol.P.y, mol.molObj.get_name(), olc::WHITE, 1);
			
			}

			DrawString({ SCREEN_OFFSET + 5, pool_size_h - 25 }, "GOAL:", olc::BLACK, 1);

		};

		void DrawHelp()
		{
			GAME_PAUSE = true;
			DrawString(pool_size_x+5, pool_size_y+5,
				"   \n"
				"   \n"
				"   \n"
				
				"   \n"
				"         CLICK ON THE MOLECULE ON THE LEFT                                                                    ^                       \n"
				"<--                                                                                                      HELP  |                                            \n"
				"         TO ADD IT IN THE                                                                                                                             \n"
				"                                                                                                            AND                                   \n"
				"         EXPERIMENT POOL                                                                                                                              \n"
				"                                                                                                          RESTART    \n"
				"   \n"
				"   \n"
				"   \n"
				"   \n"
				"   \n"
				"   \n"
				"   \n"

				"   \n"
				"   \n"
				"   \n"

				"   \n"
				"   \n"
				"   \n"
				"   \n"
				"                                 HI! THIS IS DEMO LEVEL FOR CHEMICAL EXPERIMENT ENGINE - GAME MECHANIC  \n"
				"   \n"
				"                                             \n"
				"   \n"
				"                                          \n"

				"              CLICK AND PULL                             IF A RECIPE EXISTS                    MOLECULES OF THE SAME TYPE                       \n"
				"   \n"
				"              ON MOLECULES IN THE POOL                   MOLECULES  ITERACT                    COMBINE INTO TEMPORARY                        \n"
				"   \n"
				"              TO MAKE THEM MOVE                          WITH EACH OTHER                       AND DECAY WITH TIME                      \n"
				"   \n"
				"   \n"
				"   \n"
				"   \n"
				"   \n"			
				"   \n"
				"   \n"
				"   \n"
				"   \n"
				"   \n"
				"   \n"
				"   \n"
				"   \n"
				"   \n"
				"                                                                                                STATISTICS   -->             \n"
				"                                                                                                                 \n"
				"                                                                                                  AND                        \n"
				"                                                                                                                 \n"
				"                                                                                                  LOG                    \n"
				"                                                                                                                 \n"
				"                                                                                                   |           \n"
				"                                                                                                   v         \n"
				"                                                                                                        \n"
				"                                                                                                        \n"
				"           CREATE                                                                                                    \n"
				"                                                                                                        \n"
				"<--        THIS CHEMICAL\n"
				"                                                                                                        \n"
				"           TO WIN THE GAME \n"
				, olc::BLACK, 1);



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
				DrawString(mol.P.x - mol.radius+5, mol.P.y,

#ifndef SHOW_TEST_INFO
					mol.molObj.get_name(),
#endif // !SHOW_TEST_INFO

#ifdef SHOW_TEST_INFO
					mol.molObj.get_name(),
					//std::to_string(mol.Id),
#endif // SHOW_TEST_INFO
					olc::WHITE, 1);


				std::string sLifetime = (mol.lifetime == -1)?"inf":std::to_string(mol.lifetime);
				DrawString(mol.P.x + mol.radius,mol.P.y - mol.radius, sLifetime, mol.molColor,1);


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
		void DrawRecepies() 
		{

#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG
			int step = 0;

			auto drawPosX = SCREEN_OFFSET + 10;

			DrawString(drawPosX, SCREEN_OFFSET + pool_size_h + 10 + (step++) * 10, std::string("RECIPES:"), olc::BLACK, 1);

			
			for (auto& s : csv_to_RECIPIES::RECIPIES)
			{
				std::string res="";
				if (std::get<0>(s) != 1)res += std::to_string(std::get<0>(s));
				res += std::get<1>(s);
				res += "+";
				if (std::get<2>(s) != 1)res += std::to_string(std::get<2>(s));
				res += std::get<3>(s);
				res += "=";
				res += std::get<4>(s);

				DrawString(drawPosX, SCREEN_OFFSET + pool_size_h + 10 + (step++) * 10,res, olc::BLACK, 1);
			}


		};

		void DrawStatistics() 
		{
#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG
			int step = 0;

			auto drawPosX = pool_size_w + SCREEN_OFFSET + 10 + left_pool_width;
			
			DrawString(drawPosX, SCREEN_OFFSET + 10 + (step++) * 10, std::string("STATISTICS:"), olc::BLACK, 1);

				for(auto& m:molStatistics)
				{
					DrawString(drawPosX, SCREEN_OFFSET+10 +(step++)*10,
						std::string(m.first +
							"-"+
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

							if ((mol1.molObj.is_iterract(mol2.molObj)))
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
				std::cout << "Iteraction_vector::size() = " << iteractionVector.size() << std::endl;
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

				const auto const mol1 = molIterById(i.first,molStates);
				const auto const mol2 = molIterById(i.second, molStates);


				auto msg = std::string(mol1->molObj.get_name() + "+" + mol2->molObj.get_name() + "=");
#ifdef  TEST
				std::cout << mol1->molObj.get_name()<<"+"<<mol2->molObj.get_name();

#endif


				std::vector<molecules> sum(mol1->molObj + mol2->molObj);

				//std::cout << "molIteractions::sum.size() = " << sum.size() << std::endl;
		         //std::cout << " |=> ";
				for (auto& r : sum)
				{

#ifdef  TEST
					std::cout << r.get_name() << "|";
#endif //  TEST

					msg += r.get_name();
					msg += "+";

				}
#ifdef  TEST
				std::cout << std::endl;
				b = true;
#endif //  TEST


				if (msg.back() == '+') msg.pop_back();
				IterLog log;
				log.addIteractionLogMsg(msg, "");

				auto pos_middle = (mol1->P + mol2->P) / 2.0f;
				auto dir_middle = (mol1->D + mol2->D) / 2.0f;
				int molNum = 0;

				auto max_r = std::max_element(sum.begin(), sum.end(), [](const molecules& a, const  molecules& b) {return a.get_radius() < b.get_radius(); });
				float R = max_r->get_radius() * sum.size() / 3.14f;
				if (R < max_r->get_radius())R = max_r->get_radius() * 1.3f;


				olc::vf2d prev_rotVec;
				int32_t prev_radius;

				for (molecules& s : sum)
				{
#ifdef TESTT
					std::cout << m.Id << "||" << std::endl;
#endif
					float segSize = 2 * 3.14 / sum.size();
					olc::vf2d rotVec = { R * cos(segSize * molNum),
									   R * -1.f * sin(segSize * molNum) };

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

					auto pos = pos_middle + rotVec * 1.1f;

					addMol(s, pos, dir_middle, molStates);
					molNum++;
#ifdef TESTT
					std::cout << "| " << pos << "  = " << pos_middle << " + " << rotVec << "radius= " << s.get_radius() << std::endl;
#endif
				}

				removeVector.push_back(i.first);
				removeVector.push_back(i.second);


			}

			std::sort(removeVector.begin(), removeVector.end());
			removeVector.erase(std::unique(removeVector.begin(), removeVector.end()), removeVector.end());

			for(auto &r:removeVector)
				removeMol(r);
			

			iteractionVector.clear();

#ifdef TESTT

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

						addMol(s, pos, dir_middle, molStates);
						molNum++;
#ifdef TESTT
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
				float speedLimit = 250.f;
				
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

			//MOLS
#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG

auto isPointOverlap = [](const int32_t& x, const int32_t& y, molState& mol) {
				return x < (mol.P.x + mol.radius)&&
					   x > (mol.P.x - mol.radius)&&
					   y > (mol.P.y - mol.radius)&&
					   y < (mol.P.y + mol.radius);
			};


auto isPointOverlapRect = [](const int32_t& x, const int32_t& y,
							 const int32_t& rX, const int32_t& rY,
							 const int32_t& rW, const int32_t& rH) {

		return x < (rX+rW) &&
			   x > (rX) &&
		       y < (rY + rH) &&
		       y > (rY );
};


			if (GetMouse(0).bPressed||GetMouse(1).bPressed)
			{
				if (HELP_FLAG) {
					HELP_FLAG = false; GAME_PAUSE = false;
				}
				//if button HELP pressed
				else if (isPointOverlapRect(GetMouseX(), GetMouseY(), TARGET_SCREEN_SIZE_X - SCREEN_OFFSET - 40, SCREEN_OFFSET - 10, 30, 10))
						HELP_FLAG = true;

				//if button RESTART pressed
				if (isPointOverlapRect(GetMouseX(), GetMouseY(), TARGET_SCREEN_SIZE_X - SCREEN_OFFSET - 140, SCREEN_OFFSET - 10, 60, 10))
				{
					restart_lvl();
					load_lvl(current_game_lvl);
					HELP_FLAG = true;

				}
						 
						

				pSelectedMol = nullptr;

				for (auto& mol : molStates) 
					if (isPointOverlap(GetMouseX(), GetMouseY(), mol))
					{
						pSelectedMol = &mol;
						break;
					}

				for(auto& mol:molLeftPool)
					if (isPointOverlap(GetMouseX(), GetMouseY(), mol))
					{
						addMol(mol.molObj, {mol.P.x + SCREEN_OFFSET+ SCREEN_OFFSET, mol.P.y}, {mol.D.x + 10, mol.D.y}, molStates);
						break;
					}
				
			}

#ifdef TEST
			if (GetMouse(0).bReleased)
			{
				pSelectedMol = nullptr;
			}
#endif

			if (GetMouse(1).bReleased||GetMouse(0).bReleased)
			{
				if (pSelectedMol != nullptr) 
				{
					pSelectedMol->D.x = 5.0f * ((pSelectedMol->P.x) - static_cast<float>(GetMouseX()));
					pSelectedMol->D.y = 5.0f * ((pSelectedMol->P.y) - static_cast<float>(GetMouseY()));
				};
				pSelectedMol = nullptr;
			}

#ifdef TEST
			if (GetMouse(0).bHeld)
			{

				if (pSelectedMol != nullptr)
				{
					pSelectedMol->P.x = GetMouseX();
					pSelectedMol->P.y = GetMouseY();
				}
			}
#endif
			///BUTTONS
			/// 
			/// 

		};

		void poolInit() {

#ifdef TEST_HARD_DEBUG
			std::cout << __FUNCTION__ << std::endl;
#endif TEST_HARD_DEBUG

			log_pool_height = 100;
			log_pool_width = 600;

			left_pool_width = SCREEN_OFFSET * 2;
			
			right_pool_width= SCREEN_OFFSET * 2;
			

			pool_size_x = SCREEN_OFFSET + left_pool_width;
			pool_size_y = SCREEN_OFFSET;
			pool_size_h = TARGET_SCREEN_SIZE_Y - SCREEN_OFFSET * 2 - log_pool_height;
			pool_size_w = TARGET_SCREEN_SIZE_X - SCREEN_OFFSET * 2 - left_pool_width - right_pool_width;

			right_pool_height = pool_size_h;
			left_pool_height = pool_size_h;

		
		}

		//create mol - super add
		void addMol(molecules& mol, olc::vf2d position, olc::vf2d direction,
			        olc::vf2d acceleration, olc::Pixel color, float mass,std::vector<molState>& molVec,bool bCountInStatistic_)
		{
			std::lock_guard<std::mutex> lg(addMol_mtx);
			olc::vf2d CurrentPos;
			int32_t randX = RAND%pool_size_w;
			int32_t randY = RAND%pool_size_h;

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
			mol.get_lifetime(),//lifetime
			bCountInStatistic_//count in statistic
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
			
			//friend bool operator==(const molState& mol1, const molState& mol2);

			*/
			molVec.push_back(currentMolState);

			if (bCountInStatistic_) {
			auto val = [&]() {
				auto search = molStatistics.find(mol.get_name());
				if (search == molStatistics.end()) return int32_t(1);
				else return int32_t(search->second + 1);
				};

			molStatistics.insert_or_assign(mol.get_name(), val());
			};
		};

		//add at  possition with direction
		void addMol(molecules& mol, olc::vf2d position, olc::vf2d direction, std::vector<molState>& molVec)
		{

			addMol(mol,
				position,//possition x y
				direction,//direction aka velocity vector
				{ 0,0 },//acceleration 
				olc::Pixel(mol.get_color()[0], mol.get_color()[1], mol.get_color()[2]),
				1.f,molVec,true);

		};




		void addMol(molecules& mol, olc::vf2d position, std::vector<molState>& molVec)
		{

			addMol(mol,
				position,//possition x y
				{ 0,0 },//direction aka velocity vector
				{ 0,0 },//acceleration 
				olc::Pixel(mol.get_color()[0], mol.get_color()[1], mol.get_color()[2]),
				1.f, molVec,true);

		};
		

		void addMol(molecules& mol, olc::vf2d position, std::vector<molState>& molVec,bool bCountStatistic)
		{

			addMol(mol,
				position,//possition x y
				{ 0,0 },//direction aka velocity vector
				{ 0,0 },//acceleration 
				olc::Pixel(mol.get_color()[0], mol.get_color()[1], mol.get_color()[2]),
				1.f,
				molVec,
				bCountStatistic);

		};

		//add at random possition with some attributes
		void addMol(molecules& mol, std::vector<molState>& molVec)
		{
			int32_t radius = 30;
			olc::vf2d CurrentPos;
			int32_t randX = RAND % pool_size_w;
			int32_t randY = RAND % pool_size_h;

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
				1.f,
			    molVec,true);
				  
		};


		 const std::vector<molState>::iterator molIterById(int32_t &id, std::vector<molState>& vec)
		{
			auto search = std::find_if(
				vec.begin(),
				vec.end(),
				[&id](molState& mol) {return mol.Id == id; });

			if (search != vec.end())
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
			auto search = molIterById(id, molStates);
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
			int step2 = 0;

			auto drawPos = pool_size_h + SCREEN_OFFSET + SCREEN_OFFSET+log_pool_height;
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

			/*DrawString(SCREEN_OFFSET, drawPos + (step++) * 20,
				std::string("last added mol ID: " + std::to_string(molStates.back().Id))
				, olc::BLACK, 1);
*/
			DrawString(SCREEN_OFFSET, drawPos + (step++) * 20,
				std::string("iteractionVector.size: " + std::to_string(iteractionVector.size()))
				, olc::BLACK, 1);

			DrawString(SCREEN_OFFSET, drawPos + (step++) * 20,
				std::string("SECONDS FROM START: " + std::to_string(iSecondFromStart))
				, olc::BLACK, 1);

			/*
			for (auto& msg : IterLog::iteractionsLog){
				DrawString(left_pool_width+ SCREEN_OFFSET+10, pool_size_h + SCREEN_OFFSET+10 + (step2++) * 10, msg,olc::BLACK,1);
				//std::cout << "for (auto& msg : IterLog::iteractionsLog): " << msg<<std::endl;

			}
			//iteractionVector*/

		}


#endif

		void DrawLog()
		{
			int step = 0;
			DrawString(TARGET_SCREEN_SIZE_X - log_pool_width - SCREEN_OFFSET + 10, pool_size_h + SCREEN_OFFSET + 10 + (step++) * 10, "LOG:", olc::BLACK, 1);
			for (auto& msg : IterLog::iteractionsLog) {
				DrawString(TARGET_SCREEN_SIZE_X - log_pool_width - SCREEN_OFFSET + 10, pool_size_h + SCREEN_OFFSET + 10 + (step++) * 10, msg, olc::BLACK, 1);
				if (step == 8)break;
				//std::cout << "for (auto& msg : IterLog::iteractionsLog): " << msg<<std::endl;

			}
	
		};

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
			bool bCountInStatistic=true;
			
			//friend bool operator==(const molState& mol1, const molState& mol2);
			//molState(molecules mol) :molObj(mol) {};
		};

		molState& getMollStateByID(int32_t collisionId) {

			for (auto& s : molStates)if(s.Id == collisionId)return s;

		}

		 std::vector<std::pair<molState*,molState*>> collisionVector;
		 std::vector<std::pair<int32_t, int32_t>> iteractionVector;
		 std::vector<molState> molStates;
		 std::map<std::string, int32_t> molStatistics;
		 std::vector<molState> molLeftPool;
		 std::vector<molState> molResult;

		int32_t pool_size_x,
				pool_size_y,
				pool_size_w,
				pool_size_h,
				left_pool_width,
				left_pool_height,
				right_pool_width,
				right_pool_height,
				log_pool_height,
				log_pool_width;


		molState* pSelectedMol=nullptr;

		int32_t iSecondFromStart=0;
		float fLastSecond=0;
		bool HELP_FLAG=true;
		bool GAME_PAUSE = false;

		std::string current_game_lvl;

//////////////////////////////////////////////////////
	};



//static vars init

std::vector<std::string> IterLog::iteractionsLog{};

std::vector<std::tuple<int32_t, std::string, int32_t, std::string, std::string>> csv_to_RECIPIES::RECIPIES{};

std::map<std::string, std::vector<int32_t>> molecules::MOLS{ {"NULL",{} } };

int random_generator::iRand = 0;
std::vector<int> random_generator::vRand = { {} };

/////////////////////////


int main()
{
	//Initialisations of Experiment Engine
	//csv_to_RECIPIES::csv_to_RECIPIES("recep.csv");
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
	R.rand_init(10);

	for (int i = 0; i < 100; i++) {
	
		std::cout << "RAND= " << RAND<<std::endl;
		}

	//end of Inits


	experiment_proto main_screen;
	if (main_screen.Construct(SCREEN_SIZE_X, SCREEN_SIZE_Y, 1, 1))
		main_screen.Start();



	return 0;
}

 
