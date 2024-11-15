#pragma once
#include "FECS.h"
#include <string>
#include <iostream>

namespace FECSDebug {
	 void debug_entity_id(std::string name, EntityID id) {
		  std::cout << name << ": ID: " << id << std::endl;
	 }
}
