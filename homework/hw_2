/////////////////////////////////////////////////////////
// 과제명: HW2. 엘리베이터

//과제개요: 
// 층 입력 명령에 따라 엘리베이터 이동 동작을 실행함
// 층 입력 명령 방법
 // (1) SW(0~7)에 의한 층 명령 입력
//  (2) JS(Joy-Stick)(LEFT, RIGHT)에 의한 층 명령 입력
 //층 이동은 LED(0~7)의 점멸로 표현
 //Reset State: 0층 (0층부터 시작)
//사용한 하드웨어(기능): GPIO,Joy-stick,LED,SW,Keyscan,Update_LED
//제출일: 2024.5.15
//제출자 클래스: 목요일반
//              학번: 2021134050
//              이름: 이준혁
/////////////////////////////////////////////////////////

#include "stm32f4xx.h"

void _GPIO_Init(void);
void DelayMS(unsigned short wMS);       //
void DelayUS(unsigned short wUS);
uint16_t KEY_Scan(void);       

uint16_t JOY_Scan(void);

void BEEP(void);

void Update_LED_Pattern(uint16_t input); //비트수 증가용  함수제작


int main(void)
{
	_GPIO_Init();		// GPIO(LED(OUTPUT), SWITCH(INPUT)) 초기화
	GPIOG->ODR &= ~0x00FF;	// 초기값: LED0~7 Off
    
	//GPIO LED0 ON 
        GPIOG->ODR |= 0x0001; 	// GPIOG->ODR.0 'H'(LED ON)
    
	while(1)
	{
                switch(KEY_Scan())      //SW가 눌려져있는지 확인
                		{
			case 0xFE00  : 	//SW0
                            BEEP();
                            Update_LED_Pattern(0x0001); // SW에 맞는 LED 값 대입

			break;
			case 0xFD00 : 	//SW1
                            BEEP();
                            Update_LED_Pattern(0x0002); // SW에 맞는 LED 값 대입

			break;
                       case 0xFB00 : 	//SW2
                            BEEP();
                            Update_LED_Pattern(0x0004); // SW에 맞는 LED 값 대입
    
			break;
                       case 0xF700 : 	//SW3
                            BEEP();
                            Update_LED_Pattern(0x0008); // SW에 맞는 LED 값 대입
 
			break;
                       case 0xEF00 : 	//SW4
                            BEEP();
                            Update_LED_Pattern(0x0010); // SW에 맞는 LED 값 대입
 
			break;
                        case 0xDF00 : 	//SW5
                            BEEP();
                            Update_LED_Pattern(0x0020); // SW에 맞는 LED 값 대입

                        break;
                        case 0xBF00 : 	//SW6
                            BEEP();
                            Update_LED_Pattern(0x0040); //SW에 맞는 LED 값 대입
    
			break;
                        case 0x7F00 : 	//SW7
                            BEEP();
                            Update_LED_Pattern(0x0080); // SW에 맞는 LED 값 대입
                            
			break;
                       
		}
                switch(JOY_Scan()) //JOYSCAN이 눌려져있는지 확인
                      {
			case 0x01E0 :	// NAVI_LEFT
                        BEEP();
                        if(0x0001==GPIOG->ODR) //ODR이 led0번이 불이 켜져있는경우
                        {
                        //경고음
                        DelayMS(200); 
                        BEEP();
                        break;
                        }
                        else{
                        //딜레이 후 비트수 증가
                        DelayMS(500);
                        GPIOG->ODR >>= 1;
			break;
                        }
                        
			case 0x02E0:	// NAVI_RIGHT
                        BEEP();
                        if(0x0080==GPIOG->ODR)//ODR이 led7번이 불이 켜져있는경우
                        {
                         //경고음
                        DelayMS(200);
                        BEEP();
                        break;
                        }
                        else{
                         //딜레이 후 비트수 증가
                        DelayMS(500);
                        GPIOG->ODR <<= 1;
			break;
                        }
                     }

	}
}

/* GPIO (GPIOG(LED), GPIOH(Switch)) 초기 설정	*/
void _GPIO_Init(void)
{
	// LED (GPIO G) 설정 : Output mode
	RCC->AHB1ENR	|=  0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6) Enable							
	GPIOG->MODER 	|=  0x00005555;	// GPIOG 0~7 : Output mode (0b01)						
	GPIOG->OTYPER	&= ~0x00FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
	GPIOG->OSPEEDR 	|=  0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 
        //앞에 0000인것은 생략가능 하지만 뒤에 0000은 생략 불가
    
	// SW(GPIO H) 설정 : Input mode
	RCC->AHB1ENR    |=  0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7)에 Clock Enable							
	GPIOH->MODER 	&= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)				
	GPIOH->PUPDR 	&= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state
        //pullup 저항이 있어서 pullup저항을 활성화를 안했다.
        
        
	// Buzzer (GPIO F) 설정 : Output mode 
	RCC->AHB1ENR	|=  0x00000020; // RCC_AHB1ENR : GPIOF(bit#5) Enable							
	GPIOF->MODER 	&= ~0x000C0000;	// GPIOF 9 : Output mode (0b01)						
	GPIOF->MODER 	|= 0x00040000;	// GPIOF 9 : Push-pull  

	//Joy Stick SW(PORT I) 설정
	RCC->AHB1ENR	|= 0x00000100;	// RCC_AHB1ENR GPIOI Enable
	GPIOI->MODER	&=~0x000FFC00;	// GPIOI 5~9 : Input mode (reset state)
        //5핀을 input mode 0b0000 0000 0000 1111 1111 1100 0000 0000
	GPIOI->PUPDR	&= ~0x000FFC00;	// GPIOI 5~9 : Floating input (No Pull-up, pull-down) (reset state)
}	

uint8_t key_flag = 0; //SW가 계속 눌려졌는지를 알리는 인덱스
uint16_t KEY_Scan(void)	// input key SW0 - SW7 
{ 
	uint16_t key;
	key = GPIOH->IDR & 0xFF00;	// any key pressed ? 8개의 스위치 값이 인식
	if(key == 0xFF00)		// if no key, check key off 
	{	if(key_flag == 0)    // 1.아무것도 안할 계속 여기로 도는것이다
			return key;
		else //4. 때는 순간  kel_flag가 초기화 된다.
		{	DelayMS(10);
			key_flag = 0;
			return key;
		}
	}
	else				// if key input, check continuous key 8중 한개는 눌려졌다
	{	if(key_flag != 0)	// if continuous key, treat as no key input //3.지금 누르고 있다 하자만 삑 소리는 안난다
			return 0xFF00;
		else			// if new key,delay for debounce //2. 꾹누르고 있으면 첫번째 삑한다
		{	key_flag = 1; // keyflag 가  눌려졌다 sw input
			DelayMS(10); // 기다림
 			return key; //0xFE00
		}
	}
}



uint8_t joy_flag = 0;
uint16_t JOY_Scan(void)	// input joy stick NAVI_* 
{ 
	uint16_t key;
	key = GPIOI->IDR & 0x03E0;	// any key pressed ?
	if(key == 0x03E0)		// if no key, check key off
	{	if(joy_flag == 0)
			return key;
		else
		{	DelayMS(10);
			joy_flag = 0;
			return key;
		}
	}
	else				// if key input, check continuous key
	{	if(joy_flag != 0)	// if continuous key, treat as no key input
			return 0x03E0;
		else			// if new key,delay for debounce
		{	joy_flag = 1;
			DelayMS(10);
			return key;
		}
	}
}



void DelayMS(unsigned short wMS) //delayMS 진행
{
	register unsigned short i;
	for (i=0; i<wMS; i++)
		DelayUS(1000);	// 1000us => 1ms
}



void DelayUS(unsigned short wUS) //delayUS 진행
{
	volatile int Dly = (int)wUS*17;
	for(; Dly; Dly--);
}



void BEEP(void)			/* beep for 30 ms */
{ 	
	GPIOF->ODR |=  0x0200;	// PF9 'H' Buzzer on
	DelayMS(30);			// Delay 50 ms
	GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
}



void Update_LED_Pattern(uint16_t input) { //input이 들어가면  대소관계를 비교
    
    if(GPIOG->ODR<input)     //현재 led값이 input값보다 작을때
    {
      while(GPIOG->ODR!=input) //지정한 LED에  도달할때 까지 반복
      {
      DelayMS(500); //delay0.5초
      GPIOG->ODR <<= 1; //비트수 이동
      }
      //도달하고 나서 Beep 진행
      BEEP();
      DelayMS(500);
      BEEP();
      DelayMS(500);
      BEEP();      
      
      }
    
    else if(GPIOG->ODR>input)  //현재 led값이 input 값보다 높을때
    {
      while(GPIOG->ODR!=input) //지정한 LED에  도달할때 까지 반복
      {
      DelayMS(500); //delay
      GPIOG->ODR >>= 1; //비트수 이동
      }
      //도달하고 나서 beep 진행
      BEEP();
      DelayMS(500);
      BEEP();
      DelayMS(500);
      BEEP();      
    }
    //같은 경우 연속 beep 진행
    else
    {
      DelayMS(200);
      BEEP();  
    }
 
}

