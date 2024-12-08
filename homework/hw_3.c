/////////////////////////////////////////////////////////
// 과제명: HW3  Great Escape(대탈출)

//과제개요: ROOM 마다 비밀번호(1자리 숫자)를 눌러 탈출
//사용한 하드웨어(기능): GPIO,Joy-stick,LED,SW,Keyscan,EXTI,GLCD,SWITCH
//제출일: 2024.06.07
//제출자 클래스: 목요일반
//              학번: 2021134050
//              이름: 이준혁
/////////////////////////////////////////////////////////


#include "stm32f4xx.h"
#include "GLCD.h"

void _GPIO_Init(void);  //gpio 초기화
void _EXTI_Init(void); //EXTI 초기화

void DisplayInitScreen(void); //화면 초기화
uint16_t KEY_Scan(void); //버튼 스캔
uint16_t JOY_Scan(void); //스이스틱
void BEEP(void); //beep 30ms
void BEEP_500(void); //beep 500ms

void DelayMS(unsigned short wMS); //Ms delay
void DelayUS(unsigned short wUS); //Us delay

#define RGB_PINK GET_RGB(255,0,255) //핑크 선정

char get = 'S'; //char 값은  LCD 화면에 도출값 구별

int main(void)
{
	_GPIO_Init(); 		// gpio 초기화
	LCD_Init();		// LCD 모듈 초기화
	DelayMS(100);
	_EXTI_Init();	// EXTI 초기화
	DisplayInitScreen();	// LCD 초기화면
	GPIOG->ODR &= ~0x00FF;	// LED 초기값: LED0~7 Off
        while(1)//while로 돌리는 값
        {
         }
    
}

//gpio_init 설정
void _GPIO_Init(void) 
{
	// LED (GPIO G) 설정 : Output mode
	RCC->AHB1ENR	|=  0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6) Enable							
	GPIOG->MODER 	&=  ~0x0000FFFF;	// GPIOG 0~7 : Clear (0b00)						
	GPIOG->MODER 	|=  0x00005555;	// GPIOG 0~7 : Output mode (0b01)						

	GPIOG->OTYPER	&= ~0x00FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	

	GPIOG->OSPEEDR &= ~0x0000FFFF;	// GPIOG 0~7 : Clear (0b00) 
	GPIOG->OSPEEDR 	|=  0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 
   
	// SW (GPIO H) 설정 : Input mode 
	RCC->AHB1ENR    |=  0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7) Enable							
	GPIOH->MODER 	&= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)				
	GPIOH->PUPDR 	&= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state

	// Buzzer (GPIO F) 설정 : Output mode
	RCC->AHB1ENR	|=  0x00000020;	// RCC_AHB1ENR : GPIOF(bit#5) Enable							

	GPIOF->MODER 	&= ~0x000C0000;	// GPIOF 9 : Clear (0b00)						
	GPIOF->MODER 	|=  0x00040000;	// GPIOF 9 : Output mode (0b01)						

	GPIOF->OTYPER 	&= ~0x0200;	// GPIOF 9 : Push-pull  	

	GPIOF->OSPEEDR 	&= ~0x000C0000;	// GPIOF 9 : Clear (0b00) 
	GPIOF->OSPEEDR 	|=  0x00040000;	// GPIOF 9 : Output speed 25MHZ Medium speed 

	//Joy Stick SW(PORT I) 설정
	RCC->AHB1ENR	|= 0x00000100;	// RCC_AHB1ENR GPIOI Enable
	GPIOI->MODER	&=~0x0000C000;	// GPIOI 5~9 : Input mode (reset state)

}

void _EXTI_Init(void)
{
    	RCC->AHB1ENR 	|= 0x00000080;	// RCC_AHB1ENR GPIOH Enable
	RCC->AHB1ENR	|= 0x00000100;	// RCC_AHB2NR  Enable
	RCC->APB2ENR 	|= 0x00004000;	// Enable System Configuration Controller Clock
	
	GPIOH->MODER 	&= ~0xFFFF0000;	// GPIOH PIN8~PIN15 Input mode (reset state)				 
	GPIOI->MODER	&=~0x0000C000;	// GPIOI 5~9 : Input mode


	SYSCFG->EXTICR[1] &= ~ 0x00FF; 	// EXTI[1] 초기
	SYSCFG->EXTICR[1] |= 0x8000; 	// EXTI7 에 대한 소스 입력은 GPIO로

	SYSCFG->EXTICR[2] &= ~ 0x00FF; 	// EXTI[2] 초기
	SYSCFG->EXTICR[2] |= 0x7077; 	// EXTI8,9,11 에 대한 소스 입력은 GPIO로

	SYSCFG->EXTICR[3] &= ~ 0x00FF; 	// EXTI[3] 초기
	SYSCFG->EXTICR[3] |= 0x7777; 	// EXTI 12,13,14,15 에 대한 소스 입력은 GPIO로

	EXTI->FTSR |= 0x0080;		// EXTI7: Falling Trigger Enable
	EXTI->FTSR |= 0x0100;		// EXTI8: Falling Trigger Enable
	EXTI->FTSR |= 0x0200;		// EXTI9: Falling Trigger Enable
	EXTI->FTSR |= 0x0800;		// EXTI11: Falling Trigger Enable
	EXTI->FTSR |= 0x1000;		// EXTI12: Falling Trigger Enable
	EXTI->FTSR |= 0x2000;		// EXTI13: Falling Trigger Enable
	EXTI->FTSR |= 0x4000;		// EXTI14: Falling Trigger Enable
	EXTI->FTSR |= 0x8000;		// EXTI15: Falling Trigger Enable

    	EXTI->IMR  |= 0x000100;  	// EXTI8인터럽트 mask (Interrupt Enable) 설정
	
	NVIC->ISER[0] |= (1 << 23);	// Enable 'Global Interrupt 

	NVIC->ISER[1] |= (1 << (40-32));	// Enable 'Global Interrupt 
}

void EXTI9_5_IRQHandler(void)		// handler value(5 ~ 9)
{       
        LCD_SetBrushColor(RGB_PINK); //전체 pink 색칠
	if(EXTI->PR & 0x0100)                   // EXTI8 Interrupt Pending(발생) 여부
	{
               BEEP(); //beep함
		EXTI->PR |= 0x0100; 		// EXTI8 Interrupt Pending(발생) 여부
                LCD_DrawFillRect(18, 26, 4, 4);//분홍색 사각형 채우는것

                EXTI->IMR  &= ~0x0100;  	// EXTI8 Interrupt Pending(발생) 초기화
                EXTI->IMR  |= 0x000200;  	// EXTI9인터럽트 mask (Interrupt Enable) 설정
                GPIOG->ODR |= 0x0001; //LED 0 on
              if(get=='S') // LCD 값 S에서 W로 바뀜
              {
                 get='W';
                 LCD_DisplayText(2, 18,"W");	
              }      
	}


	else if(EXTI->PR & 0x0200)                   // EXTI9 Interrupt Pending(발생) 여부
	{
               BEEP();
		EXTI->PR |= 0x0200; 		 // EXTI9 Interrupt Pending(발생) 여부
                LCD_DrawFillRect(34, 26, 4, 4);//분홍색 사각형 채우는것
                EXTI->IMR  &= ~0x200;  	// EXTI9 Interrupt Pending(발생) 초기화
                EXTI->IMR  |= 0x00080;  	// EXTI7인터럽트 mask (Interrupt Enable) 설정
                GPIOG->ODR |= 0x0002;
	}

    	else if(EXTI->PR & 0x0080)			// EXTI7 Interrupt Pending(발생) 여부
	{
               BEEP();
		EXTI->PR |= 0x0080;		// Pending bit Clear (clear를 안하면 인터럽트 수행후 다시 인터럽트 발생)
                LCD_DrawFillRect(50, 26, 4, 4);//분홍색 사각형 채우는것
                EXTI->IMR  &= ~0x0080;   	// EXTI7 Interrupt Pending(발생) 초기화
                EXTI->IMR  |= 0x0800;  	// EXTI11인터럽트 mask (Interrupt Enable) 설정
                GPIOG->ODR |= 0x0004; //led 불킴
	}
}

void EXTI15_10_IRQHandler(void) //EXTI 10 ~ 15사용
{
        LCD_SetBrushColor(RGB_PINK); //pink 지정
    	if(EXTI->PR & 0x0800)			// EXTI11 Interrupt Pending(발생) 여부
	{
               BEEP();
		EXTI->PR |= 0x0800;		// Pending bit Clear (clear를 안하면 인터럽트 수행후 다시 인터럽트 발생)
                LCD_DrawFillRect(66, 26, 4, 4);//분홍색 사각형 채우는것
                EXTI->IMR  &= ~0x800; 	// EXTI11 Interrupt Pending(발생) 초기화
                EXTI->IMR  |= 0x1000;  	// EXTI12인터럽트 mask (Interrupt Enable) 설정
                GPIOG->ODR |= 0x0008; //led on
	}
	//EXTI Example(2) : SW1가 Low에서 High가 될 때(Rising edge Trigger mode) EXTI9 인터럽트 발생, LED1 toggle
	else if(EXTI->PR & 0x1000)		// EXTI12 Interrupt Pending(발생) 여부
	{        
               BEEP();                                                     
		EXTI->PR |= 0x1000;		// Pending bit Clear (clear를 안하면 인터럽트 수행후 다시 인터럽트 발생)
                LCD_DrawFillRect(82, 26, 4, 4);//분홍색 사각형 채우는것
                EXTI->IMR  &= ~0x1000; 	// EXTI12 Interrupt Pending(발생) 초기화
                EXTI->IMR  |= 0x2000;  	// EXTI13인터럽트 mask (Interrupt Enable) 설정
                GPIOG->ODR |= 0x0010; //led 불킴
	}
	else if(EXTI->PR & 0x2000)		// EXTI13 Interrupt Pending(발생) 여부
	{                 
               BEEP();                                                     
		EXTI->PR |= 0x2000;		// Pending bit Clear (clear를 안하면 인터럽트 수행후 다시 인터럽트 발생)
                LCD_DrawFillRect(98, 26, 4, 4);//분홍색 사각형 채우는것
                EXTI->IMR  &= ~0x2000; 	// EXTI13 Interrupt Pending(발생) 초기화
                EXTI->IMR  |= 0x4000;  	// EXTI14인터럽트 mask (Interrupt Enable) 설정
                GPIOG->ODR |= 0x0020; //led 불
        }
	else if(EXTI->PR & 0x4000)		// EXTI14 Interrupt Pending(발생) 여부?
	{                 
               BEEP();                                                     
		EXTI->PR |= 0x4000;		// Pending bit Clear (clear를 안하면 인터럽트 수행후 다시 인터럽트 발생)
                LCD_DrawFillRect(114, 26, 4, 4);//분홍색 사각형 채우는것
                EXTI->IMR  &= ~0x4000; 	// EXTI14 Interrupt Pending(발생) 초기화
                EXTI->IMR  |= 0x8000;  	// EXTI15인터럽트 mask (Interrupt Enable) 설정
                GPIOG->ODR |= 0x0040; //led 불킴
        }
	else if(EXTI->PR & 0x8000)		// EXTI15 Interrupt Pending(발생) 여부?
	{                 
               BEEP();                                                     
		EXTI->PR |= 0x8000;		// Pending bit Clear (clear를 안하면 인터럽트 수행후 다시 인터럽트 발생)
                LCD_DrawFillRect(130, 26, 4, 4);//분홍색 사각형 채우는것
                EXTI->IMR  &= ~0x8000; 	// EXTI15 Interrupt Pending(발생) 초기화
                GPIOG->ODR |= 0x0080; //led 불
              if(get=='W')
              {
                 get='C';
                 LCD_DisplayText(2, 18,"C");	// 실행 바꿈
                 DelayMS(1000);
                BEEP(); 
                 DelayMS(500);
                BEEP(); 
                 DelayMS(500);
                BEEP(); 
                 DelayMS(3000);
                BEEP_500(); //beep 500ms
                 LCD_SetBrushColor(RGB_WHITE); //분홍색 에서 -> 하얀색
                LCD_DrawFillRect(18, 26, 4, 4);  //적용
                LCD_DrawFillRect(34, 26, 4, 4);
                LCD_DrawFillRect(50, 26, 4, 4);
                LCD_DrawFillRect(66, 26, 4, 4);
                LCD_DrawFillRect(82, 26, 4, 4);
                LCD_DrawFillRect(98, 26, 4, 4);
                LCD_DrawFillRect(114, 26, 4, 4);
                LCD_DrawFillRect(130, 26, 4, 4);
                 get='S';                //Get을 S로
                 LCD_DisplayText(2, 18,"S");	// Title값을 S로 
                GPIOG->ODR &= ~0x00FF;	// LED 초기값: LED0~7 Off
                 EXTI->IMR  |= 0x000100;  	// EXTI8인터럽트 mask (Interrupt Enable) 설정
              }
        }
}



/* Switch가 입력되었는지 여부와 어떤 switch가 입력되었는지의 정보를 return하는 함수  */ 
uint8_t key_flag = 0;
uint16_t KEY_Scan(void)	// input key SW0 - SW7 
{ 
	uint16_t key;
	key = GPIOH->IDR & 0xFF00;	// any key pressed ?
	if(key == 0xFF00)		// if no key, check key off
	{  	if(key_flag == 0)
        		return key;
      		else
		{	DelayMS(10);
        		key_flag = 0;
        		return key;
        	}
    	}
  	else				// if key input, check continuous key
	{	if(key_flag != 0)	// if continuous key, treat as no key input
        		return 0xFF00;
      		else			// if new key,delay for debounce
		{	key_flag = 1;
			DelayMS(10);
 			return key;
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


void BEEP(void)			
{ 	
	GPIOF->ODR |=  0x0200;	// PF9 'H' Buzzer on
	DelayMS(30);		// Delay 30 ms
	GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
}

void BEEP_500(void) //500m 함수
{ 	
	GPIOF->ODR |=  0x0200;	// PF9 'H' Buzzer on
	DelayMS(500);		// Delay 500ms
	GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
}

void DelayMS(unsigned short wMS)
{
	register unsigned short i;
	for (i=0; i<wMS; i++)
		DelayUS(1000);	// 1000us => 1ms
}

void DelayUS(unsigned short wUS)
{
	volatile int Dly = (int)wUS*17;
	for(; Dly; Dly--);
}

void DisplayInitScreen(void)
{
	LCD_Clear(RGB_WHITE);		// 화면 클리어
	LCD_SetFont(&Gulim8);		// 폰트 : 굴림 8 & : 포인터 표시 활용
	LCD_SetBackColor(RGB_WHITE); //화면 흰색
        LCD_SetTextColor(RGB_BLUE);	// 글자배경색 : Blue
	LCD_DisplayText(0,0,"2021134050 LJH");
	LCD_SetTextColor(RGB_BLACK);	// 글자색 : Blackg
	LCD_DisplayText(1,0,"R 0 1 2 3 4 5 6 7");	// Title two
        LCD_SetPenColor(RGB_BLACK); //상자틀색
        LCD_DrawRectangle(17, 25, 5, 5); //상자 네모 그리기
        LCD_DrawRectangle(33, 25, 5, 5);
        LCD_DrawRectangle(49, 25, 5, 5);
        LCD_DrawRectangle(65, 25, 5, 5);
        LCD_DrawRectangle(81, 25, 5, 5);
        LCD_DrawRectangle(97, 25, 5, 5);
        LCD_DrawRectangle(113, 25, 5, 5);
        LCD_DrawRectangle(129, 25, 5, 5);
         LCD_SetBrushColor(RGB_WHITE); //하얀색
        LCD_DrawFillRect(18, 26, 4, 4);  //적용
        LCD_DrawFillRect(34, 26, 4, 4);
        LCD_DrawFillRect(50, 26, 4, 4);
        LCD_DrawFillRect(66, 26, 4, 4);
        LCD_DrawFillRect(82, 26, 4, 4);
        LCD_DrawFillRect(98, 26, 4, 4);
        LCD_DrawFillRect(114, 26, 4, 4);
        LCD_DrawFillRect(130, 26, 4, 4);
        LCD_DisplayText(2, 18,"S");	// text S 선정
        
}
