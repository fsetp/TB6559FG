//	TB6559FG.ino
//
// https://coskxlabsite.stars.ne.jp/html/for_students/M5Stack/PWMTest/PWMTest2.html
//
#include <M5Stack.h>

#define	DUTY_BITS	8
#define	MAX_DUTY	255
#define	FREQ_PWM	164700.0

// PWM周波数の最大値 Maxfreq = 80000000.0/2^n[Hz] = 312500[Hz]
#define	PWM_CH		2	// PWMチャンネル
#define	PWM_PIN		2	// PWM出力に使用するGPIO PIN番号
#define	IN1_PIN		16	//
#define	IN2_PIN		17	//
#define	SB_PIN		5	//

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

int	g_nDutyPWM	= 0;		// デューティ n / 255
int	g_nCntSeq	= STOP;		//

////////////////////////////////////////
//
void Control(int com)
{
	digitalWrite(IN1_PIN, CntTbale[com].in1 ? HIGH : LOW);
	digitalWrite(IN2_PIN, CntTbale[com].in2 ? HIGH : LOW);
	digitalWrite(SB_PIN, CntTbale[com].sb ? HIGH : LOW);

	Serial.printf("In1 %s : In2 %s : Sb %s\r\n", CntTbale[com].in1 ? "on" : "off", CntTbale[com].in2 ? "on" : "off", CntTbale[com].sb ? "on" : "off");
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
	M5.Lcd.printf("  %d / %d   ", g_nDutyPWM, MAX_DUTY);
	M5.Lcd.setCursor(10, 80);
	M5.Lcd.print("Frequency");
	M5.Lcd.setCursor(10, 120);
	M5.Lcd.printf("  %.1f	", FREQ_PWM);

	double fCurrent = g_nDutyPWM * 3.5012 + 0.2;

	M5.Lcd.setCursor(10, 160);
	M5.Lcd.print("Current");
	M5.Lcd.setCursor(10, 200);
	M5.Lcd.printf("  %.1fmA	   ", fCurrent);

	M5.Lcd.setCursor(220, 0);
	M5.Lcd.setCursor(220, 40);
	char* pText = NULL;
	if (g_nCntSeq == STOP)
		pText = "STOP";
	if (g_nCntSeq == CW)
		pText = "CW";
	M5.Lcd.printf("%s  ", pText);
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
	ledcSetup(PWM_CH, FREQ_PWM, DUTY_BITS);

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

	int nLastCntSeq = g_nCntSeq;

	//static int ValueIndex = 0;
	M5.update();

	if (M5.BtnA.wasPressed()) {
		g_nDutyPWM++;

		if (g_nDutyPWM > 255)
			g_nDutyPWM = 0;

		Serial.printf("%d\r\n", g_nDutyPWM);
	}

	if (M5.BtnB.wasPressed()) {
		if (g_nCntSeq == STOP)
			g_nCntSeq = CW;

		else if (g_nCntSeq == CW)
			g_nCntSeq = STOP;

		Control(g_nCntSeq);

		char* pText = NULL;
		if (g_nCntSeq == STOP)
			pText = "STOP";
		if (g_nCntSeq == CW)
			pText = "CW";
		Serial.printf("%s  ", pText);
	}

	if (M5.BtnC.wasPressed()) {
		g_nDutyPWM--;

		if (g_nDutyPWM < 0)
			g_nDutyPWM = 255;

		Serial.printf("%d\r\n", g_nDutyPWM);
	}

	if (nLastCntSeq != g_nCntSeq)
		DisplayValue();

	if (nLastDutyPWM != g_nDutyPWM) {

//		M5.Speaker.tone(523.251, 200); //Peett
//		M5.Speaker.tone(659.255, 200); //Peett
//		M5.Speaker.tone(783.991, 200); //Peett

		DisplayValue();

		// チャンネルと周波数を更新
		ledcSetup(PWM_CH, FREQ_PWM, DUTY_BITS);

		// 出力再開
		ledcWrite(PWM_CH, g_nDutyPWM);
	}
}
