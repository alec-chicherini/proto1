/*
Chemical experiment prototype engine
GitHub:		https://github.com/ikvasir/proto1
License: MIT
*/
#pragma once
void inline parseMol(std::string& name, int32_t& num) {
	num = 0;
	if (isdigit(name[0]))
	{
		num += std::stoi(name.substr(0, 1));
		name.erase(0, 1);

		if (isdigit(name[0]))
		{
			num = 10 * num + std::stoi(name.substr(0, 1));
			name.erase(0, 1);

			if (isdigit(name[0]))
			{
				num = 10 * num + std::stoi(name.substr(0, 1));
				name.erase(0, 1);

				if (isdigit(name[0]))
				{
					num = 10 * num + std::stoi(name.substr(0, 1));
					name.erase(0, 1);
					if (isdigit(name[0]))throw("TWO MUCH MOLECULES TRIIYNG TO JOIN");
				};

			};
		};
	}

	else
	{
		num = 1;
	}

};

//max mol size = 99999H20
void inline parseMol(std::string& name) {
	if (isdigit(name[0]))
	{
		name.erase(0, 1);

		if (isdigit(name[0]))
		{
			name.erase(0, 1);
			if (isdigit(name[0]))
			{
				name.erase(0, 1);
				if (isdigit(name[0]))
				{
					name.erase(0, 1);
					if (isdigit(name[0]))
					{
						name.erase(0, 1);
						if (isdigit(name[0]))throw("TWO MUCH MOLECULES TRIIYNG TO JOIN");
					};
				};
			};
		};
	}


};