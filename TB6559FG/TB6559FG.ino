//	TB6559FG.ino
//
// https://coskxlabsite.stars.ne.jp/html/for_students/M5Stack/PWMTest/PWMTest2.html
//
#include <M5Stack.h>

const uint8_t	nResBits	= 8;		// PWMに使用するビット数　n=1～16[bit]
										// PWM周波数の最大値 Maxfreq = 80000000.0/2^n[Hz] = 312500[Hz]
const uint8_t	PWM_CH		= 2;		// PWMチャンネル
const uint8_t	PWM_PIN		= 2;		// PWM出力に使用するGPIO PIN番号
const uint8_t	IN1_PIN		= 3;		
const uint8_t	IN2_PIN		= 1;		
const uint8_t	SB_PIN		= 16;		

typedef struct tagCNT_TABLE {
	uint8_t	in1;
	uint8_t	in2;
	uint8_t	sb;
} CNT_TABLE;
const CNT_TABLE CntTbale[] = {	{1, 1, 1},	// Short Brake
								{0, 1, 1},	// CCW
								{1, 0, 1},	// CW
								{0, 0, 1},	// Stop
								{0, 0, 0}	// Standby
							};
enum {
	SHORT_BRAKE = 0,
	CCW,
	CW,
	STOP,
	STANDBY
};

int	g_nDutyPWM				= 0;		// デューティ n / 255
double	nFreqPWM			= 164700.0;	//
int	g_nCntSeq				= STOP;		//

////////////////////////////////////////
//
void Control(int com)
{
	digitalWrite(IN1_PIN, CntTbale[com].in1 ? HIGH : LOW);
	digitalWrite(IN2_PIN, CntTbale[com].in2 ? HIGH : LOW);
	digitalWrite(SB_PIN, CntTbale[com].sb ? HIGH : LOW);
}

////////////////////////////////////////
//
void DisplayValue()
{
	M5.Lcd.setCursor(10, 0);
	M5.Lcd.setTextSize(3);
//	M5.Lcd.setTextFont(3);
	M5.Lcd.print("Duty Value");
	M5.Lcd.setCursor(10, 40);
	M5.Lcd.printf("  %d / 255   ", g_nDutyPWM);
	M5.Lcd.setCursor(10, 80);
	M5.Lcd.print("Frequency");
	M5.Lcd.setCursor(10, 120);
	M5.Lcd.printf("  %.1f	", nFreqPWM);

	double fCurrent = g_nDutyPWM * 3.5012 + 0.2;

	M5.Lcd.setCursor(10, 160);
	M5.Lcd.print("Current");
	M5.Lcd.setCursor(10, 200);
	M5.Lcd.printf("  %.1fmA	   ", fCurrent);
}

////////////////////////////////////////
//
void setup()
{
	M5.begin();
//	delay(10);
	M5.Power.begin();
//	delay(10);
	Serial.begin(115200);
//	delay(10);

	DisplayValue();

	pinMode(PWM_PIN,	OUTPUT); 
	pinMode(IN1_PIN,	OUTPUT); 
	pinMode(IN2_PIN,	OUTPUT); 
	pinMode(SB_PIN,		OUTPUT); 

	Serial.print("Initialize I/O");

	Control(STOP);

	Serial.print("Control I/O");

	// チャンネルと周波数の分解能を設定
	ledcSetup(PWM_CH, nFreqPWM, nResBits);

	// PWM出力ピンとチャンネルの設定
	ledcAttachPin(PWM_PIN, PWM_CH);

	// デューティーの設定と出力開始
	ledcWrite(PWM_CH, g_nDutyPWM);

	Serial.print("Initialize Duty");
}

////////////////////////////////////////
//
void loop()
{
	int nLastDutyPWM = g_nDutyPWM;

	//static int ValueIndex = 0;
	M5.update();

	if (M5.BtnA.wasPressed()) {
		g_nDutyPWM++;

		if (g_nDutyPWM > 255)
			g_nDutyPWM = 0;

		Serial.printf("%d\r\n", g_nDutyPWM);

	}
	if (M5.BtnC.wasPressed()) {
		if (g_nCntSeq == STOP)
			g_nCntSeq = CW;

		if (g_nCntSeq == CW)
			g_nCntSeq = STOP;

		Control(g_nCntSeq);

	}
	if (M5.BtnC.wasPressed()) {
		g_nDutyPWM--;

		if (g_nDutyPWM < 0)
			g_nDutyPWM = 255;

		Serial.printf("%d\r\n", g_nDutyPWM);
	}

	if (nLastDutyPWM != g_nDutyPWM) {

//		M5.Speaker.tone(523.251, 200); //Peett
//		M5.Speaker.tone(659.255, 200); //Peett
//		M5.Speaker.tone(783.991, 200); //Peett

		DisplayValue();

		// チャンネルと周波数を更新
		ledcSetup(PWM_CH, nFreqPWM, nResBits);

		// 出力再開
		ledcWrite(PWM_CH, g_nDutyPWM);
	}
}
