#include "checkJson.h"

bool check_array_map_json(const char *json_input)
{
	if (json_input == NULL)
	{    
		return false; 
	}    

	if (std::string(json_input).empty())
	{    
		return false; 
	}    

	stack<char> jsonstr;
	bool once = false;
	int len = strlen(json_input);

	if (json_input[0] != '[' && json_input[len - 1] != ']')
		return false;
	else if (len == 2 && json_input[0] == '[' && json_input[1] == ']')
		return true;
	else if (len < 2)
		return false;

	vector<string> jsons;
	bool ret = true;
	split_string(string(json_input).substr(1, len - 2), jsons, "},{");
	for (int i = 0; i < jsons.size(); i++)
	{
		if (checkSingleJson(jsons[i].c_str()) == false)
		{
			return false;
		}
	}

	return true;
}

bool checkSingleJson(const char *json_input)
{
	if (json_input == NULL)
		return false;

	int len = strlen(json_input);
	const char *JSON = "\"\":\"\"";
	const int JSONLEN = strlen(JSON);
	const int posIndex = 2;

	for (int index = 0; index < len; index++)
	{
		int indexJson = 0;
		while (index < len && indexJson < JSONLEN)
		{
			if (json_input[index] == JSON[indexJson])
				indexJson++;
			if (indexJson == posIndex)
			{
				if (json_input[index] == ',')
				{
					return false;
				}
			}
			index++;
		}
	}

	return true;
}

