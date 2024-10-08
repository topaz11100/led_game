#include "Arduino.h"

#include "Parts_base.h"


void Joystick::init(int x, int y, int z, int s) {
	xPin = y;
	yPin = x;
	zPin = z;
	pinMode(xPin, INPUT);
	pinMode(yPin, INPUT);
	pinMode(zPin, INPUT_PULLUP);
	sense = s;
}

int Joystick::x_triread() {
	int temp = x_read();
	if (temp < (512 - sense)) return -1;
	else if (temp > (512 + sense)) return 1;
	else return 0;
}

int Joystick::y_triread() {
	int temp = y_read();
	if (temp < (512 - sense)) return -1;
	else if (temp > (512 + sense)) return 1;
	else return 0;
}

void Ultrasonic::init(int trig, int echo) {
	trigPin = trig;
	echoPin = echo;
	pinMode(echoPin, INPUT);
	pinMode(trigPin, OUTPUT);
}

float Ultrasonic::distance() {
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);
	long duration = pulseIn(echoPin, HIGH);
	float result = ((float)(340 * duration) / 1000) / 2;
	return result;
}

void Servo_vector::attach(int pin[]) {
	for (int i = 0; i < size; i += 1) {
		sv[i].attach(pin[i]);
	}
}

bool Servo_vector::issame(int angle[]) {
	for (int i = 0; i < size; i += 1) {
		if (sv[i].read() != angle[i]) return false;
	}
	return true;
}

void Servo_vector::move_arr(int angle[], int speed) {
	while ( !issame(angle) ) {
		for (int i = 0; i < size; i += 1) {
			int temp = sv[i].read();
			if (temp > angle[i])      sv[i].write(temp - 1);
			else if (temp < angle[i]) sv[i].write(temp + 1);
		}
		delayMicroseconds(speed);
	}
}

void Servo_vector::move_one(int n, int angle, int speed) {
	while (true) {
		int temp = sv[n].read();
		if      (temp > angle) sv[n].write(temp - 1);
		else if (temp < angle) sv[n].write(temp + 1);
		else return;
		delayMicroseconds(speed);
	}
}

void lcd_print(const LiquidCrystal_I2C& lcd, const String& str, int x, int y) {
	lcd.setCursor(x, y);
	lcd.print(str);
}
String make_space(const String& str, int lcd_width, int mode = 0) {
	int len_space = lcd_width - str.length();
	if (len_space <= 0) return str;

	String space = "";

	if (mode == 2) {
		String space2 = "";
		for (int i = 0; i < len_space / 2; i += 1) space += " ";
		for (int i = 0; i <= len_space / 2; i += 1) space2 += " ";
		return space + str + space2;
	}

	for (int i = 0; i < len_space; i += 1) space += " ";

	if      (mode == 0) return str + space;
	else if (mode == 1) return space + str;
}

char receive_char(char error_char) {
	if ( Serial.available() ) return Serial.read();
	else return error_char;
}
String receive_String(char terminal_char) {
	String result = "";
	if ( !(Serial.available()) ) return result;
	while (true) {
		char temp = receive_char('?');
		if (temp == terminal_char) break;
		if (temp == '?') continue;
		result += String(temp);
	}
	return result;
}

void Protocol::strip(String str) {
	int* interval = new int[sep_count];
	int mark = 0;
	for (int i = 0; i < sep_count; i += 1) {
		interval[i] = str.indexOf(sep, mark);
		mark = interval[i] + 1;
	}
	s[0] = str.substring(0, interval[0]);
	for (int i = 1; i < sep_count; i += 1) {
		s[i] = str.substring(interval[i - 1] + 1, interval[i]);
	}
	s[sep_count] = str.substring(interval[sep_count - 1] + 1);
	delete[] interval;
}
