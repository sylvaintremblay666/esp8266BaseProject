/*
 * KeyValueFlash.h
 *
 *  Created on: Nov 12, 2018
 *      Author: stremblay
 */

#ifndef KEYVALUEFLASH_H_
#define KEYVALUEFLASH_H_

#include "Arduino.h"
#include "FS.h"

class KeyValueFlash {
public:
	KeyValueFlash();
	KeyValueFlash(String configName);
	virtual ~KeyValueFlash();

	void   set(String key, String value);
	String get(String key);
	bool   exist(String key);
	void   remove(String key);

private:
	String configFolder    = "/default";

};

#endif /* KEYVALUEFLASH_H_ */
