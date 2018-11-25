/*
 * KeyValueFlash.cpp
 *
 *  Created on: Nov 12, 2018
 *      Author: stremblay
 */

#include "KeyValueFlash.h"

KeyValueFlash::KeyValueFlash(String configName) {
	this->configFolder = "/" + configName;

	KeyValueFlash();
}

KeyValueFlash::KeyValueFlash() {
	SPIFFS.begin();
}

KeyValueFlash::~KeyValueFlash() {
}

void KeyValueFlash::set(String key, String value) {

	String valueFileName = configFolder + "/" + key;

	Serial.print("KeyValueFlash [save] : "); Serial.print(valueFileName); Serial.println(" (" + value + ")");

	File f = SPIFFS.open(valueFileName, "w");
	if (!f) {
		// Error opening file!
		Serial.print("KeyValueFlash [save] : error opening file: "); Serial.println(valueFileName);
	}
	f.print(value);
	f.close();
}

bool KeyValueFlash::exist(String key) {
	String valueFileName = configFolder + "/" + key;
	return SPIFFS.exists(valueFileName);
}

String KeyValueFlash::get(String key) {
	String valueFileName = configFolder + "/" + key;
	Serial.print("KeyValueFlash [load] : Getting: " + valueFileName);
	if (SPIFFS.exists(valueFileName)){
		File valueFile = SPIFFS.open(valueFileName, "r");
		if (!valueFile) {
			Serial.println("KeyValueFlash [load] : error reading key file: " + valueFileName);
			return "";
		}
		String value = valueFile.readString();
		valueFile.close();
		Serial.println("  -> " + value);
		return value;
	}

	return "";
}

void KeyValueFlash::remove(String key) {
	String valueFileName = configFolder + "/" + key;
	Serial.println("KeyValueFlash: Removing key: " + key);
	SPIFFS.remove(valueFileName);
}
