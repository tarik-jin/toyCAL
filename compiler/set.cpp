#include "set.h"

Set::Set(){
	count = 0;
}

Set::Set(int size, bool val){
	init(size, val);
}

void Set::init(int size, bool val){
	count = size;
	size = size / 32 + (size % 32 != 0);
	unsigned int v = val ? 0xffffffff : 0;
	for(int i = 0; i < size; i++){
		bmList.push_back(v);
	}
}

void Set::set(int i){
	if(i < 0 || i >= count){
		return;
	}
	else{
		bmList[i / 32] |= (1 << (i % 32));
	}
}

Set Set::operator |(Set val){
	Set ret(count, 0);
	for(unsigned int i = 0; i < bmList.size(); i++){
		ret.bmList[i] = bmList[i] | val.bmList[i];
	}
	return ret;
}

Set Set::operator -(Set val){
	Set ret(count, 0);
	for(unsigned int i = 0; i < bmList.size(); i++){
		ret.bmList[i] = bmList[i] & ~val.bmList[i];
	}
	return ret;
}

bool Set::operator !=(Set& val){
	if(count != val.count){
		return true;
	}
	else{
		for(unsigned int i = 0; i < bmList.size(); i++){
			if(bmList[i] != val.bmList[i]){
				return true;
			}
			else{}
		}
		return false;
	}
}

Set Set::operator &(Set val){
	Set ret(count, 0);
	for(unsigned int i = 0; i < bmList.size(); i++){
		ret.bmList[i] = bmList[i] & val.bmList[i];
	}
	return ret;
}

bool Set::get(int i){
	if(i < 0 || i >= count){
		return false;
	}
	else{
		return !!(bmList[i / 32] & (1 << (i % 32)));
	}
}
