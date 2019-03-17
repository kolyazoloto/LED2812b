#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <alloca.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>



class LED_Strip{
	public:
	LED_Strip(int length){
		DDRB |= (1<<PB0);
		strip_length=length;
		temp = (int**)malloc(strip_length*sizeof(int*)); //sozdaem massive
		for(unsigned char i=0;i<strip_length;i++){
			temp[i] = (int*)malloc(3*sizeof(int));
		}
	}
	void LED_color(unsigned char red,unsigned char green,unsigned char blue)
	{
		unsigned char led_on[3] = {green,red,blue};
		unsigned char j,i;	//zapolnyaem massive
		for (i=0; i<strip_length; i++){
			for (j=0;j<3;j++)
			{
				temp[i][j]=led_on[j];//çàäâèãàåì âî âðåìåííûé ìàññèâ
			}
		}

	}
	void LED_grad(int red1,int green1,int blue1,int red2,int green2,int blue2){
		int mean_red = (red1-red2)/(strip_length-1);
		int mean_green = (green1-green2)/(strip_length-1);
		int mean_blue = (blue1-blue2)/(strip_length-1);
		int mean[3] = {mean_green,mean_red,mean_blue};
		int first_color[3] = {green1,red1,blue1};
		for (int i=0; i<strip_length; i++){
			for (int j=0;j<3;j++)
			{
				temp[i][j]=first_color[j]-mean[j]*i;//na kajdii shag izmenyaem cvet
			}
		}
	}
	
	void LED_2color_string(int red1,int green1,int blue1,int red2,int green2,int blue2,int effect_length){
		int effect_color[3] = {green2,red2,blue2};
		int strip_color[3] = {green1,red1,blue1};
		for (int i=0; i<strip_length; i++){
			for (int j=0;j<3;j++){
				if(i<effect_length){
					temp[i][j] = effect_color[j];
				}
				else{
					temp[i][j] = strip_color[j];
				}
			}
		}
		LED_Write();
	}
	void LED_dwig_string(int effect_length)
	{
		for(int i=0;i<strip_length-effect_length;i++)
		{
			right_sdwig();
			LED_Write();
			_delay_ms(100);
		}
		for(int i=0;i<strip_length-effect_length;i++)
		{
			left_sdwig();
			LED_Write();
			_delay_ms(100);
		}
		
	}
	void LED_time_grad(int red1,int green1,int blue1,int red2,int green2,int blue2){
		int mean_red = (red1-red2)/(strip_length-1);
		int mean_green = (green1-green2)/(strip_length-1);
		int mean_blue = (blue1-blue2)/(strip_length-1);
		int mean[3] = {mean_green,mean_red,mean_blue};
		int sign = 1;
		while(1){
			if(sign>0){
				LED_color(red1,green1,blue1);
				LED_Write();
			}
			else{
				LED_color(red2,green2,blue2);
				LED_Write();
			}
			for(int k=0;k<100-1;k++){
				_delay_ms(100);
				for (int i=0; i<100; i++){
					for (int j=0;j<3;j++){
						temp[i][j] -= sign * mean[j];
					}
				}
				LED_Write();
			}
			sign *= -1;
		}
	}

	void LED_Write(void){
		int a;
		int j,i,n;
		for (n=0; n<strip_length; n++)
		{
			for (j=0; j<3; j++)			//ïåðâûì âûñòàâëÿåòñÿ G (Hi->Low), ïîòîì R è B
			{
				a = 0x80;				//10000000
				for (i=0;i<8;i++)			//8 bit ishem edidnicu ili nol
				{
					if ((temp[n][j]&a)==0)
					{
						zero();
					}
					else
					{
						one();
					}
					a=a>>1;
				}
			}
		}
		stop_cycle(); //îêîí÷àíèå öèêëà
	}
	void LED_rand(){
		for (int i=0; i<strip_length; i++)
		for (int j=0;j<3;j++)
		temp[i][j]=rand() % 256;
		LED_Write();
		_delay_ms(100);

	}

	void operator>>(int a){
		for(int i=0;i<a;i++){
			right_sdwig();
		}
	}
	void operator<<(int a){
		for(int i=0;i<a;i++){
			left_sdwig();
		}
	}
	void color_single(int number,int r,int g,int b){
		for(int i=0;i<3;i++){
			switch (i)
			{
			case 0:
				temp[number-1][i] = g;
				break;
			case 1:
				temp[number-1][i] = r;
				break;
			case 2:
				temp[number-1][i] = b;
				break;
			}
		}
	}
	

	private:
	int strip_length;
	int **temp;
	
	void one(){
		PORTB |= (1<<PB0);
		_delay_us(0.65);
		PORTB &= ~(1<<PB0);
		_delay_us(0.25);
	}
	void zero(){
		PORTB |= (1<<PB0);
		_delay_us(0.25);
		PORTB &= ~(1<<PB0);
		_delay_us(0.65);
	}
	void stop_cycle(){
		_delay_us(60);
	}
	void right_sdwig(){
		int first[3]={temp[strip_length-1][0],temp[strip_length-1][1],temp[strip_length-1][2]};
		for (int i=strip_length-1; i>0; i--){
			for (int j=0;j<3;j++)
			{
				temp[i][j] = temp[i-1][j];
			}
		}
		temp[0][0]=first[0];
		temp[0][1]=first[1];
		temp[0][2]=first[2];
	}
	void left_sdwig(){
		int first[3]={temp[0][0],temp[0][1],temp[0][2]};
		for (int i=0; i<strip_length-1; i++){
			for (int j=0;j<3;j++)
			{
				temp[i][j] = temp[i+1][j];
			}
		}
		temp[strip_length-1][0]=first[0];
		temp[strip_length-1][1]=first[1];
		temp[strip_length-1][2]=first[2];
	}

};




int main(void){
	LED_Strip strip(20); //118
	strip.LED_color(0,0,0);
	strip.color_single(3,0,0,255);

	strip.LED_Write();
	while (1){

	}
	
}