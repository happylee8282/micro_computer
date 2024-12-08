/////////////////////////////////////////////////////////
// ������: HW2. ����������

//��������: 
// �� �Է� ��ɿ� ���� ���������� �̵� ������ ������
// �� �Է� ��� ���
 // (1) SW(0~7)�� ���� �� ��� �Է�
//  (2) JS(Joy-Stick)(LEFT, RIGHT)�� ���� �� ��� �Է�
 //�� �̵��� LED(0~7)�� ����� ǥ��
 //Reset State: 0�� (0������ ����)
//����� �ϵ����(���): GPIO,Joy-stick,LED,SW,Keyscan,Update_LED
//������: 2024.5.15
//������ Ŭ����: ����Ϲ�
//              �й�: 2021134050
//              �̸�: ������
/////////////////////////////////////////////////////////

#include "stm32f4xx.h"

void _GPIO_Init(void);
void DelayMS(unsigned short wMS);       //
void DelayUS(unsigned short wUS);
uint16_t KEY_Scan(void);       

uint16_t JOY_Scan(void);

void BEEP(void);

void Update_LED_Pattern(uint16_t input); //��Ʈ�� ������  �Լ�����


int main(void)
{
	_GPIO_Init();		// GPIO(LED(OUTPUT), SWITCH(INPUT)) �ʱ�ȭ
	GPIOG->ODR &= ~0x00FF;	// �ʱⰪ: LED0~7 Off
    
	//GPIO LED0 ON 
        GPIOG->ODR |= 0x0001; 	// GPIOG->ODR.0 'H'(LED ON)
    
	while(1)
	{
                switch(KEY_Scan())      //SW�� �������ִ��� Ȯ��
                		{
			case 0xFE00  : 	//SW0
                            BEEP();
                            Update_LED_Pattern(0x0001); // SW�� �´� LED �� ����

			break;
			case 0xFD00 : 	//SW1
                            BEEP();
                            Update_LED_Pattern(0x0002); // SW�� �´� LED �� ����

			break;
                       case 0xFB00 : 	//SW2
                            BEEP();
                            Update_LED_Pattern(0x0004); // SW�� �´� LED �� ����
    
			break;
                       case 0xF700 : 	//SW3
                            BEEP();
                            Update_LED_Pattern(0x0008); // SW�� �´� LED �� ����
 
			break;
                       case 0xEF00 : 	//SW4
                            BEEP();
                            Update_LED_Pattern(0x0010); // SW�� �´� LED �� ����
 
			break;
                        case 0xDF00 : 	//SW5
                            BEEP();
                            Update_LED_Pattern(0x0020); // SW�� �´� LED �� ����

                        break;
                        case 0xBF00 : 	//SW6
                            BEEP();
                            Update_LED_Pattern(0x0040); //SW�� �´� LED �� ����
    
			break;
                        case 0x7F00 : 	//SW7
                            BEEP();
                            Update_LED_Pattern(0x0080); // SW�� �´� LED �� ����
                            
			break;
                       
		}
                switch(JOY_Scan()) //JOYSCAN�� �������ִ��� Ȯ��
                      {
			case 0x01E0 :	// NAVI_LEFT
                        BEEP();
                        if(0x0001==GPIOG->ODR) //ODR�� led0���� ���� �����ִ°��
                        {
                        //�����
                        DelayMS(200); 
                        BEEP();
                        break;
                        }
                        else{
                        //������ �� ��Ʈ�� ����
                        DelayMS(500);
                        GPIOG->ODR >>= 1;
			break;
                        }
                        
			case 0x02E0:	// NAVI_RIGHT
                        BEEP();
                        if(0x0080==GPIOG->ODR)//ODR�� led7���� ���� �����ִ°��
                        {
                         //�����
                        DelayMS(200);
                        BEEP();
                        break;
                        }
                        else{
                         //������ �� ��Ʈ�� ����
                        DelayMS(500);
                        GPIOG->ODR <<= 1;
			break;
                        }
                     }

	}
}

/* GPIO (GPIOG(LED), GPIOH(Switch)) �ʱ� ����	*/
void _GPIO_Init(void)
{
	// LED (GPIO G) ���� : Output mode
	RCC->AHB1ENR	|=  0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6) Enable							
	GPIOG->MODER 	|=  0x00005555;	// GPIOG 0~7 : Output mode (0b01)						
	GPIOG->OTYPER	&= ~0x00FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
	GPIOG->OSPEEDR 	|=  0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 
        //�տ� 0000�ΰ��� �������� ������ �ڿ� 0000�� ���� �Ұ�
    
	// SW(GPIO H) ���� : Input mode
	RCC->AHB1ENR    |=  0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7)�� Clock Enable							
	GPIOH->MODER 	&= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)				
	GPIOH->PUPDR 	&= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state
        //pullup ������ �־ pullup������ Ȱ��ȭ�� ���ߴ�.
        
        
	// Buzzer (GPIO F) ���� : Output mode 
	RCC->AHB1ENR	|=  0x00000020; // RCC_AHB1ENR : GPIOF(bit#5) Enable							
	GPIOF->MODER 	&= ~0x000C0000;	// GPIOF 9 : Output mode (0b01)						
	GPIOF->MODER 	|= 0x00040000;	// GPIOF 9 : Push-pull  

	//Joy Stick SW(PORT I) ����
	RCC->AHB1ENR	|= 0x00000100;	// RCC_AHB1ENR GPIOI Enable
	GPIOI->MODER	&=~0x000FFC00;	// GPIOI 5~9 : Input mode (reset state)
        //5���� input mode 0b0000 0000 0000 1111 1111 1100 0000 0000
	GPIOI->PUPDR	&= ~0x000FFC00;	// GPIOI 5~9 : Floating input (No Pull-up, pull-down) (reset state)
}	

uint8_t key_flag = 0; //SW�� ��� ������������ �˸��� �ε���
uint16_t KEY_Scan(void)	// input key SW0 - SW7 
{ 
	uint16_t key;
	key = GPIOH->IDR & 0xFF00;	// any key pressed ? 8���� ����ġ ���� �ν�
	if(key == 0xFF00)		// if no key, check key off 
	{	if(key_flag == 0)    // 1.�ƹ��͵� ���ҋ� ��� ����� ���°��̴�
			return key;
		else //4. ���� ����  kel_flag�� �ʱ�ȭ �ȴ�.
		{	DelayMS(10);
			key_flag = 0;
			return key;
		}
	}
	else				// if key input, check continuous key 8�� �Ѱ��� ��������
	{	if(key_flag != 0)	// if continuous key, treat as no key input //3.���� ������ �ִ� ���ڸ� �� �Ҹ��� �ȳ���
			return 0xFF00;
		else			// if new key,delay for debounce //2. �ڴ����� ������ ù��° ���Ѵ�
		{	key_flag = 1; // keyflag ��  �������� sw input
			DelayMS(10); // ��ٸ�
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



void DelayMS(unsigned short wMS) //delayMS ����
{
	register unsigned short i;
	for (i=0; i<wMS; i++)
		DelayUS(1000);	// 1000us => 1ms
}



void DelayUS(unsigned short wUS) //delayUS ����
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



void Update_LED_Pattern(uint16_t input) { //input�� ����  ��Ұ��踦 ��
    
    if(GPIOG->ODR<input)     //���� led���� input������ ������
    {
      while(GPIOG->ODR!=input) //������ LED��  �����Ҷ� ���� �ݺ�
      {
      DelayMS(500); //delay0.5��
      GPIOG->ODR <<= 1; //��Ʈ�� �̵�
      }
      //�����ϰ� ���� Beep ����
      BEEP();
      DelayMS(500);
      BEEP();
      DelayMS(500);
      BEEP();      
      
      }
    
    else if(GPIOG->ODR>input)  //���� led���� input ������ ������
    {
      while(GPIOG->ODR!=input) //������ LED��  �����Ҷ� ���� �ݺ�
      {
      DelayMS(500); //delay
      GPIOG->ODR >>= 1; //��Ʈ�� �̵�
      }
      //�����ϰ� ���� beep ����
      BEEP();
      DelayMS(500);
      BEEP();
      DelayMS(500);
      BEEP();      
    }
    //���� ��� ���� beep ����
    else
    {
      DelayMS(200);
      BEEP();  
    }
 
}

