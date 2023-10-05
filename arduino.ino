#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX
int pulse=A0;
int main_relay =9;
int sub_relay1=4;
int sub_relay2=8;
float Total_current;
float adc_value;
int count =0;
#define SAMPLES 300   //Number of samples you want to take everytime you loop
#define ACS_pin1 A1    //ACS712 data pin analong input
#define ACS_pin2 A2
float High_peak1,Low_peak1;
float High_peak2,Low_peak2;  //Variables to measure or calculate
float Amps_Peak_Peak1, Amps_RMS1;
float Amps_Peak_Peak2, Amps_RMS2;
float units_per_hr;
float units;
float rupees;
String transmit;
int flag1;
int flag;
String input=""; // Declare an Empty String Variable
String input_trimmed="";// a variable to store the trimmed string
String substring=""; // a variable to store the substring 
int length_before_trim = 0, length_after_trim = 0, length_substring=0; // 3 variables for storing the string lengths

 



void setup() {
  
  Serial.begin(115200);
  mySerial.begin(9600);
  
  pinMode(pulse,INPUT);
  

pinMode(ACS_pin1,INPUT);
pinMode(ACS_pin2,INPUT);
pinMode(main_relay,OUTPUT);
pinMode(sub_relay1,OUTPUT);
pinMode(sub_relay2,OUTPUT);
digitalWrite(main_relay,HIGH);
digitalWrite(sub_relay1,HIGH);
digitalWrite(sub_relay2,HIGH);


}

void loop() {
    receive_msg();
  


 
 int value =analogRead(pulse);
 adc_value=(value/1023.0)*5.0;

 
 
 if (adc_value>3)
 {
  
  count++;
 
  units=(adc_value/10)*count;
  units_per_hr=(312.5/1000)*count;
  units=units_per_hr*2.4;
  rupees=units*7.0;


 delay(500);
 }
 read_Amps1();                               //Launch the read_Amps function
 read_Amps2();      
 Amps_RMS1 = (Amps_Peak_Peak1*0.3536);     //Now we have the peak to peak value normally the formula requires only multiplying times 0.3536
 Amps_RMS2 = Amps_Peak_Peak2*0.3536;
 Total_current=Amps_RMS1+Amps_RMS2;   
                                             //but since the values will be very big you should multiply by 0.06, you can first not use it, 
                                             //do your calculations and compare them to real values measured by an Ammeter. eg: 0.06=Real value/Measured value
                                             

  transmit=String(Amps_RMS1)+String(",")+String(Amps_RMS2);
  Serial.println(transmit);
  delay(1000);

 if(Total_current>0.35)
 {
 
  if(flag==0)
  {
  send_sms_over();
  flag=1;
    
 }
 else
 {
  flag=1;
 }
 }
 
 if(rupees>150)
 {

  if (flag1==0)
  {
    
  
  send_sms_cost();
  flag1=1;

  }
  else
  {
    flag1=1;
  }
 }
// 
 
}
 
void receive_msg()
{
  if(mySerial.available()) // Check if there is incoming data in the Serial Buffer
  {
    input = ""; // reset the input variable
    while(mySerial.available()) // Process incoming data till the buffer becomes empty
    {     
      input += (char)mySerial.read();// Read a byte of data, convert it to character and append it to the string variable
      delay(1);
    }
    input_trimmed = input; // assign the trimmed variable with the input value
    input_trimmed.trim();
    substring = input_trimmed.substring(49,55);
 
  
    if(substring=="mainr1")
    {
 
      digitalWrite(main_relay,LOW);
     
    }

    if(substring=="mainr2")
    
    {
      digitalWrite(sub_relay1,LOW);

    }
    if(substring=="mainr3")
    { 
     
      digitalWrite(sub_relay2,LOW);
      
    }
}
}



 void send_sms_over()
{
  mySerial.println("AT+CMGF=1");    //To send SMS in Text Mode
  delay(1000);
  mySerial.println("AT+CMGS=\"+918078054844\"\r"); //Change to destination phone number 
  delay(1000);
  mySerial.print("Total current consumption is");//the content of the message
  mySerial.print(Total_current);
  delay(200);
  mySerial.println((char)26); //the stopping character Ctrl+Z
  delay(1000);  
}
void send_sms_cost()
{
  mySerial.println("AT+CMGF=1");    //To send SMS in Text Mode
  delay(1000);
  mySerial.println("AT+CMGS=\"+918078054844\"\r"); //Change to destination phone number 
  delay(1000);
  mySerial.print("Total cost is Rs.  ");//the content of the message
  mySerial.print(rupees);
  delay(200);
  mySerial.println((char)26); //the stopping character Ctrl+Z
  delay(1000);  
}
void read_Amps1()            //read_Amps function calculate the difference between the high peak and low peak 
{                           //get peak to peak value
  int cnt1;            //Counter
  High_peak1 = 0;      //We first assume that our high peak is equal to 0 and low peak is 1024, yes inverted
  Low_peak1 = 1024;
  
      for(cnt1=0 ; cnt1<SAMPLES ; cnt1++)          //everytime a sample (module value) is taken it will go through test
      {
        float ACS_Value1 = analogRead(ACS_pin1);
        //float ACS_Value2 = analogRead(ACS_Pin2);//We read a single value from the module

        
        if(ACS_Value1 > High_peak1)                //If that value is higher than the high peak (at first is 0)
            {
              High_peak1 = ACS_Value1;            //The high peak will change from 0 to that value found
            }
        
        if(ACS_Value1 < Low_peak1)                //If that value is lower than the low peak (at first is 1024)
            {
              Low_peak1 = ACS_Value1;             //The low peak will change from 1024 to that value found
            }
      }                                        //We keep looping until we take all samples and at the end we will have the high/low peaks values
      
  Amps_Peak_Peak1 = ((High_peak1 - Low_peak1)*0.0264)-0.4;//Subtracting offset value
  if (Amps_Peak_Peak1<0)
  {
    Amps_Peak_Peak1=0;
  }//Calculate the difference
  }
  
  void read_Amps2()            //read_Amps function calculate the difference between the high peak and low peak 
{                           //get peak to peak value
  int cnt2;            //Counter
  High_peak2 = 0;      //We first assume that our high peak is equal to 0 and low peak is 1024, yes inverted
  Low_peak2 = 1024;
  
      for(cnt2=0 ; cnt2<SAMPLES ; cnt2++)          //everytime a sample (module value) is taken it will go through test
      {
        float ACS_Value2 = analogRead(ACS_pin2);
        //float ACS_Value2 = analogRead(ACS_Pin2);//We read a single value from the module

        
        if(ACS_Value2 > High_peak2)                //If that value is higher than the high peak (at first is 0)
            {
              High_peak2 = ACS_Value2;            //The high peak will change from 0 to that value found
            }
        
        if(ACS_Value2 < Low_peak2)                //If that value is lower than the low peak (at first is 1024)
            {
              Low_peak2 = ACS_Value2;             //The low peak will change from 1024 to that value found
            }
      }                                        //We keep looping until we take all samples and at the end we will have the high/low peaks values
      
  Amps_Peak_Peak2 = ((High_peak2 - Low_peak2)*0.0264)-0.4;//Subtracting offset value
  if (Amps_Peak_Peak2<0)
  {
    Amps_Peak_Peak2=0;
  }//Calculate the difference
  }
  
