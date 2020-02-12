#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H

#include "lua.hpp"

#include <Logger/Logger.h>

#include <string>

class LuaScript {

	lua_State* L;
	int level;

public:

	LuaScript(const std::string& filepath);
	~LuaScript();

	inline void clean(void);

	template<typename T>
	T Get(const std::string& identifier);

	//template<typename T>
	//T Set(const std::string& identifier, T value);

	template<typename T>
	bool Set(const std::string& variableName, const T value, const bool bOverwrite, const bool bUpdate);

	template<typename T>
	bool LuaSetDefault(const std::string& variableName, const T value, const bool bOverwrite, const bool bUpdate);

private:

	bool LuaGetToStack(const std::string& identifier);

	template<typename T>
	T LuaGetDefault() {
		return 0;
	};

	template<typename T>
	T LuaGet(const std::string& identifier) {
		return 0;
	}

	template<typename T>
	T LuaSet(const std::string& variableName, const T value, const bool bOverwrite, const bool bUpdate) {
		return false;
	}

	void PrintError(const std::string& identifier, const std::string& reason) const;

};

template<typename T>
T LuaScript::Get(const std::string & identifier) {
	if (!L) {
		Console::Error << "Script not loaded\n";
		return LuaGetDefault<T>();
	}

	T result;

	if (LuaGetToStack(identifier)) {
		result = LuaGet<T>(identifier);
	} else {
		result = LuaGetDefault<T>();
	}

	lua_pop(L, level + 1);
	return result;
}

//template<typename T>
//T LuaScript::Set(const std::string& identifier, T value) {
//	if (!L) {
//		Console::Error << "Script not loaded\n";
//		return LuaGetDefault<T>();
//	}
//
//	T result;
//}

template<typename T>
bool LuaScript::Set(const std::string& variableName, const T value, const bool bOverwrite, const bool bUpdate)
{
	if (!L) {
		Console::Error << "Script not loaded\n";
		return false;
	}

	T result = false;
	// If overwriting existing variable, then don't check if it exists in the Lua file
	if (bOverwrite)
	{
		// variable succesfully on top of stack
		result = LuaSet<T>(variableName, value, bOverwrite, bUpdate);
	}
	else
	{
		// Check if the variable exists in the Lua file
		if (LuaGetToStack(variableName))
		{
			// variable succesfully on top of stack
			result = LuaSet<T>(variableName, value, bOverwrite, bUpdate);
		}
	}

	//clean();
	return result;
}

//template<typename T>
//T LuaScript::LuaGet(const std::string & identifier)

//template <>
//inline bool LuaScript::LuaSet<int>(const std::string& variableName, const int iValue, const bool bOverwrite, const bool bUpdate) {
//	lua_getglobal(L, "SetToLuaFile");
//	char outputString[80];
//	sprintf(outputString, "%s = %d", variableName.c_str(), iValue);
//}


template<typename T>
inline bool LuaScript::LuaSetDefault(const std::string& variableName, const T value, const bool bOverwrite, const bool bUpdate) {
	return false;
}

template<>
inline std::string LuaScript::LuaGetDefault<std::string>() {
	return "null";
}

template<>
inline bool LuaScript::LuaGet<bool>(const std::string & identifier) {
	return (bool)lua_toboolean(L, -1);
}

template<>
inline int LuaScript::LuaGet<int>(const std::string & identifier) {
	if (!lua_isnumber(L, -1)) {
		PrintError(identifier, "not a number");
		return LuaGetDefault<int>();
	}

	return (int)lua_tonumber(L, -1);
}

template<>
inline float LuaScript::LuaGet<float>(const std::string & identifier) {
	if (!lua_isnumber(L, -1)) {
		PrintError(identifier, "not a number");
		return LuaGetDefault<float>();
	}

	return (float)lua_tonumber(L, -1);
}

template<>
inline std::string LuaScript::LuaGet<std::string>(const std::string & identifier) {
	std::string result = LuaGetDefault<std::string>();

	if (lua_isstring(L, -1)) {
		result = std::string(lua_tostring(L, -1));
	} else {
		PrintError(identifier, "not a number");
	}

	return result;
}

#endif
