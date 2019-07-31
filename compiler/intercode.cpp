#include "intercode.h"

InterCode::~InterCode(){
	for(int i = 0; i < code.size; i++){
		delete code[i];
	}
}
